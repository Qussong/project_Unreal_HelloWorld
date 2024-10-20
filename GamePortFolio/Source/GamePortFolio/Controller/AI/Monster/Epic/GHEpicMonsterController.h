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
	virtual void OnPossess(APawn* InPawn) override;	// 컨트롤러가 특정 폰을 소유할 떄 호출
	virtual void OnUnPossess() override;
	
};
