// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstSaveGame.h"


#include "ItemStorage.h"
#include "Kismet/GameplayStatics.h"

UFirstSaveGame::UFirstSaveGame()
{
    PlayerName = TEXT("Default");
    UserIndex  = 0;
}
