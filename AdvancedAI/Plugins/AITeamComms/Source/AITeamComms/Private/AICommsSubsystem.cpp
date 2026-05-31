#include "AICommsSubsystem.h"
#include "AIKnowledgeComponent.h"
#include "SharedFact.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

void UAICommsSubsystem::BroadcastFact(UAIKnowledgeComponent* Sender, const FSharedFact& Fact)
{
    if (!Sender || !Sender->GetOwner()) return;

    if (!Fact.CanRelay())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("%s: Fact reached MaxHops (%d), not broadcasting"),
                    *Sender->GetOwner()->GetName(),
                    Fact.MaxHops));
        }
        return;  // Don't broadcast - hop limit reached
    }

    const FVector Origin = Sender->GetOwner()->GetActorLocation();
    const float Radius = Sender->BroadcastRadius;
    const int32 Channel = Fact.Channel;
    const float CurrentTime = GetWorld()->GetTimeSeconds();

    for (auto It = KnowledgeComponents.CreateIterator(); It; ++It)
    {
        if (UAIKnowledgeComponent* Receiver = It->Get())
        {
            if (Receiver == Sender || !Receiver->GetOwner()) continue;

            const bool bListeningToChannel = Receiver->IsListeningToChannel(Channel);
            const bool bInRange = FVector::DistSquared(Origin, Receiver->GetOwner()->GetActorLocation()) <= FMath::Square(Radius);
            const FVector ReceiverLocation = Receiver->GetOwner()->GetActorLocation();

            if (bListeningToChannel && bInRange)
            {
                FSharedFact Copy = Fact;
                Copy.Source = EFactSource::System;
                Copy.CurrentHops ++ ;

                float Distance = FVector::Dist(Origin, Receiver->GetOwner()->GetActorLocation());
                float DistanceFactor = FMath::Clamp(1.f - (Distance / Radius), 0.2f, 1.f);
                Copy.Confidence *= DistanceFactor;

                bool bHasLOS = HasLineOfSight(Origin, ReceiverLocation, Sender->GetOwner());


                if (!bHasLOS)
                {
                    Copy.Confidence *= 0.3f; // 70% signal loss through walls

                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
                            FString::Printf(TEXT("%s -> %s: LOS BLOCKED (confidence reduced to %.2f)"),
                                *Sender->GetOwner()->GetName(),
                                *Receiver->GetOwner()->GetName(),
                                Copy.Confidence));
                    }
                }
                else
                {
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
                            FString::Printf(TEXT("%s -> %s: Clear LOS (confidence %.2f)"),
                                *Sender->GetOwner()->GetName(),
                                *Receiver->GetOwner()->GetName(),
                                Copy.Confidence));
                    }
                }

                if (bEnableLatency)
                {
                    // Calculate delivery delay: base latency + distance-based latency
                    float Latency = BaseLatency + (Distance * LatencyPerMeter);
                    float DeliveryTime = CurrentTime + Latency;

                    // Queue for delayed delivery
                    PendingDeliveries.Add(FPendingFactDelivery(Receiver, Copy, DeliveryTime));

                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
                            FString::Printf(TEXT("📡 Queued for %s (%.2fs delay)"),
                                *Receiver->GetOwner()->GetName(),
                                Latency));
                    }
                }
                else
                {
                    // Immediate delivery (no latency)
                    DeliverFactImmediate(Receiver, Copy);
                }

               // Receiver->AddOrUpdateFact(Copy, false);
            }
            else if (!bListeningToChannel)
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                        FString::Printf(TEXT("%s blocked fact - not listening to Channel %d"),
                            *Receiver->GetOwner()->GetName(), Channel));
                }
            }
        }
        else
        {
            It.RemoveCurrent();
        }
    }
}

void UAICommsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Enable ticking for this subsystem
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TickTimerHandle,
            FTimerDelegate::CreateUObject(this, &UAICommsSubsystem::TickSubsystem, 0.016f),
            0.016f,  // Tick every ~60fps
            true
        );
    }
}

void UAICommsSubsystem::Deinitialize()
{
    KnowledgeComponents.Empty();
    PendingDeliveries.Empty();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TickTimerHandle);
    }

    Super::Deinitialize();
}

void UAICommsSubsystem::TickSubsystem(float DeltaTime)
{
    if (!GetWorld()) return;

    const float CurrentTime = GetWorld()->GetTimeSeconds();

    // Process pending deliveries
    for (int32 i = PendingDeliveries.Num() - 1; i >= 0; --i)
    {
        FPendingFactDelivery& Pending = PendingDeliveries[i];

        // Time to deliver?
        if (CurrentTime >= Pending.DeliveryTime)
        {
            // Deliver if receiver still valid
            if (Pending.Receiver.IsValid())
            {

                UAIKnowledgeComponent* ReceiverComp = Pending.Receiver.Get();
                if (ReceiverComp && ReceiverComp->GetOwner())  // Verify owner exists too
                {
                    DeliverFactImmediate(Pending.Receiver.Get(), Pending.Fact);

                    /*if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan,
                            FString::Printf(TEXT("⏰ Delivered to %s (%.2fs delay)"),
                                *Pending.Receiver->GetOwner()->GetName(),
                                CurrentTime - (Pending.DeliveryTime - BaseLatency)));
                    }*/
                }
            }

            // Remove from queue
            PendingDeliveries.RemoveAt(i);
        }
    }
}

void UAICommsSubsystem::DeliverFactImmediate(UAIKnowledgeComponent* Receiver, const FSharedFact& Fact)
{
    if (!Receiver) return;

    bool bShouldRebroadcast = false;

    if (Receiver->bCanRebroadcast && Fact.CanRelay())
    {
        bShouldRebroadcast = true;
    }

    if (GEngine && bShouldRebroadcast)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta,
            FString::Printf(TEXT("%s will RELAY fact (hop %d→%d)"),
                *Receiver->GetOwner()->GetName(),
                Fact.CurrentHops,
                Fact.CurrentHops + 1));
    }

    Receiver->AddOrUpdateFact(Fact, bShouldRebroadcast);
}

bool UAICommsSubsystem::HasLineOfSight(const FVector& Start, const FVector& End, AActor* IgnoreActor) const
{
    if (!GetWorld()) return false;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(IgnoreActor);
    QueryParams.bTraceComplex = false;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        QueryParams
    );

    return !bHit;
}