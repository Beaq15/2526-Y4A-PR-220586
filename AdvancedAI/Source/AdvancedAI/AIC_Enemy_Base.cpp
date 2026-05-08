// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "AdvancedAICharacter.h"

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
