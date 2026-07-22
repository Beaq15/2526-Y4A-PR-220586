#include "BTT_MeleeAttack.h"
#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyMelee.h"

UBTT_MeleeAttack::UBTT_MeleeAttack()
{
    NodeName = "Melee Attack";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetAIOwner());
    AEnemyMelee* ControllerPawn = Cast<AEnemyMelee>(AIController->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AIController || !ControllerPawn || !BB) return EBTNodeResult::Failed;

    AActor* AttackTarget = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if (!AttackTarget) return EBTNodeResult::Failed;

    const float AttackRadius = BB->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);

    if (!IEnemyInterface::Execute_DidAttackStart(ControllerPawn, AttackTarget, TokensNeeded))
        return EBTNodeResult::Failed;

    CachedEnemy = ControllerPawn;
    CachedTarget = AttackTarget;
    CachedOwnerComp = &OwnerComp;

    IEnemyInterface::Execute_SetMovementSpeed(ControllerPawn, EMovementSpeed::Sprinting);
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

        switch (AttackName)
        {
        case EMelee_Attacks::ShortRangeAttack:
            ControllerPawn->ShortRangeAttack(AttackTarget);
            break;
        case EMelee_Attacks::LongRangeAttack:
            ControllerPawn->LongRangeAttack(AttackTarget);
            break;
        default:
            IEnemyInterface::Execute_Attack(ControllerPawn, AttackTarget);
            break;
        }


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
        AIController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UBTT_MeleeAttack::OnMoveCompleted);
    }

    return EBTNodeResult::InProgress;
}

void UBTT_MeleeAttack::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
    if (!CachedEnemy || !CachedTarget) return;

    AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(CachedEnemy->GetController());
    if (AIController)
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTT_MeleeAttack::OnMoveCompleted);

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