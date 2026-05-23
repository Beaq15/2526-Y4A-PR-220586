// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMelee.generated.h"

UCLASS()
class ADVANCEDAI_API AEnemyMelee : public AEnemyBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> EquipSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DropSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	virtual void Attack() override;

	UFUNCTION()
	void OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);


	UFUNCTION()
	void OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	virtual void EquipWeapon_Implementation() override;
	virtual void UnequipWeapon_Implementation() override;	
};
