// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_BossAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIC_Enemy_Base.h"

UBTT_BossAttack::UBTT_BossAttack()
{
	NodeName = "Boss Attack";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_BossAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetOwner());
	AEnemyBase* ControllerPawn = Cast<AEnemyBase>(AIController->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	AEnemyBoss* Boss = Cast<AEnemyBoss>(ControllerPawn);

	CachedOwnerComp = &OwnerComp;

	AttackBasedOnName(Boss);

	AIController->OnAttackEndDelegate.BindLambda([this]()
		{
			if (CachedOwnerComp)
			{
				FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
			}
		});
	return EBTNodeResult::InProgress;
}

void UBTT_BossAttack::AttackBasedOnName(AEnemyBoss* BossRef)
{
	UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent();
	AActor* AttackTarget = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	switch (AttackName)
	{
	case EBoss_Attacks::Combo1:
		BossRef->AttackCombo1(AttackTarget);
		break;
	}
}
