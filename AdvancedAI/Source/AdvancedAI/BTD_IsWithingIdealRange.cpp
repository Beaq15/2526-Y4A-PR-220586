// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsWithingIdealRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTD_IsWithingIdealRange::UBTD_IsWithingIdealRange()
{
	NodeName = "IsWithinIdealRange";
	bNotifyTick = true;
}

bool UBTD_IsWithingIdealRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();

	float ErrorMargin = 50.f;
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	float Distance = Target->GetDistanceTo(Pawn);
	float range = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);

	return (Distance - ErrorMargin) <= range;
}

void UBTD_IsWithingIdealRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const bool bCurrentValue = CalculateRawConditionValue(OwnerComp, NodeMemory);
	
	if (!bCurrentValue)
		OwnerComp.RequestExecution(this);
}
