// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AI/Monster/GHMonsterAnim.h"
#include "GHNormalMonsterAnim.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API UGHNormalMonsterAnim : public UGHMonsterAnim
{
	GENERATED_BODY()
	
public:
	UGHNormalMonsterAnim();

public:
	virtual void NativeInitializeAnimation() override;					// �ʱ�ȭ
	virtual void NativeBeginPlay() override;							// ABP ������ ���۵� �� �ѹ� ȣ��
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;	// �� ������ ������Ʈ

};
