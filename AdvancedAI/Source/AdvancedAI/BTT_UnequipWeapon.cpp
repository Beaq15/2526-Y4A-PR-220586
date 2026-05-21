// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_UnequipWeapon.h"
#include "EnemyBase.h"
#include "AIController.h"

UBTT_UnequipWeapon::UBTT_UnequipWeapon()
{
	NodeName = "Unequip Weapon";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_UnequipWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)  return EBTNodeResult::Failed;

	Enemy->OnDropWeaponEnd.AddDynamic(this, &UBTT_UnequipWeapon::OnUnequipWeaponEnd);
	IEnemyInterface::Execute_UnequipWeapon(Enemy);
	return EBTNodeResult::InProgress;
}

void UBTT_UnequipWeapon::OnUnequipWeaponEnd()
{
	if (CachedOwnerComp)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(CachedOwnerComp->GetAIOwner()->GetPawn());
		if (Enemy)
			Enemy->OnDropWeaponEnd.RemoveDynamic(this, &UBTT_UnequipWeapon::OnUnequipWeaponEnd);

		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}
