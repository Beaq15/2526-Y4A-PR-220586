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

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);


UENUM(BlueprintType)
enum class EPlayerStance : uint8
{
	Unarmed,
	Magic,
	Melee
};



UCLASS(config=Game)
class AAdvancedAICharacter : public ACharacter, public IGenericTeamAgentInterface, public IDamageableInterface
{
	GENERATED_BODY()

	//----------------------------------------------------------------------
	// Private — Camera
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	//----------------------------------------------------------------------
	// Private — Input
	//----------------------------------------------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ChangeStateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MakeNoiseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DoDamageAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ChangeStanceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SwordBlockAction;

	//----------------------------------------------------------------------
	// Private — State
	//----------------------------------------------------------------------

	bool bPressed = false;
	bool bCanMove = true;
	bool bAttacking = false;

	//----------------------------------------------------------------------
	// Private — Movement Tuning
	//----------------------------------------------------------------------

	const float MagicWalkSpeed = 200.f;
	const float DefaultWalkSpeed = 700.f;
	const float MeleeWalkSpeed = 500.f;

	//----------------------------------------------------------------------
	// Pivate — Config
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY()
	TObjectPtr<AActor> WeaponActor;

	//----------------------------------------------------------------------
	// Private — Timeline
	//----------------------------------------------------------------------

	FTimeline AimTimeline;

	UPROPERTY()
	TObjectPtr<UCurveFloat> LinearCurve;

	UFUNCTION()
	void OnAimTimeLineUpdate(float Value);

public:
	//----------------------------------------------------------------------
	// Public — Lifecycle
	//----------------------------------------------------------------------
	AAdvancedAICharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	//----------------------------------------------------------------------
	// Public — Team
	//----------------------------------------------------------------------

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(0); }

	//----------------------------------------------------------------------
	// Public — State (read-only)
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EPlayerStance Stance = EPlayerStance::Unarmed;

	//----------------------------------------------------------------------
	// Public — Accessors
	//----------------------------------------------------------------------

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	//----------------------------------------------------------------------
	// Protected — Lifecycle Overrides
	//----------------------------------------------------------------------

	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//----------------------------------------------------------------------
	// Protected — Input Handlers
	//----------------------------------------------------------------------

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void ChangeStance(const FInputActionValue& Value);
	void MakeSomeNoise(const FInputActionValue& Value);
	void DoDamage(const FInputActionValue& Value);
	void MeleeAttack(const FInputActionValue& Value);

	//----------------------------------------------------------------------
	// Protected — Stance 
	//----------------------------------------------------------------------

	void MagicStance();
	void UnarmedStance();
	void MeleeStance();

	//----------------------------------------------------------------------
	// Protected — Animation Callbacks
	//----------------------------------------------------------------------

	UFUNCTION()
	void EquipWeapon();

	UFUNCTION()
	void UnequipWeapon();

	UFUNCTION()
	void StartBlock(const FInputActionValue& Value);

	UFUNCTION()
	void EndBlock(const FInputActionValue& Value);

	UFUNCTION()
	void OnBlocked(bool bCanBeParried, AActor* DamageCauser);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitResponse_Event(EDamageResponse DamageResponse, AActor* DamageCauser);

	UFUNCTION()
	void OnDeath_Event();

	UFUNCTION()
	void OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnShieldBlockMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	//----------------------------------------------------------------------
	// Protected — Animation Assets
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsonly, Category = "Animation")
	TObjectPtr<UAnimMontage> MagicSpellMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UPROPERTY(EditDefaultsonly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwordSlashMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> EquipSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DropSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwordBlockMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwordBlockHitMontage;

	//----------------------------------------------------------------------
	// Protected — Components
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UDamageSystem> DamageSystem;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UAttackSystem> AttackSystem;

	//----------------------------------------------------------------------
	// Protected — Camera Tuning
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector AimBoomOffset = { 200.f, 65.f, 45.f };

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector DefaultBoomOffset = { 0.f, 60.f, 30.f };

	//----------------------------------------------------------------------
	// Protected — UI
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWidgetPlayerHUD> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UWidgetPlayerHUD> PlayerHUDWidget;

	UFUNCTION()
	void DisplayHUD();
	
	//----------------------------------------------------------------------
	// Protected — IDamageableInterface
	//----------------------------------------------------------------------

	virtual float GetCurrentHealth_Implementation() override { return DamageSystem->Health; }
	virtual float GetMaxHealth_Implementation()     override { return DamageSystem->MaxHealth; }
	virtual bool  IsDead_Implementation()           override { return DamageSystem->isDead; }
	virtual bool  IsAttacking_Implementation()      override;
	virtual float Heal_Implementation(float Amount) override;
	virtual bool  TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser) override;
	virtual bool ReserveAttackToken_Implementation(int32 Amount) override;
	virtual void ReturnAttackToken_Implementation(int32 Amount) override;
};

