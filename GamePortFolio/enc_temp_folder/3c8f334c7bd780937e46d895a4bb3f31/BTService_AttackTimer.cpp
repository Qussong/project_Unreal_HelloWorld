// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/BTService_AttackTimer.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/AI/Monster/GHMonster.h"


UBTService_AttackTimer::UBTService_AttackTimer()
{
	NodeName = TEXT("AttackTimer");	// BT�� ǥ�õǴ� ����� �̸� ����
	Interval = 1.f;
}

void UBTService_AttackTimer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// ���ǵ� Monster ��ü�� �ִ��� Ȯ��
	MonsterPawn = OwnerComp.GetAIOwner()->GetPawn<AGHMonster>();
	if (nullptr == MonsterPawn) return;

	// ���ǵ� ���Ͱ� World �� �����ִ��� Ȯ��
	UWorld* World = MonsterPawn->GetWorld();
	if (nullptr == World) return;

	int32 AttackCoolTime = OwnerComp.GetBlackboardComponent()->GetValueAsInt(FName("AttackCoolTime"));
	bool bAttackTime = OwnerComp.GetBlackboardComponent()->GetValueAsBool(FName("IsAttackTime"));

	if (bAttackTime) return;

	if (0 == AttackCoolTime)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("IsAttackTime"), true);
		// ����� ���� ���� + AttackRange ����
		int32 AttackType = FMath::RandRange(1, 3);

		switch (AttackType)
		{
		case 1:
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("AttackType"), 1);
			OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FName("AttackRange"), 400.f);
			break;
		}
		case 2 :
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("AttackType"), 2);
			OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FName("AttackRange"), 300.f);
			break;
		}
		case 3:
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("AttackType"), 3);
			OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FName("AttackRange"), 300.f);
			break;
		}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("AttackCoolTime"), --AttackCoolTime);
	}

}
