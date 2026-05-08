// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_WeildSword.h"
#include "EnemyBase.h"
#include "AIController.h"

UBTT_WieldSword::UBTT_WieldSword()
{
	NodeName = "Wield Sword";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_WieldSword::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    CachedOwnerComp = &OwnerComp;

    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    Enemy->OnEquipSwordEnd.AddDynamic(this, &UBTT_WieldSword::OnWieldSwordEnd);
    Enemy->WieldSword();
    return EBTNodeResult::InProgress;
}

void UBTT_WieldSword::OnWieldSwordEnd()
{
    if (CachedOwnerComp)
    {
        AEnemyBase* Enemy = Cast<AEnemyBase>(CachedOwnerComp->GetAIOwner()->GetPawn());
        if (Enemy)
            Enemy->OnEquipSwordEnd.RemoveDynamic(this, &UBTT_WieldSword::OnWieldSwordEnd);

        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    }
}