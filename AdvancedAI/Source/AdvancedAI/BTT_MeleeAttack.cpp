#include "BTT_MeleeAttack.h"
#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyBase.h"

UBTT_MeleeAttack::UBTT_MeleeAttack()
{
    NodeName = "Melee Attack";
}

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetAIOwner());
    AEnemyBase* Enemy = Cast<AEnemyBase>(AIController->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AIController || !Enemy || !BB) return EBTNodeResult::Failed;
    if (!Enemy->Implements<UEnemyInterface>()) return EBTNodeResult::Failed;

    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if (!TargetActor) return EBTNodeResult::Failed;

    const float AcceptanceRadius = BB->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);

    if (!IEnemyInterface::Execute_DidAttackStart(Enemy, TargetActor, TokensNeeded))
        return EBTNodeResult::Failed;

    Enemy->bIsStrafing = false;

    CachedOwnerComp = &OwnerComp;
    CachedEnemy = Enemy;
    CachedTarget = TargetActor;

    AIController->OnAttackEndDelegate.BindLambda([this, &OwnerComp]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        });

    IEnemyInterface::Execute_SetMovementSpeed(Enemy, EMovementSpeed::Sprinting);
    AIController->ClearFocus(EAIFocusPriority::Gameplay);

    EPathFollowingRequestResult::Type RequestResult = AIController->MoveToActor(TargetActor, AcceptanceRadius);
    if (RequestResult == EPathFollowingRequestResult::Failed)
    {
        IEnemyInterface::Execute_AttackEnd(Enemy, TargetActor);
        return EBTNodeResult::Failed;
    }

    AIController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UBTT_MeleeAttack::OnMoveCompleted);
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
    IEnemyInterface::Execute_Attack(CachedEnemy, CachedTarget);
}