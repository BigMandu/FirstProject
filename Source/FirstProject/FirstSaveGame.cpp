// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstSaveGame.h"

UFirstSaveGame::UFirstSaveGame()
{
	PlayerName = TEXT("Default");
	UserIndex = 0;

	//���Ⱑ �������� �� Defaults������ �����Ǽ�, Main�� EquippedWeapon�� ������ �˱� ����.
	CharacterStats.WeaponName = TEXT("");
	CharacterStats.LevelName = TEXT("");
}