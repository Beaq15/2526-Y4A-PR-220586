// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DefaultAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIC_Enemy_Base.h"
#include "EnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_DefaultAttack::UBTT_DefaultAttack()
{
	NodeName = "Default Attack";
}

EBTNodeResult::Type UBTT_DefaultAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)  return EBTNodeResult::Failed;

	AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetAIOwner());

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BB) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	CachedOwnerComp = &OwnerComp;

	Enemy->OnAttackEnd.RemoveDynamic(this, &UBTT_DefaultAttack::OnAttackEnd);
	Enemy->OnAttackEnd.AddUniqueDynamic(this, &UBTT_DefaultAttack::OnAttackEnd);

	if (Enemy->Implements<UEnemyInterface>())
	{
		IEnemyInterface::Execute_Attack(Enemy, Target);
	}
	return EBTNodeResult::InProgress;
}

void UBTT_DefaultAttack::OnAttackEnd()
{
	if (!CachedOwnerComp) return;

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	CachedOwnerComp = nullptr;
}