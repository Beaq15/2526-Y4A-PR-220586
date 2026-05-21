// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyRanged.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "AIC_Enemy_Base.h"

void AEnemyRanged::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon_Implementation();
}

float AEnemyRanged::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:
		Movement->MaxWalkSpeed = 0.f;
		break;
	case EMovementSpeed::Walking:
		Movement->MaxWalkSpeed = 200.f;
		break;
	case EMovementSpeed::Jogging:
		Movement->MaxWalkSpeed = 300.f;
		break;
	case EMovementSpeed::Sprinting:
		Movement->MaxWalkSpeed = 500.f;
		break;
	}

	return Movement->MaxWalkSpeed;
}

void AEnemyRanged::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 600.f;
	DefendRadius = 600.f;
}

void AEnemyRanged::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackEnd.Broadcast();
}

void AEnemyRanged::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (NotifyName == FName("Fire"))
	{
		if (!WeaponClass) return;

		AAIC_Enemy_Base* AIC = Cast<AAIC_Enemy_Base>(GetController());
		if (!AIC) return;

		AActor* AttackTarget = AIC->AttackTargetActor;

		if (!AttackTarget)
			return;

		FVector Start = WeaponActor->GetActorLocation();
		FVector End = AttackTarget->GetActorLocation();

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, Params);

		if (bHit)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit Target"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
				FString::Printf(TEXT("Hit: %s"), *HitResult.GetActor()->GetName()));

		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not the player"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
				FString::Printf(TEXT("Hit: %s"), *HitResult.GetActor()->GetName()));
		}

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 1.f);
	}
}

void AEnemyRanged::Attack()
{
	if (FireRifleMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(FireRifleMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AEnemyRanged::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyRanged::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, FireRifleMontage);
		}
	}
}

void AEnemyRanged::EquipWeapon_Implementation()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);
	if (!WeaponActor) return;

	WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("ik_hand_r_rifle_socket"));

	bIsWieldingWeapon = true;

	OnEquipWeaponEnd.Broadcast();
}

