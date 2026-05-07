// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIC_Enemy_Base.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API AAIC_Enemy_Base : public AAIController
{
	GENERATED_BODY()

protected:

	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName AttackTargetKeyName = "AttackTarget";

	UFUNCTION()
	void StartBehaviorTree();
	
};
