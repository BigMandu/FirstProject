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

	//Query Only를 하게되면 Overlap event에만 physics를 사용하지 않도록 설정한뒤, Overlap envent를 generate하게 된다.
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//World에서 움직일 수 있는 무언가가 될거고, static성질이 아닐거다. 또, 기본적으로 자동적으로 overlap 매개변수가 설정되어 있다.
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	//이렇게 하게 되면 일단 모든것을 무시한뒤, Enemy가 Pawn이니 Pawn에 대한 Overlap에 대해서만 response하게 되는거다.
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//부모버전을 호출하기 위함.
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	//Overlap을 하게 되면 Main Char에게 Weapon(자기자신)을 장착하게 했다.

	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)//해당 Weapon이 Pickup State가 아니라면 수행할 필요가 없음
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			//Equip(Main); 장착여부를 플레이어에게 넘기기 위해서 주석처리.
			//Weapon클래스를 넘겨서 Overlapping된 Item을 확인시켜줌.
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
			//Overlap을 했는데 장착하지 않았다면 Nullptr로 바꿔서 ActiveOverlappingItem을 비워줌.
			Main->SetActiveOverlappingItem(nullptr);
		}
	}

}

void AWeapon::Equip(AMain* Char) //Main Header파일을 include시켜주자.
{
	if (Char)
	{
		//Instigator 파라미터를 설정함.
		SetInstigator(Char->GetController());

		/** SkeletalMesh의 Collision response가 ignore되어있는지 확인해야한다.
			또, Camera의 Collision을 ignore하도록 설정할거다.
			(Weapon이 Character와 Camera사이에 있으면 Camera가 Character를 확대하지 않도록 하기 위함이다.)
		*/
		//이렇게 하면 Camera가 확대되지 않는다. 무시하게됨.
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		//또, Pawn이 무시될 수 있도록 한다.
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		//이렇게 하면 Weapon에 대한 Camera와 pawn의 Camera를 무시하게 된다.

		/** 그리고 Simulating physics를 하고있다면, Player와 Attach를 해야 하기 때문에 Simulating physics를 멈춰야한다.
		*/
		SkeletalMesh->SetSimulatePhysics(false);
		//이렇게 해서, Collision과 Physics문제를 해결했다. 

		//본격적으로 Attach를 시작하자. SkeletalMeshSocket.h를 include해줘야함.
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		/** 만약 위의 결과가 성공이라면 이 RightHandSocket변수는 Character skeleton에 있는 RightHandSocket에 대한 참조가 될거다.
			그래서 이 결과가 유효한지 한번더 확인 해야한다.
		*/

		
		if(RightHandSocket)
		{
			//rightHandSocket이 NULL이 아니면, attach시킨다.
			//이렇게하면 Weapon을 Skeleton에 붙이게 된다.
			RightHandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false;
			

			/** 이렇게 해도 되지만 강의에서는 Main.h의 SetEquippedWeapon함수를 forceinline으로 하지 않고 따로 빼서
				아래를 구현했다.
			//EquippedWeapon을 가져오고
			const AWeapon* EquippedWeapon = Char->GetEquippedWeapon();
			if (EquippedWeapon) //있으면
			{
				Char->GetEquippedWeapon()->Destroy();//Destory시킴.
			}
			*/
			//이 특정Weapon instance를 set해줬다.
			Char->SetEquippedWeapon(this);
			Char->SetActiveOverlappingItem(nullptr);
		}
		if (OnEquipSound)
		{
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
		if (!bWeaponParticle)
		{
			//ParticleSystemComponent.h파일 include해줘야함.
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
				//Weapon으로부터 Enemy에게 Damage를 입힐 수 있다.
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