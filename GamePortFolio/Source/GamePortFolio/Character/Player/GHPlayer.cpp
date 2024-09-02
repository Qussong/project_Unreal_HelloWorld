// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/GHPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Input/GHPlayerInputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controller/GHPlayerController.h"
#include "UI/Player/GHPlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Stat/Player/GHPlayerStatComponent.h"
#include "Component/Inventory/GHInventoryComponent.h"
#include "Animation/Player/GHPlayerAnim.h"
#include "Item/Equip/GHWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ChildActorComponent.h"
#include "Camera/PlayerAttack/GHPlayerAttackCameraShake.h"
#include "Sound/SoundCue.h"
#include "Components/TextBlock.h"
#include "Particles/ParticleSystem.h"
#include "Components/Button.h"

//#include "Components/TimelineComponent.h"

AGHPlayer::AGHPlayer()
{
	// Tick Section
	PrimaryActorTick.bCanEverTick = true;

	// Team Section
	SetTeamID(ETeamID::PLAYER);

	// Camera Section
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetWorldRotation(FRotator(-50.f, 0.f, 0.f));
	CameraBoom->TargetArmLength = 1600.f;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->FieldOfView = 55.f;

	// Capsule Section
	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerProfile"));

	// Skeletal Mesh Section
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		PlayerMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Gihoon/UE_Characters/Mannequin_UE4/Meshes/SK_Mannequin_GH.SK_Mannequin_GH'"));
	if (PlayerMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(PlayerMeshRef.Object);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	
	// AnimInstance Section
	static ConstructorHelpers::FClassFinder<UAnimInstance>
		PlayerAnimClassRef(TEXT("/Game/Gihoon/Animations/Player/ABP_GHPlayer.ABP_GHPlayer_C"));
	if (PlayerAnimClassRef.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(PlayerAnimClassRef.Class);
	}

	// IMC Section
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>
		IMCRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Gihoon/Input/IMC_Player.IMC_Player'"));
	if (IMCRef.Succeeded())
	{
		IMC = IMCRef.Object;
	}

	// Player State Widget Section
	static ConstructorHelpers::FClassFinder<UGHPlayerWidget>
		StateWidgetRef(TEXT("/Game/Gihoon/UI/WB_Player.WB_Player_C"));
	if (StateWidgetRef.Succeeded())
	{
		StateWidgetClass = StateWidgetRef.Class;
	}

	// GameOver Widget Section
	static ConstructorHelpers::FClassFinder<UGHPlayerWidget>
		GameOverWidgetRef(TEXT("/Game/Gihoon/UI/WB_GameOver.WB_GameOver_C"));
	if (GameOverWidgetRef.Succeeded())
	{
		GameOverWidgetClass = GameOverWidgetRef.Class;
	}

	// YouDie Widget Section
	static ConstructorHelpers::FClassFinder<UGHPlayerWidget>
		YouDieWidgetRef(TEXT("/Game/Gihoon/UI/WB_Youdie.WB_Youdie_C"));
	if (YouDieWidgetRef.Succeeded())
	{
		YouDieWidgetClass = YouDieWidgetRef.Class;
	}

	// Inventory Section
	Inventory = CreateDefaultSubobject<UGHInventoryComponent>(TEXT("Inventory"));

	// Stat Section
	Stat = CreateDefaultSubobject<UGHPlayerStatComponent>(TEXT("PlayerStat"));
	Stat->SetMaxHealth(200.f);
	Stat->SetATK(20.f);
	Stat->SetDEF(10.f);

	// Hit Section
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		HitParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Blood/P_Blood_Splat_Cone.P_Blood_Splat_Cone'"));
	if (HitParticleRef.Succeeded())
	{
		HitParticle = HitParticleRef.Object;
	}

}

void AGHPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AGHPlayer::BeginPlay()
{
	Super::BeginPlay();

	// IMC Section
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (IsValid(Subsystem))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}

		AGHPlayerController* CustomController = Cast<AGHPlayerController>(PlayerController);
		if (IsValid(CustomController))
		{
			MyController = CustomController;
		}
	}

	// Player State Widget Section
	if (IsValid(StateWidgetClass))
	{
		StateWidgetInstance = CreateWidget<UGHPlayerWidget>(GetWorld(), StateWidgetClass);
		StateWidgetInstance->AddToViewport();
		UpdateStateWidget();
	}
	
	// GameOver Widget Section
	if (IsValid(GameOverWidgetClass))
	{
		GameOverWidgetInstance = CreateWidget<UGHPlayerWidget>(GetWorld(), GameOverWidgetClass);
		YesBtn = Cast<UButton>(GameOverWidgetInstance->GetWidgetFromName(TEXT("YesBtn")));
		NoBtn = Cast<UButton>(GameOverWidgetInstance->GetWidgetFromName(TEXT("NoBtn")));
		YesBtn->OnClicked.AddDynamic(this, &AGHPlayer::YesBtnClicked);
		NoBtn->OnClicked.AddDynamic(this, &AGHPlayer::NoBtnClicked);
	}

	// YouDie Widget Section
	if (IsValid(YouDieWidgetClass))
	{
		YouDieWidgetInstance = CreateWidget<UGHPlayerWidget>(GetWorld(), YouDieWidgetClass);
		MainBtn = Cast<UButton>(YouDieWidgetInstance->GetWidgetFromName(TEXT("MainBtn")));
		MainBtn->OnClicked.AddDynamic(this, &AGHPlayer::MainBtnClicked);
	}

	// Anim Section
	Anim = Cast<UGHPlayerAnim>(GetMesh()->GetAnimInstance());

	// Roll Section (Timeline)
	RollSetting();

}

void AGHPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Death Widget Section
	if (MyController->IsZoomIn())
	{
		MyController->ZoomIn(DeltaTime);
	}

	// Run Section
	if (bBoost)
	{
		// ���¹̳� �Ҹ�
		Cast<UGHPlayerStatComponent>(Stat)->UseStamina(EStaminUseType::RUN);
		// ���¹̳� üũ
		bool bRun = Cast<UGHPlayerStatComponent>(Stat)->CheckStamina(EStaminUseType::RUN);

		// Boost ������ Ȯ��
		UCharacterMovementComponent* Movement = GetCharacterMovement();
		if (3.f >= Movement->Velocity.Size()
			|| false == bRun)
		{
			Movement->MaxWalkSpeed = PrevMaxSpeed;
			bBoost = false;
		}
	}

	// Roll Section
	TimelineRoll.TickTimeline(DeltaTime);
}

void AGHPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	const UGHPlayerInputAction* PlayerInput = GetDefault<UGHPlayerInputAction>();
	if (IsValid(EnhancedInputComp) && IsValid(PlayerInput))
	{
		// Move
		MoveActionValue = &EnhancedInputComp->BindActionValue(PlayerInput->IA_SetDestination);
		EnhancedInputComp->BindAction(PlayerInput->IA_SetDestination, ETriggerEvent::Triggered, this, &AGHPlayer::IA_SetDestination_Triggered);
		EnhancedInputComp->BindAction(PlayerInput->IA_SetDestination, ETriggerEvent::Started, this, &AGHPlayer::IA_SetDestination_Started);
		EnhancedInputComp->BindAction(PlayerInput->IA_SetDestination, ETriggerEvent::Canceled, this, &AGHPlayer::IA_SetDestination_Canceled);
		EnhancedInputComp->BindAction(PlayerInput->IA_SetDestination, ETriggerEvent::Completed, this, &AGHPlayer::IA_SetDestination_Completed);
		// Item Slot
		EnhancedInputComp->BindAction(PlayerInput->IA_SlotNum1, ETriggerEvent::Started, this, &AGHPlayer::IA_SlotNum1_Started);
		// Drop
		DropActionValue = &EnhancedInputComp->BindActionValue(PlayerInput->IA_Drop);
		// Inventory
		EnhancedInputComp->BindAction(PlayerInput->IA_Inventory, ETriggerEvent::Started, this, &AGHPlayer::IA_Inventory_Started);
		// Equip
		EnhancedInputComp->BindAction(PlayerInput->IA_Equip, ETriggerEvent::Started, this, &AGHPlayer::IA_Equip_Started);
		// Noraml Attack
		EnhancedInputComp->BindAction(PlayerInput->IA_NormalAttack, ETriggerEvent::Started, this, &AGHPlayer::IA_NormalAttack_Started);
		// Run
		EnhancedInputComp->BindAction(PlayerInput->IA_Run, ETriggerEvent::Started, this, &AGHPlayer::IA_Run_Started);
		// Roll
		EnhancedInputComp->BindAction(PlayerInput->IA_Roll, ETriggerEvent::Started, this, &AGHPlayer::IA_Roll_Started);
	}
}

