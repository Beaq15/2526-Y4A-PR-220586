// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Mage_Teleport.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_Mage_Teleport : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector TeleportLocation;

	UBehaviorTreeComponent* CachedOwnerComp;

public:
	UBTT_Mage_Teleport();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
