// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_EquipWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_EquipWeapon : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_EquipWeapon();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnEquipWeaponEnd();

	UBehaviorTreeComponent* CachedOwnerComp = nullptr;
	
};
