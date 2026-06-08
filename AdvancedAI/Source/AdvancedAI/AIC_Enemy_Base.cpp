// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "AdvancedAICharacter.h"
#include "DamageableInterface.h"

AAIC_Enemy_Base::AAIC_Enemy_Base()
{
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerception);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 800.f;
    SightConfig->LoseSightRadius = 1200.f;
    SightConfig->PeripheralVisionAngleDegrees = 60.f;
    SightConfig->SetMaxAge(5.f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 500.f;
    HearingConfig->SetMaxAge(3.f);

    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;

    DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
    DamageConfig->SetMaxAge(5.f);

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->ConfigureSense(*HearingConfig);
    AIPerception->ConfigureSense(*DamageConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    AIPerception->OnPerceptionUpdated.AddUniqueDynamic(this, &AAIC_Enemy_Base::OnPerceptionUpdated);
}
void AAIC_Enemy_Base::CheckIfForgottonSeenActor()
{
    TArray<AActor*> KnownPerceivedActors;
    AIPerception->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), KnownPerceivedActors);
    if (KnownSeenActors.Num() != KnownPerceivedActors.Num())
    {
        for (AActor * Actor : KnownSeenActors)
        {
            if (!IsValid(Actor)) continue;

            if (KnownPerceivedActors.Find(Actor) == -1)
                HandleForgotActor(Actor);
        }
    }
}
void AAIC_Enemy_Base::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
    if (!Enemy)
        return;

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

void AAIC_Enemy_Base::StartBehaviorTree(AEnemyBase* Enemy)
{
    if (!Enemy->BehaviorTree)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Valid Behavior Tree in Enemy Actor"));
        return;
    }

    RunBehaviorTree(Enemy->BehaviorTree);

    SetStateAsPassive();

    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        float AttackRadius = 0.f, DefendRadius = 0.f;
        IEnemyInterface::Execute_GetIdealRange(Enemy, AttackRadius, DefendRadius);
        BB->SetValueAsFloat(AttackRadiusKeyName, AttackRadius);
        BB->SetValueAsFloat(DefendRadiusKeyName, DefendRadius);

        //AAdvancedAICharacter* Player = Cast<AAdvancedAICharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        //BB->SetValueAsObject(AttackTargetKeyName, Player);

        TArray<AActor*> PerceivedActors;
        AIPerception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
        for (AActor* Actor : PerceivedActors)
        {
            HandleSensedSight(Actor);
        }
    }

}

bool AAIC_Enemy_Base::CanSenseActor(AActor* Actor, EAISense Sense, FAIStimulus& OutStimulus) // + outsense
{
    if (!AIPerception || !Actor) return false;

    FAISenseID SenseID;

    switch (Sense)
    {
    case EAISense::None: 
        return false;
    case EAISense::Sight:
        SenseID = UAISense::GetSenseID<UAISense_Sight>();
        break;
    case EAISense::Hearing:
        SenseID = UAISense::GetSenseID<UAISense_Hearing>();
        break;
    case EAISense::Damage:
        SenseID = UAISense::GetSenseID<UAISense_Damage>();
        break;
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

void AAIC_Enemy_Base::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        if (CanSenseActor(Actor, EAISense::Sight))
        {
            HandleSensedSight(Actor);
        }
        else
        {
            HandleLostSight(Actor);
        }
        FAIStimulus OutStimulus;
        if (CanSenseActor(Actor, EAISense::Hearing, OutStimulus))
        {
            HandleSensedSound(OutStimulus.StimulusLocation);
        }
        if (CanSenseActor(Actor, EAISense::Damage))
        {
            
            HandleSensedDamage(Actor);
        }
    }
}

void AAIC_Enemy_Base::HandleSensedSight(AActor* Actor)
{
    KnownSeenActors.AddUnique(Actor);

    switch (GetCurrentState())
    {
    case int32(EAIState::Passive):
        SetStateAsAttacking(Actor, true);
        break;
    case int32(EAIState::Investigating):
        SetStateAsAttacking(Actor, true);
        break;
    }
}

void AAIC_Enemy_Base::HandleSensedSound(FVector Location)
{
    switch (GetCurrentState())
    {
    case int32(EAIState::Passive):
        SetStateAsInvestigating(Location);
        break;
    case int32(EAIState::Investigating):
        SetStateAsInvestigating(Location);
        break;
    }
}

void AAIC_Enemy_Base::HandleSensedDamage(AActor* Actor)
{
    if (Actor->Implements<UGenericTeamAgentInterface>())
    {
        IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Actor);
        if (TeamAgent && TeamAgent->GetGenericTeamId() == GetGenericTeamId())
            return;
    }

    switch (GetCurrentState())
    {
        case int32(EAIState::Passive):
            SetStateAsAttacking(Actor, false);
            break;
        case int32(EAIState::Investigating):
            SetStateAsAttacking(Actor, false);
            break;
    }
}

void AAIC_Enemy_Base::HandleForgotActor(AActor* Actor)
{
    KnownSeenActors.Remove(Actor);
    if (Actor == AttackTargetActor)
    {
        SetStateAsPassive();
    }
}

void AAIC_Enemy_Base::HandleLostSight(AActor* Actor)
{
    if (Actor == AttackTargetActor)
    {
        switch (GetCurrentState())
        {
        case int32(EAIState::Attacking):
            SetStateASSeeking(AttackTargetActor->GetActorLocation());
            break;
        case int32(EAIState::Frozen):
            SetStateASSeeking(AttackTargetActor->GetActorLocation());
            break;
        case int32(EAIState::Investigating):
            SetStateASSeeking(AttackTargetActor->GetActorLocation());
            break;
        }
        
    }
}

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
    AActor* NewAttackTarget = (UseLastKnownAttackTarget && IsValid(AttackTarget)) ? AttackTarget : AttackTargetActor;

    if (!IsValid(NewAttackTarget))
    {
        SetStateAsPassive();
        return;
    }

    if (NewAttackTarget->Implements<UDamageableInterface>())
    {
        bool bIsDead = IDamageableInterface::Execute_IsDead(NewAttackTarget);
        if (bIsDead)
            SetStateAsPassive();
        else
        {
            if (UBlackboardComponent* BB = GetBlackboardComponent())
            {
                BB->SetValueAsInt(StateKeyName, (int32)EAIState::Attacking);
                BB->SetValueAsObject(AttackTargetKeyName, NewAttackTarget);
            }

            AttackTargetActor = NewAttackTarget;
        }
    }
}

void AAIC_Enemy_Base::SetStateAsInvestigating(FVector Location)
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Investigating);
        BB->SetValueAsVector(PointOfInterestKeyName, Location);
    }
}

void AAIC_Enemy_Base::SetStateASSeeking(FVector Location)
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Seeking);
        BB->SetValueAsVector(PointOfInterestKeyName, Location);
    }
}

uint8 AAIC_Enemy_Base::GetCurrentState()
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        return uint8(BB->GetValueAsInt(StateKeyName));
    }
    return -1;
}