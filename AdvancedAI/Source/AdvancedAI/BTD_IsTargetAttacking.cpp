// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsTargetAttacking.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DamageableInterface.h"

UBTD_IsTargetAttacking::UBTD_IsTargetAttacking()
{
	NodeName = "Is Target Attacking";
	bNotifyTick = true;
}

bool UBTD_IsTargetAttacking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AActor* Actor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	if (Actor && Actor->Implements<UDamageableInterface>())
	{
		if (IDamageableInterface::Execute_IsAttacking(Actor))
			return true;
	}
	return false;
}

void UBTD_IsTargetAttacking::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (CalculateRawConditionValue(OwnerComp, NodeMemory))
		OwnerComp.RequestExecution(this);
}
