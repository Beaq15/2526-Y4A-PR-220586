// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsWithingIdealRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTD_IsWithingIdealRange::UBTD_IsWithingIdealRange()
{
	NodeName = "Is Within Ideal Range";
	bNotifyTick = true;
}

bool UBTD_IsWithingIdealRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	if (!Pawn || !Target) return false;

	const float Distance = Target->GetDistanceTo(Pawn);
	const float IdealRange = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);

	return (Distance - ErrorMargin) <= IdealRange;
}

void UBTD_IsWithingIdealRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!CalculateRawConditionValue(OwnerComp, NodeMemory))
		OwnerComp.RequestExecution(this);
}
