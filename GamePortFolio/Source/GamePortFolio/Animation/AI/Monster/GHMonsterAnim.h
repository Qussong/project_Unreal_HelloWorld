// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AI/GHAIAnim.h"
#include "GHMonsterAnim.generated.h"

UENUM(BlueprintType)
enum class EMonsterAnimState : uint8
{
	// Common
	IDLE,
	WALK,
	RUN,
	ATTACK,
	ATTACK2,
	DEATH,
	DEATH2,
	ANGRY,
	HIT,
	// Normal
	DETECT,
};

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API UGHMonsterAnim : public UGHAIAnim
{
	GENERATED_BODY()
	
public:
	UGHMonsterAnim();

public:
	virtual void NativeInitializeAnimation() override;					// �ʱ�ȭ
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;	// �� ������ ������Ʈ
	virtual void NativeBeginPlay() override;							// ABP ������ ���۵� �� �ѹ� ȣ��

	// Anim State Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	EMonsterAnimState AnimState = EMonsterAnimState::IDLE;
public:
	FORCEINLINE EMonsterAnimState GetMonsterAnimState() { return AnimState; }
	FORCEINLINE void SetMonsterAnimState(EMonsterAnimState NewState) { AnimState = NewState; }
};
