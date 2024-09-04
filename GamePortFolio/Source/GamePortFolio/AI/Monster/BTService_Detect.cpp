// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/BTService_Detect.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/AI/Monster/GHMonster.h"
#include "Engine/OverlapResult.h"
#include "Controller/AI/GHAIController.h"
#include "Character/AI/Monster/Normal/GHNormalMonster.h"
// EpicMonster Section ��
#include "Character/AI/Monster/Epic/GHEpicMonster.h"

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

	AGHNormalMonster* Normal = Cast<AGHNormalMonster>(MonsterPawn);
	if (IsValid(Normal)) DetectRadius = 600.f;

	AGHEpicMonster* Epic = Cast<AGHEpicMonster>(MonsterPawn);
	if (IsValid(Epic)) DetectRadius = 1000.f;

	// ���ǵ� ���Ͱ� World �� �����ִ��� Ȯ��
	World = MonsterPawn->GetWorld();
	if (nullptr == World) return;

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

	if (false == bResult)
	{
#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.5f);
#endif
		return;
	}

	// Normal Monster �� Detect System
	if (nullptr != Normal)
	{
		DetectTarget_Normal(OwnerComp, OverlapResults);
		return;
	}

	// Epic Monster �� Detect System
	if (nullptr != Epic)
	{
		DetectTarget_Epic(OwnerComp, OverlapResults);
		return;
	}

}

void UBTService_Detect::DetectTarget_Normal(UBehaviorTreeComponent& OwnerComp, TArray<FOverlapResult>& OverlapResults)
{
	// �浹�� ��ü�� �ִ°��
	for (auto const& OverlapResult : OverlapResults)
	{
		// �浹�� ��ü�� ���� ������
		APawn* CollisionPawn = Cast<APawn>(OverlapResult.GetActor());
		// �浹�� ��ü���� ���踦 ������ (����?)
		AGHAIController* AIController = Cast<AGHAIController>(MonsterPawn->GetController());
		ETeamAttitude::Type RelationShip = AIController->GetTeamAttitudeTowards(*CollisionPawn);
		// BB���� PreTarget �� ���� ������
		UObject* PreTarget = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PreTarget"));

		// �浹�� ��ü�� �̹� ��ϵ� PreTarget �� �ٸ��� �����ΰ�� ���ο� PreTarget ���� ���
		if (PreTarget != CollisionPawn
			&& RelationShip == ETeamAttitude::Hostile)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("PreTarget"), CollisionPawn);
#if ENABLE_DRAW_DEBUG
			DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.5f);
			DrawDebugLine(World, Center, CollisionPawn->GetActorLocation(), FColor::Red, false, 0.5f);
#endif
		}

		AGHNormalMonster* NoramlMonster = Cast<AGHNormalMonster>(MonsterPawn);
		EMonsterState NormalMonsterState = MonsterPawn->GetState();
		if (IsValid(NoramlMonster)
			&& EMonsterState::DETECT == NormalMonsterState
			&& RelationShip == ETeamAttitude::Hostile)
		{
			int32 Warniss = OwnerComp.GetBlackboardComponent()->GetValueAsInt(TEXT("Warniss"));
			Warniss += 1;
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("Warniss"), Warniss);

#if ENABLE_DRAW_DEBUG
			DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Yellow, false, 0.5f);
			DrawDebugLine(World, Center, CollisionPawn->GetActorLocation(), FColor::Yellow, false, 0.5f);
#endif
			return;
		}
	}

	// ���� ��ü�� Normal Monster �̰� Detect �����̸�,
	// Detect ����� ���� ���
	AGHNormalMonster* NormalMonster = Cast<AGHNormalMonster>(MonsterPawn);
	if (NormalMonster
		&& EMonsterState::DETECT == MonsterPawn->GetState())
	{
		int32 Warniss = OwnerComp.GetBlackboardComponent()->GetValueAsInt(TEXT("Warniss"));
		Warniss -= 1;
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("Warniss"), Warniss);
	}
	
}

void UBTService_Detect::DetectTarget_Epic(UBehaviorTreeComponent& OwnerComp, TArray<FOverlapResult>& OverlapResults)
{
	// �浹�� ��ü�� �ִ°��
	for (auto const& OverlapResult : OverlapResults)
	{
		// �浹�� ��ü�� ���� ������
		APawn* CollisionPawn = Cast<APawn>(OverlapResult.GetActor());
		// �浹�� ��ü���� ���踦 ������ (����?)
		AGHAIController* AIController = Cast<AGHAIController>(MonsterPawn->GetController());
		ETeamAttitude::Type RelationShip = AIController->GetTeamAttitudeTowards(*CollisionPawn);

		// �浹�� ��ü�� �����ΰ�� ���ο� Target ���� ���
		if (RelationShip == ETeamAttitude::Hostile)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), CollisionPawn);
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsDetect"), true);

#if ENABLE_DRAW_DEBUG
			DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.5f);
			DrawDebugLine(World, Center, CollisionPawn->GetActorLocation(), FColor::Red, false, 0.5f);
#endif
		}
	}

}