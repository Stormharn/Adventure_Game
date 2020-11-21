// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Animation/WidgetAnimation.h"
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
    AMainPlayerController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    TSubclassOf<UUserWidget> HUDOverlayAsset;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    UUserWidget* HUDOverlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    TSubclassOf<UUserWidget> EnemyHealthBarAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    UUserWidget* EnemyHealthBar;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    TSubclassOf<UUserWidget> PauseMenuAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    UUserWidget* PauseMenu;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    TSubclassOf<UUserWidget> GameOverLoseAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    UUserWidget* GameOverLose;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    TSubclassOf<UUserWidget> GameOverWinAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI | Widgets")
    UUserWidget* GameOverWin;

    bool bEnemyHealthBarVisible;
    bool bPauseMenuVisible;
    bool bGameOverVisible;

    void ShowEnemyHealthBar();
    void HideEnemyHealthBar();
    void TogglePauseMenu();
    void ShowGameOver(bool bPlayerWon);
    void HideGameOver();

    UFUNCTION(BlueprintCallable, Category="Save System")
    void SaveGame();
    UFUNCTION(BlueprintCallable, Category="Save System")
    void LoadGame();
    void LoadGameDataOnly();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="UI")
    void ShowPauseMenu();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="UI")
    void HidePauseMenu();

    FVector EnemyLocation;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
};
