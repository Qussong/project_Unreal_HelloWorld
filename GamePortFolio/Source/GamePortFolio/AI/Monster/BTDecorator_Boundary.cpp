// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/BTDecorator_Boundary.h"
#include "AIController.h"
#include "Character/AI/Monster/GHMonster.h"
#include "BehaviorTree/BlackBoardComponent.h"

UBTDecorator_Boundary::UBTDecorator_Boundary()
{
	NodeName = TEXT("IsClose");
}

bool UBTDecorator_Boundary::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// ������ Pawn ��ü�� Monster �� �´��� Ȯ��
	AGHMonster* MonsterPawn = OwnerComp.GetAIOwner()->GetPawn<AGHMonster>();
	if (nullptr == MonsterPawn) return false;

	// Target �� �����ϴ��� Ȯ��
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (nullptr == Target) return false;

	// ���� Ÿ�̹��� �ƴ��� Ȯ��
	bool AttackTime = OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("IsAttackTime"));
	if (AttackTime) return false;

	float DistanceToTarget = MonsterPawn->GetDistanceTo(Target);	// Target �� Monster ������ �Ÿ�

	return (DistanceToTarget <= BoundartDistance);
}
