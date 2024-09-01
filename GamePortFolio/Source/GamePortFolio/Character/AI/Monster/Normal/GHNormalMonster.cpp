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
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


AGHNormalMonster::AGHNormalMonster()
{
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

	// UI Section
	HUDWidgetComp = CreateDefaultSubobject<UGHMonsterWidgetComponent>(TEXT("HUDWidgetComp"));
	if (IsValid(HUDWidgetComp))
	{
		HUDWidgetComp->SetupAttachment(RootComponent);
		HUDWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
		HUDWidgetComp->SetVisibility(true);
		HUDWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		HUDWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		static ConstructorHelpers::FClassFinder<UGHMonsterWidget>
			HUDWidgetRef(TEXT("/Game/Gihoon/UI/WB_MonsterHUD.WB_MonsterHUD_C"));
		if (HUDWidgetRef.Succeeded())
		{
			HUDWidgetComp->SetWidgetClass(HUDWidgetRef.Class);
		}
	}
	
	// Hit Section
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		HitParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Blood/P_Blood_Splat_Cone.P_Blood_Splat_Cone'"));
	if (HitParticleRef.Succeeded())
	{
		HitParticle = HitParticleRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>
		HitSoundRef(TEXT("/Script/Engine.SoundCue'/Game/Gihoon/Sound/HitSound_Cue.HitSound_Cue'"));
	if (HitSoundRef.Succeeded())
	{
		HitSoundCue = HitSoundRef.Object;
	}

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

void AGHNormalMonster::UpdateHUD()
{
	UUserWidget* Widget = HUDWidgetComp->GetWidget();
	if (IsValid(Widget))
	{
		UGHMonsterWidget* MonsterWidget = Cast<UGHMonsterWidget>(Widget);
		if (IsValid(MonsterWidget))
		{
			// Progress Bar
			UProgressBar* HpBar = Cast<UProgressBar>(MonsterWidget->GetWidgetFromName(TEXT("GHHealthBar")));
			if (IsValid(HpBar))
			{
				HpBar->SetPercent(Stat->GetCurrnetHealth() / Stat->GetMaxHealth());
			}

			// Text Block
			UTextBlock* CurHpTxtBlock = Cast<UTextBlock>(MonsterWidget->GetWidgetFromName(TEXT("GHCurrentHealth")));
			UTextBlock* MaxHpTxtBlock = Cast<UTextBlock>(MonsterWidget->GetWidgetFromName(TEXT("GHMaxHealth")));
			if (IsValid(CurHpTxtBlock) && IsValid(MaxHpTxtBlock))
			{
				float CurHp = Stat->GetCurrnetHealth();
				float MaxHp = Stat->GetMaxHealth();

				CurHpTxtBlock->SetText(FText::AsNumber(CurHp));
				MaxHpTxtBlock->SetText(FText::AsNumber(MaxHp));
			}
		}
	}
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
