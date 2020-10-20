// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	//Editor���� UMG Asset�� ���� ���� , TsubclassOf -> PlayerController Blueprint���� �� �׸��� ������ �� �ְ� ����.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	//Widget�� �����ϰ� ����(����)�� ������.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	bool bPauseMenuVisible;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void DisplayPauseMenu();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void RemovePauseMenu();

	void TogglePauseMenu();

	bool bEnemyHealthBarVisible;

	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();

	FVector EnemyLocation;

	/** TSubClass��Ӵٿ����� ������ UClass�� ������, C++���� ������ ���� UUser Widget�� HUD Overlay�� �ִ�. */

	void GameModeOnly();

protected:
	//�ٸ� classó�� �����ذ���.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
