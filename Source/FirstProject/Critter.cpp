// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"
#include "Components/SkeletalMeshComponent.h" //헤더파일 추가해줌.
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//정적메쉬 컴포넌트를 생성함.
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	//헤더파일을 추가해줘야함. MeshComponent UE docs에서 검색하면 나온다.
	//https://docs.unrealengine.com/en-US/API/Runtime/Engine/Components/UStaticMeshComponent/index.html
	MeshComponent->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	//카메라를 root component에 포함시켜야함.(attach시켜야함)
	//위와 동일하게 <>안에 CameraComponent.h 파일을 찾아줘야한다. UE docs에서 검색하거나, 솔루션 탐색기를 이용하자.
	Camera->SetupAttachment(GetRootComponent());

	//카메라 컴포넌트의 몇가지 기능을 추가했다.
	Camera->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	Camera->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	CurrentVelocity = FVector(0.f);
	MaxSpeed = 100.f;

}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//CurrentVelocity는 속도 단위가 시간당 거리의 값이기 때문에 DeltaTime과 곱을 한다.
	//이렇게 하면 지금과 마지막 frame에 있는 DeltaTime은 currentVelocity에 곱해진다.
	//currentvelocity는 MaxSpeed의 scale에 있을거다. 그래서 최대값은 100이 될거임.
	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	SetActorLocation(NewLocation);

}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);
	
}

void ACritter::MoveForward(float value)
{
	//전달된 value값을 사용한다.
	CurrentVelocity.X = FMath::Clamp(value, -1.f, 1.f) * MaxSpeed;

}

void ACritter::MoveRight(float value)
{
	CurrentVelocity.Y = FMath::Clamp(value, -1.f, 1.f) * MaxSpeed;
}

