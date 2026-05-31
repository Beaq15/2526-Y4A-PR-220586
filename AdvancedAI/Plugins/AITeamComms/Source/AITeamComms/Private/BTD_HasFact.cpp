// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_HasFact.h"
#include "AIController.h"
#include "AIKnowledgeComponent.h"
#include "SharedFact.h"
#include "Behaviortree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"


UBTD_HasFact::UBTD_HasFact()
{
	NodeName = TEXT("Has Fact");
    bNotifyTick = false;
}

bool UBTD_HasFact::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    if (!RequiredFact.IsValid())
    {
        return false;
    }

    if (bUseBlackboard)
    {
        return CheckBlackboard(OwnerComp);
    }
    else
    {
        return CheckKnowledgeComponent(OwnerComp);
    }

    //if (auto* BB = OwnerComp.GetBlackboardComponent())
    //{
    //    if (BB->GetValueAsBool(TEXT("Processed_Fact")))
    //    {
    //        return false; 
    //    }
    //}

    //const AAIController* AIC = OwnerComp.GetAIOwner();
    //const APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    //if (!Pawn) return false;

    //const UAIKnowledgeComponent* KC = Pawn->FindComponentByClass<UAIKnowledgeComponent>();
    //if (!KC) return false;

    //FSharedFact Fact;

    //if(KC->GetLatestFact(RequiredFact, Fact))
    //{
    //    const float Now = Pawn->GetWorld()->GetTimeSeconds();
    //    const float CurrentConf = Fact.GetCurrentConfidence(Now);
    //    return CurrentConf >= 0.1f; // Or use KC->MinimumConfidence
    //}

    //return false;
}

bool UBTD_HasFact::CheckBlackboard(UBehaviorTreeComponent& OwnerComp) const
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return false;

    // Check if fact exists via blackboard key
    FName HasFactKey = GetBlackboardKeyName(RequiredFact, TEXT("HasFact"));
    bool bHasFact = BB->GetValueAsBool(HasFactKey);

    if (!bHasFact)
    {
        return false;
    }

    // Optional: Check confidence threshold if specified
    if (MinimumConfidence > 0.0f)
    {
        FName ConfidenceKey = GetBlackboardKeyName(RequiredFact, TEXT("Confidence"));
        float Confidence = BB->GetValueAsFloat(ConfidenceKey);

        if (Confidence < MinimumConfidence)
        {
            return false;
        }
    }

    return true;
}

bool UBTD_HasFact::CheckKnowledgeComponent(UBehaviorTreeComponent& OwnerComp) const
{
    const AAIController* AIC = OwnerComp.GetAIOwner();
    const APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    if (!Pawn) return false;

    const UAIKnowledgeComponent* KC = Pawn->FindComponentByClass<UAIKnowledgeComponent>();
    if (!KC) return false;

    FSharedFact Fact;
    if (!KC->GetLatestFact(RequiredFact, Fact))
    {
        return false;
    }

    // Check if expired
    const float Now = Pawn->GetWorld()->GetTimeSeconds();
    if (Fact.IsExpired(Now))
    {
        return false;
    }

    // Check confidence
    const float CurrentConf = Fact.GetCurrentConfidence(Now);
    const float MinConf = (MinimumConfidence > 0.0f) ? MinimumConfidence : KC->MinimumConfidence;

    return CurrentConf >= MinConf;
}

FName UBTD_HasFact::GetBlackboardKeyName(const FGameplayTag& FactTag, const FString& Suffix) const
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

FString UBTD_HasFact::GetStaticDescription() const
{
    FString Description = FString::Printf(TEXT("Has Fact: %s"), *RequiredFact.ToString());

    if (MinimumConfidence > 0.0f)
    {
        Description += FString::Printf(TEXT("\nMin Confidence: %.2f"), MinimumConfidence);
    }

    Description += bUseBlackboard ? TEXT("\n(Using Blackboard)") : TEXT("\n(Direct Query)");

    return Description;
}