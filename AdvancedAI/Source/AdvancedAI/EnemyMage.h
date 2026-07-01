// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMage.generated.h"

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

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

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

	

protected:
	//----------------------------------------------------------------------
	// Protected — Lifecycle
	//----------------------------------------------------------------------

	virtual void BeginPlay() override;

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
};
