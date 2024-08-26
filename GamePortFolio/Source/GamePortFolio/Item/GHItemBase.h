// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GHItemBase.generated.h"

UENUM()
enum class EItemInventoryType
{
	NONE,
	POTION,
	SCROLL,
	WEAPON,
	CLOTH,
	ETC,
};

USTRUCT(BlueprintType)
struct FItemHoldableData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> Equipper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;
};

USTRUCT(BlueprintType)
struct FItemInventoryData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemInventoryType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> PickupSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> PickupStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemHoldableData HoldableSettings;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIDDelegate, FName, NewID);

UCLASS()
class GAMEPORTFOLIO_API AGHItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AGHItemBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SceneComp")
	TObjectPtr<USceneComponent> ItemSceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshComp")
	TObjectPtr<class USkeletalMeshComponent> ItemSkeletalMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshComp")
	TObjectPtr<class UStaticMeshComponent> ItemStaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveComp")
	TObjectPtr<class URotatingMovementComponent> RotatingMovementComp;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemComponent")
	//TObjectPtr<class USphereComponent> ItemCollisionComp;

// ID Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item"/*, meta = (ExposeOnSpawn = "true")*/)
	FName ID;
	UPROPERTY()
	FIDDelegate OnIDChanged;
public:
	void SetID(FName NewID);
	UFUNCTION()
	void HandleIDChanged(FName NewID);

// Collision Overlap Section
//protected:
//	UFUNCTION()
//	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
//		AActor* OtherActor,
//		UPrimitiveComponent* OtherComp,
//		int32 OtherBodyIndex,
//		bool bFromSweep,
//		const FHitResult& SweepResult);

// Data Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable)
	TObjectPtr<UDataTable> ItemDataTable;
};
