// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem.h"

// Sets default values
UDamageSystem::UDamageSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void UDamageSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

float UDamageSystem::Heal(float Amount)
{
	if (!isDead)
		Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	return Health;
}

bool UDamageSystem::TakeDamage(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	switch (CanBeDamaged(DamageInfo.bShouldDamageInvincible, DamageInfo.bCanBeBlocked))
	{
	case EDamageResult::BlockDamage:
		OnBlocked.Broadcast(DamageInfo.bCanBeParried, DamageCauser);
		return false;
		break;
	case EDamageResult::DoDamage:
		Health = Health - DamageInfo.Amount;
		if (Health <= 0)
		{
			isDead = true;
			OnDeath.Broadcast();
		}
		else if (isInterruptible || DamageInfo.bShouldForceInterrupt)
			break;
	case EDamageResult::NoDamage:
		break;
	}
	return false;
}

// Called every frame
void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

