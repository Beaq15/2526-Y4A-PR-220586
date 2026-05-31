#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsAreaClaimed.generated.h"


UCLASS()
class AITEAMCOMMS_API UBTD_IsAreaClaimed : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTD_IsAreaClaimed();

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    /** Location to check */
    UPROPERTY(EditAnywhere, Category = "Claim")
    FBlackboardKeySelector LocationKey;

    /** Check radius */
    UPROPERTY(EditAnywhere, Category = "Claim")
    float CheckRadius = 500.f;

    /** Invert result? (check if NOT claimed) */
    UPROPERTY(EditAnywhere, Category = "Claim")
    bool bInvertCondition = false;
};