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

	// CameraBoom 생성 (충돌이 발생할 경우 player쪽으로 땡김)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; //카메라 위치
	CameraBoom->bUsePawnControlRotation = true; //카메라 막대를 컨트롤 기반으로 회전시키게 함.

	//collision 캡슐 크기 설정을 할 수 있다. CapsuleComponent.h를 include해줘야한다.
	GetCapsuleComponent()->SetCapsuleSize(34.0f, 86.0f);

	//FollowCamera 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	

	/**	카메라가 controller roatation을 따라가는걸 원하지 않음. 그냥 카메라붐에 붙어있길 원하고, controller에 의존성을 띄지 않기 위함이다.
		컨트롤러가 카메라 boom을 회전시키면, follow camera가 따라간다.
	*/
	// 카메라를 boom끝에 연결하고, boom을 컨트롤러 방향에 맞게 조정한다.
	FollowCamera->bUsePawnControlRotation = false;

	// TurnRate 세팅
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//회전시 카메라에만 영향가도록 하기 위함.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;


	// 캐릭터 움직임 수정
	GetCharacterMovement()->bOrientRotationToMovement = true; //캐릭터는 자신이 움직이는 방향으로 자동으로 방향을 전환한다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ㄴ이 회전속도로
	GetCharacterMovement()->JumpZVelocity = 430.f; //점프높이 설정.
	GetCharacterMovement()->AirControl = 0.2f; //공중에 머물때 아주 조금 움직일 수 있게 설정함.


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

	//Enum초기화
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	//어차피 값을 변경할 수 있기때문에 적당히 값을 주면된다.
	//최대스테미나를 고려해서 적당히 값을 주자.
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

	//DeltaStamina는 특정 프레임에서 Stamina가 얼만큼 변해야 하는가를 나타냄.
	//Stamina의 소진속도와 회복속도를 다르게 하고 싶다면, 회복 Rate를 따로 선언해주고 위와 동일하게 해주면 된다.
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	//ESS enum처럼 주어진 시간에 Status중 하나를 할때는 여러개의 if else문을 수행하는것 보다 switch문을 사용하는게 좋다.

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown) //ShiftKey가 눌렸을때
		{
			//Stamina를 감소시키기 이전에 Minimul Stamina를 넘었는지를 확인해야한다. 넘으면 Below minimum state로 넘겨줘야함. 
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminiaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}

			//ShiftKey가 눌리고 위의 Stamina 감소 동작을 한 후에, Movement Status를 변경해준다.
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else //ShiftKey가 눌리지 않았을때
		{
			//Stamina가 회복되어야 하는지 확인해야한다.(최대치를 넘기지 않아야하기 때문)
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);//눌리지 않았을때는 Normal State로
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
		else //Shift Key가 눌리지 않았을때
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
		else //Shift Key가 눌리지 않았을때
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
		/** 위 값들이 valid하면, Character의 Lookup rotaion을 부드럽게 회전시킬거다. */
	
		//우선 이렇게 하게되면 해당 Yaw 방향으로 바로 바라보게 된다.
		FRotator LookAtYaw = GetLookatRotationYaw(CombatTarget->GetActorLocation());
		
		//부드러운 회전 보간을 위한 작업을 한다.
		//정확한 방향으로 해당 frame내에서 부드러운 전환을 하게 된다.
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		//마지막으로 해당하는 Rotation을 설정해주면 된다.
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
	//이 Target에 방향을 맞추기 위해 필요한 Rotation을 구함.
	// 그러기 위해서 UKismetMathLibarary에 있는 FindLookAtRotation함수를 사용할거다.
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	
	//Yaw가 중요하니 따로 Yaw만 구하는 FRotator변수를 두자.
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

	/** Turn과 LookUp은 마우스를 위한 기능인데, 우리는 마우스를 위한 어떠한 함수도 정의 하지않았다.
		우리는 이미 Pawn에 AddControllerYawInput과 PitchInput를 갖고있기 때문이다.
		여기서 상속받은 함수에 대한 포인터를 주면된다.
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

//여기서는 단순히 Bind하기 위한거고, 실제 구현은 바로위의 InputComponent함수에서 구현한다.
void AMain::MoveForward(float Value)
{
	bMovingForward = false;
	if (CanMove(Value))
	{
		/*어느방향이 앞으로 가야하는지 알아내는거임 */
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		//forward 벡터를 구하는거임 우선, 음 FRotation 행렬 생성자에 의해 회전행렬을 얻을 수 있다.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//위에서 계산한 방향으로 움직인다.
		AddMovementInput(Direction, Value); 
		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	if (CanMove(Value))
	{
		/*어느방향이 앞으로 가야하는지 알아내는거임 */
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
	/** Yaw Axis의 컨트롤러를 입력 및 회전 시키는 함수임. float값이 필요함	*/
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

//movement Status, Speed를 변경함
void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;

	//Enum에 접근하려면 Scope가 지정되어 있기 때문에 이름을 다 적어줘야함.
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

	if (ActiveOverlappingItem) //Weapon.h include해줘야함
	{
		//ActiveOverlappingItem은 Item class, 이를 Weapon class로 cast해줌.
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
	//배열은 배열의 크기를 되돌릴 수 있는 기능을 갖고있기 때문에 적절한 횟수로 반복해주자.
	//Array.Num은 배열 원소의 개수를 리턴함.
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

		//AnimInstance형식을 가져오는데, 얘는 Mesh(우리 캐릭터의 Skeletal Mesh)의 AnimInstance를 가져오는거죠.
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //SkeletalMeshComponent 헤더파일 include

		if (AnimInstance && CombatMontage)//UAminInstance 헤더파일 include
		{
			int32 Section = FMath::RandRange(0, 1);
			
			if (Section == 0)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f); //Combat Montage를 play할거고, play rate는 1.35로 약간 빠르게 해줬다.

				//Combat Montage안에도 여러 Section이 있기때문에 어떠한 SEction을 선택할건지 해준다.
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
	//0이하로 가면 죽게 함.
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		Die();
	}
	else Health -= Amount;
}

