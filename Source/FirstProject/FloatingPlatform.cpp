// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	StartPoint = FVector(0.0f);
	EndPoint = FVector(0.0f);
	
	bInterping = false;
	InterpSpeed = 4.0f;
	InterpTime = 1.0f;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;
	
	bInterping = false;
	 
	//설정한 타이머만큼 (1초) 대기했다가, ToggleInterping을 호출하게 될거다.
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

	//해당 Vector의 크기를 float으로 리턴, 이동한 벡터의 크기를 구함.
	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		/** 목적지에 도달하면, Start와 End Point를 swap		*/
		
		//얼마나 왔는지 계산.
		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		
		//거어어의 다왔을때
		if (Distance - DistanceTraveled <= 1.f)
		{
			ToggleInterping(); // 토글해서 false,
			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime); // ToggleInterping호출
			SwapVectors(StartPoint, EndPoint); //Start, End를 Swap
		} 
	}

}

void AFloatingPlatform::ToggleInterping()
{
	//뒤집음.
	bInterping = !bInterping;

}

void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo)
{
	FVector Temp = VecOne;
	VecOne = VecTwo;
	VecTwo = Temp;
}