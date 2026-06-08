// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GenericTeamAgentInterface.h"
#include "DamageableInterface.h"
#include "DamageSystem.h"
#include "Components/WidgetComponent.h"
#include "WidgetHealthBar.h"
#include "WidgetPlayerHUD.h"
#include "AttackSystem.h"
#include "Components/TimelineComponent.h"
#include "AdvancedAICharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerStance : uint8
{
	Default,
	Magic
};

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AAdvancedAICharacter : public ACharacter, public IGenericTeamAgentInterface, public IDamageableInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChangeStateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MakeNoiseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DoDamageAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChangeStanceAction;

public:
	AAdvancedAICharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	bool Pressed = false;

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(0); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPlayerStance Stance = EPlayerStance::Default;

	UPROPERTY(EditDefaultsonly)
	TObjectPtr<UAnimMontage>MagicSpellMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitResponse_Event(EDamageResponse DamageResponse, AActor* DamageCauser);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void ChangeState(const FInputActionValue& Value);

	void MakeSomeNoise(const FInputActionValue& Value);
			
	void DoDamage(const FInputActionValue& Value);

	void EnterMagicStance();

	void EnterDefaultStance();

	UPROPERTY(VisibleAnywhere)
	UDamageSystem* DamageSystem;

	UPROPERTY(VisibleAnywhere)
	UAttackSystem* AttackSystem;

	UPROPERTY(EditDefaultsOnly)
	FVector AimBoomOffset = { 200.f, 65.f, 45.f };

	UPROPERTY(EditDefaultsOnly)
	FVector DefaultBoomOffset = { 0.f, 60.f, 30.f };

	float MagicWalkSpeed = 200.f;
	float DefaultWalkSpeed = 2000.f;
	bool CanMove = true;
	bool Attacking = false;

	FTimeline AimTimeline;

	UPROPERTY()
	UCurveFloat* LinearCurve;

	UFUNCTION()
	void OnAimTimeLineUpdate(float Value);

	UFUNCTION()
	void OnDeath_Event();

	UFUNCTION()
	void DisplayHUD();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWidgetPlayerHUD>PlayerHUDWidgetClass;

	UWidgetPlayerHUD* PlayerHUDWidget;
	
protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	float GetCurrentHealth_Implementation() { return DamageSystem->Health; }

	float GetMaxHealth_Implementation() { return DamageSystem->MaxHealth; }

	float Heal_Implementation(float Amount);

	bool TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser);

	bool IsDead_Implementation() { return DamageSystem->isDead; }

	bool IsAttacking_Implementation();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

