// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_EquipWeapon.h"
#include "EnemyBase.h"
#include "AIController.h"

UBTT_EquipWeapon::UBTT_EquipWeapon()
{
	NodeName = "Equip Weapon";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_EquipWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    CachedOwnerComp = &OwnerComp;

    Enemy->OnEquipWeaponEnd.AddUniqueDynamic(this, &UBTT_EquipWeapon::OnEquipWeaponEnd);
    IEnemyInterface::Execute_EquipWeapon(Enemy);
    return EBTNodeResult::InProgress;
}

void UBTT_EquipWeapon::OnEquipWeaponEnd()
{
    if (!CachedOwnerComp) return;

    AEnemyBase* Enemy = Cast<AEnemyBase>(CachedOwnerComp->GetAIOwner()->GetPawn());
    if (Enemy)
        Enemy->OnEquipWeaponEnd.RemoveDynamic(this, &UBTT_EquipWeapon::OnEquipWeaponEnd);

    FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    CachedOwnerComp = nullptr;
}