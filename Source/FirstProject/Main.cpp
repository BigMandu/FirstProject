// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CameraBoom ���� (�浹�� �߻��� ��� player������ ����)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; //ī�޶� ��ġ
	CameraBoom->bUsePawnControlRotation = true; //ī�޶� ���븦 ��Ʈ�� ������� ȸ����Ű�� ��.

	//collision ĸ�� ũ�� ������ �� �� �ִ�. CapsuleComponent.h�� include������Ѵ�.
	GetCapsuleComponent()->SetCapsuleSize(34.0f, 86.0f);

	//FollowCamera ����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	

	/**	ī�޶� controller roatation�� ���󰡴°� ������ ����. �׳� ī�޶�տ� �پ��ֱ� ���ϰ�, controller�� �������� ���� �ʱ� �����̴�.
		��Ʈ�ѷ��� ī�޶� boom�� ȸ����Ű��, follow camera�� ���󰣴�.
	*/
	// ī�޶� boom���� �����ϰ�, boom�� ��Ʈ�ѷ� ���⿡ �°� �����Ѵ�.
	FollowCamera->bUsePawnControlRotation = false;

	// TurnRate ����
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//ȸ���� ī�޶󿡸� ���Ⱑ���� �ϱ� ����.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;


	// ĳ���� ������ ����
	GetCharacterMovement()->bOrientRotationToMovement = true; //ĳ���ʹ� �ڽ��� �����̴� �������� �ڵ����� ������ ��ȯ�Ѵ�.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ���� ȸ���ӵ���
	GetCharacterMovement()->JumpZVelocity = 430.f; //�������� ����.
	GetCharacterMovement()->AirControl = 0.2f; //���߿� �ӹ��� ���� ���� ������ �� �ְ� ������.


	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;
	bEKeyDown = false;
	bLMBDown = false;
	bAttacking = false;
	bESCDown = false;

	//Enum�ʱ�ȭ
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	//������ ���� ������ �� �ֱ⶧���� ������ ���� �ָ�ȴ�.
	//�ִ뽺�׹̳��� ����ؼ� ������ ���� ����.
	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (MainPlayerController)
	{
		MainPlayerController->GameModeOnly();
	}

	if (Map != "SunTemple")
	{
		LoadGameNoSwitch();
	}
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	//DeltaStamina�� Ư�� �����ӿ��� Stamina�� ��ŭ ���ؾ� �ϴ°��� ��Ÿ��.
	//Stamina�� �����ӵ��� ȸ���ӵ��� �ٸ��� �ϰ� �ʹٸ�, ȸ�� Rate�� ���� �������ְ� ���� �����ϰ� ���ָ� �ȴ�.
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	//ESS enumó�� �־��� �ð��� Status�� �ϳ��� �Ҷ��� �������� if else���� �����ϴ°� ���� switch���� ����ϴ°� ����.

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown) //ShiftKey�� ��������
		{
			//Stamina�� ���ҽ�Ű�� ������ Minimul Stamina�� �Ѿ������� Ȯ���ؾ��Ѵ�. ������ Below minimum state�� �Ѱ������. 
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminiaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}

			//ShiftKey�� ������ ���� Stamina ���� ������ �� �Ŀ�, Movement Status�� �������ش�.
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else //ShiftKey�� ������ �ʾ�����
		{
			//Stamina�� ȸ���Ǿ�� �ϴ��� Ȯ���ؾ��Ѵ�.(�ִ�ġ�� �ѱ��� �ʾƾ��ϱ� ����)
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);//������ �ʾ������� Normal State��
		}

		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminiaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0.f;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		}
		else //Shift Key�� ������ �ʾ�����
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminiaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}

		break;

	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else //Shift Key�� ������ �ʾ�����
		{
			SetStaminiaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);

		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminiaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);

		break;

	default:
		;
	}
	
	if (bInterpToEnemy && CombatTarget)
	{
		/** �� ������ valid�ϸ�, Character�� Lookup rotaion�� �ε巴�� ȸ����ų�Ŵ�. */
	
		//�켱 �̷��� �ϰԵǸ� �ش� Yaw �������� �ٷ� �ٶ󺸰� �ȴ�.
		FRotator LookAtYaw = GetLookatRotationYaw(CombatTarget->GetActorLocation());
		
		//�ε巯�� ȸ�� ������ ���� �۾��� �Ѵ�.
		//��Ȯ�� �������� �ش� frame������ �ε巯�� ��ȯ�� �ϰ� �ȴ�.
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		//���������� �ش��ϴ� Rotation�� �������ָ� �ȴ�.
		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

FRotator AMain::GetLookatRotationYaw(FVector Target)
{
	//�� Target�� ������ ���߱� ���� �ʿ��� Rotation�� ����.
	// �׷��� ���ؼ� UKismetMathLibarary�� �ִ� FindLookAtRotation�Լ��� ����ҰŴ�.
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	
	//Yaw�� �߿��ϴ� ���� Yaw�� ���ϴ� FRotator������ ����.
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMain::EkeyDown);
	PlayerInputComponent->BindAction("Equip", IE_Released, this, &AMain::EkeyUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	/** Turn�� LookUp�� ���콺�� ���� ����ε�, �츮�� ���콺�� ���� ��� �Լ��� ���� �����ʾҴ�.
		�츮�� �̹� Pawn�� AddControllerYawInput�� PitchInput�� �����ֱ� �����̴�.
		���⼭ ��ӹ��� �Լ��� ���� �����͸� �ָ�ȴ�.
	*/
	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

}

bool AMain::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Value != 0.0f) && (!bAttacking) &&
			(MovementStatus != EMovementStatus::EMS_Dead) &&
			!(MainPlayerController->bPauseMenuVisible);
	}
	return false;
}

