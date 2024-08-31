// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/BTService_Detect.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/AI/Monster/GHMonster.h"
#include "Engine/OverlapResult.h"
#include "Controller/AI/GHAIController.h"
#include "Character/AI/Monster/Normal/GHNormalMonster.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");	// BT�� ǥ�õǴ� ����� �̸� ����
	Interval = 1.f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// ���ǵ� Monster ��ü�� �ִ��� Ȯ��
	MonsterPawn = OwnerComp.GetAIOwner()->GetPawn<AGHMonster>();
	if (nullptr == MonsterPawn) return;

	// ���ǵ� ���Ͱ� World �� �����ִ��� Ȯ��
	World = MonsterPawn->GetWorld();
	if (nullptr == World) return;

	// �ֺ� ����
	DetectTarget(OwnerComp);
}

void UBTService_Detect::DetectTarget(UBehaviorTreeComponent& OwnerComp)
{
	Center = MonsterPawn->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams ColliionQueryParma(SCENE_QUERY_STAT(Detact), false, MonsterPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECC_GameTraceChannel5,	// GHMonsterAction
		FCollisionShape::MakeSphere(DetectRadius),
		ColliionQueryParma);

	
	if (bResult)
	{
		// �浹�� ��ü�� �ִ°��
		for (auto const& OverlapResult : OverlapResults)
		{
			APawn* DetectPawn = Cast<APawn>(OverlapResult.GetActor());
			AGHAIController* AIController = Cast<AGHAIController>(MonsterPawn->GetController());
			ETeamAttitude::Type RelationShip = AIController->GetTeamAttitudeTowards(*DetectPawn);

			UObject* PreTarget = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PreTarget"));

			// �̹� ��ϵ� PreTarget �� �ٸ��� �����ΰ�� ���ο� PreTarget ���� ���
			if (PreTarget != DetectPawn
				&& RelationShip == ETeamAttitude::Hostile)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("PreTarget"), DetectPawn);
#if ENABLE_DRAW_DEBUG
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.5f);
				DrawDebugLine(World, Center, DetectPawn->GetActorLocation(), FColor::Red, false, 0.5f);
#endif
			}

			// Normal Monster
			AGHNormalMonster* NormalMonster = Cast<AGHNormalMonster>(MonsterPawn);
			EMonsterState NormalMonsterState = NormalMonster->GetState();
			if (NormalMonster
				&& EMonsterState::DETECT == NormalMonsterState
				&& RelationShip == ETeamAttitude::Hostile)
			{
				NormalMonster->IncreaseWarniss();
#if ENABLE_DRAW_DEBUG
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Yellow, false, 0.5f);
				DrawDebugLine(World, Center, DetectPawn->GetActorLocation(), FColor::Yellow, false, 0.5f);
#endif
				return;
			}
		}
	}

	// ���� ��ü�� Normal Monster �̰� Detect �����̸�,
	// Detect ����� ���� ���
	AGHNormalMonster* NormalMonster = Cast<AGHNormalMonster>(MonsterPawn);
	if (NormalMonster
		&& EMonsterState::DETECT == MonsterPawn->GetState())
	{
		NormalMonster->DecreaseWarniss();
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.5f);
#endif
	
}
