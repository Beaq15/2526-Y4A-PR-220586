// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyZombie.generated.h"

UCLASS()
class ADVANCEDAI_API AEnemyZombie : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyZombie();

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage>ScreamMontage;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void Scream();

	TFunction<void()> OnScreamEndCallback;

	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual void GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;
};
