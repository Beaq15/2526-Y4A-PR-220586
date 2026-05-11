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
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 500.f;
    HearingConfig->SetMaxAge(3.f);

    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

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

    FTimerHandle BTStartTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(BTStartTimerHandle, this, &AAIC_Enemy_Base::StartBehaviorTree, 0.2f, false);
    SetStateAsPassive();
}

void AAIC_Enemy_Base::StartBehaviorTree()
{
    if (!BehaviorTreeAsset) return;

    RunBehaviorTree(BehaviorTreeAsset);
    AAdvancedAICharacter* Player = Cast<AAdvancedAICharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsObject(AttackTargetKeyName, Player);
    }

}

bool AAIC_Enemy_Base::CanSenseACtor(AActor* Actor, EAISense Sense) // + outsense
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
            return true;

    }
    return false;
}

void AAIC_Enemy_Base::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        if (CanSenseACtor(Actor, EAISense::Sight))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Sensed Sight"));
        }
        if (CanSenseACtor(Actor, EAISense::Hearing))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Sensed Sound"));
        }
        if (CanSenseACtor(Actor, EAISense::Damage))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Sensed Damage"));
        }
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
}
