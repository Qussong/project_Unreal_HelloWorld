// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GHAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API AGHAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGHAIController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;	// ��Ʈ�ѷ��� Ư�� ���� ������ �� ȣ��
	virtual void OnUnPossess() override;

protected:
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;

protected:
	void RunAI();
	void StopAI();

};
