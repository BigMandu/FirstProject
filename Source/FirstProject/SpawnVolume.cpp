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

// spawn volume에서 random한 point를 리턴.
FVector ASpawnVolume::GetSpawnPoint()
{
	//Spawningbox의 크기.
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	//Spawningbox의 원점 위치.
	FVector Origin = SpawningBox->GetComponentLocation();

	/*	box내의 random한 point를 얻는 함수를 사용하기 위해 UKismet Math Library를 사용함.
		얘를 사용하려면 헤더파일을 include해줘야하는데 검색하고 unreal documentation가면 나옴.
	*/
	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return Point;
}    

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor() //무작위로 어떤 Actor를 Spawn할건지 선택함.
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

/** 여기가 중요한데, Blueprint native event를 만들때, C++에서의 구현은 기본적으로  함수이름_Implementation 이렇게 사용해야한다.
	이렇게 함수이름을 써야, Unreal Engine에서 C++로 스크립팅한 구현임을 알기 때문이다. error가 떠도 상관없다.(compiler가 모르는거임)
*/
void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		//World.h 파일 include
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World)
		{
			/** 마지막 파라미터는 FActorSpawnparameter가 필요하다. 위에서 만들어주자.
				이렇게 해서 함수를 호출할 수 있다. SpawnActor는 생성한 Actor를 return해준다.
				그래서 ACritter* CritterSpawned = 을 붙여준다. 
				이렇게 하게 되면, 나중에 이를 다룰 수 있다. 그리고 Critter 헤더파일을 추가하고, 여기서 말고 UE에서 컴파일하자.
				그리고 다시 여기로 와서 컴파일하면 성공이 뜬다.
			*/
			//ACritter* CritterSpawned = World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.f), SpawnParams);
			AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);

			//이 Actor가 Enemy일 경우 Enemy의 AIController를 가져오기 위함.
			AEnemy * Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				//이 함수는 이 Enemy의 Controller를 생성하고, pawn에 대해 설정한다.
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