// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMage.h"
#include "GameFramework/CharacterMovementComponent.h"


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

	if (FireMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(FireMontage, 1.0f);
		}
	}
}

void AEnemyMage::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{

}