// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ZombieAttack.h"
#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTT_ZombieAttack::UBTT_ZombieAttack()
{
    NodeName = "Zombie Attack";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_ZombieAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyZombie* ControllerPawn = Cast<AEnemyZombie>(AIController->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!ControllerPawn || !BB) return EBTNodeResult::Failed;

    AActor* AttackTarget = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if (!AttackTarget) return EBTNodeResult::Failed;

    const float AttackRadius = BB->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);

    if (!IEnemyInterface::Execute_DidAttackStart(ControllerPawn, AttackTarget, TokensNeeded))
        return EBTNodeResult::Failed;

    CachedEnemy = ControllerPawn;
    CachedTarget = AttackTarget;
    CachedOwnerComp = &OwnerComp;

    AIController->ClearFocus(EAIFocusPriority::Gameplay);

    EPathFollowingRequestResult::Type RequestResult = AIController->MoveToActor(AttackTarget, AttackRadius);

    if (RequestResult == EPathFollowingRequestResult::Failed)
    {
        IEnemyInterface::Execute_AttackEnd(ControllerPawn, AttackTarget);
        return EBTNodeResult::Failed;
    }
    else if (RequestResult == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        AIController->SetFocus(AttackTarget);

        IEnemyInterface::Execute_Attack(ControllerPawn, AttackTarget);

        AIController->OnAttackEndDelegate.BindLambda([this]()
            {
                if (CachedOwnerComp)
                {
                    FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
                }
            });
    }
    else
    {
        AIController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UBTT_ZombieAttack::OnMoveCompleted);
    }

    return EBTNodeResult::InProgress;
}

void UBTT_ZombieAttack::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
    if (!CachedEnemy || !CachedTarget) return;

    AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(CachedEnemy->GetController());
    if (!AIController) return;

    AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTT_ZombieAttack::OnMoveCompleted);

    if (Result != EPathFollowingResult::Success)
    {
        IEnemyInterface::Execute_AttackEnd(CachedEnemy, CachedTarget);
        if (CachedOwnerComp)
            FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
        CachedOwnerComp = nullptr;
        return;
    }

    AIController->SetFocus(CachedTarget);

    AIController->OnAttackEndDelegate.BindLambda([this]()
        {
            if (CachedOwnerComp)
            {
                FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
            }
        });

    IEnemyInterface::Execute_Attack(CachedEnemy, CachedTarget);
}