// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	OnTakeAnyDamage.AddDynamic(this, &AEnemyBase::OnTakeDamage);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

APatrolRoute* AEnemyBase::GetPatrolRoute_Implementation()
{
	return PatrolRoute;
}

float AEnemyBase::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:
		Movement->MaxWalkSpeed = 0.f;
		break;
	case EMovementSpeed::Walking:
		Movement->MaxWalkSpeed = 100.f;
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

void AEnemyBase::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 100.f;
	DefendRadius = 350.f;
}

void AEnemyBase::Heal_Implementation(float HealPercentage)
{
	Health = FMath::Clamp(Health + (HealPercentage * MaxHealth), 0.f, MaxHealth);
}

void AEnemyBase::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	if (Health <= 0)
		isDead = true;
}

void AEnemyBase::Attack()
{

}

