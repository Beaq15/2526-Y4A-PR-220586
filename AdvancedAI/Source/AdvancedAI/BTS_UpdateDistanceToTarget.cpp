// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_UpdateDistanceToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTS_UpdateDistanceToTarget::UBTS_UpdateDistanceToTarget()
{
	NodeName = "Update Distance To Target";
	Interval = 1.0f;
	RandomDeviation = 0.f;
}

void UBTS_UpdateDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	const float Distance = FVector::Distance(Pawn->GetActorLocation(), Target->GetActorLocation());

	BB->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
}
