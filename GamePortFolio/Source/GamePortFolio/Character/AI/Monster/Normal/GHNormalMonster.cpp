// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AI/Monster/Normal/GHNormalMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controller/AI/Monster/Normal/GHNormalMonsterController.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AI/Monster/GHMonsterAnim.h"
#include "UI/Monster/GHMonsterWidgetComponent.h"
#include "UI/Monster/GHMonsterWidget.h"
#include "Components/TextBlock.h"
#include "Stat/GHBaseStatComponent.h"
#include "Components/ProgressBar.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


AGHNormalMonster::AGHNormalMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	// Controller Section
	AIControllerClass = AGHNormalMonsterController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Movement Section
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	// Skeletal Mesh Section
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		NormalMonsterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Mixamo/Mesh/XBot/X_Bot.X_Bot'"));
	if (NormalMonsterMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(NormalMonsterMeshRef.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// AnimInstance Section
	static ConstructorHelpers::FClassFinder<UAnimInstance>
		NormalMonsterAnimRef(TEXT("/Game/Gihoon/Animations/Monster/Normal/ABP_NormalMonster.ABP_NormalMonster_C"));
	if (NormalMonsterAnimRef.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(NormalMonsterAnimRef.Class);
	}

	// Collision Section
	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MonsterProfile"));

	// Stat Section
	Stat->SetMaxHealth(100.f);
	Stat->SetATK(20.f);
	Stat->SetDEF(10.f);
}		

void AGHNormalMonster::BeginPlay()
{
	Super::BeginPlay();

	// Animation Section
	MonsterAnim = Cast<UGHMonsterAnim>(GetMesh()->GetAnimInstance());

	// UI Section
	UpdateHUD();
}

void AGHNormalMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGHNormalMonster::SetDeath()
{
	Super::SetDeath();

	HUDWidgetComp->SetVisibility(false);
}

float AGHNormalMonster::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UpdateHUD();

	AGHNormalMonsterController* MonsterController = Cast<AGHNormalMonsterController>(GetController());
	MonsterController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsHit"), true);
	bool IsMonster = DamageCauser->IsA(AGHMonster::StaticClass());
	if (false == IsMonster)
	{
		MonsterController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), DamageCauser);
	}

	return DamageAmount;
}

void AGHNormalMonster::AttackCheck_Begin()
{
	HitCheckContainer.Empty();
	Center = GetMesh()->GetSocketLocation(FName("RightHandSocket"));
}

void AGHNormalMonster::AttackCheck_Tick()
{
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	bool IsHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Center,
		Radius,
		TArray<TEnumAsByte<EObjectTypeQuery>>(), // üũ�� ������Ʈ Ÿ�Ե�
		nullptr,			// ������ Ŭ����, nullptr�̸� ��� Ŭ���� ����
		IgnoreActors,		// ������ ���͵�
		OverlappedActors	// ����� ��ȯ�� ���͵�
	);

	if (IsHit)
	{
		Hit(OverlappedActors);
#if ENABLE_DRAW_DEBUG
		DrawDebugSphere(
			GetWorld(),		// ���� ���ؽ�Ʈ
			Center,			// ���� �߽�
			Radius,         // ���� ������
			3,              // ���� ���׸�Ʈ �� (�� ���� ���� �� ���� ����� ���� ����ϴ�)
			FColor::Red,	// ���� ����
			false,          // ���� �ð� ���� ǥ�õ� ������ ���� (true = ����, false = ���)
			1.0f            // ���� �ð� (0�̸� �� ������ ���ȸ� ǥ��)
		);
#endif
	}
	else
	{
#if ENABLE_DRAW_DEBUG
		DrawDebugSphere(GetWorld(), Center,Radius, 3, FColor::Green, false, 1.0f);
#endif
	}

	Center = GetMesh()->GetSocketLocation(FName("RightHandSocket"));
}

void AGHNormalMonster::Hit(TArray<AActor*>& HitResults)
{
	// �������� ���͵鿡 ���� ó��
	for (AActor* Hitter : HitResults)
	{
		bool IsContain = HitCheckContainer.Contains(Hitter);
		if (false == IsContain)
		{
			HitCheckContainer.Add(Hitter);
			
			// ������ ó��
			HitCheckContainer.Add(Hitter);
			UGameplayStatics::ApplyDamage(Hitter, Stat->GetATK(), GetController(), this, UDamageType::StaticClass());

			// ��ƼŬ ó��
			UParticleSystem* HitEffect = Cast<AGHCharacterBase>(Hitter)->GetHitParticle();
			if (nullptr != HitEffect)
			{
				FVector HitLocation = Hitter->GetActorLocation();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitLocation);
			}

			// ���� ó��
		}
	}
}
