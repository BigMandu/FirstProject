// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	//Pawn�� Null���� üũ
	if (Pawn == nullptr)
	{
		//�� �ִϸ��̼��� instance �����ڸ� ã�� �־���.
		Pawn = TryGetPawnOwner();

		/** Sprint����*/
		if (Pawn)
		{
			//Pawn�� valid�� Main���� Cast�� ����
			Main = Cast<AMain>(Pawn);
		}
	}
} 


	//Tick �Լ�ó�� �� ������ �� ȣ���ϴ� �Լ�
	// �� �Լ��� �ӵ�, inAir�� ������ �������ٰŴ�.
void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}


	if (Pawn)
	{
		//ĳ������ �ӵ���, ���߿� �ִ��� ���θ� Ȯ���ؾ���.

		FVector Speed = Pawn->GetVelocity(); //�ӵ��� ������.
		//�� �ӵ��� ��������� �ƴ� �����̸�, �׷��ϱ� ���߿� ������(�߶�, ����, ���� ��) Jump Animation�� ���������.
		// �ٵ� ���� �ִٸ� �׳� ���� �ӵ����� �Ű澴��.
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);

		//�̵��ӵ��� ������Ʈ ���ٰǵ�, �켱 �⺻������ �� ������ ũ�⸦ ���ؾ��Ѵ�. 
		//float���� ��Į�� ������ �����ϸ� �⺻������ ������ ũ���̸�, FVector Ŭ�������� size��� �Լ��� �ִ�.
		MovementSpeed = LateralSpeed.Size(); //������ ����� float���� ���.

		//Pawn character movement component���� �� ������ ��´�.
		//Charactermovementcomponent.h �� include�������.
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		/** Sprint����*/
		//���⼭�� ����������, Main�� valid�� Pawn���� Main���� cast�� ����
		if (Main == nullptr)
		{
			Main = Cast<AMain>(Pawn);
		}
	}
}

