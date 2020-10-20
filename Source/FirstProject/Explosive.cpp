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
	//Damage���� ����
	Damage = 15.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Item (�θ�)Ŭ������ ����� ȣ���ϱ� ����
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//Overlap �� Actor�� �ٸ� Actor���� Ȯ�� �������.
	// ���� �ִ� �Ķ���͵��� Overlap�� Component������ component �Ǵ� class�� �����ֱ� ���� ������, ���� �ϳ��� Overlap�� Actor�̴�.

	if (OtherActor)
	{
		/** �� Actor�� ���״�� Actor class�� �� �� �ְ�, Character�� Pawn�� �� �� �ִ�. ������(Polymorphism)���� ���� Actor�� ������.
			Character�� �Լ��� �����ϰų�, �� Actor�� AMain Class�� ���� Character���� Ȯ���Ϸ��� Cast�� �� �� �ִ�.
			�켱 Main.h�� include���ش�.

			MainCharacter�� Cast�ϱ� ���� Cast��� Template �Լ��� �����.
			Cast<to>(from)
		*/
		AMain* Main = Cast<AMain>(OtherActor); //OtherActor type�� AMain type���� cast�ϰ� Main�� ������. �� cast����� OtherActor != AMain�̸�, NULL�̵�.
		AEnemy * Enemy = Cast<AEnemy>(OtherActor); //Enemy���� Cast��Ŵ.
		if (Main || Enemy)//AMainType�̸�
		{
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			//Main�� �����س��� �Լ��� Damage���� �ѱ�
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