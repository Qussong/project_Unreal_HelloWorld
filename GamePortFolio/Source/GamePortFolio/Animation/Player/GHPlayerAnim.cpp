// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Player/GHPlayerAnim.h"
#include "Character/Player/GHPlayer.h"
#include "Controller/GHPlayerController.h"

UGHPlayerAnim::UGHPlayerAnim()
{
}

void UGHPlayerAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UGHPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Owner = Cast<AGHPlayer>(TryGetPawnOwner());
}

void UGHPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UGHPlayerAnim::PlayNormalAttackMontage()
{
	// ��Ÿ�� ��ȿ�� Ȯ��
	if (!IsValid(NormalAttackMontage)) return;

	// ��Ÿ�� ��������� Ȯ��
	if (Montage_IsPlaying(NormalAttackMontage)) return;

	// ���� ����
	Cast<AGHPlayerController>(Owner->GetController())->GetLocationUnderCursor();	// ���콺�� ������� ��ġ Ȯ��
	Cast<AGHPlayerController>(Owner->GetController())->Rotate();					// Player ȸ��

	// ��Ÿ�� ���
	Montage_Play(NormalAttackMontage);
}

void UGHPlayerAnim::PlayKnockDownMontage()
{
	// ��Ÿ�� ��ȿ�� Ȯ��
	if (!IsValid(KnockDownMontage)) return;

	// ��Ÿ�� ��������� Ȯ��
	if (Montage_IsPlaying(KnockDownMontage)) return;

	Montage_Play(KnockDownMontage);
}

void UGHPlayerAnim::PlayDeathMontage()
{
	// ��Ÿ�� ��ȿ�� Ȯ��
	if (!IsValid(KnockDownMontage)) return;

	// ��Ÿ�� ��������� Ȯ��
	if (Montage_IsPlaying(KnockDownMontage))
	{
		Montage_Stop(0.2f);
		Montage_Play(KnockDownMontage);
		Montage_JumpToSection(TEXT("KnockDownDead"));

		SetWhenStopMontage(1.6f); // 1.6f �� �Ŀ� Montage ������Ʈ ����
	}
}

void UGHPlayerAnim::SetWhenStopMontage(float CallTime)
{
	float TimeToTrigger = CallTime;
	FTimerHandle EndTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		EndTimerHandle, this, &UGHPlayerAnim::MontageStop, TimeToTrigger, false);
}

void UGHPlayerAnim::MontageStop()
{
	// �ִϸ��̼��� ������Ʈ�� �Ͻ������� �ߴ�
	GetOwningComponent()->bPauseAnims = true;
}