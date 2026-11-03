// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CastHeal.h"
#include "EnemyMage.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTT_CastHeal::UBTT_CastHeal()
{
    NodeName = "Cast Heal";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_CastHeal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    AEnemyMage* Mage = AIC ? Cast<AEnemyMage>(AIC->GetPawn()) : nullptr;
    if (!Mage) return EBTNodeResult::Failed;

    Mage->HealOverTime();

    CachedOwnerComp = &OwnerComp;
    Mage->OnHealOverTimeEndCallback = [this]()
        {
            if (CachedOwnerComp.IsValid())
            {
                FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
                CachedOwnerComp = nullptr;
            }
        };

    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTT_CastHeal::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (AEnemyMage* Mage = AIC ? Cast<AEnemyMage>(AIC->GetPawn()) : nullptr)
    {
        Mage->OnHealOverTimeEndCallback = nullptr;
        Mage->StopAnimMontage();
    }

    CachedOwnerComp = nullptr;
    return EBTNodeResult::Aborted;
}