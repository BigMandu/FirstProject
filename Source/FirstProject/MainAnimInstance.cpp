// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	//Pawn이 Null인지 체크
	if (Pawn == nullptr)
	{
		//이 애니메이션의 instance 소유자를 찾아 넣어줌.
		Pawn = TryGetPawnOwner();

		/** Sprint구현*/
		if (Pawn)
		{
			//Pawn이 valid면 Main으로 Cast를 해줌
			Main = Cast<AMain>(Pawn);
		}
	}
} 


	//Tick 함수처럼 매 프레임 을 호출하는 함수
	// 이 함수로 속도, inAir등 변수를 조절해줄거다.
void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}


	if (Pawn)
	{
		//캐릭터의 속도와, 공중에 있는지 여부를 확인해야함.

		FVector Speed = Pawn->GetVelocity(); //속도를 가져옴.
		//이 속도가 수평방향이 아닌 방향이면, 그러니깐 공중에 있으면(추락, 비행, 점프 등) Jump Animation을 보여줘야함.
		// 근데 땅에 있다면 그냥 수평 속도에만 신경쓴다.
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);

		//이동속도를 업데이트 해줄건데, 우선 기본적으로 이 벡터의 크기를 구해야한다. 
		//float값을 스칼라 값으로 변경하면 기본적으로 벡터의 크기이며, FVector 클래스에는 size라는 함수가 있다.
		MovementSpeed = LateralSpeed.Size(); //벡터의 사이즈를 float으로 줬다.

		//Pawn character movement component에서 이 정보를 얻는다.
		//Charactermovementcomponent.h 를 include해줘야함.
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		/** Sprint구현*/
		//여기서도 마찬가지로, Main이 valid면 Pawn에서 Main으로 cast를 해줌
		if (Main == nullptr)
		{
			Main = Cast<AMain>(Pawn);
		}
	}
}

