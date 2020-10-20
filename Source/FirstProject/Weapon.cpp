// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Enemy.h"


AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	bWeaponParticle = false;

	WeaponState = EWeaponState::EWS_Pickup;	//SetWeaponState(EWeaponState::EWS_Pickup);


	Damage = 20.f;

}
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	//Query Only�� �ϰԵǸ� Overlap event���� physics�� ������� �ʵ��� �����ѵ�, Overlap envent�� generate�ϰ� �ȴ�.
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//World���� ������ �� �ִ� ���𰡰� �ɰŰ�, static������ �ƴҰŴ�. ��, �⺻������ �ڵ������� overlap �Ű������� �����Ǿ� �ִ�.
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	//�̷��� �ϰ� �Ǹ� �ϴ� ������ �����ѵ�, Enemy�� Pawn�̴� Pawn�� ���� Overlap�� ���ؼ��� response�ϰ� �Ǵ°Ŵ�.
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//�θ������ ȣ���ϱ� ����.
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	//Overlap�� �ϰ� �Ǹ� Main Char���� Weapon(�ڱ��ڽ�)�� �����ϰ� �ߴ�.

	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)//�ش� Weapon�� Pickup State�� �ƴ϶�� ������ �ʿ䰡 ����
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			//Equip(Main); �������θ� �÷��̾�� �ѱ�� ���ؼ� �ּ�ó��.
			//WeaponŬ������ �Ѱܼ� Overlapping�� Item�� Ȯ�ν�����.
			Main->SetActiveOverlappingItem(this);
		}
	}

}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			//Overlap�� �ߴµ� �������� �ʾҴٸ� Nullptr�� �ٲ㼭 ActiveOverlappingItem�� �����.
			Main->SetActiveOverlappingItem(nullptr);
		}
	}

}

void AWeapon::Equip(AMain* Char) //Main Header������ include��������.
{
	if (Char)
	{
		//Instigator �Ķ���͸� ������.
		SetInstigator(Char->GetController());

		/** SkeletalMesh�� Collision response�� ignore�Ǿ��ִ��� Ȯ���ؾ��Ѵ�.
			��, Camera�� Collision�� ignore�ϵ��� �����ҰŴ�.
			(Weapon�� Character�� Camera���̿� ������ Camera�� Character�� Ȯ������ �ʵ��� �ϱ� �����̴�.)
		*/
		//�̷��� �ϸ� Camera�� Ȯ����� �ʴ´�. �����ϰԵ�.
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		//��, Pawn�� ���õ� �� �ֵ��� �Ѵ�.
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		//�̷��� �ϸ� Weapon�� ���� Camera�� pawn�� Camera�� �����ϰ� �ȴ�.

		/** �׸��� Simulating physics�� �ϰ��ִٸ�, Player�� Attach�� �ؾ� �ϱ� ������ Simulating physics�� ������Ѵ�.
		*/
		SkeletalMesh->SetSimulatePhysics(false);
		//�̷��� �ؼ�, Collision�� Physics������ �ذ��ߴ�. 

		//���������� Attach�� ��������. SkeletalMeshSocket.h�� include�������.
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		/** ���� ���� ����� �����̶�� �� RightHandSocket������ Character skeleton�� �ִ� RightHandSocket�� ���� ������ �ɰŴ�.
			�׷��� �� ����� ��ȿ���� �ѹ��� Ȯ�� �ؾ��Ѵ�.
		*/

		
		if(RightHandSocket)
		{
			//rightHandSocket�� NULL�� �ƴϸ�, attach��Ų��.
			//�̷����ϸ� Weapon�� Skeleton�� ���̰� �ȴ�.
			RightHandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false;
			

			/** �̷��� �ص� ������ ���ǿ����� Main.h�� SetEquippedWeapon�Լ��� forceinline���� ���� �ʰ� ���� ����
				�Ʒ��� �����ߴ�.
			//EquippedWeapon�� ��������
			const AWeapon* EquippedWeapon = Char->GetEquippedWeapon();
			if (EquippedWeapon) //������
			{
				Char->GetEquippedWeapon()->Destroy();//Destory��Ŵ.
			}
			*/
			//�� Ư��Weapon instance�� set�����.
			Char->SetEquippedWeapon(this);
			Char->SetActiveOverlappingItem(nullptr);
		}
		if (OnEquipSound)
		{
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
		if (!bWeaponParticle)
		{
			//ParticleSystemComponent.h���� include�������.
			IdleParticlesComponent->Deactivate();
		}
		
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
				
			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass)
			{
				//Weapon���κ��� Enemy���� Damage�� ���� �� �ִ�.
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}