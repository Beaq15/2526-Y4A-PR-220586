// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveAlongPatrolRoute.h"
#include "EnemyInterface.h"
#include "PatrolRoute.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UBTT_MoveAlongPatrolRoute::UBTT_MoveAlongPatrolRoute()
{
	NodeName = "Move Along Patrol Route";

	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_MoveAlongPatrolRoute::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController->GetPawn();

	if (Pawn->Implements<UEnemyInterface>())
	{
		APatrolRoute* Route = IEnemyInterface::Execute_GetPatrolRoute(Pawn);
		if (IsValid(Route))
		{
			CurrentTargetLocation = Route->GetSplinePointAsWorldPosition();
			AIController->MoveToLocation(CurrentTargetLocation, AcceptanceRadius);
			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}
void UBTT_MoveAlongPatrolRoute::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController->GetPawn();

	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		float Distance = FVector::Dist(Pawn->GetActorLocation(), CurrentTargetLocation);
		bool bReachedTarget = Distance <= AcceptanceRadius;
		if (bReachedTarget)
		{
			if (Pawn->Implements<UEnemyInterface>())
			{
				APatrolRoute* Route = IEnemyInterface::Execute_GetPatrolRoute(Pawn);
				if (IsValid(Route))
					Route->IncrementPatrolRoute();
			}
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
		
	}
}

EBTNodeResult::Type UBTT_MoveAlongPatrolRoute::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	AIController->StopMovement();

	return EBTNodeResult::Aborted;
}
