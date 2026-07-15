// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "AOE_Heal.h"
#include "EnemyMage.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealOverTimeEnd);

UCLASS()
class ADVANCEDAI_API AEnemyMage : public AEnemyBase
{
	GENERATED_BODY()

	//----------------------------------------------------------------------
	// Private — Animation Assets
	//----------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HealMontage;

	//----------------------------------------------------------------------
	// Private — Animation Callbacks
	//----------------------------------------------------------------------
	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
	TObjectPtr<AActor> CachedAttackTarget;

	//----------------------------------------------------------------------
	// Private — Teleport
	//----------------------------------------------------------------------
	

	UFUNCTION()
	void TeleportEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> P_GideonBurde;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> P_GideonMeteor;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> TeleportBodyEffect;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> TeleportTrailEffect;

	FTimerHandle TeleportMoveTimerHandle;

	UPROPERTY()
	FVector CachedTeleportLocation;

	//----------------------------------------------------------------------
	// Private — Heal
	//----------------------------------------------------------------------

	UFUNCTION()
	void HealEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHealTick(AActor* Actor);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorToSpawn;
	
	UPROPERTY()
	TObjectPtr<AAOE_Heal> HealAOE;

protected:
	//----------------------------------------------------------------------
	// Protected — Lifecycle
	//----------------------------------------------------------------------

	virtual void BeginPlay() override;

	AEnemyMage();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//----------------------------------------------------------------------
	// Public — IEnemyInterface
	//----------------------------------------------------------------------

	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual void GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;
	virtual void Attack_Implementation(AActor* AttackTarget) override;

	UFUNCTION()
	void Teleport(FVector Location);
	TFunction<void()> OnTeleportEndCallback;

	UFUNCTION()
	void HealOverTime();

	UPROPERTY(BlueprintAssignable)
	FOnHealOverTimeEnd OnHealOverTimeEnd;

	TFunction<void()> OnHealOverTimeEndCallback;
};
