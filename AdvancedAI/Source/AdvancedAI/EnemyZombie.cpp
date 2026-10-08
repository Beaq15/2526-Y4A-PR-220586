// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

AEnemyZombie::AEnemyZombie()
{
	DamageSystem->MaxHealth = 100.f;
	DamageSystem->Health = 100.f;

	PrimaryActorTick.bCanEverTick = true;
}

float AEnemyZombie::SetMovementSpeed_Implementation(EMovementSpeed Speed)
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

void AEnemyZombie::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 200.0f;
	DefendRadius = 200.0f;
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
