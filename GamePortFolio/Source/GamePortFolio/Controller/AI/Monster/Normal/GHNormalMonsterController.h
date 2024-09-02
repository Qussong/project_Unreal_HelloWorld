// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/AI/Monster/GHMonsterBaseController.h"
#include "GHNormalMonsterController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API AGHNormalMonsterController : public AGHMonsterBaseController
{
	GENERATED_BODY()
	
public:
	AGHNormalMonsterController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;	// ��Ʈ�ѷ��� Ư�� ���� ������ �� ȣ��
	virtual void OnUnPossess() override;

// Perception Section
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAIPerceptionComponent> MonsterPerception;

	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
	TObjectPtr<class UAISenseConfig_Damage> DamageConfig;
private:
	UFUNCTION()
	void OnTargetDetect(AActor* Target, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetForget(AActor* Target);
};
