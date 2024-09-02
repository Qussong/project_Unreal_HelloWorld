// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI/Monster/Normal/GHNormalMonsterController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

AGHNormalMonsterController::AGHNormalMonsterController()
{
	// BB Section
	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		NormalMonsterBBRef(TEXT("/Script/AIModule.BlackboardData'/Game/Gihoon/AI/NormalMonster/BB_NormalMonster.BB_NormalMonster'"));
	if (NormalMonsterBBRef.Succeeded())
	{
		BBAsset = NormalMonsterBBRef.Object;
	}

	// BT Section
	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		NormalMonsterBTRef(TEXT("/Script/AIModule.BehaviorTree'/Game/Gihoon/AI/NormalMonster/BT_NormalMonster.BT_NormalMonster'"));
	if (NormalMonsterBTRef.Succeeded())
	{
		BTAsset = NormalMonsterBTRef.Object;
	}

	// Perception Section
	MonsterPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("MonsterPerception"));
	SetPerceptionComponent(*MonsterPerception);	// AIController �� Perception �� ����� �� �ִ�.

	// SenseConfig_Sight Section
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("MonsterSight"));
	SightConfig->SightRadius = 800.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 45.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 1000.f;

	// Sight Sense ��� (������� �迭�� �߰�)
	MonsterPerception->ConfigureSense(*SightConfig);

	// ��ǥ ������� ����
	MonsterPerception->SetDominantSense(SightConfig->GetSenseImplementation());

}

void AGHNormalMonsterController::BeginPlay()
{
	Super::BeginPlay();

	// Target �� ���ŵ� ��� ȣ��� �Լ��� OnTargetDetect ����
	MonsterPerception->OnTargetPerceptionUpdated.AddDynamic(this,
		&AGHNormalMonsterController::OnTargetDetect);
	// Target �� ��ĥ ��� ȣ��� �Լ��� OnTargetForget ����
	MonsterPerception->OnTargetPerceptionForgotten.AddDynamic(this,
		&AGHNormalMonsterController::OnTargetForget);
}

void AGHNormalMonsterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGHNormalMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AGHNormalMonsterController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AGHNormalMonsterController::OnTargetDetect(AActor* Target, FAIStimulus Stimulus)
{
	// ������ ������ ���
	if (Stimulus.WasSuccessfullySensed())
	{
		ETeamAttitude::Type Relationship = GetTeamAttitudeTowards(*Target);

		// ���� Target �� �ٸ� ��ü�̰� �Ʊ��� �ƴѰ�� Target���� �ν�
		if (Target != Blackboard->GetValueAsObject(TEXT("Target"))
			&& Relationship != ETeamAttitude::Friendly)
		{
			Blackboard->SetValueAsObject(TEXT("Target"), Target);
		}
	}
	else
	{
		// Target ���� ������ ���
		UObject* Target = Blackboard->GetValueAsObject(FName("Target"));
		if (nullptr != Target)
		{
			// ������ Target�� �����ߴٸ� PreTarget���� ��ȯ���ش�.
			Blackboard->SetValueAsObject(FName("PreTarget"), Target);
		}

		Blackboard->SetValueAsObject(FName("Target"), nullptr);
	}
}

void AGHNormalMonsterController::OnTargetForget(AActor* Target)
{
	// 
}
