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

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> IdleMontage;


	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void Scream();
	void Idle();

	TFunction<void()> OnScreamEndCallback;

	virtual void Tick(float DeltaTime) override;

	void ShortRangeAttack(AActor* AttackTarget);

	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual void GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;
	virtual void Attack_Implementation(AActor* AttackTarget) override;
};
