// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_UnequipWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_UnequipWeapon : public UBTTaskNode
{
	GENERATED_BODY()

	UBehaviorTreeComponent* CachedOwnerComp = nullptr;

	UFUNCTION()
	void OnUnequipWeaponEnd();

public:
	UBTT_UnequipWeapon();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
