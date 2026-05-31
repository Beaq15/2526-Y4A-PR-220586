#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SharedFact.h"
#include "AIKnowledgeComponent.h"
#include "AICommsSubsystem.generated.h"


USTRUCT()
struct FPendingFactDelivery
{
    GENERATED_BODY()

    TWeakObjectPtr<UAIKnowledgeComponent> Receiver;

    UPROPERTY()
    FSharedFact Fact;

    UPROPERTY()
    float DeliveryTime = 0.f;  // World time when this should be delivered

    FPendingFactDelivery() {}

    FPendingFactDelivery(UAIKnowledgeComponent* InReceiver, const FSharedFact& InFact, float InDeliveryTime)
        : Fact(InFact), DeliveryTime(InDeliveryTime)
    {
        Receiver = InReceiver;
    }
};

UCLASS()
class AITEAMCOMMS_API UAICommsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    void BroadcastFact(UAIKnowledgeComponent* Sender, const FSharedFact& Fact);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Communication")
    float BaseLatency = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Communication")
    float LatencyPerMeter = 0.0001f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Communication")
    bool bEnableLatency = true;

    void Register(UAIKnowledgeComponent* Comp) { KnowledgeComponents.Add(Comp); }
    void Unregister(UAIKnowledgeComponent* Comp) { KnowledgeComponents.Remove(Comp); }

private:
    TSet<TWeakObjectPtr<UAIKnowledgeComponent>> KnowledgeComponents;

    UPROPERTY()
    TArray<FPendingFactDelivery> PendingDeliveries;

    void TickSubsystem(float DeltaTime);

    void DeliverFactImmediate(UAIKnowledgeComponent* Receiver, const FSharedFact& Fact);

    bool HasLineOfSight(const FVector& Start, const FVector& End, AActor* IgnoreActor) const;

    FTimerHandle TickTimerHandle;
};