// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Test.generated.h"

/**
 * 
 */
UCLASS()
class AITEAMCOMMS_API UBTT_Test : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_Test();

	UPROPERTY(EditAnywhere, Category = "Fact")
	FGameplayTag FactToProcess;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
