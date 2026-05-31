#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ClaimArea.generated.h"


UCLASS()
class AITEAMCOMMS_API UBTT_ClaimArea : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTT_ClaimArea();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;

    /** Where to claim - from blackboard */
    UPROPERTY(EditAnywhere, Category = "Claim")
    FBlackboardKeySelector LocationKey;

    /** How long to hold the claim */
    UPROPERTY(EditAnywhere, Category = "Claim")
    float ClaimDuration = 30.f;

    /** Which channel to broadcast on */
    UPROPERTY(EditAnywhere, Category = "Claim")
    int32 Channel = 0;
};