// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


//----------------------------------------------------------------------
// Public — IEnemyInterface
//----------------------------------------------------------------------

float AEnemyMage::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:      Movement->MaxWalkSpeed = 0.f;   break;
	case EMovementSpeed::Walking:   Movement->MaxWalkSpeed = 150.f; break;
	case EMovementSpeed::Jogging:   Movement->MaxWalkSpeed = 250.f; break;
	case EMovementSpeed::Sprinting: Movement->MaxWalkSpeed = 400.f; break;
	}

	return Movement->MaxWalkSpeed;
}

void AEnemyMage::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 500.0f;
	DefendRadius = 700.0f;
}

void AEnemyMage::Attack_Implementation(AActor* AttackTarget)
{
	Super::Attack_Implementation(AttackTarget);
	CachedAttackTarget = AttackTarget;

	if (FireMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(FireMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyMage::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMage::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, FireMontage);
		}
	}
}

//----------------------------------------------------------------------
// Animation Callbacks
//----------------------------------------------------------------------

void AEnemyMage::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IEnemyInterface::Execute_AttackEnd(this, CachedAttackTarget);
}

void AEnemyMage::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (NotifyName == "Fire")
	{
		FDamageInfo DamageInfo;
		DamageInfo.Amount = 20.f;
		DamageInfo.DamageType = EDamageType::Explosion;
		DamageInfo.DamageResponse = EDamageResponse::HitReaction;
		DamageInfo.bCanBeBlocked = true;

		FVector SocketLocation = GetMesh()->GetSocketLocation(FName("RightHand"));
		FVector SpawnLocation = SocketLocation + GetActorForwardVector() * 50.f;

		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, CachedAttackTarget->GetActorLocation());
		FTransform SpawnTransform(SpawnRotation, SpawnLocation, FVector::OneVector);

		AttackSystem->MagicSpell(SpawnTransform, CachedAttackTarget, DamageInfo);
	}
}