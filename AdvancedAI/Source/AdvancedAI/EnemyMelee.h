// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Components/TimelineComponent.h"
#include "AOE_Base.h"
#include "EnemyMelee.generated.h"


UENUM(BlueprintType)
enum class EMelee_Attacks : uint8
{
	Default,
	ShortRangeAttack,
	LongRangeAttack,
	SpinningAttack,
	GroundSmashAttack
};

UCLASS()
class ADVANCEDAI_API AEnemyMelee : public AEnemyBase
{
	GENERATED_BODY()

	//----------------------------------------------------------------------
	// Private — Animation Assets
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> EquipSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DropSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwordBlockMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwordJumpAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SpinningAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> GroundSmashMontage;

	//----------------------------------------------------------------------
	// Private — Animation Callbacks
	//----------------------------------------------------------------------

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	//----------------------------------------------------------------------
	// Private — Block Helpers
	//----------------------------------------------------------------------

	UPROPERTY()
	TObjectPtr<AActor> CachedAttackTarget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY()
	TObjectPtr<AAOE_Base> AOE;

	UFUNCTION()
	void AOEDamageActor(AActor* Actor);

public:
	// ----------------------------------------------------------------------
	// Public — Lifecycle
	//----------------------------------------------------------------------

	virtual void BeginPlay() override;
	AEnemyMelee();
	

	//----------------------------------------------------------------------
	// Public — Combat API
	//----------------------------------------------------------------------
	
	void ShortRangeAttack(AActor* AttackTarget);
	void LongRangeAttack(AActor* AttackTarget);
	void SpinningAttack(AActor* AttackTarget);
	void GroundSmashAttack(AActor* AttackTarget);

	//----------------------------------------------------------------------
	// Public — IEnemyInterface
	//----------------------------------------------------------------------

	virtual void EquipWeapon_Implementation()   override;
	virtual void UnequipWeapon_Implementation() override;
	virtual void  GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;
	virtual void Attack_Implementation(AActor* AttackTarget) override;
};
