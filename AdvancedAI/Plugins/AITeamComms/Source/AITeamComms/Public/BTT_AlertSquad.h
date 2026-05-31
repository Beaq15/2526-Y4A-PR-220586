#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTT_AlertSquad.generated.h"

UENUM(BlueprintType)
enum EBTTaskSubjectType
{
    Self        UMETA(DisplayName = "Self (This AI)"),
    Target      UMETA(DisplayName = "Target (From Blackboard)"),
    Location    UMETA(DisplayName = "Location (From Blackboard)")
};

UCLASS()
class AITEAMCOMMS_API UBTT_AlertSquad : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTT_AlertSquad();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;

    /** What type of fact to broadcast */
    UPROPERTY(EditAnywhere, Category = "Alert", meta = (Categories = "Fact"))
    FGameplayTag FactType;

    /** Subject of the fact - what/who is this about? */
    UPROPERTY(EditAnywhere, Category = "Alert")
    TEnumAsByte<EBTTaskSubjectType> SubjectType;

    /** Initial confidence (how sure are we?) */
    UPROPERTY(EditAnywhere, Category = "Alert", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Confidence = 1.0f;

    /** How long the fact should last (seconds) */
    UPROPERTY(EditAnywhere, Category = "Alert")
    float TTL = 10.0f;

    /** Which communication channel to use */
    UPROPERTY(EditAnywhere, Category = "Alert")
    int32 Channel = 0;

    /** Optional: Use a blackboard key for the subject instead */
    UPROPERTY(EditAnywhere, Category = "Alert")
    FBlackboardKeySelector SubjectKey;

    /** Should we print a debug message? */
    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bShowDebugMessage = true;
};