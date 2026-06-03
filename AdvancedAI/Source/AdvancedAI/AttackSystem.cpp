// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSystem.h"
#include "DamageableInterface.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UAttackSystem::UAttackSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttackSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttackSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttackSystem::MagicSpell(FTransform SpawnTransform, AActor* TargetActor, FDamageInfo DamageInfo)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, SpawnTransform, SpawnParams);

	Projectile->BoxCollision->IgnoreActorWhenMoving(GetOwner(), true);
	
	Projectile->Speed = 1000.f;
	Projectile->Target = TargetActor;
	DamageInfoRef = DamageInfo;

	Projectile->OnProjectileImpact.AddDynamic(this, &UAttackSystem::OnProjectileHit);
}

void UAttackSystem::FireBullet(FVector TraceStart, FVector TraceEnd, FDamageInfo DamageInfo)
{
	FHitResult HitResult;

	TArray <TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, TraceStart, TraceEnd, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor && HitActor->Implements<UDamageableInterface>())
		{
			DamageInfo.Amount = 20.f;
			DamageInfo.DamageType = EDamageType::Projectile;
			DamageInfo.DamageResponse = EDamageResponse::HitReaction;
			IDamageableInterface::Execute_TakeDamage(HitActor, DamageInfo, GetOwner());
		}

		UAISense_Damage::ReportDamageEvent(GetWorld(), HitActor, GetOwner(), DamageInfo.Amount, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation());
	}
}

void UAttackSystem::OnProjectileHit(AActor* OtherActor, FHitResult Hit)
{
	if (!IsValid(OtherActor))
		return;
	if (!OtherActor->Implements<UDamageableInterface>())
		return;

	IDamageableInterface::Execute_TakeDamage(OtherActor, DamageInfoRef, GetOwner());

	UAISense_Damage::ReportDamageEvent(GetWorld(), OtherActor, GetOwner(), DamageInfoRef.Amount, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation());

	OnAttackEnd.Broadcast();
}

