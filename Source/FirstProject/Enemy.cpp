// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "Weapon.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent()); 
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	LeftCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCombatCollision"));
	LeftCombatCollision->SetupAttachment(GetMesh(), FName("EnemyLeftSocket"));
	//LeftCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemyLeftSocket"));

	/** Attach To Component함수는 Component를 다른것에 Attach할 수 있게 하는 함수다.
		첫번째 파라미터는 USceneCompoent인데 Mesh는 SceneComponent에서 파생되니 GetMesh로 넘겨주면된다. (Attach 할 SceneComponent)
		두번째 파라미터는 FAttachmentTransformRules이다. 이것들은 fully qualifying을 이용해 접근 가능하다.
		세번째 파라미터는 SocketName을 FName type으로 넘겨주면 된다.
	*/
	RightCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCombatCollision"));
	RightCombatCollision->SetupAttachment(GetMesh(), FName("EnemyRightSocket"));
	//RightCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemyRightSocket"));

	AgroSphere->InitSphereRadius(600.f);
	CombatSphere->InitSphereRadius(50.f);

	bOverlappingCombatSphere = false;
	   
	Health = 85.f;
	MaxHealth = 100.f;
	Damage = 15.f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 1.5f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDealy = 3.f;

	bHasValidTarget = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//내 Controller를 AIController로 캐스트, 저장함.
	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlapEnd);	
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOverlapEnd);

	LeftCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	LeftCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
	RightCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	RightCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemy::AgroSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Move to function을 호출. controller로 부터 AI move to function이 호출되기 때문에 Controller를 참조해야함. ->BeginPlay
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			MoveToTarget(Main);
		}
	}
}

void AEnemy::AgroSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bHasValidTarget = false;
				if (Main->CombatTarget == this)
				{
					Main->SetCombatTarget(nullptr);
				}
				Main->SetHasCombatTarget(false);
				
				Main->UpdateCombatTarget();

				/*if (Main->MainPlayerController)
				{
					Main->MainPlayerController->RemoveEnemyHealthBar();
				}*/
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
				if (AIController)
				{
					AIController->StopMovement();
				}
			}
		}
	}
}


void AEnemy::CombatSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bHasValidTarget = true;
				Main->SetCombatTarget(this);
				Main->SetHasCombatTarget(true);

				Main->UpdateCombatTarget();
				//if (Main->MainPlayerController)
				//{
				//	Main->MainPlayerController->DisplayEnemyHealthBar();
				//}

				CombatTarget = Main;
				bOverlappingCombatSphere = true;
				
				//Attack(); AttackEnd의 함수에 있는것처럼 Delay를 줌.
				//Attack min time -> 0.5f, Max time -> 1.5f;
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
			}
		}
	}
}

void AEnemy::CombatSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Alive())
	{
		if (OtherActor && OtherComp)
		{
			AMain* Main = Cast<AMain>(OtherActor);
			{
				if (Main)
				{
					bOverlappingCombatSphere = false;
					MoveToTarget(Main);
					CombatTarget = nullptr;

					if (Main->CombatTarget == this) //이 Enemy를 계속 갖고있으면 안되기 때문에.
					{
						Main->SetCombatTarget(nullptr);
						Main->bHasCombatTarget = false;
						Main->UpdateCombatTarget();
					}
					if (Main->MainPlayerController)
					{
						USkeletalMeshComponent* MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
						if (MainMesh)
						{
							Main->MainPlayerController->RemoveEnemyHealthBar();
						}
					}

					GetWorldTimerManager().ClearTimer(AttackTimer);
				}
			}
		}
	}
}

void AEnemy::MoveToTarget(AMain* Target)
{
	if (Alive())
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

		if (AIController)
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(5.f);

			//OutPath는 우리가 이걸 호출하면 정보를 저장해둔다.
			FNavPathSharedPtr NavPath;
			AIController->MoveTo(MoveRequest, &NavPath);

			/* MoveToTarget의 Path Point를 보여주기 위한 코드*/
			/*
			auto PathPoints = NavPath->GetPathPoints();
			TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();

			for (auto Point : PathPoints)
			{
				UKismetSystemLibrary::DrawDebugSphere(this, Point.Location, 15.f, 8, FLinearColor::Blue, 30.f, 5.f);
			}
			*/
		}
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->HitParticles)
			{
				const USkeletalMeshSocket* LeftTipSocket = GetMesh()->GetSocketByName("LeftTipSocket");
				const USkeletalMeshSocket* RightTipSocket = GetMesh()->GetSocketByName("RightTipSocket");
				if (LeftTipSocket && RightTipSocket)
				{
					FVector LeftSocketLocation = LeftTipSocket->GetSocketLocation(GetMesh());
					FVector RightSocketLocation = RightTipSocket->GetSocketLocation(GetMesh());
					switch(Section)
					{
					case 0:
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, RightSocketLocation, FRotator(0.f), false);
						break;
					case 1:
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, LeftSocketLocation, FRotator(0.f), false);
						break;
					default:
						break;
					}					
				}
			}

			if (Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}

		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::ActivateCollision()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::DeactivateCollision()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	if (Alive() && bHasValidTarget)
	{
		if (AIController)
		{
			//AIController가 valid하다면, 움직임을 멈추고 EMS를 Attacking으로 바꿔준다.
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		if (!bAttacking) //만일, Enemy가 공격하고 있지 않다면
		{
			bAttacking = true; //해당 boolean 변수를 true로 변경해준뒤 Anim Montage를 play해주면된다.
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				Section = FMath::RandRange(0, 1);
				switch (Section)
				{
				case 0:
					AnimInstance->Montage_Play(CombatMontage, 1.35f);
					AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
					break;
				case 1:
					AnimInstance->Montage_Play(CombatMontage, 1.35f);
					AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
					break;
				default:
					break;
				}
			}
		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	if (bOverlappingCombatSphere)
	{
		//Min -> 0.5, Max -> 1.5f
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		UE_LOG(LogTemp, Warning, TEXT("Next AttackTime : %f"), AttackTime);
		
		//Min과 Max사이의 랜덤 초를 얻었으면 이를 WorldTimerManager를 이용해서 Setting한다.
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die(DamageCauser);
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AEnemy::Die(AActor* Causer)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"),CombatMontage);
	}

	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AMain* Main = Cast<AMain>(Causer);
	AWeapon* Weapon = Cast<AWeapon>(Causer);

	if (Main)
	{
		Main->UpdateCombatTarget();
	}

	if (Weapon)
	{
		if (CombatTarget)
		{
			CombatTarget->UpdateCombatTarget();
		}
	}
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDealy);
}

bool AEnemy::Alive() 
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	Destroy();
}