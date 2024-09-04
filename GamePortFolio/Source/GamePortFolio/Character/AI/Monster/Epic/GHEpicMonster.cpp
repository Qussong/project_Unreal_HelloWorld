
#include "Character/AI/Monster/Epic/GHEpicMonster.h"
#include "Controller/AI/Monster/Epic/GHEpicMonsterController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Stat/GHBaseStatComponent.h"
#include "Animation/AI/Monster/GHMonsterAnim.h"

#include "UI/Monster/GHMonsterWidgetComponent.h"
#include "UI/Monster/GHMonsterWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/ChildActorComponent.h"
#include "Item/Equip/GHWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AGHEpicMonster::AGHEpicMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	// Controller Section
	AIControllerClass = AGHEpicMonsterController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Skeletal Mesh Section
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		EpicMonsterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Gihoon/UE_Characters/Mannequins/Meshes/SKM_Manny5_GH.SKM_Manny5_GH'"));
	if (EpicMonsterMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(EpicMonsterMeshRef.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// AnimInstance Section
	static ConstructorHelpers::FClassFinder<UAnimInstance>
		EpicMonsterAnimRef(TEXT("/Game/Gihoon/Animations/Monster/Epic/ABP_EpicMonster_New.ABP_EpicMonster_New_C"));
	if (EpicMonsterAnimRef.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(EpicMonsterAnimRef.Class);
	}

	// Collision Section
	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MonsterProfile"));

	// Movement Section
	GetCharacterMovement()->MaxWalkSpeed = 450.f;

	// Weapon Section
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		SwordObjectRef(TEXT("/Script/Engine.StaticMesh'/Game/Gihoon/Item/Mesh/SM_GHSword.SM_GHSword'"));
	if (SwordObjectRef.Succeeded())
	{
		SwordMesh = SwordObjectRef.Object;
	}

	WeaponActorComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponActorComp"));
	if (IsValid(WeaponActorComp))
	{
		// �θ� ������Ʈ(SkeletalMesh)�� ����
		WeaponActorComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("clavicle_r_weaponhold"));
		// Child Actor Class ����
		WeaponActorComp->SetChildActorClass(AGHWeapon::StaticClass());
		// �ʱ� Ʈ������ ����
		WeaponActorComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		WeaponActorComp->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	}

	// Stat Section
	Stat->SetMaxHealth(500.f);
	Stat->SetATK(30.f);
	Stat->SetDEF(20.f);
}

void AGHEpicMonster::BeginPlay()
{
	Super::BeginPlay();

	SetState(EMonsterState::WAIT);

	// Animation Section
	MonsterAnim = Cast<UGHMonsterAnim>(GetMesh()->GetAnimInstance());

	// UI Section
	UpdateHUD();

	// Weapon Section
	if (IsValid(WeaponActorComp))
	{
		WeaponActorComp->RegisterComponent();	// ������ Child Actor Component �� ���忡 ���
		UStaticMeshComponent* WeaponStatiMeshComp = Cast<AGHWeapon>(WeaponActorComp->GetChildActor())->GetItemStaticMeshComp();
		if (IsValid(WeaponStatiMeshComp))
		{
			WeaponStatiMeshComp->SetStaticMesh(SwordMesh);
		}
	}
}

void AGHEpicMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGHEpicMonster::SetDeath()
{
	Super::SetDeath();

	HUDWidgetComp->SetVisibility(false);
}

float AGHEpicMonster::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UpdateHUD();

	AGHEpicMonsterController* MonsterController = Cast<AGHEpicMonsterController>(GetController());

	// Hit Section
	if (MonsterState == EMonsterState::BOUNDARY)
	{
		MonsterController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsHit"), true);
	}

	// Target Setiing
	AActor* Target = Cast<AActor>(MonsterController->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	bool IsMonster = DamageCauser->IsA(AGHMonster::StaticClass());
	if (nullptr == Target && false == IsMonster)
	{
		MonsterController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), DamageCauser);
	}

	return DamageAmount;
}

void AGHEpicMonster::AttackCheck_Begin(FVector& Start_V, FVector End_V, FVector& Start_H, FVector& End_H)
{
	if (this == nullptr) return;

	if (nullptr != WeaponActorComp)
	{
		AActor* ChildActor = WeaponActorComp->GetChildActor();
		if (nullptr == ChildActor) return;

		AGHWeapon* Weapon = Cast<AGHWeapon>(ChildActor);
		if (nullptr == Weapon) return;

		UStaticMeshComponent* SwordStaticMesh = Weapon->GetItemStaticMeshComp();
		if (nullptr == SwordStaticMesh) return;

		// Start_V, End_V ��ġ ����
		Start_V = SwordStaticMesh->GetSocketLocation(FName("Start"));
		End_V = SwordStaticMesh->GetSocketLocation(FName("End"));
		// Start_H, End_H ��ġ ����
		Start_H = End_V;
		End_H = End_V;

		// �ǰݴ�� �����̳� �����
		HitCheckContainer.Empty();
	}

}

