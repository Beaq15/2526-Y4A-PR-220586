// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MageAttack.h"
#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MageAttack::UBTT_MageAttack()
{
	NodeName = "Mage Attack";
}

EBTNodeResult::Type UBTT_MageAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetOwner());
	AEnemyBase* ControllerPawn = Cast<AEnemyBase>(AIController->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIController || !ControllerPawn || !BB) return EBTNodeResult::Failed;

	AActor* AttackTarget = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	if (!AttackTarget) return EBTNodeResult::Failed;

	if (!IEnemyInterface::Execute_DidAttackStart(ControllerPawn, AttackTarget, TokensNeeded))
		return EBTNodeResult::Failed;

	AIController->SetFocus(AttackTarget);

	AEnemyMage* Mage = Cast<AEnemyMage>(ControllerPawn);
	if (!Mage) return EBTNodeResult::Failed;

	CachedOwnerComp = &OwnerComp;

	auto RunAttack = [this, Mage, AttackTarget]()
		{
			switch (AttackName)
			{
			case EMage_Attacks::GroundSmashAttack:
				Mage->GroundSmashAttack(AttackTarget);
				break;
			case EMage_Attacks::BasicAttack:
				IEnemyInterface::Execute_Attack(Mage, AttackTarget);
				break;
			default:
				IEnemyInterface::Execute_Attack(Mage, AttackTarget);
				break;
			}
		};

	AIController->OnAttackEndDelegate.BindLambda([this]()
		{
			if (CachedOwnerComp)
			{
				FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
			}
		});

	if (bShouldTeleport)
	{
		Mage->OnTeleportEndCallback = [RunAttack]()
			{
				RunAttack();
			};

		Mage->Teleport(BB->GetValueAsVector(TeleportLocationKey.SelectedKeyName));
	}
	else
	{
		RunAttack();
	}

	return EBTNodeResult::InProgress;
}
