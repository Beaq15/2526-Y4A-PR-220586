// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Heal.h"
#include "AIController.h"
#include "DamageableInterface.h"

UBTT_Heal::UBTT_Heal()
{
	NodeName = "Heal";
}

EBTNodeResult::Type UBTT_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();


	if (Pawn && Pawn->Implements<UDamageableInterface>())
	{
		const float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(Pawn);
		IDamageableInterface::Execute_Heal(Pawn, MaxHealth * HealPercentage);
	}

	return EBTNodeResult::Succeeded;
}
