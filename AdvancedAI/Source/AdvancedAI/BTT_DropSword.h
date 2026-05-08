// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DropSword.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_DropSword : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_DropSword();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnSwordDropEnd();

	UBehaviorTreeComponent* CachedOwnerComp = nullptr;
	
};
