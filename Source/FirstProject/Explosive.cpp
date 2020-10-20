// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AExplosive::AExplosive()
{
	//Damage값을 세팅
	Damage = 15.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Item (부모)클래스의 멤버를 호출하기 위함
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//Overlap 된 Actor가 다른 Actor인지 확인 해줘야함.
	// 위에 있는 파라미터들은 Overlap된 Component정보를 component 또는 class로 보내주기 위한 것으로, 이중 하나는 Overlap된 Actor이다.

	if (OtherActor)
	{
		/** 이 Actor는 말그대로 Actor class가 될 수 있고, Character나 Pawn이 될 수 있다. 다형성(Polymorphism)으로 인해 Actor로 간주함.
			Character의 함수에 접근하거나, 이 Actor가 AMain Class와 같은 Character인지 확인하려면 Cast를 할 수 있다.
			우선 Main.h를 include해준다.

			MainCharacter로 Cast하기 위해 Cast라는 Template 함수를 사용함.
			Cast<to>(from)
		*/
		AMain* Main = Cast<AMain>(OtherActor); //OtherActor type을 AMain type으로 cast하고 Main에 저장함. 이 cast결과가 OtherActor != AMain이면, NULL이됨.
		AEnemy * Enemy = Cast<AEnemy>(OtherActor); //Enemy까지 Cast시킴.
		if (Main || Enemy)//AMainType이면
		{
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			//Main에 정의해놓은 함수에 Damage값을 넘김
			//Main->DecrementHealth(Damage);
			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
			Destroy();
		}
		
		
	}
}


void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}