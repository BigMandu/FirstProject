// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Critter.h"
#include "Enemy.h"
#include "AIController.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	if (Actor_1 && Actor_2 && Actor_3 && Actor_4)
	{
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
	}
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// spawn volume���� random�� point�� ����.
FVector ASpawnVolume::GetSpawnPoint()
{
	//Spawningbox�� ũ��.
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	//Spawningbox�� ���� ��ġ.
	FVector Origin = SpawningBox->GetComponentLocation();

	/*	box���� random�� point�� ��� �Լ��� ����ϱ� ���� UKismet Math Library�� �����.
		�긦 ����Ϸ��� ��������� include������ϴµ� �˻��ϰ� unreal documentation���� ����.
	*/
	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return Point;
}    

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor() //�������� � Actor�� Spawn�Ұ��� ������.
{
	if (SpawnArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1); //0 to 3
		return SpawnArray[Selection];
	}
	else
	{
		return nullptr;
	}
}

/** ���Ⱑ �߿��ѵ�, Blueprint native event�� ���鶧, C++������ ������ �⺻������  �Լ��̸�_Implementation �̷��� ����ؾ��Ѵ�.
	�̷��� �Լ��̸��� ���, Unreal Engine���� C++�� ��ũ������ �������� �˱� �����̴�. error�� ���� �������.(compiler�� �𸣴°���)
*/
void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		//World.h ���� include
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World)
		{
			/** ������ �Ķ���ʹ� FActorSpawnparameter�� �ʿ��ϴ�. ������ ���������.
				�̷��� �ؼ� �Լ��� ȣ���� �� �ִ�. SpawnActor�� ������ Actor�� return���ش�.
				�׷��� ACritter* CritterSpawned = �� �ٿ��ش�. 
				�̷��� �ϰ� �Ǹ�, ���߿� �̸� �ٷ� �� �ִ�. �׸��� Critter ��������� �߰��ϰ�, ���⼭ ���� UE���� ����������.
				�׸��� �ٽ� ����� �ͼ� �������ϸ� ������ ���.
			*/
			//ACritter* CritterSpawned = World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.f), SpawnParams);
			AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);

			//�� Actor�� Enemy�� ��� Enemy�� AIController�� �������� ����.
			AEnemy * Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				//�� �Լ��� �� Enemy�� Controller�� �����ϰ�, pawn�� ���� �����Ѵ�.
				Enemy->SpawnDefaultController(); //AIController for this and actually set it for pawn.
				

				AAIController* AIController = Cast<AAIController>(Enemy->GetController());

				if (AIController)
				{
					Enemy->AIController = AIController;
				}
			}

		}
	}
}