// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AEnemyBoss::AEnemyBoss()
{
	DamageSystem->MaxHealth = 700.f;
	DamageSystem->Health = 700.f;

	HealthBarComponent->SetWidget(nullptr);

	if (HealthBarWidgetClass)
	{
		HealthBarWidget = CreateWidget<UWidgetBossHealthBar>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->UpdateHealthPercentage(DamageSystem->MaxHealth, DamageSystem->Health);
		}
	}
}
float AEnemyBoss::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:      Movement->MaxWalkSpeed = 0.f;   break;
	case EMovementSpeed::Walking:   Movement->MaxWalkSpeed = 100.f; break;
	case EMovementSpeed::Jogging:   Movement->MaxWalkSpeed = 300.f; break;
	case EMovementSpeed::Sprinting: Movement->MaxWalkSpeed = 350.f; break;
	}

	return Movement->MaxWalkSpeed;
}

bool AEnemyBoss::TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	bool ActualDamage = Super::TakeDamage_Implementation(DamageInfo, DamageCauser);

	HealthBarWidget->UpdateHealthPercentage(DamageSystem->MaxHealth, DamageSystem->Health);

	return ActualDamage;
}

void AEnemyBoss::EquipWeapon_Implementation()
{
	if (!WeaponClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);
	if (!WeaponActor) return;

	WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("hand_r_sword_socket"));

	bIsWieldingWeapon = true;

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			OnEquipWeaponEnd.Broadcast();
		});
}

void AEnemyBoss::UnequipWeapon_Implementation()
{
	if (WeaponActor)
	{
		WeaponActor->Destroy();
		WeaponActor = nullptr;
	}

	bIsWieldingWeapon = false;


	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			OnDropWeaponEnd.Broadcast();
		});
}

void AEnemyBoss::ThrowAxe(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 20.0f;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = ThrowAxeMontage;
	AttackSystem->RangeAttack(AttackInfo, 40.0f, 220.f);
}

void AEnemyBoss::QuickAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 20.0f;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = QuickAttackMontage;
	AttackSystem->RangeAttack(AttackInfo, 40.0f, 220.f);
}

void AEnemyBoss::AttackCombo1(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 20.0f;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = AxeComboMontage1;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->RangeAttack(AttackInfo, 40.0f, 220.f);
}

void AEnemyBoss::AttackCombo2(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 15.0f;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = AxeComboMontage2;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->RangeAttack(AttackInfo, 40.0f, 220.f);
}

void AEnemyBoss::JumpAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 15.0f;
	DamageInfo.bCanBeBlocked = false;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = JumpAttackMontage;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->RangeAttack(AttackInfo,300.0f, 220.f);
}

void AEnemyBoss::GroundSmashAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 25.0f;
	DamageInfo.bCanBeBlocked = false;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = GroundSmashAttackMontage;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->AroundAttack(AttackInfo, 1100.0f);
}

void AEnemyBoss::Teleport(FVector Location, AActor* AttackTarget)
{
	GetMesh()->SetVisibility(false, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	TeleportBodyEffect = UGameplayStatics::SpawnEmitterAttached(
		P_GideonBurde, GetMesh(), FName("Spine_01"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.25f), EAttachLocation::KeepRelativeOffset, false);

	TeleportTrailEffect = UGameplayStatics::SpawnEmitterAttached(
		P_GideonMeteor, GetMesh(), FName("Spine_01"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.25f), EAttachLocation::KeepRelativeOffset, false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
		AnimInstance->StopAllMontages(0.25f);

	if (AttackTarget)
	{
		const FVector ToBoss = (GetActorLocation() - AttackTarget->GetActorLocation()).GetSafeNormal();
		Location = AttackTarget->GetActorLocation() + ToBoss * 150.f;
	}

	CachedTeleportLocation = Location;

	GetWorldTimerManager().SetTimer(TeleportMoveTimerHandle, [this]()
		{
			float Distance = FVector::Dist(GetActorLocation(), CachedTeleportLocation);

			if (Distance <= 50.f)
			{
				GetWorldTimerManager().ClearTimer(TeleportMoveTimerHandle);
				TeleportEnd();
				return;
			}

			FVector Direction = (CachedTeleportLocation - GetActorLocation()).GetSafeNormal();
			FVector NewLocation = GetActorLocation() + Direction * 80.f;
			SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		}, 0.016f, true);
}

void AEnemyBoss::TeleportEnd()
{
	GetMesh()->SetVisibility(true, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	if (OnTeleportEndCallback)
	{
		OnTeleportEndCallback();
		OnTeleportEndCallback = nullptr;
	}

	if (TeleportBodyEffect.Get()) { TeleportBodyEffect->DestroyComponent(); TeleportBodyEffect = nullptr; }
	if (TeleportTrailEffect.Get()) { TeleportTrailEffect->DestroyComponent(); TeleportTrailEffect = nullptr; }
}

void AEnemyBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(TeleportMoveTimerHandle);
	Super::EndPlay(EndPlayReason);
}
