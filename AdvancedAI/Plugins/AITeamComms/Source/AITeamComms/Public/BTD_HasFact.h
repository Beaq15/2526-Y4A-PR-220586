// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"
#include "BTD_HasFact.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Has Fact"))
class AITEAMCOMMS_API UBTD_HasFact : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTD_HasFact();

	UPROPERTY(EditAnywhere, Category = "Fact", meta = (Categories = "Fact"))
	FGameplayTag RequiredFact;

	UPROPERTY(EditAnywhere, Category = "Fact", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinimumConfidence = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Fact")
	bool bUseBlackboard = true;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;

private:
	bool CheckBlackboard(UBehaviorTreeComponent& OwnerComp) const;
	bool CheckKnowledgeComponent(UBehaviorTreeComponent& OwnerComp) const;
	FName GetBlackboardKeyName(const FGameplayTag& FactTag, const FString& Suffix) const;
}; 

