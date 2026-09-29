// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Components/TimelineComponent.h"
#include "AOE_Base.h"
#include "EnemyMelee.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlockEnd);

UENUM(BlueprintType)
enum class EBlockingState : uint8
{
	None,
	Blocking,
	BlockedSuccessfully
};

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
	TObjectPtr<UAnimMontage> SwordBlockHitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwordJumpAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SpinningAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> GroundSmashMontage;

	// ----------------------------------------------------------------------
	// Private — State
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	EBlockingState BlockingState = EBlockingState::None;

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
	void OnBlockHitMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	//----------------------------------------------------------------------
	// Private — Block Helpers
	//----------------------------------------------------------------------

	UPROPERTY()
	TObjectPtr<AActor> CachedAttackTarget;

	void EndBlock();
	void TryToBlock();
	UFUNCTION()
	void OnBlocked(bool bCanBeParried, AActor* DamageCauser);

	UPROPERTY(EditAnywhere, Category = "Combat")
	float BlockChance = 0.5f;

	FTimerHandle HoldBlockTimer;

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
	// Public — Delegates
	//----------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable)
	FOnBlockEnd OnBlockEnd;

	//----------------------------------------------------------------------
	// Public — Combat API
	//----------------------------------------------------------------------

	void StartBlock();
	
	void ShortRangeAttack(AActor* AttackTarget);
	void LongRangeAttack(AActor* AttackTarget);
	void SpinningAttack(AActor* AttackTarget);
	void GroundSmashAttack(AActor* AttackTarget);

	//----------------------------------------------------------------------
	// Public — IEnemyInterface
	//----------------------------------------------------------------------

	virtual void EquipWeapon_Implementation()   override;
	virtual void UnequipWeapon_Implementation() override;
	virtual bool  TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser) override;
	virtual void  GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;
	virtual void Attack_Implementation(AActor* AttackTarget) override;
};
