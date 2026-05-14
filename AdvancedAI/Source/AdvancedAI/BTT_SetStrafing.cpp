// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetStrafing.h"
#include "AIController.h"
#include "EnemyBase.h"

UBTT_SetStrafing::UBTT_SetStrafing()
{
	NodeName = "SetStrafing";
}

EBTNodeResult::Type UBTT_SetStrafing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	AEnemyBase* Enemy = Cast<AEnemyBase>(AIController->GetPawn());

	if (Enemy)
		Enemy->bIsStrafing = bStrafingValue;
	return EBTNodeResult::Succeeded;
}
