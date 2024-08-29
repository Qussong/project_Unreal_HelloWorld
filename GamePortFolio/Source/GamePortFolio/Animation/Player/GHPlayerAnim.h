// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/GHBaseAnim.h"
#include "GHPlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API UGHPlayerAnim : public UGHBaseAnim
{
	GENERATED_BODY()
	
public:
	UGHPlayerAnim();

public:
	virtual void NativeInitializeAnimation() override;					// �ʱ�ȭ
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;	// �� ������ ������Ʈ
	virtual void NativeBeginPlay() override;							// ABP ������ ���۵� �� �ѹ� ȣ��

// Owner Section
protected:
	TObjectPtr<class AGHPlayer> Owner;

// Normal Attack Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	TObjectPtr<UAnimMontage> NormalAttackMontage;
public:
	void PlayNormalAttackMontage();

};
