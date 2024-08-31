// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API UBTService_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Detect();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	TObjectPtr<class AGHMonster> MonsterPawn;
	TObjectPtr<UWorld> World;

	FVector Center;				// Ž���� ������ �߽���ġ
	float DetectRadius = 500.f;	// Ž�� �ݰ�

protected:
	void DetectTarget(UBehaviorTreeComponent& OwnerComp);

};