void AMain::Die()
{
	//이미 죽은상태라면 그냥 return시켜서 더이상 dead anim이 재생되지 않도록.
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	//Attack 함수에서 했던것처럼 AnimInstance와 CombatMontage를 play할거다.
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
		//ACharacter::Jump(); Super가 안되서 이렇게 했는데 다시 되서 super를 쓰기로함.
		Super::Jump();
	}
}

//이 함수를 사용하는 이유는 여기 있는 여러 파라미터들을 'Apply Damage'라는 함수 호출파라미터로 적용할 수 있기 때문이다.
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
	//GetOverlappingActor의 첫번째 파라미터
	TArray<AActor*> OverlappingActors;

	/** 우선 overlapping된 모든 Actor를 얻어와야한다.
		이를 위해 GetOverlappingActor함수를 사용하는데 첫번째파라미터는 TArray를 넘겨주면된다(Data store)
		두번째 파라미터는 TSubclassof class filter인데 클래스에서 지정한거나 파생된것들을 걸러낸다.
		이를 이용해 Explosive나 Pickups같은것을 걸러내고 오로지 Enemy만 forcus할 수 있다.
	*/
	GetOverlappingActors(OverlappingActors, EnemyFilter);
	
	
	//overlapping된 actor를 얻어왔으면 이것중 하나를 CombatTarget으로 setting해준다.
	//setting하기전에 어떤걸 우선순위로 둘지 정해야하는데 Main Char와 가장 가까운 Enemy로 하자.

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
	//World에 있는 GetMapName을 이용
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		/** CurrentLevel은 FString, Level Name은 FName이라 type을 맞추기.
			FName은 FString으로 초기화 할 수 없지만 String literal로 초기화할 수 있다.
			이 String literal은 FString에서 역참조 연산자(Dereference operator)를 통해 얻을 수 있다. ->그냥 포인터 참조방식이다. 
			*(Asterisk) 연산자라고도 함. */
		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName) //지금 level name과 같지 않으면(같으면 무의미하기에) 넘어가게한다.
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

	//얘의 리턴값은 (마우스 호버링) USaveGame이니 UFirstSaveGame으로 cast해줘야 사용할 수 있다.
	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));
	
	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;


	//Weapon을 불러오기 전에, BP에 저장된 WeaponStorage가 있는지 확인부터 한다.
	if (WeaponStorage)
	{
		//있다면 해당 instance를 생성하기 위해 Uclass를 넘겨줬고 Weapon을 담아 생성해준다.
		AItemStorage* Weapon = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapon)
		{
			//FirstSaveGame에 저장된 WeaponName을 넣어주고
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			//Weapon->WeaponMap[WeaponName]; key값에 저장된 무기의 이름을 넣어주면 된다.
			//위 FString은 분할해서 보기 편하게 하기 위해서 따로 빼둔것.
			if (WeaponName != "")//WeaponName을 무조건 담기 때문에 Weaponname이 없으면==무기없음. 체크를 해줬다.
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapon->WeaponMap[WeaponName]);

				/** 스폰을 시킨 뒤 해야할것은 장착시키기.*/
				WeaponToEquip->Equip(this);
			}
		}
	}

	if (LoadGameInstance->CharacterStats.LevelName != TEXT("")) //이걸 안쓰면 같은 레벨에서의 위치값 저장은 된다.
	{
		FName LevelName(*LoadGameInstance->CharacterStats.LevelName);
		SwitchLevel(LevelName);
	}

	if (bSetPosition) //LevelChange때에도 이 함수를 사용해야함. 그렇기 때문에 위치,회전값을 load할껀지 check
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	//죽고난 뒤 Load했을때 다시 Animation을 업데이트해줘야함.
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