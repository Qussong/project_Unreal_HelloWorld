// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/GHCameraShake.h"

UGHCameraShake::UGHCameraShake()
{
	// ���� �ð� ���� (�ʿ信 ����)
	{
		OscillationDuration = 0.5f;			// ����ũ�� ���� �ð� (��)
		OscillationBlendInTime = 0.1f;		// ������ �� ���̵� �� �ð�
		OscillationBlendOutTime = 0.1f;		// ������ �� ���̵� �ƿ� �ð�
	}

	// ȸ�� ���� (Pitch, Yaw, Roll)
	{
		RotOscillation.Pitch.Amplitude = 0.3f; //FMath::RandRange(5.0f, 10.0f);
		RotOscillation.Pitch.Frequency = 5.f; //FMath::RandRange(20.0f, 30.0f);
		RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

		RotOscillation.Yaw.Amplitude = 0.3f; //= FMath::RandRange(5.0f, 10.0f);
		RotOscillation.Yaw.Frequency = 5.f; //= FMath::RandRange(20.0f, 30.0f);
		RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

		RotOscillation.Roll.Amplitude = 0.3f; //= FMath::RandRange(5.0f, 10.0f);
		RotOscillation.Roll.Frequency = 5.f; //= FMath::RandRange(20.0f, 30.0f);
		RotOscillation.Roll.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	}
	

	// ��ġ ���� (X, Y, Z)
	{
		LocOscillation.X.Amplitude = 2.f; //= FMath::RandRange(2.0f, 5.0f);
		LocOscillation.X.Frequency = 5.f; //= FMath::RandRange(10.0f, 20.0f);
		LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

		LocOscillation.Y.Amplitude = 2.f; //= FMath::RandRange(2.0f, 5.0f);
		LocOscillation.Y.Frequency = 5.f; //= FMath::RandRange(10.0f, 20.0f);
		LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

		LocOscillation.Z.Amplitude = 2.f; //= FMath::RandRange(2.0f, 5.0f);
		LocOscillation.Z.Frequency = 5.f; //= FMath::RandRange(10.0f, 20.0f);
		LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	}

	// FOV ����
	{
		FOVOscillation.Amplitude = 0.5f; //= FMath::RandRange(1.0f, 2.0f);
		FOVOscillation.Frequency = 5.f; //FMath::RandRange(5.0f, 10.0f);
		FOVOscillation.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	}
}