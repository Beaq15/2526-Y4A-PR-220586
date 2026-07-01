// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Mage_Teleport.h"
#include "EnemyMage.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Mage_Teleport::UBTT_Mage_Teleport()
{
	NodeName = "MageTeleport";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_Mage_Teleport::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyMage* Enemy = Cast<AEnemyMage>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    if (!Enemy) return EBTNodeResult::Failed;

    FVector TargetLocation = BB->GetValueAsVector(TeleportLocation.SelectedKeyName);

    CachedOwnerComp = &OwnerComp;
    Enemy->OnTeleportEndCallback = [this]()
        {
            if (CachedOwnerComp)
            {
                FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
                CachedOwnerComp = nullptr;
            }
        };
    Enemy->Teleport(TargetLocation);

    return EBTNodeResult::InProgress;
}