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

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------

AAdvancedAICharacter::AAdvancedAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// Rotation — controller drives camera only, not character
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	// Camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Components
	CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));
	DamageSystem = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystem"));
	AttackSystem = CreateDefaultSubobject<UAttackSystem>(TEXT("AttackSystem"));

	// Tokens
	DamageSystem->AttackTokensCount = 1;
}

void AAdvancedAICharacter::BeginPlay()
{
	Super::BeginPlay();

	DisplayHUD();

	DamageSystem->OnDeath.AddUniqueDynamic(this, &AAdvancedAICharacter::OnDeath_Event);
	DamageSystem->OnDamageResponse.AddUniqueDynamic(this, &AAdvancedAICharacter::OnHitResponse_Event);
	DamageSystem->OnBlocked.AddDynamic(this, &AAdvancedAICharacter::OnBlocked);

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

//----------------------------------------------------------------------
// Input Setup
//----------------------------------------------------------------------

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

		EnhancedInputComponent->BindAction(ChangeStateAction, ETriggerEvent::Triggered, this, &AAdvancedAICharacter::ChangeStance);
		EnhancedInputComponent->BindAction(MakeNoiseAction, ETriggerEvent::Triggered, this, &AAdvancedAICharacter::MakeSomeNoise);
		EnhancedInputComponent->BindAction(DoDamageAction, ETriggerEvent::Triggered, this, &AAdvancedAICharacter::MeleeAttack);

		//EnhancedInputComponent->BindAction(ChangeStanceAction, ETriggerEvent::Started, this, &AAdvancedAICharacter::MagicStance);
		//EnhancedInputComponent->BindAction(ChangeStanceAction, ETriggerEvent::Completed, this, &AAdvancedAICharacter::UnarmedStance);

		EnhancedInputComponent->BindAction(SwordBlockAction, ETriggerEvent::Started, this, &AAdvancedAICharacter::StartBlock);
		EnhancedInputComponent->BindAction(SwordBlockAction, ETriggerEvent::Completed, this, &AAdvancedAICharacter::EndBlock);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

//----------------------------------------------------------------------
// Input Handlers
//----------------------------------------------------------------------

void AAdvancedAICharacter::Move(const FInputActionValue& Value)
{
	if (!bCanMove || !Controller)
		return;
	
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AAdvancedAICharacter::Look(const FInputActionValue& Value)
{
	if (!Controller)
		return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AAdvancedAICharacter::ChangeStance(const FInputActionValue& Value)
{
	bPressed = !bPressed;

	if (bPressed)
		EquipWeapon();
	else
		UnequipWeapon();
}

void AAdvancedAICharacter::MakeSomeNoise(const FInputActionValue& Value)
{
	MakeNoise(1.f, this, GetActorLocation());
	DrawDebugSphere(GetWorld(), GetActorLocation(), 50.f, 12, FColor::Red, false, 1.f);
}

void AAdvancedAICharacter::DoDamage(const FInputActionValue& Value)
{
	if (Stance != EPlayerStance::Magic || bAttacking) return;

	bCanMove = false;

	if (!MagicSpellMontage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	bAttacking = true;
	AnimInstance->Montage_Play(MagicSpellMontage, 1.0f);
	AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AAdvancedAICharacter::OnMontageNotifyBegin);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AAdvancedAICharacter::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, MagicSpellMontage);
}

void AAdvancedAICharacter::MeleeAttack(const FInputActionValue& Value)
{
	if (Stance != EPlayerStance::Melee || bAttacking) return;
	bAttacking = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(SwordSlashMontage, 1.0f);
	AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AAdvancedAICharacter::OnMontageNotifyBegin);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AAdvancedAICharacter::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, SwordSlashMontage);
}

void AAdvancedAICharacter::MagicStance()
{
	Stance = EPlayerStance::Magic;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->MaxWalkSpeed = MagicWalkSpeed;

	PlayerHUDWidget->ShowCrosshair();
	AimTimeline.PlayFromStart();

	if (IsValid(WeaponActor))
		WeaponActor->Destroy();
}

void AAdvancedAICharacter::UnarmedStance()
{
	Stance = EPlayerStance::Unarmed;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

	PlayerHUDWidget->HideCrosshair();
	AimTimeline.Reverse();

	if (IsValid(WeaponActor))
		WeaponActor->Destroy();
}

void AAdvancedAICharacter::MeleeStance()
{
	Stance = EPlayerStance::Melee;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->MaxWalkSpeed = MeleeWalkSpeed;
}

