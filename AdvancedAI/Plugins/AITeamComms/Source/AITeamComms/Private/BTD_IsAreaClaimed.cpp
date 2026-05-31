#include "BTD_IsAreaClaimed.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKnowledgeComponent.h"
#include "GameFramework/Pawn.h"

UBTD_IsAreaClaimed::UBTD_IsAreaClaimed()
{
    NodeName = TEXT("Is Area Claimed");
}

bool UBTD_IsAreaClaimed::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const AAIController* AIC = OwnerComp.GetAIOwner();
    const APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    if (!Pawn) return false;

    const UAIKnowledgeComponent* KC = Pawn->FindComponentByClass<UAIKnowledgeComponent>();
    if (!KC) return false;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return false;

    FVector LocationToCheck = BB->GetValueAsVector(LocationKey.SelectedKeyName);
    if (LocationToCheck.IsZero()) return false;

    bool bIsClaimed = KC->IsLocationClaimed(LocationToCheck, CheckRadius, Pawn);

    return bInvertCondition ? !bIsClaimed : bIsClaimed;
}