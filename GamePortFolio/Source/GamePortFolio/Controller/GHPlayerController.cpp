// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/GHPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Character/Player/GHPlayer.h"
#include "GameFramework/SpringArmComponent.h"

AGHPlayerController::AGHPlayerController()
{
}

void AGHPlayerController::BeginPlay()
{
	Super::BeginPlay();

    // Mouse Cursor Section (마우스 커서 표시)
    bShowMouseCursor = IsShowMouseCursor;
    bEnableClickEvents = IsEnableClickEvents;
    bEnableMouseOverEvents = IsEnableMouseOverEvents;
}

void AGHPlayerController::GetLocationUnderCursor()
{
	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel7);
	isHit = GetHitResultUnderCursorByChannel(TraceType, true, HitResult);
	if (isHit)
	{
		Destination = HitResult.Location;	// 마우스 커서 충돌위치 = 목적지
	}
}

void AGHPlayerController::Follow()
{
	FVector From = GetPawn()->GetActorLocation();
	FVector To = Destination;
	Direction = (To - From).GetSafeNormal();
	Direction.Z = 0.f;

	GetPawn()->AddMovementInput(Direction, 1.f, false);
}

void AGHPlayerController::MoveTo()
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Destination);

	UNiagaraSystem* NiagaraSystem = 
		LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/Gihoon/VFX/Cursor/FX_Cursor.FX_Cursor'"));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, Destination);
}

void AGHPlayerController::Rotate()
{
	FVector From = GetPawn()->GetActorLocation();
	FVector To = Destination;
	Direction = (To - From).GetSafeNormal();
	Direction.Z = 0.f;
	FRotator TargetRotation = Direction.Rotation();

	GetPawn()->SetActorRotation(FRotator(0.f, TargetRotation.Yaw, 0.f));
}

void AGHPlayerController::ZoomIn(float DeltaTime)
{
	APawn* PossessedPawn = GetPawn();
	AGHPlayer* PlayerChar = Cast<AGHPlayer>(PossessedPawn);

	if (IsValid(PlayerChar))
	{
		USpringArmComponent* CameraBoom = PlayerChar->GetCameraBoom();
		
		float CurrentArmLength = CameraBoom->TargetArmLength;
		float TargetArmLength = 400.0f; // 원하는 줌 인 길이
		float SmoothSpeed = 6.0f;		// 스무딩 속도

		CameraBoom->TargetArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, SmoothSpeed);
	}
}
