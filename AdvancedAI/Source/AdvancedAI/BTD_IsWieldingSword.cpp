// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsWieldingSword.h"
#include "AIController.h"
#include "EnemyBase.h"

UBTD_IsWieldingSword::UBTD_IsWieldingSword()
{
	NodeName = "Is Wielding Sword";
}

bool UBTD_IsWieldingSword::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy) return false;

	return Enemy->bIsWieldingWeapon;
}
