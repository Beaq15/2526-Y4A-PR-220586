// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "AdvancedAICharacter.h"
#include "DamageableInterface.h"

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------

AAIC_Enemy_Base::AAIC_Enemy_Base()
{
    // Perception component
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerception);

    // Sight
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1500.f;
    SightConfig->LoseSightRadius = 2000.f;
    SightConfig->PeripheralVisionAngleDegrees = 60.f;
    SightConfig->SetMaxAge(20.f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

    // Hearing
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 500.f;
    HearingConfig->SetMaxAge(3.f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;

    // Damage
    DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
    DamageConfig->SetMaxAge(5.f);

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->ConfigureSense(*HearingConfig);
    AIPerception->ConfigureSense(*DamageConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    AIPerception->OnPerceptionUpdated.AddUniqueDynamic(this, &AAIC_Enemy_Base::OnPerceptionUpdated);
}
void AAIC_Enemy_Base::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
    if (!Enemy) return;

    // Short delay so the blackboard is ready before running the BT
    FTimerHandle BTStartTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(BTStartTimerHandle, [this, Enemy]() 
        {
            StartBehaviorTree(Enemy); 
            if (UAIPerceptionComponent* Perception = GetAIPerceptionComponent())
            {
                Perception->RequestStimuliListenerUpdate();
            }
        }, 0.2f, false);
    
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(this, &AAIC_Enemy_Base::CheckIfForgottonSeenActor);
    GetWorldTimerManager().SetTimer(ForgottenActorTimerHandle, TimerDelegate, 0.5f, true);
}

void AAIC_Enemy_Base::OnUnPossess()
{
    Super::OnUnPossess();

    GetWorldTimerManager().ClearTimer(ForgottenActorTimerHandle);
    ForgottenActorTimerHandle.Invalidate();
}

FGenericTeamId AAIC_Enemy_Base::GetGenericTeamId() const
{
    if (const AEnemyBase* Enemy = Cast <AEnemyBase>(GetPawn()))
        return Enemy->GetGenericTeamId();
    return FGenericTeamId(0);
}

bool AAIC_Enemy_Base::OnSameTeam(AActor* OtherActor)
{
    if (!OtherActor) return false;
    const IGenericTeamAgentInterface* OtherAgent = Cast<IGenericTeamAgentInterface>(OtherActor);
    if (!OtherAgent) return false;

    return GetGenericTeamId() == OtherAgent->GetGenericTeamId();
}

//----------------------------------------------------------------------
// Internal Helpers
//----------------------------------------------------------------------

void AAIC_Enemy_Base::StartBehaviorTree(AEnemyBase* Enemy)
{
    if (!Enemy->BehaviorTree)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Valid Behavior Tree in Enemy Actor"));
        return;
    }

    RunBehaviorTree(Enemy->BehaviorTree);

    SetStateAsPassive();

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;
    
    float AttackRadius = 0.f, DefendRadius = 0.f;
    IEnemyInterface::Execute_GetIdealRange(Enemy, AttackRadius, DefendRadius);
    BB->SetValueAsFloat(AttackRadiusKeyName, AttackRadius);
    BB->SetValueAsFloat(DefendRadiusKeyName, DefendRadius);

    // Handle any actors already in sight when we possess
    TArray<AActor*> PerceivedActors;
    AIPerception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
    for (AActor* Actor : PerceivedActors)
    {
        HandleSensedSight(Actor);
    }
}

void AAIC_Enemy_Base::CheckIfForgottonSeenActor()
{
    TArray<AActor*> KnownPerceivedActors;
    AIPerception->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), KnownPerceivedActors);

    if (KnownSeenActors.Num() == KnownPerceivedActors.Num()) return;
        
    for (AActor* Actor : KnownSeenActors)
    {
        if (!IsValid(Actor)) continue;

        if (!KnownPerceivedActors.Contains(Actor))
            HandleForgotActor(Actor);
    }
}

bool AAIC_Enemy_Base::CanSenseActor(AActor* Actor, EAISense Sense, FAIStimulus& OutStimulus) // + outsense
{
    if (!AIPerception || !Actor) return false;

    FAISenseID SenseID;

    switch (Sense)
    {
    case EAISense::Sight:    SenseID = UAISense::GetSenseID<UAISense_Sight>();   break;
    case EAISense::Hearing:  SenseID = UAISense::GetSenseID<UAISense_Hearing>(); break;
    case EAISense::Damage:   SenseID = UAISense::GetSenseID<UAISense_Damage>();  break;
    default:                 return false;
    }

    FActorPerceptionBlueprintInfo PerceptionInfo;
    AIPerception->GetActorsPerception(Actor, PerceptionInfo);

    for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
    {
        if (Stimulus.Type == SenseID && Stimulus.WasSuccessfullySensed())
        {
            OutStimulus = Stimulus;
            return true;
        }
    }
    return false;
}

