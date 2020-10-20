// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	// 플랫폼 Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	/** FVector UPROPERTY에서 사용가능한 특별한 키워드가 있는데 
		Editor에서 시각화 위젯을 조작할때 사용하는 키워드다.
		이전에도 했었는데 바로 meta키워드다, 이 중 meta specifies다.
	*/
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true")) // true가 string이라 좀 이상하지만 잘 작동한다.
	FVector EndPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpTime;

	//이 InterpTimer를 사용
	FTimerHandle InterpTimer;

	//보간여부를 on/off하기 위함.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	bool bInterping;

	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterping();

	void SwapVectors(FVector& VecOne, FVector& VecTwo);

};
