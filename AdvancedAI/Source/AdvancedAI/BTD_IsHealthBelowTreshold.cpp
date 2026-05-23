// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsHealthBelowTreshold.h"
#include "AIController.h"
#include "DamageableInterface.h"

UBTD_IsHealthBelowTreshold::UBTD_IsHealthBelowTreshold()
{
	NodeName = "Is Health Below Treshold";
	bNotifyTick = true;
}

bool UBTD_IsHealthBelowTreshold::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();

	if (Pawn->Implements<UDamageableInterface>())
	{
		float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(Pawn);
		float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(Pawn);

		if (CurrentHealth / MaxHealth <= HealthTreshold)
			return true;
	}

	return false;
}

void UBTD_IsHealthBelowTreshold::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const bool bCurrentValue = CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (bCurrentValue)
		OwnerComp.RequestExecution(this);
}

