// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstSaveGame.h"

UFirstSaveGame::UFirstSaveGame()
{
	PlayerName = TEXT("Default");
	UserIndex = 0;

	//무기가 없을때는 이 Defaults값으롷 설정되서, Main의 EquippedWeapon이 없으면 알기 위함.
	CharacterStats.WeaponName = TEXT("");
	CharacterStats.LevelName = TEXT("");
}