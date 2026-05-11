// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetStateAsPassive.h"
#include "AIC_Enemy_Base.h"

UBTT_SetStateAsPassive::UBTT_SetStateAsPassive()
{
	NodeName = "SetStateAsPassive";
}

EBTNodeResult::Type UBTT_SetStateAsPassive::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetAIOwner());

	AIController->SetStateAsPassive();
	
	return EBTNodeResult::Succeeded;
}
