// Fill out your copyright notice in the Description page of Project Settings.

#include "ToonTanksGameMode.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void AToonTanksGameMode::ActorDied(AActor *DeadActor)
{
    if (DeadActor == Tank)
    {
        Tank->HandleDestruction();
        if (ToonTanksPlayerController)
        {
            ToonTanksPlayerController->SetPlayerEnabledState(false);
        }
        GameOver(false);
    }
    else if (ATower *DestroyedTower = Cast<ATower>(DeadActor))
    {
        DestroyedTower->HandleDestruction();
        TargetTowers--;
        if (TargetTowers == 0)
        {
            GameOver(true);
        }
    }
}

int32 AToonTanksGameMode::GetTargetTowerCount()
{
    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), OutActors);
    return OutActors.Num();
}

void AToonTanksGameMode::BeginPlay()
{
    Super::BeginPlay();
    HandleGameStart();
}

void AToonTanksGameMode::HandleGameStart()
{
    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
    ToonTanksPlayerController = Cast<AToonTanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    
    StartGame();
    TargetTowers = GetTargetTowerCount();
    UE_LOG(LogTemp, Warning, TEXT("Started game"))
    if (ToonTanksPlayerController)
    {
        ToonTanksPlayerController->SetPlayerEnabledState(false);
    }
    FTimerHandle PlayerEnabledTimerHandle;
    FTimerDelegate PlayerEnabledTimerDelegate = FTimerDelegate::CreateUObject(ToonTanksPlayerController, &AToonTanksPlayerController::SetPlayerEnabledState, true);
    GetWorldTimerManager().SetTimer(PlayerEnabledTimerHandle, PlayerEnabledTimerDelegate, StartDelay, false /*no loop*/);
}