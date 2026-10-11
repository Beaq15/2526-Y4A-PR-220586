// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_BossHeal.h"
#include "EnemyBoss.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_BossHeal::UBTT_BossHeal()
{
    NodeName = "Boss Heal";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_BossHeal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyBoss* Enemy = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());

    if (!Enemy) return EBTNodeResult::Failed;

    Enemy->HealOverTime();

    CachedOwnerComp = &OwnerComp;
    Enemy->OnHealOverTimeEndCallback = [this]()
        {
            if (CachedOwnerComp)
            {
                if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
                {
                    BB->SetValueAsBool(HasBossHealedKey.SelectedKeyName, true);
                }

                FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
                CachedOwnerComp = nullptr;
            }
        };
    return EBTNodeResult::InProgress;
}
