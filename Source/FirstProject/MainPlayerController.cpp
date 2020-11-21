// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"


#include "SavingSystem.h"
#include "Blueprint/UserWidget.h"
#define OUT

AMainPlayerController::AMainPlayerController()
{
    bEnemyHealthBarVisible = false;
    bPauseMenuVisible = false;
    bGameOverVisible = false;
}

void AMainPlayerController::ShowEnemyHealthBar()
{
    if (EnemyHealthBar)
    {
        bEnemyHealthBarVisible = true;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
    }
}

void AMainPlayerController::HideEnemyHealthBar()
{
    if (EnemyHealthBar)
    {
        bEnemyHealthBarVisible = false;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AMainPlayerController::ShowPauseMenu_Implementation()
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

void AMainPlayerController::HidePauseMenu_Implementation()
{
    if (PauseMenu)
    {
        bPauseMenuVisible = false;
        PauseMenu->SetVisibility(ESlateVisibility::Hidden);

        FInputModeGameOnly InputModeGameOnly;
        SetInputMode(InputModeGameOnly);
        bShowMouseCursor = false;

        if (bGameOverVisible)
        {
            HideGameOver();
        }
    }
}

void AMainPlayerController::TogglePauseMenu()
{
    if (bPauseMenuVisible)
    {
        HidePauseMenu();
    }
    else
    {
        ShowPauseMenu();
    }
}

void AMainPlayerController::ShowGameOver(bool bPlayerWon)
{
    if (bPlayerWon && GameOverWin)
    {
        bGameOverVisible = true;
        GameOverWin->SetVisibility(ESlateVisibility::Visible);
    }
    else if (!bPlayerWon && GameOverLose)
    {
        bGameOverVisible = true;
        GameOverLose->SetVisibility(ESlateVisibility::Visible);
    }
    ShowPauseMenu();
}

void AMainPlayerController::HideGameOver()
{
    if (bGameOverVisible)
    {
        GameOverWin->SetVisibility(ESlateVisibility::Hidden);
        GameOverLose->SetVisibility(ESlateVisibility::Hidden);
        bGameOverVisible = false;
    }
}

void AMainPlayerController::SaveGame()
{
    USavingSystem::SaveGame(GetWorld(), TEXT("Default"));
}

void AMainPlayerController::LoadGame()
{
    USavingSystem::LoadGame(GetWorld(), TEXT("Default"), true);
    if (PauseMenu && bPauseMenuVisible)
    {
        HidePauseMenu();
    }
}

void AMainPlayerController::LoadGameDataOnly()
{
    USavingSystem::LoadGame(GetWorld(), TEXT("Default"), false);
}

void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (USavingSystem::SaveGameExists(TEXT("Default")))
    {
        LoadGameDataOnly();
    }
    
    FInputModeGameOnly InputModeGameOnly;
    SetInputMode(InputModeGameOnly);
    bShowMouseCursor = false;

    if(HUDOverlayAsset)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
        if (HUDOverlay)
        {
            HUDOverlay->AddToViewport();
            HUDOverlay->SetVisibility(ESlateVisibility::Visible); 
        }
    }

    if(EnemyHealthBarAsset)
    {
        EnemyHealthBar = CreateWidget<UUserWidget>(this, EnemyHealthBarAsset);
        if (EnemyHealthBar)
        {
            bEnemyHealthBarVisible = false;
            EnemyHealthBar->AddToViewport();
            EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
        
            FVector2D AlignmentVector{0.0f};
            EnemyHealthBar->SetAlignmentInViewport(AlignmentVector);
        }
    }
    if (GameOverLoseAsset)
    {
        GameOverLose = CreateWidget<UUserWidget>(this, GameOverLoseAsset);
        if (GameOverLose)
        {
            GameOverLose->AddToViewport();
            GameOverLose->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    if (GameOverWinAsset)
    {
        GameOverWin = CreateWidget<UUserWidget>(this, GameOverWinAsset);
        if (GameOverWin)
        {
            GameOverWin->AddToViewport();
            GameOverWin->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    if (PauseMenuAsset)
    {
        PauseMenu = CreateWidget<UUserWidget>(this, PauseMenuAsset);
        if (PauseMenu)
        {
            PauseMenu->AddToViewport();
            PauseMenu->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void AMainPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (EnemyHealthBar)
    {
        FVector2D ViewportPosition;
        ProjectWorldLocationToScreen(EnemyLocation, OUT ViewportPosition);
        ViewportPosition.Y -= 200.0f;
        ViewportPosition.X -= 125.0f;

        FVector2D SizeInViewport{250.0f, 20.0f};
        EnemyHealthBar->SetPositionInViewport(ViewportPosition);
        EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
    }
    
}
