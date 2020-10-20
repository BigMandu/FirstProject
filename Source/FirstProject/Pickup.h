// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API APickup : public AItem
{
	GENERATED_BODY()
public:
	APickup();

	//coin������ ������Ű�� ���� ��������, Coin�� Count�Ǵ� ������ �����ϱ� ����.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	int32 CoinCount;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	//Pickup�� ��ĥ�� ���� ���ƴ��� �˱� ���� parameter�� �Ѱ�����Ѵ�.
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickupBP(class AMain* Target);
};
