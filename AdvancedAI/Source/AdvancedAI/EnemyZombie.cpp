// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

AEnemyZombie::AEnemyZombie()
{
	DamageSystem->MaxHealth = 100.f;
	DamageSystem->Health = 100.f;

	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxAcceleration = 2048.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f; 
	GetCharacterMovement()->GroundFriction = 8.f;
}

float AEnemyZombie::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:      Movement->MaxWalkSpeed = 0.f;   break;
	case EMovementSpeed::Walking:   Movement->MaxWalkSpeed = 70.f; break;
	case EMovementSpeed::Jogging:   Movement->MaxWalkSpeed = 250.f; break;
	case EMovementSpeed::Sprinting: Movement->MaxWalkSpeed = 400.f; break;
	}

	return Movement->MaxWalkSpeed;
}

void AEnemyZombie::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 70.0f;
	DefendRadius = 200.0f;
}

void AEnemyZombie::Attack_Implementation(AActor* AttackTarget)
{
	Super::Attack_Implementation(AttackTarget);
	ShortRangeAttack(AttackTarget);
}

void AEnemyZombie::ShortRangeAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.f;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.DamageInfo = DamageInfo;
	AttackInfo.Montage = AttackMontage;
	AttackSystem->RangeAttack(AttackInfo, 20.f, 70.f);
}

void AEnemyZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	if (!Movement || !Anim) return;

	float Curve = 1.f;
	Anim->GetCurveValueWithDefault(TEXT("MoveSpeed"), 1.f, Curve);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Green,
			FString::Printf(TEXT("Velocity: %.1f  MaxWalkSpeed: %.1f"),
				GetVelocity().Size2D(), GetCharacterMovement()->MaxWalkSpeed));
	}

	Movement->MaxWalkSpeed = 30.f * FMath::Clamp(Curve, 0.f, 1.f);
}

void AEnemyZombie::Scream()
{
	if (!ScreamMontage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->Montage_Play(ScreamMontage, 1.0f);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AEnemyZombie::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ScreamMontage);
}

void AEnemyZombie::Idle()
{
	if (!IdleMontage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->Montage_Play(IdleMontage, 1.0f);
}

void AEnemyZombie::OnMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{

	if (!IsDead_Implementation())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	if (OnScreamEndCallback)
	{
		OnScreamEndCallback();
		OnScreamEndCallback = nullptr;
	}
}
