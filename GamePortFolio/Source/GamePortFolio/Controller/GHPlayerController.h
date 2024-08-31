// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GHPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API AGHPlayerController : public APlayerController

{
	GENERATED_BODY()
	
public:
	AGHPlayerController();

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse")
	uint8 IsShowMouseCursor : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse")
	uint8 IsEnableClickEvents : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse")
	uint8 IsEnableMouseOverEvents : 1;

// IA_SetDestination Section
public:
	bool isHit = false;		// ���콺 Ŀ�� �浹����
	FVector Destination;	// ���콺 Ŀ�� �浹��ġ = �÷��̾� ������
public:
	void GetLocationUnderCursor();
	void Follow();
	void MoveTo();

// IA_NormalAttack Section
public:
	void Rotate();

};