void AGHEpicMonster::AttackCheck_Tick(FVector& Start_V, FVector End_V, FVector& Start_H, FVector& End_H)
{
	if (this == nullptr) return;

	if (nullptr != WeaponActorComp)
	{
		AActor* ChildActor = WeaponActorComp->GetChildActor();
		if (nullptr == ChildActor) return;

		AGHWeapon* Weapon = Cast<AGHWeapon>(ChildActor);
		if (nullptr == Weapon) return;

		UStaticMeshComponent* SwordStaticMesh = Weapon->GetItemStaticMeshComp();
		if (nullptr == SwordStaticMesh) return;

		// Start_V, End_V ��ġ ����
		Start_V = SwordStaticMesh->GetSocketLocation(FName("Start"));
		End_V = SwordStaticMesh->GetSocketLocation(FName("End"));

		// End_H ��ġ ����
		End_H = End_V;

		// Line1
		TArray<FHitResult> HitResultsVertical;
		FCollisionQueryParams CollisionParamsVertical;
		CollisionParamsVertical.AddIgnoredActor(this); // �ڱ� �ڽ��� ����
		bool IsHit = GetWorld()->LineTraceMultiByChannel(HitResultsVertical, Start_V, End_V, ECC_GameTraceChannel5, CollisionParamsVertical);

		if (IsHit)
		{
			DrawDebugLine(GetWorld(), Start_V, End_V, FColor::Red, false, 1, 0, 1);
			EnemyHit(HitResultsVertical);
		}
		else
		{
			DrawDebugLine(GetWorld(), Start_V, End_V, FColor::Green, false, 1, 0, 1);
		}

		// Line2
		TArray<FHitResult> HitResultsHorizontal;
		FCollisionQueryParams CollisionParamsHorizontal;
		CollisionParamsVertical.AddIgnoredActor(this); // �ڱ� �ڽ��� ����
		IsHit = GetWorld()->LineTraceMultiByChannel(HitResultsHorizontal, Start_H, End_H, ECC_GameTraceChannel5, CollisionParamsVertical);

		if (IsHit)
		{
			EnemyHit(HitResultsHorizontal);
#if ENABLE_DRAW_DEBUG
			DrawDebugLine(GetWorld(), Start_H, End_H, FColor::Red, false, 1, 0, 1);
#endif
		}
		else
		{
#if ENABLE_DRAW_DEBUG
			DrawDebugLine(GetWorld(), Start_H, End_H, FColor::Green, false, 1, 0, 1);
#endif
		}

		// Start_H ��ġ ����
		Start_H = End_V;
	}
}

void AGHEpicMonster::EnemyHit(TArray<FHitResult>& HitResults)
{
	for (FHitResult HitResult : HitResults)
	{
		AActor* Hitter = HitResult.GetActor();
		bool isExist = HitCheckContainer.Contains(Hitter);
		if (false == isExist)
		{
			UE_LOG(LogTemp, Log, TEXT("HIT!!"));

			// ������ ó��
			HitCheckContainer.Add(Hitter);
			UGameplayStatics::ApplyDamage(Hitter, Stat->GetATK(), GetController(), this, UDamageType::StaticClass());

			// ����Ʈ ó��
			UParticleSystem* HitEffect = Cast<AGHCharacterBase>(Hitter)->GetHitParticle();
			if (nullptr != HitEffect)
			{
				FVector HitLocation = HitResult.ImpactPoint;
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitLocation);
			}

			// ���� ó��
			USoundCue* HitSound = Cast<AGHCharacterBase>(Hitter)->GetHitSound();
			if (nullptr != HitSound)
			{
				FVector HitLocation = HitResult.ImpactPoint;
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitLocation);
			}
		}
	}
}

void AGHEpicMonster::AttackCheck_Begin2()
{
	HitCheckContainer.Empty();
	Attack2Center = GetMesh()->GetSocketLocation(FName("hand_l_weapon"));
}

void AGHEpicMonster::AttackCheck_Tick2()
{
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	bool IsHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Attack2Center,
		Attack2Radius,
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
			Attack2Center,	// ���� �߽�
			Attack2Radius,  // ���� ������
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
		DrawDebugSphere(GetWorld(), Attack2Center, Attack2Radius, 3, FColor::Green, false, 1.0f);
#endif
	}

	Attack2Center = GetMesh()->GetSocketLocation(FName("hand_l_weapon"));
}

void AGHEpicMonster::Hit(TArray<AActor*>& HitResults)
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
		}
	}
}
