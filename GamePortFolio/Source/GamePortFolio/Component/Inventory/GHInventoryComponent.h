// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/GHBaseActorComponent.h"
#include "Item/GHItemStruct.h"
#include "GHInventoryComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPORTFOLIO_API UGHInventoryComponent : public UGHBaseActorComponent
{
	GENERATED_BODY()

public:
	UGHInventoryComponent();
protected:
	//virtual void BeginPlay() override;
public:
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// DataTable Section
private:
	TObjectPtr<UDataTable> ItemDataTable;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InventoryList)
	TMap<FName, FItemInventoryData> Items;

// Drop Section
public:
	bool Drop(FName ID, int32 NewQuantity = -1);
private:
	FItemInventoryData DropItem = {};
	bool CheckItemExist(FName ID, int32 Quantity);	// ID �� �ش��ϴ� ������ ������ �ִ��� Ȯ��

// Inventory Check Section
public:
	void ReviewInventory();

// Weapon Section
public:
	bool Armed(FName ID);
	bool DisArmed();
private:
	bool CreateEquipmentComp(FItemHoldableData& HoldableData);	// Child Actor Comp ����
	bool SetEquipChildComp(FItemInventoryData* ItemData);		// ������ Child Actor Comp �� Actor ����
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UChildActorComponent> EquipChildComp;

};
