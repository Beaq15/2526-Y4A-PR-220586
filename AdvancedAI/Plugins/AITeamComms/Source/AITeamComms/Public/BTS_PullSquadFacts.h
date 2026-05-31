#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"
#include "BTS_PullSquadFacts.generated.h"

UCLASS()
class AITEAMCOMMS_API UBTS_PullSquadFacts : public UBTService
{
    GENERATED_BODY()

public:
    UBTS_PullSquadFacts();

protected:

    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "Facts")
    TArray<FGameplayTag> FactTypesToMonitor;

    void UpdateBlackboardWithFacts(UBehaviorTreeComponent& OwnerComp);

    FName GetBlackboardKeyName(const FGameplayTag& FactTag, const FString& Suffix) const;
};