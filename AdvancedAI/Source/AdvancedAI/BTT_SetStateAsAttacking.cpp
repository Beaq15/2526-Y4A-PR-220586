// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetStateAsAttacking.h"
#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SetStateAsAttacking::UBTT_SetStateAsAttacking()
{
	NodeName = "Set State As Attacking";
}

EBTNodeResult::Type UBTT_SetStateAsAttacking::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetAIOwner());
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BB) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	AIController->SetStateAsAttacking(Target, true);

	return EBTNodeResult::Succeeded;
}
