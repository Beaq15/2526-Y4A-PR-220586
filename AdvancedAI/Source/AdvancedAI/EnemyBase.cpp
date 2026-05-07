// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Animation/AnimMontage.h"
//#include "Engine/ActorSpawnParameters.h"

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

void AEnemyBase::Attack()
{
	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(AttackMontage, 1.0f);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyBase::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
		}
	}
}

void AEnemyBase::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
		OnAttackEnd.Broadcast();
}

void AEnemyBase::WieldSword()
{
	if (!SwordClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	SpawnedSword = GetWorld()->SpawnActor<AActor>(SwordClass, GetActorTransform(), SpawnParams);
	if (!SpawnedSword) return;

	SpawnedSword->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("ik_hand_r_sword_socket"));

	bIsWieldingSword = true;
}

