// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolRoute.h"
#include "EnemyInterface.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Passive, //0
	Attacking, //1
	Frozen, //2
	Investigating, //3 
	Dead //4
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipSwordEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDropSwordEnd);

UCLASS()
class ADVANCEDAI_API AEnemyBase : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnAttackEnd OnAttackEnd;
	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnEquipSwordEnd OnEquipSwordEnd;
	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnDropSwordEnd OnDropSwordEnd;

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

	UFUNCTION()
	void DropSword();

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool bIsWieldingSword = false;

	UPROPERTY()
	EAIState State = EAIState::Passive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> EquipSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DropSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> SwordClass;
	UPROPERTY()
	TObjectPtr<AActor> SpawnedSword;

	UPROPERTY(EditAnywhere, Category = "AI")
	APatrolRoute* PatrolRoute;


	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	virtual APatrolRoute* GetPatrolRoute_Implementation() override;
	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;

};