void AGHPlayer::SetDeath()
{
	Super::SetDeath();

	// �÷��̾� ��Ʈ�ѷ� Ÿ�� ����
	AGHPlayerController* PlayerController = Cast<AGHPlayerController>(GetController());
	if (IsValid(PlayerController) && IsValid(GameOverWidgetInstance))
	{
		// GameOver Widget ���
		GameOverWidgetInstance->AddToViewport();

		// �Է� Ÿ�� ���� : Game -> GameOver Widget
		FInputModeUIOnly InputUIMode;
		InputUIMode.SetWidgetToFocus(GameOverWidgetInstance->TakeWidget());
		PlayerController->SetInputMode(InputUIMode);
	}

	// �÷��̾� �̵� ����
	GetCharacterMovement()->StopMovementImmediately();

	// �÷��̾� �浹 ����
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Dead �ִϸ��̼� ���
	Anim->PlayKnockDownMontage();
}

void AGHPlayer::UpdateStateWidget()
{
	UGHPlayerStatComponent* PlayerStat = Cast<UGHPlayerStatComponent>(Stat);
	if (IsValid(StateWidgetInstance) && IsValid(PlayerStat))
	{
		StateWidgetInstance->GetHealthBar()->SetPercent(PlayerStat->GetCurrnetHealth() / PlayerStat->GetMaxHealth());
		StateWidgetInstance->GetManaBar()->SetPercent(PlayerStat->GetCurrentMana() / PlayerStat->GetMaxMana());
		StateWidgetInstance->GetStaminaBar()->SetPercent(PlayerStat->GetCurrentStamina() / PlayerStat->GetMaxStamina());
		StateWidgetInstance->GetEXPBar()->SetPercent(PlayerStat->GetCurrentEXP() / PlayerStat->GetMaxEXP());

		float CurHp = Stat->GetCurrnetHealth();
		float MaxHp = Stat->GetMaxHealth();
		float CurMana = PlayerStat->GetCurrentMana();
		float MaxMana = PlayerStat->GetMaxMana();
		StateWidgetInstance->GetCurHealthTextBlock()->SetText(FText::AsNumber(CurHp));
		StateWidgetInstance->GetMaxHealthTextBlock()->SetText(FText::AsNumber(MaxHp));
		StateWidgetInstance->GetCurManaTextBlock()->SetText(FText::AsNumber(CurMana));
		StateWidgetInstance->GetMaxManaTextBlock()->SetText(FText::AsNumber(MaxMana));
	}
}

void AGHPlayer::UpdateStaminaWidget()
{
	UGHPlayerStatComponent* PlayerStat = Cast<UGHPlayerStatComponent>(Stat);
	if (IsValid(StateWidgetInstance) && IsValid(PlayerStat))
	{
		StateWidgetInstance->GetStaminaBar()->SetPercent(PlayerStat->GetCurrentStamina() / PlayerStat->GetMaxStamina());
	}
}

void AGHPlayer::YesBtnClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Click Yes Btn"));

	// ���� ������ �̵�
	UGameplayStatics::OpenLevel(this, FName("CurrentLevel"));

	// �Է� Ÿ�� ���� : GameOver Widget -> Game
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (PlayerController)
	{
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(false);
		PlayerController->SetInputMode(InputMode);
	}
}

void AGHPlayer::NoBtnClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Click No Btn"));

	// �÷��̾� ��Ʈ�ѷ� Ÿ�� ����
	AGHPlayerController* PlayerController = Cast<AGHPlayerController>(GetController());
	if (IsValid(PlayerController) && IsValid(YouDieWidgetInstance))
	{
		// Platyer State Widget ����
		StateWidgetInstance->RemoveFromParent();

		// GameOver Widget ����
		GameOverWidgetInstance->RemoveFromParent();

		// YouDie Widget ���
		YouDieWidgetInstance->AddToViewport();

		// �Է� Ÿ�� ���� : GameOver Widget -> YouDie Widget
		FInputModeUIOnly InputUIMode;
		InputUIMode.SetWidgetToFocus(YouDieWidgetInstance->TakeWidget());
		PlayerController->SetInputMode(InputUIMode);
	}

	// Player Ŭ�����
	MyController->SetZoomIn(true);
	// Dead �ִϸ��̼� ���
	Anim->PlayDeathMontage();

}

