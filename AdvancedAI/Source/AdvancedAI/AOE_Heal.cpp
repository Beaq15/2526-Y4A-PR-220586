// Fill out your copyright notice in the Description page of Project Settings.


#include "AOE_Heal.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

AAOE_Heal::AAOE_Heal()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
}

void AAOE_Heal::BeginPlay()
{
	Super::BeginPlay();
	
	if (TriggerOnBeginPlay)
		Trigger();
}

void AAOE_Heal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAOE_Heal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
	GetWorldTimerManager().ClearTimer(SpawnIntervalTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AAOE_Heal::Trigger()
{
	SpawnAOESphere();

	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AAOE_Heal::DestroyActorByTimer, Duration, false);

	if (SpawnInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &AAOE_Heal::SpawnAOESphere, SpawnInterval, true);
	}

	UGameplayStatics::SpawnEmitterAttached(
		P_HealAura, RootComponent, FName("None"), FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector);
}

void AAOE_Heal::SpawnAOESphere()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnAOESphere called at time %f"), GetWorld()->GetTimeSeconds());
	
	if (DrawDebugSphere)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 12, FLinearColor::Blue, 1.0f, 1.0f);
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	if (IgnoreInstigator)
		if (AActor* Inst = GetInstigator())
		{
			ActorsToIgnore.Add(Inst);
		}
	
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), Radius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);

	for (AActor* Actors : OutActors)
	{
		OnAOEOverlapActor.Broadcast(Actors);
	}
}

void AAOE_Heal::DestroyActorByTimer()
{
	Destroy();
}
