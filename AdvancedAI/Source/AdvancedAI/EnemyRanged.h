// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyRanged.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API AEnemyRanged : public AEnemyBase
{
	GENERATED_BODY()

	//----------------------------------------------------------------------
	// Private — Animation Assets
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage>FireRifleMontage;

	//----------------------------------------------------------------------
	// Private — Animation Callbacks
	//----------------------------------------------------------------------

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

protected:

	//----------------------------------------------------------------------
	// Protected — Lifecycle
	//----------------------------------------------------------------------

	virtual void BeginPlay() override;

	//----------------------------------------------------------------------
	// Protected — IEnemyInterface
	//----------------------------------------------------------------------

	virtual void  EquipWeapon_Implementation() override;
	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual void  GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;

	//----------------------------------------------------------------------
	// Protected — Combat
	//----------------------------------------------------------------------

	virtual void Attack() override;
};