void AGHPlayer::MainBtnClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Click Main Btn"));
}

UChildActorComponent* AGHPlayer::FindChildActorMap(FName Name)
{
	UChildActorComponent** Result = ChildActorMap.Find(Name);

	if (Result != nullptr)
		return *Result;
	else
		return nullptr;
}

void AGHPlayer::AddChildActorMap(FName Name, UChildActorComponent* ChildActor)
{
	ChildActorMap.Add(Name, ChildActor);
}

void AGHPlayer::IA_SetDestination_Triggered(const FInputActionValue& Value)
{
	Cast<AGHPlayerController>(GetController())->GetLocationUnderCursor();
	Cast<AGHPlayerController>(GetController())->Follow();
}

void AGHPlayer::IA_SetDestination_Started(const FInputActionValue& Value)
{
	GetCharacterMovement()->StopMovementImmediately();
}

void AGHPlayer::IA_SetDestination_Canceled(const FInputActionInstance& Value)
{
}

void AGHPlayer::IA_SetDestination_Completed(const FInputActionInstance& Value)
{
	float ElapsedTime = Value.GetElapsedTime();

	if (ElapsedTime < 0.5f)
	{
		Cast<AGHPlayerController>(GetController())->MoveTo();
	}
}

void AGHPlayer::IA_SlotNum1_Started(const FInputActionValue& Value)
{
	if (IsValid(Stat))
	{
		float Damage = 100.f;
		UGameplayStatics::ApplyDamage(this, Damage, GetController(), this, UDamageType::StaticClass());
	}

	UE_LOG(LogTemp, Log, TEXT("Slot Num1"));
}

void AGHPlayer::IA_Inventory_Started(const FInputActionValue& Value)
{
	Inventory->ReviewInventory();
}

void AGHPlayer::IA_Equip_Started(const FInputActionValue& Value)
{
	if (ChildActorMap.IsEmpty()) return;

	if (false == bEquip)
	{
		bEquip = Inventory->Armed(FName("Sword"));
	}
	else
	{
		if(Inventory->DisArmed(FName("Sword")))
			bEquip = false;
	}
}

