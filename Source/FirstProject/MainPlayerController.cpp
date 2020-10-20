// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	//오버라이딩 할때 맨처음 해야함.
	Super::BeginPlay();

	//우선 Blueprint에서 HUDOverlay Asset을 선택했는지 확인해야한다.
	if (HUDOverlayAsset)
	{
		//CreateWidget -> Template 생성자임
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}

	/** Blueprint에서 선택한 UUserWidget을 설정했다.
		이제 이걸 Viewport에 추가해야한다.	
	*/
	HUDOverlay->AddToViewport();

	/** UUserWidget에 있는 몇몇 함수를 써본다.*/

	//이 함수를 이용해 보이지 않게 설정할 수 있기때문에 존재한다.
	//특정 HUD를 안보이게 할 수 있기 때문에 유용하다.
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			//유효하다면 Viewport에 추가한다.
			EnemyHealthBar->AddToViewport();

			//바로 보여줄게 아니기 때문에 Hidden으로 감춘다.
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		//Widget 정렬
		FVector2D Align(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Align);
	}

	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
		//Setalignment in viewport는 필요없다 canvas panel을 사용하기 때문에.
	}
}


void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		//여기서 할것은 3d의 world space에서 2d screen으로의 location을 변환하는것이다.
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 125.f;

		FVector2D SizeInViewport(300.f, 25.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);

		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		/*FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);*/
		GameModeOnly();

		bShowMouseCursor = false;

		bPauseMenuVisible = false;
		//PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}

void AMainPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}