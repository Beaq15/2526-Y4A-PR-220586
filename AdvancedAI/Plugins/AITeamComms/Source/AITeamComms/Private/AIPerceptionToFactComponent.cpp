#include "AIPerceptionToFactComponent.h"
#include "AIKnowledgeComponent.h"
#include "SharedFact.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

UAIPerceptionToFactComponent::UAIPerceptionToFactComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAIPerceptionToFactComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    ACharacter* Character = Cast<ACharacter>(Owner);
    if (Character)
    {
        AController* Controller = Character->GetController();
        if (Controller)
            PerceptionComp = Controller->FindComponentByClass<UAIPerceptionComponent>();
    }

    KnowledgeComp = Owner->FindComponentByClass<UAIKnowledgeComponent>();

    if (!PerceptionComp || !KnowledgeComp)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("%s: Missing PerceptionComp or KnowledgeComp!"), *Owner->GetName()));
        }
        return;
    }

    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UAIPerceptionToFactComponent::OnPerceptionUpdated);
}

void UAIPerceptionToFactComponent::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || !KnowledgeComp) return;

    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        HandleSightStimulus(Actor, Stimulus);
    }
    else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        HandleHearingStimulus(Actor, Stimulus);
    }
    else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
    {
        HandleDamageStimulus(Actor, Stimulus);
    }
}

void UAIPerceptionToFactComponent::HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
    if (Stimulus.WasSuccessfullySensed()) 
    {

        FSharedFact Fact;
        Fact.FactType = FGameplayTag::RequestGameplayTag(FName("Fact.EnemySighted"));
        Fact.Subject.Actor = Actor;
        Fact.Subject.Location = Stimulus.StimulusLocation;
        Fact.Confidence = 1.f;
        Fact.TTL = SightFactTTL;
        Fact.Channel = 1;
        Fact.Source = EFactSource::Perception;

        if (!KnowledgeComp->IsListeningToChannel(Fact.Channel))
        {
            return; 
        }

        KnowledgeComp->AddOrUpdateFact(Fact, true);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
                FString::Printf(TEXT("%s saw %s!"), *GetOwner()->GetName(), *Actor->GetName()));
        }
    }
    else  // Lost sight
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
                FString::Printf(TEXT("%s lost sight of %s"), *GetOwner()->GetName(), *Actor->GetName()));
        }
    }
}

void UAIPerceptionToFactComponent::HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        FSharedFact Fact;
        Fact.FactType = FGameplayTag::RequestGameplayTag(FName("Fact.Heard"));
        Fact.Subject.Actor = Actor;
        Fact.Subject.Location = Stimulus.StimulusLocation;
        Fact.Confidence = 0.7f;  // Hearing is less certain than sight
        Fact.TTL = HearingFactTTL;
        Fact.Channel = 2;
        Fact.Source = EFactSource::Perception;

        if (!KnowledgeComp->IsListeningToChannel(Fact.Channel))
        {
            return;
        }

        KnowledgeComp->AddOrUpdateFact(Fact, true);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
                FString::Printf(TEXT("%s heard noise at %s!"), *GetOwner()->GetName(), *Stimulus.StimulusLocation.ToString()));
        }
    }
    else  // Lost hearing
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
                FString::Printf(TEXT("%s lost hearing of %s"), *GetOwner()->GetName(), *Actor->GetName()));
        }
    }
}

void UAIPerceptionToFactComponent::HandleDamageStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        FSharedFact Fact;
        Fact.FactType = FGameplayTag::RequestGameplayTag(FName("Fact.UnderAttack"));
        Fact.Subject.Actor = Actor;
        Fact.Subject.Location = Stimulus.StimulusLocation;
        Fact.Confidence = 1.f;  // Damage is certain
        Fact.TTL = DamageFactTTL;
        Fact.Channel = 3;
        Fact.Source = EFactSource::Perception;

        KnowledgeComp->AddOrUpdateFact(Fact, true);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("%s is under attack by %s!"), *GetOwner()->GetName(), *Actor->GetName()));
        }
    }
}