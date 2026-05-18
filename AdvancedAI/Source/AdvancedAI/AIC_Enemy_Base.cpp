// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "AdvancedAICharacter.h"

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

    AIPerception->OnPerceptionUpdated.AddDynamic(this, &AAIC_Enemy_Base::OnPerceptionUpdated);
}
void AAIC_Enemy_Base::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
    if (!Enemy)
        return;

    FTimerHandle BTStartTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(BTStartTimerHandle, [this, Enemy]() {StartBehaviorTree(Enemy); }, 0.2f, false);
    
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

        AAdvancedAICharacter* Player = Cast<AAdvancedAICharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        BB->SetValueAsObject(AttackTargetKeyName, Player);
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
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Sensed Sight"));
            HandleSensedSight(Actor);
        }
        FAIStimulus OutStimulus;
        if (CanSenseActor(Actor, EAISense::Hearing, OutStimulus))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Sensed Sound"));
            HandleSensedSound(OutStimulus.StimulusLocation);
        }
        if (CanSenseActor(Actor, EAISense::Damage))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Sensed Damage"));
            HandleSensedDamage(Actor);
        }
    }
}

void AAIC_Enemy_Base::HandleSensedSight(AActor* Actor)
{
    switch (GetCurrentState())
    {
    case int32(EAIState::Passive):
        SetStateAsAttacking(Actor);
        break;
    case int32(EAIState::Investigating):
        SetStateAsAttacking(Actor);
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
    switch (GetCurrentState())
    {
        case int32(EAIState::Passive):
            SetStateAsAttacking(Actor);
            break;
        case int32(EAIState::Investigating):
            SetStateAsAttacking(Actor);
            break;
    }
}

void AAIC_Enemy_Base::SetStateAsPassive()
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Passive);
}

void AAIC_Enemy_Base::SetStateAsAttacking(AActor* AttackTarget)
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Attacking);
        BB->SetValueAsObject(AttackTargetKeyName, AttackTarget);
    }

    AttackTargetActor = AttackTarget;
}

void AAIC_Enemy_Base::SetStateAsInvestigating(FVector Location)
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsInt(StateKeyName, (int32)EAIState::Investigating);
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