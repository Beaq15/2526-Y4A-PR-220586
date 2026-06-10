// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DefaultAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "EnemyBase.h"

UBTT_DefaultAttack::UBTT_DefaultAttack()
{
	NodeName = "Default Attack";
}

EBTNodeResult::Type UBTT_DefaultAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)  return EBTNodeResult::Failed;

	CachedOwnerComp = &OwnerComp;

	Enemy->OnAttackEnd.RemoveDynamic(this, &UBTT_DefaultAttack::OnAttackEnd);
	Enemy->OnAttackEnd.AddUniqueDynamic(this, &UBTT_DefaultAttack::OnAttackEnd);

	Enemy->Attack();
	return EBTNodeResult::InProgress;
}

void UBTT_DefaultAttack::OnAttackEnd()
{
	if (!CachedOwnerComp) return;

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	CachedOwnerComp = nullptr;
}