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
	 
	//������ Ÿ�̸Ӹ�ŭ (1��) ����ߴٰ�, ToggleInterping�� ȣ���ϰ� �ɰŴ�.
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

	//�ش� Vector�� ũ�⸦ float���� ����, �̵��� ������ ũ�⸦ ����.
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

		/** �������� �����ϸ�, Start�� End Point�� swap		*/
		
		//�󸶳� �Դ��� ���.
		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		
		//�ž���� �ٿ�����
		if (Distance - DistanceTraveled <= 1.f)
		{
			ToggleInterping(); // ����ؼ� false,
			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime); // ToggleInterpingȣ��
			SwapVectors(StartPoint, EndPoint); //Start, End�� Swap
		} 
	}

}

void AFloatingPlatform::ToggleInterping()
{
	//������.
	bInterping = !bInterping;

}

void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo)
{
	FVector Temp = VecOne;
	VecOne = VecTwo;
	VecTwo = Temp;
}