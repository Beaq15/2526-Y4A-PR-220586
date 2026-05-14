// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToIdealRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MoveToIdealRange::UBTT_MoveToIdealRange()
{
	NodeName = "MoveToIdealRange";
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_MoveToIdealRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BB)
		return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	float AcceptanceRadius = BB->GetValueAsFloat(IdealRangeKey.SelectedKeyName);

	if (!TargetActor)
		return EBTNodeResult::Failed;

	AIController->MoveToActor(TargetActor, AcceptanceRadius);
	return EBTNodeResult::InProgress;

	/*AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BB)
		return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	CachedAcceptanceRadius = BB->GetValueAsFloat(IdealRangeKey.SelectedKeyName);

	if (!TargetActor)
		return EBTNodeResult::Failed;

	CachedTargetLocation = TargetActor->GetActorLocation();

	bIsAborting = false;

	APawn* Pawn = AIController->GetPawn();
	float Distance = FVector::Dist(Pawn->GetActorLocation(), CachedTargetLocation);
	if (Distance <= CachedAcceptanceRadius)
		return EBTNodeResult::Succeeded;

	AIController->MoveToActor(TargetActor, CachedAcceptanceRadius);


	return EBTNodeResult::InProgress;*/
}

void UBTT_MoveToIdealRange::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Always succeed when move finishes, regardless of distance
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTT_MoveToIdealRange::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
		AIController->StopMovement();
	return EBTNodeResult::Aborted;
}

