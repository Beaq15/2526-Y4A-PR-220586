// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolRoute.h"
#include "EnemyInterface.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);

UCLASS()
class ADVANCEDAI_API AEnemyBase : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnAttackEnd OnAttackEnd;

	// Sets default values for this character's properties
	AEnemyBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Attack();

	UFUNCTION()
	void WieldSword();

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool bIsWieldingSword = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> SwordClass;
	UPROPERTY()
	TObjectPtr<AActor> SpawnedSword;

	UPROPERTY(EditAnywhere, Category = "AI")
	APatrolRoute* PatrolRoute;


	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	virtual APatrolRoute* GetPatrolRoute_Implementation() override;
	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;

};