//���⼭�� �ܼ��� Bind�ϱ� ���ѰŰ�, ���� ������ �ٷ����� InputComponent�Լ����� �����Ѵ�.
void AMain::MoveForward(float Value)
{
	bMovingForward = false;
	if (CanMove(Value))
	{
		/*��������� ������ �����ϴ��� �˾Ƴ��°��� */
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		//forward ���͸� ���ϴ°��� �켱, �� FRotation ��� �����ڿ� ���� ȸ������� ���� �� �ִ�.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//������ ����� �������� �����δ�.
		AddMovementInput(Direction, Value); 
		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	if (CanMove(Value))
	{
		/*��������� ������ �����ϴ��� �˾Ƴ��°��� */
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		bMovingRight = true;
	}
}

void AMain::Turn(float Value)
{
	if ((Value != 0.0f) &&
		(MovementStatus != EMovementStatus::EMS_Dead) &&
		!(MainPlayerController->bPauseMenuVisible))
	{
		AddControllerYawInput(Value);
	}
}

//Pitch Rotation
void AMain::LookUp(float Value)
{
	if ((Value != 0.0f) &&
		(MovementStatus != EMovementStatus::EMS_Dead) &&
		!(MainPlayerController->bPauseMenuVisible))
	{
		AddControllerPitchInput(Value);
	}
}

void AMain::TurnAtRate(float Rate)
{
	/** Yaw Axis�� ��Ʈ�ѷ��� �Է� �� ȸ�� ��Ű�� �Լ���. float���� �ʿ���	*/
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AMain::IncrementCoin(int32 Amount)
{
	Coins += Amount;
}
 
void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}

//movement Status, Speed�� ������
void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;

	//Enum�� �����Ϸ��� Scope�� �����Ǿ� �ֱ� ������ �̸��� �� ���������.
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}


void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}


void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::LMBDown()
{
	bLMBDown = true;

	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible)return;
	}

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (EquippedWeapon)
	{
		Attack();
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::EkeyDown()
{
	bEKeyDown = true;
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible)return;
	}

	if (ActiveOverlappingItem) //Weapon.h include�������
	{
		//ActiveOverlappingItem�� Item class, �̸� Weapon class�� cast����.
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
}
void AMain::EkeyUp()
{
	bEKeyDown = false;
}

void AMain::ESCDown()
{
	bESCDown = true;
	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
	
}
void AMain::ESCUp()
{
	bESCDown = false;
}


void AMain::ShowPickupLocations()
{
	//�迭�� �迭�� ũ�⸦ �ǵ��� �� �ִ� ����� �����ֱ� ������ ������ Ƚ���� �ݺ�������.
	//Array.Num�� �迭 ������ ������ ������.
	//for (int32 i = 0; i < PickupLocations.Num(); i++)
	//{
	//	UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 8, FLinearColor::Green, 30.f, .5f);
	//}

	for (FVector Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 30.f, .5f);
	}
}


void AMain::SetEquippedWeapon(AWeapon* WeaponToset)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToset;
}

void AMain::Attack()
{
	if (!bAttacking && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		bAttacking = true;
		SetInterpToEnemy(true);

		//AnimInstance������ �������µ�, ��� Mesh(�츮 ĳ������ Skeletal Mesh)�� AnimInstance�� �������°���.
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //SkeletalMeshComponent ������� include

		if (AnimInstance && CombatMontage)//UAminInstance ������� include
		{
			int32 Section = FMath::RandRange(0, 1);
			
			if (Section == 0)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f); //Combat Montage�� play�ҰŰ�, play rate�� 1.35�� �ణ ������ �����.

				//Combat Montage�ȿ��� ���� Section�� �ֱ⶧���� ��� SEction�� �����Ұ��� ���ش�.
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
			}
			else if (Section == 1)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.7f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
			}
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

void AMain::DecrementHealth(float Amount)
{
	//0���Ϸ� ���� �װ� ��.
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		Die();
	}
	else Health -= Amount;
}

void AMain::Die()
{
	//�̹� �������¶�� �׳� return���Ѽ� ���̻� dead anim�� ������� �ʵ���.
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	//Attack �Լ����� �ߴ���ó�� AnimInstance�� CombatMontage�� play�ҰŴ�.
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"),CombatMontage);
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::Jump()
{
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;
	}

	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		//ACharacter::Jump(); Super�� �ȵǼ� �̷��� �ߴµ� �ٽ� �Ǽ� super�� �������.
		Super::Jump();
	}
}

