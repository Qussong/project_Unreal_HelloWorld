// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/BTDecorator_AttackInRange.h"
#include "AIController.h"
#include "Character/AI/Monster/GHMonster.h"
#include "BehaviorTree/BlackBoardComponent.h"

UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// ������ Pawn ��ü�� Monster �� �´��� Ȯ��
	AGHMonster* MonsterPawn = OwnerComp.GetAIOwner()->GetPawn<AGHMonster>();
	if (nullptr == MonsterPawn)
		return false;

	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (nullptr == Target)
		return false;

	float DistanceToTarget = MonsterPawn->GetDistanceTo(Target);	// Target �� Monster ������ �Ÿ�

	return (DistanceToTarget <= AttackRange);
}
