// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CanSummonMinion.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EnemyBoss.h"

UBTD_CanSummonMinion::UBTD_CanSummonMinion()
{
	NodeName = "Can Summon Minion";
}

bool UBTD_CanSummonMinion::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	AEnemyBoss* Boss = Cast<AEnemyBoss>(AIController->GetPawn());
	if (!Boss) return false;

	return Boss->CanSummonMinion();
}