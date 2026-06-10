// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_HasPatrolRoute.h"
#include "AIController.h"
#include "EnemyInterface.h"

UBTD_HasPatrolRoute::UBTD_HasPatrolRoute()
{
	NodeName = "Has Patrol Route";
}

bool UBTD_HasPatrolRoute::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController->GetPawn();

	if ( Pawn && Pawn->Implements<UEnemyInterface>())
	{
		APatrolRoute* Route = IEnemyInterface::Execute_GetPatrolRoute(Pawn);
		return IsValid(Route);
	}

	return false;
}
