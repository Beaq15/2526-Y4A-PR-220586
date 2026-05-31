#include "BTT_ClaimArea.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKnowledgeComponent.h"
#include "GameFramework/Pawn.h"

UBTT_ClaimArea::UBTT_ClaimArea()
{
    NodeName = TEXT("Claim Search Area");
}

EBTNodeResult::Type UBTT_ClaimArea::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    if (!Pawn) return EBTNodeResult::Failed;

    UAIKnowledgeComponent* KC = Pawn->FindComponentByClass<UAIKnowledgeComponent>();
    if (!KC) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    // Get location to claim from blackboard
    FVector LocationToClaim = BB->GetValueAsVector(LocationKey.SelectedKeyName);

    if (LocationToClaim.IsZero())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
                TEXT("Claim Area: Invalid location in blackboard!"));
        }
        return EBTNodeResult::Failed;
    }

    // Claim the area
    KC->ClaimArea(LocationToClaim, ClaimDuration, Channel);

    return EBTNodeResult::Succeeded;
}

FString UBTT_ClaimArea::GetStaticDescription() const
{
    return FString::Printf(TEXT("Claim area at %s for %.1fs (Ch%d)"),
        *LocationKey.SelectedKeyName.ToString(),
        ClaimDuration,
        Channel);
}