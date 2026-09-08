// Fill out your copyright notice in the Description page of Project Settings.


#include "AOE_Base.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAOE_Base::AAOE_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
}

// Called when the game starts or when spawned
void AAOE_Base::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerOnBeginPlay)
		Trigger();
}

// Called every frame
void AAOE_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAOE_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
	GetWorldTimerManager().ClearTimer(SpawnIntervalTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AAOE_Base::Trigger()
{
	SpawnAOESphere();

	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AAOE_Base::DestroyActorByTimer, Duration, false);

	if (SpawnInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &AAOE_Base::SpawnAOESphere, SpawnInterval, true);
	}
}

void AAOE_Base::SpawnAOESphere()
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

void AAOE_Base::DestroyActorByTimer()
{
	Destroy();
}