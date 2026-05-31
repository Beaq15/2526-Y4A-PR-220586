#include "AIKnowledgeComponent.h"
#include "AICommsSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UAIKnowledgeComponent::UAIKnowledgeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

}

void UAIKnowledgeComponent::BeginPlay()
{
    Super::BeginPlay();
    PrimaryComponentTick.bCanEverTick = true;
    if (UWorld* W = GetWorld())
    {
        if (auto* GI = W->GetGameInstance())
        {
            Comms = GI->GetSubsystem<UAICommsSubsystem>();
            if (Comms.IsValid()) { Comms->Register(this); }
        }
    }

}

void UAIKnowledgeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (Comms.IsValid()) { Comms->Unregister(this); }
    Super::EndPlay(EndPlayReason);
}

bool UAIKnowledgeComponent::IsLocationClaimed(const FVector& Location, float CheckRadius, const AActor* IgnoreClaimer) const
{
    if (!GetWorld()) return false;

    const float Now = GetWorld()->GetTimeSeconds();

    for (const FSharedFact& F : Facts)
    {
        if (F.FactType.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Fact.AreaClaim"))) ||
            F.FactType.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Fact.Searching"))))
        {
            if (F.IsExpired(Now) || F.GetCurrentConfidence(Now) < MinimumConfidence)
                continue;

            if (IgnoreClaimer && F.Subject.Actor == IgnoreClaimer)
                continue;

            float Distance = FVector::Dist(Location, F.Subject.Location);
            float CombinedRadius = CheckRadius + ClaimRadius;

            if (Distance < CombinedRadius)
            {
                if (GEngine)
                {
                    FString ClaimerName = F.Subject.Actor.IsValid() ?
                        F.Subject.Actor->GetName() : TEXT("Unknown");

                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
                        FString::Printf(TEXT("%s: Location claimed by %s (%.0f units away)"),
                            *GetOwner()->GetName(),
                            *ClaimerName,
                            Distance));
                }
                return true;
            }
        }
    }

    return false;  
}

void UAIKnowledgeComponent::ClaimArea(const FVector& Location, float Duration, int32 Channel)
{
    FSharedFact Claim;
    Claim.FactType = FGameplayTag::RequestGameplayTag(FName("Fact.Searching"));
    Claim.Subject.Actor = GetOwner();
    Claim.Subject.Location = Location;
    Claim.Confidence = 1.0f;  
    Claim.TTL = Duration;
    Claim.Channel = Channel;
    Claim.Source = EFactSource::Manual;
    Claim.CurrentHops = 0;
    Claim.MaxHops = 1;

    AddOrUpdateFact(Claim, true);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
            FString::Printf(TEXT("🏁 %s claimed area at %s (%.0fs)"),
                *GetOwner()->GetName(),
                *Location.ToString(),
                Duration));
    }
}

void UAIKnowledgeComponent::TickComponent(float Dt, ELevelTick, FActorComponentTickFunction*)
{
    CullExpired();

    if (GEngine && Facts.Num() > 0)
    {
        const float Now = GetWorld()->GetTimeSeconds();
        for (const FSharedFact& F : Facts)
        {
            float CurrentConf = F.GetCurrentConfidence(Now);
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
                FString::Printf(TEXT("%s: %s confidence %.2f"),
                    *GetOwner()->GetName(),
                    *F.FactType.ToString(),
                    CurrentConf));
        }
    }
}

void UAIKnowledgeComponent::CullExpired()
{
    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
    Facts.RemoveAll([&](const FSharedFact& F) {
        return F.IsExpired(Now) || F.GetCurrentConfidence(Now) < MinimumConfidence;
        });
}

void UAIKnowledgeComponent::AddOrUpdateFact(FSharedFact Fact, bool bBroadcast)
{
    if (UWorld* W = GetWorld())
    {
        Fact.TimeCreated = W->GetTimeSeconds();
    }

    // Merge or add
    for (FSharedFact& Existing : Facts)
    {
        if (Existing.CanMerge(Fact))
        {
            Existing.Merge(Fact);
            OnFactAdded.Broadcast(Existing);
            if (bBroadcast && Comms.IsValid()) { Comms->BroadcastFact(this, Existing); }
            return;
        }
    }

    Facts.Add(Fact);
    OnFactAdded.Broadcast(Fact);
    if (bBroadcast && Comms.IsValid()) { Comms->BroadcastFact(this, Fact); }
}

bool UAIKnowledgeComponent::GetLatestFact(FGameplayTag FactType, FSharedFact& OutFact) const
{
    float BestTime = -FLT_MAX;
    bool bFound = false;
    for (const FSharedFact& F : Facts)
    {
        if (F.FactType == FactType && F.TimeCreated > BestTime)
        {
            BestTime = F.TimeCreated;
            OutFact = F;
            bFound = true;
        }
    }
    return bFound;
}