EAIState AAIC_Enemy_Base::GetCurrentState() const
{
    if (const UBlackboardComponent* BB = GetBlackboardComponent())
    {
        return EAIState(BB->GetValueAsInt(StateKeyName));
    }
    return EAIState::Passive;
}

//----------------------------------------------------------------------
// Perception Handlers
//----------------------------------------------------------------------

void AAIC_Enemy_Base::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        if (CanSenseActor(Actor, EAISense::Sight))
            HandleSensedSight(Actor);
        else
            HandleLostSight(Actor);

        FAIStimulus OutStimulus;
        if (CanSenseActor(Actor, EAISense::Hearing, OutStimulus))
            HandleSensedSound(OutStimulus.StimulusLocation);

        if (CanSenseActor(Actor, EAISense::Damage))
            HandleSensedDamage(Actor);
    }
}

void AAIC_Enemy_Base::HandleSensedSight(AActor* Actor)
{
    if (OnSameTeam(Actor)) return;
    
    KnownSeenActors.AddUnique(Actor);

    switch (GetCurrentState())
    {
    case EAIState::Passive:
    case EAIState::Investigating:
    case EAIState::Seeking:
        SetStateAsAttacking(Actor, true);
        break;
    case EAIState::Attacking:
        if (Actor == AttackTargetActor)
        {
            GetWorldTimerManager().ClearTimer(SeekAttackTargetTimer);
            SeekAttackTargetTimer.Invalidate();
        }
        break;
    default:
        break;
    }
}

void AAIC_Enemy_Base::HandleSensedSound(FVector Location)
{
    switch (GetCurrentState())
    {
    case EAIState::Passive:
    case EAIState::Investigating:
    case EAIState::Seeking:
        SetStateAsInvestigating(Location);
        break;

    default:
        break;
    }
}

void AAIC_Enemy_Base::HandleSensedDamage(AActor* Actor)
{
    if (OnSameTeam(Actor)) return;

    switch (GetCurrentState())
    {
    case EAIState::Passive:
    case EAIState::Investigating:
    case EAIState::Seeking:
        SetStateAsAttacking(Actor, false);
        break;

    default:
        break;
    }
}

void AAIC_Enemy_Base::HandleLostSight(AActor* Actor)
{
    if (Actor != AttackTargetActor)
        return;

    switch (GetCurrentState())
    {
    case EAIState::Attacking:
    case EAIState::Frozen:
    case EAIState::Investigating:
        GetWorldTimerManager().ClearTimer(SeekAttackTargetTimer);
        SeekAttackTargetTimer.Invalidate();
        GetWorldTimerManager().SetTimer(SeekAttackTargetTimer, this, &AAIC_Enemy_Base::SeekAttackTarget, 3.f, false);
        break;

    default:
        break;
    }
}

void AAIC_Enemy_Base::HandleForgotActor(AActor* Actor)
{
    KnownSeenActors.Remove(Actor);

    if (Actor == AttackTargetActor)
        SetStateAsPassive();

}

//----------------------------------------------------------------------
// State API
//----------------------------------------------------------------------

void AAIC_Enemy_Base::SetStateAsPassive()
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Passive);
}

void AAIC_Enemy_Base::SetStateAsDead()
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Dead);
}

void AAIC_Enemy_Base::SetStateAsFrozen()
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Frozen);
}

void AAIC_Enemy_Base::SetStateAsAttacking(AActor* AttackTarget, bool UseLastKnownAttackTarget)
{
    AActor* NewAttackTarget = (UseLastKnownAttackTarget && IsValid(AttackTarget)) ? AttackTarget : AttackTargetActor.Get();

    if (!IsValid(NewAttackTarget))
    {
        SetStateAsPassive();
        return;
    }

    if (!NewAttackTarget->Implements<UDamageableInterface>()) return;
    
    if (IDamageableInterface::Execute_IsDead(NewAttackTarget))
    {
        SetStateAsPassive();
        return;
    }

    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Attacking);
        BB->SetValueAsObject(AttackTargetKeyName, NewAttackTarget);
    }

    AttackTargetActor = NewAttackTarget;
}

void AAIC_Enemy_Base::SetStateAsInvestigating(FVector Location)
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Investigating);
        BB->SetValueAsVector(PointOfInterestKeyName, Location);
    }
}

void AAIC_Enemy_Base::SetStateAsSeeking(FVector Location)
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Seeking);
        BB->SetValueAsVector(PointOfInterestKeyName, Location);
    }
}

//----------------------------------------------------------------------
// Timer Callbacks
//----------------------------------------------------------------------

void AAIC_Enemy_Base::SeekAttackTarget()
{
    SetStateAsSeeking(AttackTargetActor->GetActorLocation());
    GetWorldTimerManager().ClearTimer(SeekAttackTargetTimer);
    SeekAttackTargetTimer.Invalidate();
}
