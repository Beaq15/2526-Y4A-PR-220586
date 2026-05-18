// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyRanged.h"

void AEnemyRanged::BeginPlay()
{
	EquipWeapon_Implementation();
}

void AEnemyRanged::EquipWeapon_Implementation()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);
	if (!WeaponActor) return;

	WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("ik_hand_r_rifle_socket"));

	bIsWieldingWeapon = true;

	OnEquipSwordEnd.Broadcast();
}

