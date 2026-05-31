#include "BTT_AlertSquad.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKnowledgeComponent.h"
#include "SharedFact.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"

UBTT_AlertSquad::UBTT_AlertSquad()
{
    NodeName = TEXT("Alert Squad");
    bNotifyTick = false;

    // Default to "Fact.Alert"
    FactType = FGameplayTag::RequestGameplayTag(FName("Fact.Alert"));
}

EBTNodeResult::Type UBTT_AlertSquad::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }

    UAIKnowledgeComponent* KC = Pawn->FindComponentByClass<UAIKnowledgeComponent>();
    if (!KC)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
                FString::Printf(TEXT("%s: No AIKnowledgeComponent!"), *Pawn->GetName()));
        }
        return EBTNodeResult::Failed;
    }

    // Create the fact
    FSharedFact Fact;
    Fact.FactType = FactType;
    Fact.Confidence = Confidence;
    Fact.TTL = TTL;
    Fact.Channel = Channel;
    Fact.Source = EFactSource::Manual;

    // Determine subject based on type
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    switch (SubjectType)
    {
    case EBTTaskSubjectType::Self:
    {
        // About this AI
        Fact.Subject.Actor = Pawn;
        Fact.Subject.Location = Pawn->GetActorLocation();
        break;
    }

    case EBTTaskSubjectType::Target:
    {
        // About a target from blackboard
        if (BB)
        {
            AActor* Target = Cast<AActor>(BB->GetValueAsObject(SubjectKey.SelectedKeyName));
            if (Target)
            {
                Fact.Subject.Actor = Target;
                Fact.Subject.Location = Target->GetActorLocation();
            }
            else
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange,
                        TEXT("Alert Squad: No target in blackboard!"));
                }
                return EBTNodeResult::Failed;
            }
        }
        else
        {
            return EBTNodeResult::Failed;
        }
        break;
    }

    case EBTTaskSubjectType::Location:
    {
        // About a location from blackboard
        if (BB)
        {
            FVector Location = BB->GetValueAsVector(SubjectKey.SelectedKeyName);
            Fact.Subject.Actor = nullptr;
            Fact.Subject.Location = Location;
        }
        else
        {
            return EBTNodeResult::Failed;
        }
        break;
    }
    }

    // Broadcast the fact
    KC->AddOrUpdateFact(Fact, /*bBroadcast=*/true);

    // Debug message
    if (bShowDebugMessage && GEngine)
    {
        FString SubjectStr = Fact.Subject.Actor.IsValid() ? Fact.Subject.Actor->GetName() : TEXT("Location");
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
            FString::Printf(TEXT("🚨 %s alerted squad: %s about %s (Ch%d)"),
                *Pawn->GetName(),
                *FactType.ToString(),
                *SubjectStr,
                Channel));
    }

    return EBTNodeResult::Succeeded;
}

FString UBTT_AlertSquad::GetStaticDescription() const
{
    FString Description = FString::Printf(TEXT("Alert: %s"), *FactType.ToString());

    Description += FString::Printf(TEXT("\nChannel: %d"), Channel);
    Description += FString::Printf(TEXT("\nConfidence: %.2f"), Confidence);
    Description += FString::Printf(TEXT("\nTTL: %.1fs"), TTL);

    switch (SubjectType)
    {
    case EBTTaskSubjectType::Self:
        Description += TEXT("\nSubject: Self");
        break;
    case EBTTaskSubjectType::Target:
        Description += FString::Printf(TEXT("\nSubject: %s"), *SubjectKey.SelectedKeyName.ToString());
        break;
    case EBTTaskSubjectType::Location:
        Description += FString::Printf(TEXT("\nSubject: Location from %s"), *SubjectKey.SelectedKeyName.ToString());
        break;
    }

    return Description;
}