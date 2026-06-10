// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetMovementSpeed.h"
#include "EnemyInterface.h"
#include "AIController.h"

UBTT_SetMovementSpeed::UBTT_SetMovementSpeed()
{
	NodeName = "Set Movement Speed";
}

EBTNodeResult::Type UBTT_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController->GetPawn();

	if (Pawn && Pawn->Implements<UEnemyInterface>())
		IEnemyInterface::Execute_SetMovementSpeed(Pawn, MovementSpeed);

	return EBTNodeResult::Succeeded;
}