//�� �Լ��� ����ϴ� ������ ���� �ִ� ���� �Ķ���͵��� 'Apply Damage'��� �Լ� ȣ���Ķ���ͷ� ������ �� �ֱ� �����̴�.
float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else Health -= DamageAmount;

	return DamageAmount;
}

void AMain::UpdateCombatTarget()
{
	//GetOverlappingActor�� ù��° �Ķ����
	TArray<AActor*> OverlappingActors;

	/** �켱 overlapping�� ��� Actor�� ���;��Ѵ�.
		�̸� ���� GetOverlappingActor�Լ��� ����ϴµ� ù��°�Ķ���ʹ� TArray�� �Ѱ��ָ�ȴ�(Data store)
		�ι�° �Ķ���ʹ� TSubclassof class filter�ε� Ŭ�������� �����Ѱų� �Ļ��Ȱ͵��� �ɷ�����.
		�̸� �̿��� Explosive�� Pickups�������� �ɷ����� ������ Enemy�� forcus�� �� �ִ�.
	*/
	GetOverlappingActors(OverlappingActors, EnemyFilter);
	
	
	//overlapping�� actor�� �������� �̰��� �ϳ��� CombatTarget���� setting���ش�.
	//setting�ϱ����� ��� �켱������ ���� ���ؾ��ϴµ� Main Char�� ���� ����� Enemy�� ����.

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosetEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if (ClosetEnemy)
	{
		float MinDistance = (ClosetEnemy->GetActorLocation() - GetActorLocation()).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - GetActorLocation()).Size();
				if (MinDistance > DistanceToActor)
				{
					MinDistance = DistanceToActor;
					ClosetEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosetEnemy);
		bHasCombatTarget = true;
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	//World�� �ִ� GetMapName�� �̿�
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		/** CurrentLevel�� FString, Level Name�� FName�̶� type�� ���߱�.
			FName�� FString���� �ʱ�ȭ �� �� ������ String literal�� �ʱ�ȭ�� �� �ִ�.
			�� String literal�� FString���� ������ ������(Dereference operator)�� ���� ���� �� �ִ�. ->�׳� ������ ��������̴�. 
			*(Asterisk) �����ڶ�� ��. */
		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName) //���� level name�� ���� ������(������ ���ǹ��ϱ⿡) �Ѿ���Ѵ�.
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	//UE_LOG(LogTemp, Warning, TEXT("MapName : %s"), *MapName)
	SaveGameInstance->CharacterStats.LevelName = MapName;


	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool bSetPosition)
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	//���� ���ϰ��� (���콺 ȣ����) USaveGame�̴� UFirstSaveGame���� cast����� ����� �� �ִ�.
	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));
	
	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;


	//Weapon�� �ҷ����� ����, BP�� ����� WeaponStorage�� �ִ��� Ȯ�κ��� �Ѵ�.
	if (WeaponStorage)
	{
		//�ִٸ� �ش� instance�� �����ϱ� ���� Uclass�� �Ѱ���� Weapon�� ��� �������ش�.
		AItemStorage* Weapon = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapon)
		{
			//FirstSaveGame�� ����� WeaponName�� �־��ְ�
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			//Weapon->WeaponMap[WeaponName]; key���� ����� ������ �̸��� �־��ָ� �ȴ�.
			//�� FString�� �����ؼ� ���� ���ϰ� �ϱ� ���ؼ� ���� ���а�.
			if (WeaponName != "")//WeaponName�� ������ ��� ������ Weaponname�� ������==�������. üũ�� �����.
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapon->WeaponMap[WeaponName]);

				/** ������ ��Ų �� �ؾ��Ұ��� ������Ű��.*/
				WeaponToEquip->Equip(this);
			}
		}
	}

	if (LoadGameInstance->CharacterStats.LevelName != TEXT("")) //�̰� �Ⱦ��� ���� ���������� ��ġ�� ������ �ȴ�.
	{
		FName LevelName(*LoadGameInstance->CharacterStats.LevelName);
		SwitchLevel(LevelName);
	}

	if (bSetPosition) //LevelChange������ �� �Լ��� ����ؾ���. �׷��� ������ ��ġ,ȸ������ load�Ҳ��� check
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	//�װ� �� Load������ �ٽ� Animation�� ������Ʈ�������.
	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
}

void AMain::LoadGameNoSwitch()
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	if (WeaponStorage)
	{
		AItemStorage* Weapon = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapon)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
			if (WeaponName != "")
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapon->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}
		}
	}

	/*if (WeaponStorage) //&& EquippedWeapon != nullptr)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
			if (Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapons* WeaponToEquip = GetWorld()->SpawnActor<AWeapons>(Weapons->WeaponMap[WeaponName]);
			}
		}
	}*/

	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

}