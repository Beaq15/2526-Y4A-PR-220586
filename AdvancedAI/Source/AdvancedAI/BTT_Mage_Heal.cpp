// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Mage_Heal.h"
#include "EnemyMage.h"
#include "AIController.h"

UBTT_Mage_Heal::UBTT_Mage_Heal()
{
	NodeName = "Mage Heal";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_Mage_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyMage* Enemy = Cast<AEnemyMage>(OwnerComp.GetAIOwner()->GetPawn());

    if (!Enemy) return EBTNodeResult::Failed;

	Enemy->HealOverTime();

    CachedOwnerComp = &OwnerComp;
    Enemy->OnHealOverTimeEndCallback = [this]()
        {
            if (CachedOwnerComp)
            {
                FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
                CachedOwnerComp = nullptr;
            }
        };
	return EBTNodeResult::InProgress;
}
