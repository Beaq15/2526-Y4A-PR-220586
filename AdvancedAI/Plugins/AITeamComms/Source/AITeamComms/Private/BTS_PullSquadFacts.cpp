#include "BTS_PullSquadFacts.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKnowledgeComponent.h"
#include "SharedFact.h"
#include "GameFramework/Pawn.h"

UBTS_PullSquadFacts::UBTS_PullSquadFacts()
{
    NodeName = TEXT("Pull Squad Facts");
    bNotifyTick = true;
    Interval = 0.5f;  // Check every 0.5 seconds
    RandomDeviation = 0.1f;

    // Default fact types to monitor
    FactTypesToMonitor.Add(FGameplayTag::RequestGameplayTag(FName("Fact.EnemySighted")));
    FactTypesToMonitor.Add(FGameplayTag::RequestGameplayTag(FName("Fact.Heard")));
}

void UBTS_PullSquadFacts::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UpdateBlackboardWithFacts(OwnerComp);
}

void UBTS_PullSquadFacts::UpdateBlackboardWithFacts(UBehaviorTreeComponent& OwnerComp)
{
    const AAIController* AIC = OwnerComp.GetAIOwner();
    const APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    if (!Pawn) return;

    const UAIKnowledgeComponent* KC = Pawn->FindComponentByClass<UAIKnowledgeComponent>();
    if (!KC) return;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    const float Now = Pawn->GetWorld()->GetTimeSeconds();

    // For each fact type we're monitoring
    for (const FGameplayTag& FactTag : FactTypesToMonitor)
    {
        FSharedFact LatestFact;
        bool bHasFact = KC->GetLatestFact(FactTag, LatestFact);

        if (bHasFact && !LatestFact.IsExpired(Now))
        {
            float CurrentConfidence = LatestFact.GetCurrentConfidence(Now);

            if (CurrentConfidence >= KC->MinimumConfidence)
            {
                // Update blackboard with fact data
                FName LocationKey = GetBlackboardKeyName(FactTag, TEXT("Location"));
                FName ActorKey = GetBlackboardKeyName(FactTag, TEXT("Actor"));
                FName ConfidenceKey = GetBlackboardKeyName(FactTag, TEXT("Confidence"));
                FName HasFactKey = GetBlackboardKeyName(FactTag, TEXT("HasFact"));

                BB->SetValueAsVector(LocationKey, LatestFact.Subject.Location);
                BB->SetValueAsObject(ActorKey, LatestFact.Subject.Actor.Get());
                BB->SetValueAsFloat(ConfidenceKey, CurrentConfidence);
                BB->SetValueAsBool(HasFactKey, true);

                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan,
                        FString::Printf(TEXT("%s synced %s to BB (conf: %.2f)"),
                            *Pawn->GetName(),
                            *FactTag.ToString(),
                            CurrentConfidence));
                }
            }
            else
            {
                // Confidence too low - clear the fact
                FName HasFactKey = GetBlackboardKeyName(FactTag, TEXT("HasFact"));
                BB->SetValueAsBool(HasFactKey, false);
            }
        }
        else
        {
            // No fact or expired - clear it
            FName HasFactKey = GetBlackboardKeyName(FactTag, TEXT("HasFact"));
            BB->SetValueAsBool(HasFactKey, false);
        }
    }
}

FName UBTS_PullSquadFacts::GetBlackboardKeyName(const FGameplayTag& FactTag, const FString& Suffix) const
{
    // Extract the last part of the tag (e.g., "Fact.EnemySighted" -> "EnemySighted")
    FString TagString = FactTag.ToString();
    FString LastPart;

    if (TagString.Split(TEXT("."), nullptr, &LastPart, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
    {
        return FName(*(LastPart + TEXT("_") + Suffix));
    }

    return FName(*(TagString + TEXT("_") + Suffix));
}