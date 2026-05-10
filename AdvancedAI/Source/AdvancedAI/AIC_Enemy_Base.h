// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "AIC_Enemy_Base.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API AAIC_Enemy_Base : public AAIController
{
	GENERATED_BODY()
public:
	AAIC_Enemy_Base();

	UFUNCTION()
	void SetStateAsPassive();
	UFUNCTION()
	void SetStateAsAttacking(AActor* AttackTarget);

protected:

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Damage* DamageConfig;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName AttackTargetKeyName = "AttackTarget";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName StateKeyName = "State";


	UFUNCTION()
	void StartBehaviorTree();

	
	
};
