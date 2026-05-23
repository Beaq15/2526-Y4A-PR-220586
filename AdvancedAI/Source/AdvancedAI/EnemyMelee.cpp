// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMelee.h"


void AEnemyMelee::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackEnd.Broadcast();
}

void AEnemyMelee::OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnEquipWeaponEnd.Broadcast();
}

void AEnemyMelee::OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnDropWeaponEnd.Broadcast();
}

void AEnemyMelee::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (NotifyName == FName("HoldSword"))
	{
		if (!WeaponClass) return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;

		WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);
		if (!WeaponActor) return;

		WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("ik_hand_r_sword_socket"));

		bIsWieldingWeapon = true;

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);
	}

	if (NotifyName == FName("DropSword"))
	{
		WeaponActor->Destroy();
		bIsWieldingWeapon = false;

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);
	}
}

void AEnemyMelee::EquipWeapon_Implementation()
{
	if (EquipSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(EquipSwordMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMelee::OnEquipSwordMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, EquipSwordMontage);
		}
	}
}

void AEnemyMelee::UnequipWeapon_Implementation()
{
	if (DropSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DropSwordMontage, 1.0f);

			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMelee::OnDropSwordMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, DropSwordMontage);
		}
	}
}

void AEnemyMelee::Attack()
{
	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(AttackMontage, 1.0f);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMelee::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
		}
	}
}
