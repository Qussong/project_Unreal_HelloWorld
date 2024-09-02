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
	virtual void NativeInitializeAnimation() override;					// 초기화
	virtual void NativeBeginPlay() override;							// ABP 실제로 시작될 떄 한번 호출
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;	// 매 프레임 업데이트

// Owner Section
protected:
	TObjectPtr<class AGHPlayer> Owner;

// Normal Attack Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	TObjectPtr<UAnimMontage> NormalAttackMontage;
public:
	FORCEINLINE UAnimMontage* GetNormalAttackMontage() { return NormalAttackMontage; }
	void PlayNormalAttackMontage();

// KnockDown Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Death)
	TObjectPtr<UAnimMontage> KnockDownMontage;
public:
	FORCEINLINE UAnimMontage* GetKnockDownMontage() { return KnockDownMontage; }
	void PlayKnockDownMontage();
	void PlayDeathMontage();
protected:
	void SetWhenStopMontage(float CallTime);	// 인자에 해당하는 시간후에 몽타주 업데이트 멈춤
private:
	void MontageStop();

// Roll Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Roll)
	TObjectPtr<UAnimMontage> RollMontage;

public:
	void PlayRollMontage();
};