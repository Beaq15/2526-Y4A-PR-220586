// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_BossTeleport.h"
#include "EnemyBoss.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_BossTeleport::UBTT_BossTeleport()
{
    NodeName = "Boss Teleport";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_BossTeleport::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyBoss* Enemy = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    if (!Enemy) return EBTNodeResult::Failed;

    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(LocationOrAttackTargetKey.SelectedKeyName));

    FVector TargetLocation = BB->GetValueAsVector(LocationOrAttackTargetKey.SelectedKeyName);

    CachedOwnerComp = &OwnerComp;
    Enemy->OnTeleportEndCallback = [this]()
        {
            if (CachedOwnerComp)
            {
                FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
                CachedOwnerComp = nullptr;
            }
        };

    if (TargetActor)
        Enemy->Teleport(FVector{ 0.0f }, TargetActor);
    else
        Enemy->Teleport(TargetLocation, nullptr);

    return EBTNodeResult::InProgress;
}
