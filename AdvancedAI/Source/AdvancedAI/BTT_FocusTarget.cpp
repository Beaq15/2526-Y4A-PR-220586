// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FocusTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTT_FocusTarget::UBTT_FocusTarget()
{
	NodeName = "Focus Target";
}

EBTNodeResult::Type UBTT_FocusTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	if (!Target) return EBTNodeResult::Failed;

	AIController->SetFocus(Target);

	return EBTNodeResult::Succeeded;
}
