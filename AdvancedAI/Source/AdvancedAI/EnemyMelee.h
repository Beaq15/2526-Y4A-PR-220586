// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMelee.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlockEnd);

UENUM(BlueprintType)
enum class EBlockingState : uint8
{
	None,
	Blocking,
	BlockedSuccessfully
};

UCLASS()
class ADVANCEDAI_API AEnemyMelee : public AEnemyBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> EquipSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DropSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwordBlockMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwordBlockHitMontage;

	EBlockingState BlockingState;

	UPROPERTY(BlueprintAssignable)
	FOnBlockEnd OnBlockEnd;

	UFUNCTION()
	void OnSwordBlockMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnBlockHitMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void StartBlock();

	UFUNCTION()
	void EndBlock();

	UFUNCTION()
	void OnBlocked(bool bCanBeParried, AActor* DamageCauser);

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
