// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class FIRSTPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class UBoxComponent* SpawningBox;

	/** Tsubclassof��� ���ø��� ����. �̰��� unreal engine���� ����� �� �ִ� �ſ� Ư���� ���ε�,
		�⺻������ ���� �����ϴ� ������ �����, Blueprint�� Tsubclass ����ٿ�� �̰� ������ �� �ִ�.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_4;

	TArray< TSubclassOf<class AActor>> SpawnArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//FVector�� �����ϴ� �Լ���
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetSpawnPoint();

	UFUNCTION(BlueprintPure, Category = "Spawning")
	TSubclassOf<AActor> GetSpawnActor();
	
	/** Pawn�� �Ķ���ͷ� �Ѱ��ٲ���, ���������� UClass type���� �Ѵ�. Pawn, Actor, Character�� ��� UClass���� �Ļ���.
		�ι�° �Ķ���ʹ� spawn�� ��ġ�� �Ѱ��ٰŴ�.
		Vector���� reference�� �Ѱ��ٰŴ�. ���� �ٲ��� �����ű� ������ const�� �ٿ���.

		���� UFUNCTION��ũ�θ� ����ϴµ�, �� �Լ��� C++���� ȣ���ϸ�, ���డ���԰� ���ÿ� Blueprint������ ������ �� �ְԲ� �Ҳ���.
		��, �� �Լ��� ���̺긮��� ���ٰǵ� �̷��� �Ǹ� C++, Blueprint ���ʿ����� ����� ������ �� �ֵ�.
		�̷��� �Լ����� ���� Blueprint native events��� �Ѵ�.

		���� �Ʒ� �Լ� ����θ� ������ cpp�� �ٿ�����.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void SpawnOurActor(UClass* ToSpawn, const FVector& Location);
};

