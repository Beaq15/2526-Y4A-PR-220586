// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "WidgetBossHealthBar.h"
#include "EnemyBoss.generated.h"

/**
 * 
 */
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
};
