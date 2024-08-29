// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/GHInventoryComponent.h"
#include "Character/Player/GHPlayer.h"
#include "Components/ChildActorComponent.h"
#include "Item/GHBaseItem.h"

UGHInventoryComponent::UGHInventoryComponent()
{
	// DataTable Section
	static ConstructorHelpers::FObjectFinder<UDataTable>
		ItemDataTableRef(TEXT("/Script/Engine.DataTable'/Game/Gihoon/Item/Data/DT_InventoryItem.DT_InventoryItem'"));
	if (ItemDataTableRef.Succeeded())
	{
		ItemDataTable = ItemDataTableRef.Object;
	}
}

bool UGHInventoryComponent::Drop(FName ID, int32 Quantity)
{
	AActor* OwnweActor = GetOwner();
	AGHPlayer* Player = Cast<AGHPlayer>(OwnweActor);

	if (!CheckItemExist(ID, Quantity)) return false;

	FItemInventoryData* Item = Items.Find(ID);
	if (nullptr != Item)
	{
		// �̹� ������ �ִ� ���
		int32 NewQuantity = Item->Quantity + DropItem.Quantity;
		DropItem.Quantity = NewQuantity;
		Items.Add(ID, DropItem);
	}
	else
	{
		// ó�� �����ϴ� ���
		Items.Add(ID, DropItem);
	}

	// ȹ�� ������ ���� �ʱ�ȭ
	DropItem = {};

	return true;
}

bool UGHInventoryComponent::CheckItemExist(FName ID, int32 NewQuantity)
{
	FItemInventoryData* Item = ItemDataTable->FindRow<FItemInventoryData>(ID, TEXT(""));
	if (nullptr == Item) return false;

	DropItem = *Item;
	if (NewQuantity != -1)
	{
		DropItem.Quantity = NewQuantity;
	}

	return true;;
}

void UGHInventoryComponent::ReviewInventory()
{
	if (Items.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("Inventory Empty!"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("===================="));
	for (const TPair<FName, FItemInventoryData>& Item : Items)
	{
		FText Name = Item.Value.DisplayName;
		int32 Quantity = Item.Value.Quantity;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("%s : %d"), *Name.ToString(), Quantity));
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("==== Inventory List ===="));
}

bool UGHInventoryComponent::Armed(FName ID)
{
	// DataTable �� ���� ID �� �ش��ϴ� ���� ����
	FItemInventoryData* ItemData = Items.Find(ID);
	if (nullptr == ItemData) return false;

	// Data �� ���� ������ ���õ� ���� ����
	FItemHoldableData HoldableData = ItemData->HoldableSettings;
	if (nullptr == HoldableData.EquipmentClass) return false;

	// Child Actor Component �� �����Ǿ� ���� �ʴٸ�
	if (nullptr == EquipChildComp)
	{
		if(false == CreateEquipmentComp(HoldableData))
			return false;

		if (false == SetEquipChildComp(ItemData))
			return false;
	}
	// Child Actor Component �� �����Ǿ� �ִٸ�
	else
	{
		// ������ Child Actor Component �� ���忡 ���
		EquipChildComp->RegisterComponent();
	}

	return true;
}

bool UGHInventoryComponent::DisArmed()
{
	if (IsValid(EquipChildComp))
	{
		EquipChildComp->UnregisterComponent();
		return true;
	}

	return false;
}

bool UGHInventoryComponent::CreateEquipmentComp(FItemHoldableData& HoldableData)
{
	// Child Actor Component ����
	EquipChildComp = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass(), TEXT("DynamicChildActorComp"));
	if (nullptr == EquipChildComp) return false;

	// �θ� ������Ʈ(SkeletalMesh)�� ����
	EquipChildComp->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, HoldableData.SocketName);
	// Child Actor Class ����
	EquipChildComp->SetChildActorClass(HoldableData.EquipmentClass);
	// �ʱ� Ʈ������ ����
	EquipChildComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	// ������ Child Actor Component �� ���忡 ���
	EquipChildComp->RegisterComponent();

	return true;
}

bool UGHInventoryComponent::SetEquipChildComp(FItemInventoryData* ItemData)
{
	// ������ Child Actor�� ����
	AActor* SpawnedChildActor = Cast<AActor>(EquipChildComp->GetChildActor());
	if (nullptr == SpawnedChildActor) return false;

	if (nullptr != ItemData->PickupStaticMesh)
	{
		// Skeletal Mesh
		USkeletalMeshComponent* SkeletalMeshComp = Cast<AGHBaseItem>(SpawnedChildActor)->GetItemSkeletalMeshComp();
		if (IsValid(SkeletalMeshComp))
			SkeletalMeshComp->SetSkeletalMesh(ItemData->PickupSkeletalMesh.Get());

		// Static Mesh
		UStaticMeshComponent* StaticMeshComp = Cast<AGHBaseItem>(SpawnedChildActor)->GetItemStaticMeshComp();
		if (IsValid(StaticMeshComp))
			StaticMeshComp->SetStaticMesh(ItemData->PickupStaticMesh.Get());
	}

	return true;
}
