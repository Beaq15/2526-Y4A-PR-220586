// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DropSword.h"
#include "EnemyBase.h"
#include "AIController.h"

UBTT_DropSword::UBTT_DropSword()
{
	NodeName = "Drop Sword";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_DropSword::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)  return EBTNodeResult::Failed;

	Enemy->OnDropSwordEnd.AddDynamic(this, &UBTT_DropSword::OnSwordDropEnd);
	Enemy->DropSword();
	return EBTNodeResult::InProgress;
}

void UBTT_DropSword::OnSwordDropEnd()
{
	if (CachedOwnerComp)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(CachedOwnerComp->GetAIOwner()->GetPawn());
		if (Enemy)
			Enemy->OnDropSwordEnd.RemoveDynamic(this, &UBTT_DropSword::OnSwordDropEnd);

		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}
