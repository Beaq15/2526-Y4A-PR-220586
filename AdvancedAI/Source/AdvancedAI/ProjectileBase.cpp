// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Kismet/GameplayStatics.h"

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollision->SetNotifyRigidBodyCollision(true);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SetRootComponent(BoxCollision);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(BoxCollision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(BoxCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollision->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = Gravity;

	if (bIsHoming && IsValid(RootComponent))
	{
		ProjectileMovement->HomingTargetComponent = RootComponent;
		ProjectileMovement->bIsHomingProjectile = true;
	}

	RotateToTarget();
	PlaySpawnSound();

	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
		BoxCollision->IgnoreActorWhenMoving(OwnerActor, true);

	AActor* InstigatorActor = GetInstigator();
	if (InstigatorActor)
		BoxCollision->IgnoreActorWhenMoving(InstigatorActor, true);
}

//----------------------------------------------------------------------
// Helpers
//----------------------------------------------------------------------

void AProjectileBase::RotateToTarget()
{
	FVector Velocity;

	if (Target)
	{
		FVector From = GetActorLocation();
		FVector To = Target->GetActorLocation();

		FVector Direction = (To - From).GetSafeNormal();
		Velocity = Direction * Speed;
	}
	else
		Velocity = GetActorForwardVector() * Speed;

	ProjectileMovement->Velocity = Velocity;
}

void AProjectileBase::PlayImpactSound(FVector Location) const
{
	if (ImpactSound)	
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
}

void AProjectileBase::PlaySpawnSound() const
{
	if (SpawnSound)
		UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation());
}

void AProjectileBase::SpawnImpactEffect(FVector Location) const
{
	if (ImpactEffect)
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, Location, FRotator::ZeroRotator, FVector(1.0f), true);
}

//----------------------------------------------------------------------
// Callbacks
//----------------------------------------------------------------------

void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector ImpactPoint = Hit.ImpactPoint;
	OnProjectileImpact.Broadcast(OtherActor, Hit);

	SpawnImpactEffect(ImpactPoint);
	PlayImpactSound(ImpactPoint);

	Destroy();
}

