// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	//�������̵� �Ҷ� ��ó�� �ؾ���.
	Super::BeginPlay();

	//�켱 Blueprint���� HUDOverlay Asset�� �����ߴ��� Ȯ���ؾ��Ѵ�.
	if (HUDOverlayAsset)
	{
		//CreateWidget -> Template ��������
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}

	/** Blueprint���� ������ UUserWidget�� �����ߴ�.
		���� �̰� Viewport�� �߰��ؾ��Ѵ�.	
	*/
	HUDOverlay->AddToViewport();

	/** UUserWidget�� �ִ� ��� �Լ��� �ẻ��.*/

	//�� �Լ��� �̿��� ������ �ʰ� ������ �� �ֱ⶧���� �����Ѵ�.
	//Ư�� HUD�� �Ⱥ��̰� �� �� �ֱ� ������ �����ϴ�.
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			//��ȿ�ϴٸ� Viewport�� �߰��Ѵ�.
			EnemyHealthBar->AddToViewport();

			//�ٷ� �����ٰ� �ƴϱ� ������ Hidden���� �����.
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		//Widget ����
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
		//Setalignment in viewport�� �ʿ���� canvas panel�� ����ϱ� ������.
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
		//���⼭ �Ұ��� 3d�� world space���� 2d screen������ location�� ��ȯ�ϴ°��̴�.
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