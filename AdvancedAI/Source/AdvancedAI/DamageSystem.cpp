// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem.h"

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------

UDamageSystem::UDamageSystem()
{
	PrimaryComponentTick.bCanEverTick = true;

	Health = MaxHealth;
}

void UDamageSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

//----------------------------------------------------------------------
// TOKENS
//----------------------------------------------------------------------

bool UDamageSystem::ReserveAttackToken(int32 Amount)
{
	if (AttackTokensCount >= Amount)
	{
		AttackTokensCount -= Amount;
		return true;
	}
	return false;
}

void UDamageSystem::ReturnAttackToken(int32 Amount)
{
	AttackTokensCount += Amount;
}

//----------------------------------------------------------------------
// API
//----------------------------------------------------------------------

float UDamageSystem::Heal(float Amount)
{
	if (!isDead)
		Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	return Health;
}

bool UDamageSystem::TakeDamage(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("[TakeDamage] %s Amount=%.1f Health=%.1f/%.1f isDead=%d isBlocking=%d isInterruptible=%d"),
		*GetNameSafe(GetOwner()), DamageInfo.Amount, Health, MaxHealth, isDead, isBlocking, isInterruptible);
	switch (CanBeDamaged(DamageInfo.bCanBeBlocked))
	{
	case EDamageResult::BlockDamage:
		OnBlocked.Broadcast(DamageCauser);
		return false;
		break;
	case EDamageResult::DoDamage:
		Health -=  DamageInfo.Amount;
		if (Health <= 0)
		{
			isDead = true;
			OnDeath.Broadcast();
		}
		else if (isInterruptible)
			OnDamageResponse.Broadcast( DamageCauser);
		return true;
			break;
	case EDamageResult::NoDamage:
		return false;
		break;
	}
	return false;
}

