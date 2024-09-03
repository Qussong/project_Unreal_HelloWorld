// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AI/Monster/GHMonsterAnim.h"
#include "GHEpicMonsterAnim.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API UGHEpicMonsterAnim : public UGHMonsterAnim
{
	GENERATED_BODY()
	
public:
	UGHEpicMonsterAnim();

public:
	virtual void NativeInitializeAnimation() override;					// �ʱ�ȭ
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;	// �� ������ ������Ʈ
	virtual void NativeBeginPlay() override;							// ABP ������ ���۵� �� �ѹ� ȣ��

protected:
	virtual void SetMonsterAnimState(EMonsterAnimState NewState) override;

// ReadyFight Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Death)
	TObjectPtr<UAnimMontage> WaitMontage;

// Boundary Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Boundary)
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Boundary)
	float Direction;

};
