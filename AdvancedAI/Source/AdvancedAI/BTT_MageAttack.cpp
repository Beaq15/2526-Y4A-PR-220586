// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MageAttack.h"
#include "AIC_Enemy_Base.h"
#include "EnemyMage.h"
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

	Mage->OnTeleportEndCallback = [this, &OwnerComp, AIController, ControllerPawn, AttackTarget]()
		{
			AIController->OnAttackEndDelegate.BindLambda([this, &OwnerComp]()
				{
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				});

			IEnemyInterface::Execute_Attack(ControllerPawn, AttackTarget);
		};

	Mage->Teleport(BB->GetValueAsVector(TeleportLocationKey.SelectedKeyName));

	return EBTNodeResult::InProgress;
}
