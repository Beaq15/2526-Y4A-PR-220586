// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMage.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API AEnemyMage : public AEnemyBase
{
	GENERATED_BODY()

	//----------------------------------------------------------------------
	// Private — Animation Assets
	//----------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FireMontage;

	//----------------------------------------------------------------------
	// Private — Animation Callbacks
	//----------------------------------------------------------------------
	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

public:

	//----------------------------------------------------------------------
	// Public — IEnemyInterface
	//----------------------------------------------------------------------

	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual void GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;
	virtual void Attack_Implementation(AActor* AttackTarget) override;
};
