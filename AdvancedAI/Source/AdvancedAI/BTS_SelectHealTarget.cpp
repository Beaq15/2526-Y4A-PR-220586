#include "BTS_SelectHealTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.h"
#include "AITeamComms/Public/AIKnowledgeComponent.h"
#include "AITeamComms/Public/SharedFact.h"

UBTS_SelectHealTarget::UBTS_SelectHealTarget()
{
    NodeName = TEXT("Select Heal Target");
    Interval = 0.3f;
    RandomDeviation = 0.05f;
}

void UBTS_SelectHealTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIC = OwnerComp.GetAIOwner();
    APawn* Mage = AIC ? AIC->GetPawn() : nullptr;
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!Mage || !BB) return;

    UAIKnowledgeComponent* Knowledge = Mage->FindComponentByClass<UAIKnowledgeComponent>();
    if (!Knowledge) return;

    const float Now = OwnerComp.GetWorld()->GetTimeSeconds();
    const FGameplayTag HealTag = FGameplayTag::RequestGameplayTag(FName("Fact.NeedsHeal"));
    const float RadiusSq = FMath::Square(SupportRadius);

    AEnemyBase* BestAlly = nullptr;
    float LowestHealthPct = TNumericLimits<float>::Max();

    for (const FSharedFact& Fact : Knowledge->Facts)
    {
        if (Fact.FactType != HealTag) continue;
        if (Fact.IsExpired(Now)) continue;
        if (Fact.GetCurrentConfidence(Now) < MinRequestConfidence) continue;

        AEnemyBase* Ally = Cast<AEnemyBase>(Fact.Subject.Actor.Get());
        if (!Ally || Ally == Mage) continue;

        if (!Ally->NeedsHealing()) continue;

        const float DistSq = FVector::DistSquared(Mage->GetActorLocation(), Ally->GetActorLocation());
        if (DistSq > RadiusSq) continue;

        const float Pct = Ally->GetHealthPct();
        if (Pct < LowestHealthPct)
        {
            LowestHealthPct = Pct;
            BestAlly = Ally;
        }
    }

    BB->SetValueAsObject(HealTargetKeyName, BestAlly);
}