// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyRanged.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "AIC_Enemy_Base.h"
#include "AdvancedAICharacter.h"
#include "Kismet/GameplayStatics.h"

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------

void AEnemyRanged::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon_Implementation();
}

//----------------------------------------------------------------------
// IEnemyInterface
//----------------------------------------------------------------------

void AEnemyRanged::EquipWeapon_Implementation()
{
	if (!WeaponClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);
	if (!WeaponActor) return;

	WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("hand_r_rifle_socket"));

	bIsWieldingWeapon = true;
	OnEquipWeaponEnd.Broadcast();
}


float AEnemyRanged::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:      Movement->MaxWalkSpeed = 0.f;   break;
	case EMovementSpeed::Walking:   Movement->MaxWalkSpeed = 200.f; break;
	case EMovementSpeed::Jogging:   Movement->MaxWalkSpeed = 300.f; break;
	case EMovementSpeed::Sprinting: Movement->MaxWalkSpeed = 500.f; break;
	}

	return Movement->MaxWalkSpeed;
}

void AEnemyRanged::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 600.f;
	DefendRadius = 600.f;
}

void AEnemyRanged::Attack_Implementation(AActor* AttackTarget)
{
	Super::Attack_Implementation(AttackTarget);

	CachedAttackTarget = AttackTarget;

	if (FireRifleMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(FireRifleMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyRanged::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyRanged::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, FireRifleMontage);
		}
	}
}

//----------------------------------------------------------------------
// Animation Callbacks
//----------------------------------------------------------------------

void AEnemyRanged::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IEnemyInterface::Execute_AttackEnd(this, CachedAttackTarget);
}

void AEnemyRanged::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (NotifyName == FName("Fire"))
	{
		if (!WeaponClass || !CachedAttackTarget) return;

		FVector Start = WeaponActor->GetActorLocation();
		FVector End = CachedAttackTarget->GetActorLocation();

		float HitChance = CalculateHitChance(CachedAttackTarget);
		bool bWillHit = FMath::FRand() <= HitChance;

		if (!bWillHit)
		{
			FVector MissOffset = FVector(FMath::RandRange(-150.0f, 150.f), FMath::RandRange(-150.0f, 150.f), FMath::RandRange(-150.0f, 150.f));

			End += MissOffset;
		}

		FDamageInfo DamageInfo;
		DamageInfo.Amount = 5.f;
		DamageInfo.DamageType = EDamageType::Projectile;
		DamageInfo.DamageResponse = EDamageResponse::HitReaction;
		DamageInfo.bCanBeBlocked = true;

		AttackSystem->FireBullet(Start, End, DamageInfo);
	}
}

float AEnemyRanged::CalculateHitChance(AActor* Target) const
{
	const float BaseAccuracy = 0.9f; // almost guaranteed hit when target is still
	const float MinAccuracy = 0.15f; // little accuracy at max speed ( still some risk)
	const float MaxSpeed = 500.0f;

	if (ACharacter* TargetCharacter = Cast<AAdvancedAICharacter>(Target))
	{
		if (UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement())
		{
			float CurrentSpeed = TargetMovement->Velocity.Size();
			float SpeedRatio = FMath::Clamp(CurrentSpeed / MaxSpeed, 0.f, 1.f);

			return FMath::Lerp(BaseAccuracy, MinAccuracy, SpeedRatio);
		}
	}
	return BaseAccuracy;
}
