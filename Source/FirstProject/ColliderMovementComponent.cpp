// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	 
	FVector DesireMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f);

	if (!DesireMovementThisFrame.IsNearlyZero()) // true || false를 리턴한다. 0에 가까우면 true
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesireMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);
			//이 설명대로 하는 함수임. updated component를 하고, move함.

		//if we bump in to something, slide along the side of it.
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesireMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}
	//Nearly zero면 진행할 필요가 없음.
}