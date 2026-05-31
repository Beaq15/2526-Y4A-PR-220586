#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SharedFact.generated.h"

UENUM(BlueprintType)
enum class EFactSource : uint8 { Perception, Manual, System };

USTRUCT(BlueprintType)
struct FAISerializedActor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) 
    TWeakObjectPtr<AActor> Actor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) 
    FVector Location = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FSharedFact
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    FGameplayTag FactType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    FAISerializedActor Subject;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    float Confidence = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    float TimeCreated = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    float TTL = 5.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    int32 Channel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    EFactSource Source = EFactSource::Manual;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    float ConfidenceDecayRate = 0.1f; // confidence lost per second

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    int32 CurrentHops = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
    int32 MaxHops = 2;

    bool IsExpired(float Now) const { return TTL >= 0.f && (Now - TimeCreated) > TTL; }

    bool CanMerge(const FSharedFact& Other) const
    {
        return FactType == Other.FactType && Subject.Actor == Other.Subject.Actor && Channel == Other.Channel;
    }

    void Merge(const FSharedFact& Other)
    {
        Confidence = FMath::Max(Confidence, Other.Confidence);
        TTL = FMath::Max(TTL, Other.TTL);
        CurrentHops = FMath::Min(CurrentHops, Other.CurrentHops);
    }

    float GetCurrentConfidence(float Now) const
    {
        if (ConfidenceDecayRate <= 0.f) return Confidence;
        float Age = Now - TimeCreated;
        return FMath::Max(0.f, Confidence - (Age * ConfidenceDecayRate));
    }

    bool CanRelay() const
    {
        return CurrentHops < MaxHops;
    }
};

UENUM(BlueprintType)
enum class EAIRole : uint8
{
    Scout       UMETA(DisplayName = "Scout - Relays information"),
    Guard       UMETA(DisplayName = "Guard - Does not relay"),
    Commander   UMETA(DisplayName = "Commander - Always relays"),
    Support     UMETA(DisplayName = "Support - Selective relay")
};