void AGHPlayer::IA_NormalAttack_Started(const FInputActionValue& Value)
{
	// ��� �������� Ȯ��
	if (bEquip)
	{
		// ���¹̳� üũ
		bool bAttack = Cast<UGHPlayerStatComponent>(Stat)->CheckStamina(EStaminUseType::ATTACK);
		if (false == bAttack) return;

		// �ǰݴ�� �����̳� �����
		HitCheckContainer.Empty();

		// �÷��̾� �̵� ����
		GetCharacterMovement()->StopMovementImmediately();

		// ���� �ִϸ��̼� ���
		Anim->PlayNormalAttackMontage();

		UE_LOG(LogTemp, Log, TEXT("Player Attack"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No Weapon"));
	}
}

void AGHPlayer::AttackCheck_Begin(FVector& Start_V, FVector End_V, FVector& Start_H, FVector& End_H)
{
	if (this == nullptr) return;

	UChildActorComponent** SwordActorComp = ChildActorMap.Find(FName("Sword"));
	if (nullptr != SwordActorComp)
	{
		AActor* ChildActor = (*SwordActorComp)->GetChildActor();
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
	}
}

void AGHPlayer::AttackCheck_Tick(FVector& Start_V, FVector End_V, FVector& Start_H, FVector& End_H)
{
	if (this == nullptr) return;

	if (bEquip)
	{
		UChildActorComponent** SwordActorComp = ChildActorMap.Find(FName("Sword"));
		if (nullptr != SwordActorComp)
		{
			AActor* ChildActor = (*SwordActorComp)->GetChildActor();
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
			bool IsHit = GetWorld()->LineTraceMultiByChannel(HitResultsVertical, Start_V, End_V, ECC_GameTraceChannel4, CollisionParamsVertical);

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
			IsHit = GetWorld()->LineTraceMultiByChannel(HitResultsHorizontal, Start_H, End_H, ECC_GameTraceChannel4, CollisionParamsVertical);

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
}

void AGHPlayer::IA_Run_Started(const FInputActionValue& Value)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!IsValid(Movement)) return;

	if (bBoost)
	{
		Movement->MaxWalkSpeed = PrevMaxSpeed;
		bBoost = false;
	}
	else
	{
		float Speed = Movement->Velocity.Size();
		if (Speed > 3.f)
		{
			PrevMaxSpeed = Movement->GetMaxSpeed();
			Movement->MaxWalkSpeed = PrevMaxSpeed + 300.f;
			bBoost = true;
		}
	}
	
}

void AGHPlayer::IA_Roll_Started(const FInputActionValue& Value)
{
	// ���¹̳� üũ
	bool bRoll = Cast<UGHPlayerStatComponent>(Stat)->CheckStamina(EStaminUseType::ROLL);
	if (false == bRoll) return;

	// Roll �ִϸ��̼� ���
	Anim->PlayRollMontage();
}

void AGHPlayer::RollSetting()
{
	if (nullptr != RollCurve)
	{
		FOnTimelineFloat RollStartCallback;
		RollStartCallback.BindUFunction(this, FName("RollStart"));
		TimelineRoll.AddInterpFloat(RollCurve, RollStartCallback);

		FOnTimelineEvent RollEndCallback;
		RollEndCallback.BindUFunction(this, FName("RollEnd"));
		TimelineRoll.SetTimelineFinishedFunc(RollEndCallback);

		TimelineRoll.SetLooping(false);
	}
}

void AGHPlayer::StartRollTimeline()
{
	// ���¹̳� �Ҹ�
	Cast<UGHPlayerStatComponent>(Stat)->UseStamina(EStaminUseType::ROLL);

	// ������� ����
	RollStartPos = GetActorLocation();

	// ���콺 �������� �÷��̾� ȸ��
	AGHPlayerController* PlayerController = Cast<AGHPlayerController>(GetController());
	if (nullptr == PlayerController) return;

	PlayerController->GetLocationUnderCursor();		// ���콺�� ������� ��ġ Ȯ��
	PlayerController->Rotate();						// Player ȸ��
	RollDirection = PlayerController->Direction;	// Roll ���� ���� ����

	// Roll ������ �÷��̾��� ���ۿ� ���� ������ ����
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// Ÿ�Ӷ����� ó������ ��� (Roll)
	TimelineRoll.PlayFromStart();
}

void AGHPlayer::RollStart(float Value)
{
	// �̵� �Ÿ� ���
	FVector NewLocation = RollStartPos + (RollDirection * RollDistance * Value);

	// ĳ���� ��ġ ������Ʈ
	SetActorLocation(NewLocation);
}

void AGHPlayer::RollEnd()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	ResetRollTimeline();
}

void AGHPlayer::ResetRollTimeline()
{
	// ������� �ʱ�ȭ
	RollStartPos = FVector::ZeroVector;

	// Ÿ�Ӷ����� �����ϰ� ó������ �ʱ�ȭ (Roll)
	TimelineRoll.Stop();
}


void AGHPlayer::EnemyHit(TArray<FHitResult>& HitResults)
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

	// ī�޶� ó��
	TSubclassOf<UCameraShakeBase> ShakeClass = UGHPlayerAttackCameraShake::StaticClass();	// ī�޶� ����ũ Ŭ����
	FVector Epicenter = GetActorLocation();	// ī�޶� ����ũ �߻� ��ġ
	float InnerRadius = 10.0f;	// ����ũ ������ �ִ�ġ�� ����� �ݰ� ����
	float OuterRadius = 2000.0f;	// ����ũ ������ ������� �ܰ� �ݰ� ����
	float Falloff = 1.0f;	// ����ũ ���� ���� ���� ����
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), ShakeClass, Epicenter, InnerRadius, OuterRadius, Falloff);
}

float AGHPlayer::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// UI ������Ʈ
	UpdateStateWidget();

	if (Stat->GetCurrnetHealth() <= 0.f)
	{
		SetDeath();
	}

	return 0.0f;
}
