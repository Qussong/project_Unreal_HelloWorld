// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GHBaseWidgetComponent.h"

UGHBaseWidgetComponent::UGHBaseWidgetComponent()
{
}

void UGHBaseWidgetComponent::OnRegister()
{
	// ������Ʈ�� ���忡 �߰��ǰų� ������ ���Ͱ� ���忡 ��ġ�� �� ȣ��
	// UWidgetComponent �� ó�� Ȱ��ȭ�� ��, �ʱ�ȭ �۾� ����
	Super::OnRegister();
}

void UGHBaseWidgetComponent::InitWidget()
{
	// WidgetClass�� ����Ͽ� ������ �����ϴ� �ʱ�ȭ �۾�
	Super::InitWidget();
}

void UGHBaseWidgetComponent::BeginPlay()
{
	// ������Ʈ�� ������ ���Ͱ� �÷��̸� ������ �� ȣ��
	// ���� ���� �� �ʱ�ȭ �۾��� �� �� ���
	Super::BeginPlay();
}

void UGHBaseWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// ������Ʈ�� �� ������ ȣ��Ǿ� ������Ʈ�� �� �ֵ����Ѵ�.
	// ������ ���¸� ������Ʈ�ϰų� �ִϸ��̼��� ó���� �� ���
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
