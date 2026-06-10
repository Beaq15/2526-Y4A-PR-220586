// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetStateAsPassive.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_SetStateAsPassive : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_SetStateAsPassive();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
