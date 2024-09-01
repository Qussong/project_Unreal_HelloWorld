// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/PlayerAttack/GHPlayerAttackCameraShake.h"

UGHPlayerAttackCameraShake::UGHPlayerAttackCameraShake()
{
	// ���� �ð� ���� (�ʿ信 ����)
	{
		OscillationDuration = 0.5f;			// ����ũ�� ���� �ð� (��)
		OscillationBlendInTime = 0.1f;		// ������ �� ���̵� �� �ð�
		OscillationBlendOutTime = 0.1f;		// ������ �� ���̵� �ƿ� �ð�
	}

	// ȸ�� ���� (Pitch, Yaw, Roll)
	{
		RotOscillation.Pitch.Amplitude = 0.3f;
		RotOscillation.Pitch.Frequency = 5.f;
		RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

		RotOscillation.Yaw.Amplitude = 0.3f;
		RotOscillation.Yaw.Frequency = 5.f;
		RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

		RotOscillation.Roll.Amplitude = 0.3f;
		RotOscillation.Roll.Frequency = 5.f;
		RotOscillation.Roll.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	}


	// ��ġ ���� (X, Y, Z)
	{
		LocOscillation.X.Amplitude = 2.f;
		LocOscillation.X.Frequency = 5.f;
		LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

		LocOscillation.Y.Amplitude = 2.f;
		LocOscillation.Y.Frequency = 5.f;
		LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

		LocOscillation.Z.Amplitude = 2.f;
		LocOscillation.Z.Frequency = 5.f;
		LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	}

	// FOV ����
	{
		FOVOscillation.Amplitude = 0.5f;
		FOVOscillation.Frequency = 5.f;
		FOVOscillation.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	}
}
