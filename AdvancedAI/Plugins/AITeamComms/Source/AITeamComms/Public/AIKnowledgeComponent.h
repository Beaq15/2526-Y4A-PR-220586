#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SharedFact.h"
#include "AIKnowledgeComponent.generated.h"

class UAICommsSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFactAdded, FSharedFact, Fact);

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class AITEAMCOMMS_API UAIKnowledgeComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UAIKnowledgeComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knowledge") 
    float BroadcastRadius = 3000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knowledge")
    float MinimumConfidence = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knowledge")
    TArray<int32> ListeningChannels = { 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knowledge") 
    TArray<FSharedFact> Facts;

    UPROPERTY(BlueprintAssignable) 
    FOnFactAdded OnFactAdded;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knowledge")
    EAIRole AIRole = EAIRole::Guard; // Default: don't relay

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knowledge")
    bool bCanRebroadcast = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Claims")
    float ClaimRadius = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Claims")
    bool bRespectOtherClaims = true;

    UFUNCTION(BlueprintCallable, Category = "Knowledge")
    bool IsListeningToChannel(int32 Channel) const
    {
        return ListeningChannels.Contains(Channel);
    }

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Knowledge")
    void AddOrUpdateFact(FSharedFact Fact, bool bBroadcast = true);

    UFUNCTION(BlueprintCallable, Category = "Knowledge")
    bool GetLatestFact(FGameplayTag FactType, FSharedFact& OutFact) const;

    UFUNCTION(BlueprintCallable, Category = "Knowledge")
    void EndPlay(const EEndPlayReason::Type EndPlayReason);

    UFUNCTION(BlueprintCallable, Category = "Knowledge")
    bool IsLocationClaimed(const FVector& Location, float CheckRadius, const AActor* IgnoreClaimer = nullptr) const;

    UFUNCTION(BlueprintCallable, Category = "Knowledge")
    void ClaimArea(const FVector& Location, float Duration = 30.f, int32 Channel = 0);

private:
    TWeakObjectPtr<UAICommsSubsystem> Comms;

    void CullExpired();
};