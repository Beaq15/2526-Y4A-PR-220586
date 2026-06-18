// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_RangedAttack.h"
#include "AIC_Enemy_Base.h"
#include "EnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_RangedAttack::UBTT_RangedAttack()
{
	NodeName = "Ranged Attack";
}

EBTNodeResult::Type UBTT_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetOwner());
	AEnemyBase* ControllerPawn = Cast<AEnemyBase>(AIController->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIController || !ControllerPawn || !BB) return EBTNodeResult::Failed;

	AActor* AttackTarget = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	if (!AttackTarget) return EBTNodeResult::Failed;

	if (!IEnemyInterface::Execute_DidAttackStart(ControllerPawn, AttackTarget, TokensNeeded))
		return EBTNodeResult::Failed;

	AIController->OnAttackEndDelegate.BindLambda([this, &OwnerComp]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		});

	AIController->SetFocus(AttackTarget);

	IEnemyInterface::Execute_Attack(ControllerPawn, AttackTarget);
	

	return EBTNodeResult::InProgress;
}
