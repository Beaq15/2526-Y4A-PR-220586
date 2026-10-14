// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SummonMinions.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyBoss.h"

UBTT_SummonMinions::UBTT_SummonMinions()
{
	NodeName = "Summon Minions";
}

EBTNodeResult::Type UBTT_SummonMinions::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Boss) return EBTNodeResult::Failed;

	if (!Boss->CanSummonMinion()) return EBTNodeResult::Failed;

	const FVector Location = OwnerComp.GetBlackboardComponent()->GetValueAsVector(SummonPointKey.SelectedKeyName);
	Boss->SpawnMinions(Location);

	return EBTNodeResult::Succeeded;
}
