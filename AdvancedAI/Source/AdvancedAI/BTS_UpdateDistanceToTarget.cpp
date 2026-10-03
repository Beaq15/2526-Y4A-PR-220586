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
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB) return;

	APawn* Pawn = AIC->GetPawn();
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	if (!IsValid(Pawn) || !IsValid(Target)) return;

	const float Distance = FVector::Distance(Pawn->GetActorLocation(), Target->GetActorLocation());

	BB->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
}
