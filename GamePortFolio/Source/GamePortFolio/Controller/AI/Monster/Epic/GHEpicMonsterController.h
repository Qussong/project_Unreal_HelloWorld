// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/AI/Monster/GHMonsterBaseController.h"
#include "GHEpicMonsterController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API AGHEpicMonsterController : public AGHMonsterBaseController
{
	GENERATED_BODY()
	
public:
	AGHEpicMonsterController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;	// ��Ʈ�ѷ��� Ư�� ���� ������ �� ȣ��
	virtual void OnUnPossess() override;
	
};
