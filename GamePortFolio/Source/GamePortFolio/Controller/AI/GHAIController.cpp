// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI/GHAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

AGHAIController::AGHAIController()
{
}

void AGHAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AGHAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGHAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunAI();
}

void AGHAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AGHAIController::RunAI()
{
	if (IsValid(BBAsset) && IsValid(BTAsset))
	{
		UBlackboardComponent* BBPtr = Blackboard.Get();	// AI Controller ���� �⺻������ �����Ǵ� ����(Blackboard)
		bool isInitBB = UseBlackboard(BBAsset, BBPtr);	// BB �ʱ�ȭ
		if (isInitBB)
		{
			bool BTRunResult = RunBehaviorTree(BTAsset);// BT ����
			ensure(BTRunResult);
		}
	}
}

void AGHAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();	// �����ϰ� �����Ѵٴ� ���ڰ� �⺻������ �� �ִ�. (EBTStopMode::Safe)
	}
}
