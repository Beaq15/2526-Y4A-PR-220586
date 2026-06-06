// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdvancedAICharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "AIC_Enemy_Base.h"
#include "DrawDebugHelpers.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/KismetMathLibrary.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AAdvancedAICharacter

AAdvancedAICharacter::AAdvancedAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));

	DamageSystem = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystem"));
	AttackSystem = CreateDefaultSubobject<UAttackSystem>(TEXT("AttackSystem"));
}

void AAdvancedAICharacter::BeginPlay()
{
	Super::BeginPlay();

	DisplayHUD();

	DamageSystem->OnDeath.AddUniqueDynamic(this, &AAdvancedAICharacter::OnDeath_Event);

	// TIMELINE
	LinearCurve = NewObject<UCurveFloat>(this);
	LinearCurve->FloatCurve.AddKey(0.0f, 0.0f);
	LinearCurve->FloatCurve.AddKey(0.3f, 1.0f);

	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("OnAimTimeLineUpdate"));
	AimTimeline.AddInterpFloat(LinearCurve, UpdateDelegate);
}

void AAdvancedAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTimeline.TickTimeline(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAdvancedAICharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AAdvancedAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdvancedAICharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdvancedAICharacter::Look);

		EnhancedInputComponent->BindAction(ChangeStateAction, ETriggerEvent::Triggered, this, &AAdvancedAICharacter::ChangeState);
		EnhancedInputComponent->BindAction(MakeNoiseAction, ETriggerEvent::Triggered, this, &AAdvancedAICharacter::MakeSomeNoise);
		EnhancedInputComponent->BindAction(DoDamageAction, ETriggerEvent::Triggered, this, &AAdvancedAICharacter::DoDamage);

		EnhancedInputComponent->BindAction(ChangeStanceAction, ETriggerEvent::Started, this, &AAdvancedAICharacter::EnterMagicStance);
		EnhancedInputComponent->BindAction(ChangeStanceAction, ETriggerEvent::Completed, this, &AAdvancedAICharacter::EnterDefaultStance);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AAdvancedAICharacter::Move(const FInputActionValue& Value)
{
	if (CanMove)
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}

	}
}

void AAdvancedAICharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AAdvancedAICharacter::ChangeState(const FInputActionValue& Value)
{
	Pressed = !Pressed;
	AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyBase::StaticClass());
	if (IsValid(Actor))
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(Actor);
		if (Enemy)
		{
			AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(Enemy->GetController());
			if (AIController)
				if (Pressed)
					AIController->SetStateAsAttacking(this, true);
				else
					AIController->SetStateAsPassive();
		}
	}
}

void AAdvancedAICharacter::MakeSomeNoise(const FInputActionValue& Value)
{
	MakeNoise(1.f, this, GetActorLocation());
	DrawDebugSphere(GetWorld(), GetActorLocation(), 50.f, 12, FColor::Red, false, 1.f);
}

void AAdvancedAICharacter::DoDamage(const FInputActionValue& Value)
{
	if (!(Stance == EPlayerStance::Magic))
		return;

	if (Attacking)
		return;

	CanMove = false;

	if (MagicSpellMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			Attacking = true;
			AnimInstance->Montage_Play(MagicSpellMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AAdvancedAICharacter::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AAdvancedAICharacter::OnMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, MagicSpellMontage);
		}
	}
}

void AAdvancedAICharacter::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (NotifyName == FName("Fire"))
	{
		FDamageInfo DamageInfo;
		DamageInfo.Amount = 20.f;
		DamageInfo.DamageType = EDamageType::Explosion;
		DamageInfo.DamageResponse = EDamageResponse::HitReaction;
		DamageInfo.bCanBeBlocked = true;

		FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("hand_l"));
		FVector TargetLocation = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 10000.0f;

		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);

		FTransform SpawnTransform(SpawnRotation, SpawnLocation, FVector(1.0f));
		AttackSystem->MagicSpell(SpawnTransform, nullptr, DamageInfo);
	}
}

void AAdvancedAICharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	CanMove = true;
	Attacking = false;
}
void AAdvancedAICharacter::EnterMagicStance()
{
	Stance = EPlayerStance::Magic;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	GetCharacterMovement()->MaxWalkSpeed = MagicWalkSpeed;

	PlayerHUDWidget->ShowCrosshair();

	AimTimeline.PlayFromStart();
}

void AAdvancedAICharacter::EnterDefaultStance()
{
	Stance = EPlayerStance::Default;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

	PlayerHUDWidget->HideCrosshair();

	AimTimeline.Reverse();
}

float AAdvancedAICharacter::Heal_Implementation(float Amount)
{
	return DamageSystem->Heal(Amount);
}

bool AAdvancedAICharacter::TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	return DamageSystem->TakeDamage(DamageInfo, DamageCauser);
}

bool AAdvancedAICharacter::IsAttacking_Implementation()
{
	return Attacking;
}

void AAdvancedAICharacter::OnAimTimeLineUpdate(float Value)
{
	CameraBoom->SocketOffset = FMath::Lerp(DefaultBoomOffset, AimBoomOffset, Value);
}

void AAdvancedAICharacter::OnDeath_Event()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), AllEnemies);

	for (AActor* Actor : AllEnemies)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(Actor);
		if (!Enemy) continue;

		AAIC_Enemy_Base* AIC = Cast<AAIC_Enemy_Base>(Enemy->GetController());
		if (!AIC) continue;

		if (AIC->AttackTargetActor == this)
			AIC->SetStateAsPassive();
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	DisableInput(PC);
}

void AAdvancedAICharacter::DisplayHUD()
{
	if (IsValid(PlayerHUDWidget))
		PlayerHUDWidget->SetVisibility(ESlateVisibility::Visible);
		
	else
	{
		PlayerHUDWidget = CreateWidget<UWidgetPlayerHUD>(GetWorld(), PlayerHUDWidgetClass);
		PlayerHUDWidget->Player = this;
		if (PlayerHUDWidget)
			PlayerHUDWidget->AddToViewport();

	}
}