//----------------------------------------------------------------------
// Animation Callbacks
//----------------------------------------------------------------------

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

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, FollowCamera->GetComponentLocation(), TargetLocation, ECC_Visibility, Params);

		FVector TargetPoint = bHit ? HitResult.Location : HitResult.TraceEnd;
		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetPoint);
		FTransform SpawnTransform(SpawnRotation, SpawnLocation, FVector::OneVector);

		AttackSystem->MagicSpell(SpawnTransform, nullptr, DamageInfo);

	}
	if (NotifyName == FName("Slash"))
	{
		FVector Start = GetActorLocation();
		FVector End = GetActorForwardVector() * 200.f + GetActorLocation();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		TArray <FHitResult> OutHits;

		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, 20.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, OutHits, true);

		if (bHit)
		{
			FDamageInfo DamageInfo;
			DamageInfo.Amount = 20.f;
			DamageInfo.DamageType = EDamageType::Melee;
			DamageInfo.DamageResponse = EDamageResponse::HitReaction;
			DamageInfo.bCanBeBlocked = true;
			DamageInfo.bCanBeParried = true;

			AttackSystem->DamageAllNonTeamMembers(DamageInfo, OutHits);
		}
	}

	if (NotifyName == FName("HoldSword"))
	{
		if (!WeaponClass) return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;

		WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);
		if (!WeaponActor) return;

		WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("hand_r_sword_socket"));

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AAdvancedAICharacter::OnMontageNotifyBegin);
	}

	if (NotifyName == FName("DropSword"))
	{
		if (WeaponActor)
		{
			WeaponActor->Destroy();
			WeaponActor = nullptr;
		}

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AAdvancedAICharacter::OnMontageNotifyBegin);
	}

	if (NotifyName == FName("Block"))
	{
		DamageSystem->isBlocking = true;
	}
}

void AAdvancedAICharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bCanMove = true;
	bAttacking = false;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
		EnableInput(PC);

}

void AAdvancedAICharacter::OnHitResponse_Event(EDamageResponse DamageResponse, AActor* DamageCauser)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
		DisableInput(PC);

	if (!HitReactionMontage) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
		
	AnimInstance->Montage_Play(HitReactionMontage, 1.0f);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AAdvancedAICharacter::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactionMontage);
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

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
		DisableInput(PC);
}

void AAdvancedAICharacter::EquipWeapon()
{
	if (EquipSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(EquipSwordMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AAdvancedAICharacter::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AAdvancedAICharacter::OnEquipSwordMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, EquipSwordMontage);
		}
	}
}

void AAdvancedAICharacter::UnequipWeapon()
{
	if (DropSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DropSwordMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AAdvancedAICharacter::OnMontageNotifyBegin);


			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AAdvancedAICharacter::OnDropSwordMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, DropSwordMontage);
		}
	}
}

void AAdvancedAICharacter::StartBlock(const FInputActionValue& Value)
{
	if (Stance != EPlayerStance::Melee || DamageSystem->isBlocking) return;

	DamageSystem->isBlocking = true;
	bCanMove = false;
	
	if (!SwordBlockMontage) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	AnimInstance->Montage_Play(SwordBlockMontage, 1.0f);
}

void AAdvancedAICharacter::EndBlock(const FInputActionValue& Value)
{
	StopAnimMontage(SwordBlockMontage);
	DamageSystem->isBlocking = false;
	bCanMove = true;
}

void AAdvancedAICharacter::OnBlocked(bool bCanBeParried, AActor* DamageCauser)
{
	if (SwordBlockHitMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		AnimInstance->Montage_Play(SwordBlockHitMontage, 1.0f);
	}
}

void AAdvancedAICharacter::OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
		MeleeStance();
}

void AAdvancedAICharacter::OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
		UnarmedStance();
}

void AAdvancedAICharacter::OnShieldBlockMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	//DamageSystem->isBlocking = false;
}

//----------------------------------------------------------------------
// Timeline
//----------------------------------------------------------------------

void AAdvancedAICharacter::OnAimTimeLineUpdate(float Value)
{
	CameraBoom->SocketOffset = FMath::Lerp(DefaultBoomOffset, AimBoomOffset, Value);
}

//----------------------------------------------------------------------
// UI
//----------------------------------------------------------------------

void AAdvancedAICharacter::DisplayHUD()
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}
		
	PlayerHUDWidget = CreateWidget<UWidgetPlayerHUD>(GetWorld(), PlayerHUDWidgetClass);
	if (!PlayerHUDWidget) return;
	PlayerHUDWidget->Player = this;
	PlayerHUDWidget->AddToViewport();
}

//----------------------------------------------------------------------
// IDamageableInterface
//----------------------------------------------------------------------

float AAdvancedAICharacter::Heal_Implementation(float Amount)
{
	return DamageSystem->Heal(Amount);
}

bool AAdvancedAICharacter::TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	if (DamageSystem->isBlocking && DamageCauser)
	{
		FVector ToDamageCauser = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		float DotProduct = FVector::DotProduct(GetActorForwardVector(), ToDamageCauser);

		if (DotProduct < 0.5f)
		{
			DamageSystem->isBlocking = false;
			bool bResult = DamageSystem->TakeDamage(DamageInfo, DamageCauser);
			DamageSystem->isBlocking = true;
			return bResult;
		}
	}
	return DamageSystem->TakeDamage(DamageInfo, DamageCauser);
}

bool AAdvancedAICharacter::IsAttacking_Implementation()
{
	return bAttacking;
}

bool AAdvancedAICharacter::ReserveAttackToken_Implementation(int32 Amount)
{
	return DamageSystem->ReserveAttackToken(Amount);
}

void AAdvancedAICharacter::ReturnAttackToken_Implementation(int32 Amount)
{
	DamageSystem->ReturnAttackToken(Amount);
}

