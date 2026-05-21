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

public:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage>FireRifleMontage;

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);
	
protected:

	virtual void EquipWeapon_Implementation() override;

	virtual void BeginPlay() override;

	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;

	virtual void GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;
	
	virtual void Attack() override;
};
