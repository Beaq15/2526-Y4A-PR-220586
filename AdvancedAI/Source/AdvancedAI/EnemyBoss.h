// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "WidgetBossHealthBar.h"
#include "EnemyBoss.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EBoss_Attacks : uint8
{
	Default,
	Combo1
};

UCLASS()
class ADVANCEDAI_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyBoss();
	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual bool  TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser) override;

	virtual void EquipWeapon_Implementation()   override;
	virtual void UnequipWeapon_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UWidgetBossHealthBar> HealthBarWidget;

	//----------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AxeComboMontage1;

	void AttackCombo1(AActor* AttackTarget);

	UFUNCTION()
	void Teleport(FVector Location, AActor* AttackTarget);

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

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportAcceptanceRadius = 150.f;

	FTimerHandle TeleportMoveTimerHandle;

	UPROPERTY()
	FVector CachedTeleportLocation;

	TFunction<void()> OnTeleportEndCallback;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
