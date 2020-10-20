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

	/** Tsubclassof라는 템플릿을 쓴다. 이것은 unreal engine에서 사용할 수 있는 매우 특별한 것인데,
		기본적으로 폰을 생성하는 변수를 만들고, Blueprint의 Tsubclass 드랍다운에서 이걸 설정할 수 있다.
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
	
	//FVector를 리턴하는 함수임
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetSpawnPoint();

	UFUNCTION(BlueprintPure, Category = "Spawning")
	TSubclassOf<AActor> GetSpawnActor();
	
	/** Pawn을 파라미터로 넘겨줄껀데, 상위계층인 UClass type으로 한다. Pawn, Actor, Character는 모두 UClass에서 파생됨.
		두번째 파라미터는 spawn할 위치를 넘겨줄거다.
		Vector값을 reference로 넘겨줄거다. 값을 바꾸지 않을거기 때문에 const를 붙였다.

		이제 UFUNCTION매크로를 사용하는데, 이 함수를 C++에서 호출하며, 실행가능함과 동시에 Blueprint에서도 실행할 수 있게끔 할꺼다.
		즉, 이 함수를 하이브리드로 해줄건데 이렇게 되면 C++, Blueprint 양쪽에서의 기능을 수행할 수 있따.
		이러한 함수들을 보통 Blueprint native events라고 한다.

		이제 아래 함수 선언부를 복사해 cpp에 붙여넣자.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void SpawnOurActor(UClass* ToSpawn, const FVector& Location);
};

