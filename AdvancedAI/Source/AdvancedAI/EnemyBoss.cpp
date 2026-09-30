// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "GameFramework/CharacterMovementComponent.h"

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

void AEnemyBoss::AttackCombo1(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 20.0f;
	DamageInfo.DamageType = EDamageType::Melee;
	DamageInfo.DamageResponse = EDamageResponse::HitReaction;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = AxeComboMontage1;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->ShortRange(AttackInfo, 40.0f, 250.f);
}