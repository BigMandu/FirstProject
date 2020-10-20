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

	// �÷��� Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	/** FVector UPROPERTY���� ��밡���� Ư���� Ű���尡 �ִµ� 
		Editor���� �ð�ȭ ������ �����Ҷ� ����ϴ� Ű�����.
		�������� �߾��µ� �ٷ� metaŰ�����, �� �� meta specifies��.
	*/
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true")) // true�� string�̶� �� �̻������� �� �۵��Ѵ�.
	FVector EndPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpTime;

	//�� InterpTimer�� ���
	FTimerHandle InterpTimer;

	//�������θ� on/off�ϱ� ����.
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
