// Raffiesaurus, 2025

#include "RhythmGameplayLevel.h"

#include "RhythmGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ARhythmGameplayLevel::ARhythmGameplayLevel(): GameplayController(nullptr), GameplayWidget(nullptr) {
	PrimaryActorTick.bCanEverTick = true;

	bIsLevelInitialized = false;
	bIsGameplayActive = false;
	bIsGameplayPaused = false;
}

void ARhythmGameplayLevel::BeginPlay() {
	Super::BeginPlay();

	InitializeLevel();
}

void ARhythmGameplayLevel::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ARhythmGameplayLevel::StartGameplay() {
	if (!bIsLevelInitialized || !GameplayController || !GameplayWidget)
		return;

	bIsGameplayActive = true;
	bIsGameplayPaused = false;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
	}

	GameplayWidget->InitializeGameplayUI(GameplayController->CurrentLevel);

	if (const ARhythmHellCharacter* PlayerCharacter = Cast<ARhythmHellCharacter>(UGameplayStatics::GetPlayerPawn(this, 0))) {
		PlayerCharacter->EnableRhythmMode(GameplayController, true);
	}

	GameplayController->StartLevel();

	UE_LOG(LogTemp, Log, TEXT("Started rhythm gameplay"));
}

void ARhythmGameplayLevel::PauseGameplay() {
	if (!bIsGameplayActive || bIsGameplayPaused || !GameplayController)
		return;

	GameplayController->PauseLevel();
	bIsGameplayPaused = true;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	UE_LOG(LogTemp, Log, TEXT("Paused rhythm gameplay"));
}

void ARhythmGameplayLevel::ResumeGameplay() {
	if (!bIsGameplayActive || !bIsGameplayPaused || !GameplayController)
		return;

	GameplayController->ResumeLevel();
	bIsGameplayPaused = false;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	UE_LOG(LogTemp, Log, TEXT("Resumed rhythm gameplay"));
}

void ARhythmGameplayLevel::EndGameplay() {
	if (!bIsGameplayActive || !GameplayController)
		return;

	bIsGameplayActive = false;
	bIsGameplayPaused = false;

	GameplayController->EndLevel();

	if (const ARhythmHellCharacter* PlayerCharacter = Cast<ARhythmHellCharacter>(UGameplayStatics::GetPlayerPawn(this, 0))) {
		PlayerCharacter->EnableRhythmMode(GameplayController, false);
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		const FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	UE_LOG(LogTemp, Log, TEXT("Ended rhythm gameplay"));
}

bool ARhythmGameplayLevel::InitializeLevel() {
	if (bIsLevelInitialized)
		return true;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GameplayController = GetWorld()->SpawnActor<ARhythmGameplayController>(
		ARhythmGameplayController::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (!GameplayController) {
		UE_LOG(LogTemp, Error, TEXT("Failed to create RhythmGameplayController"));
		return false;
	}

	GameplayController->OnLevelComplete.AddDynamic(this, &ARhythmGameplayLevel::OnLevelComplete);

	if (URhythmGameInstance* GameInst = Cast<URhythmGameInstance>(GetGameInstance())) {
		LevelJSONPath = GameInst->GetVinylJSON();
	}

	if (!LevelJSONPath.IsEmpty()) {
		if (!GameplayController->LoadLevelFromJSON(LevelJSONPath)) {
			UE_LOG(LogTemp, Error, TEXT("Failed to load rhythm level from JSON: %s"), *LevelJSONPath);
			return false;
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("No level JSON path specified"));
	}

	if (GameplayWidgetClass) {
		GameplayWidget = CreateWidget<URhythmGameplayWidget>(GetWorld(), GameplayWidgetClass);

		if (GameplayWidget) {
			GameplayWidget->SetGameplayController(GameplayController);
			GameplayWidget->AddToViewport();
			GameplayWidget->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Warning, TEXT("Created gameplay widget"));
		} else {
			UE_LOG(LogTemp, Error, TEXT("Failed to create gameplay widget"));
			return false;
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("No gameplay widget class specified"));
	}

	SetupPlayerCharacter();

	bIsLevelInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("Rhythm gameplay level initialized successfully"));

	StartGameplay();

	return true;
}

void ARhythmGameplayLevel::SetupPlayerCharacter() {
	const ARhythmHellCharacter* PlayerCharacter = Cast<ARhythmHellCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!PlayerCharacter) {
		UE_LOG(LogTemp, Error, TEXT("Failed to find player character"));
		return;
	}

	PlayerCharacter->EnableRhythmMode(GameplayController, false);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		// Add any specific input configuration here
	}

	// Lock movement during gameplay if needed
	// PlayerCharacter->GetCharacterMovement()->DisableMovement();

	UE_LOG(LogTemp, Log, TEXT("Player character setup for rhythm gameplay"));
}

void ARhythmGameplayLevel::OnLevelComplete(int32 TotalScore, int32 MaxCombo, const FString& Rank) {
	bIsGameplayActive = false;

	SaveLevelResults(TotalScore, MaxCombo, Rank);

	// Show level complete UI 
	if (GameplayWidget) {
		// The widget itself will handle the UI update through its own OnLevelComplete callback
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]() {
		HandleLevelCompletion();
	}, 5.0f, false); 

	UE_LOG(LogTemp, Log, TEXT("Level complete - Score: %d, Max Combo: %d, Rank: %s"),
	       TotalScore, MaxCombo, *Rank);
}

void ARhythmGameplayLevel::HandleLevelCompletion() {
	if (const ARhythmHellCharacter* PlayerCharacter = Cast<ARhythmHellCharacter>(UGameplayStatics::GetPlayerPawn(this, 0))) {
		PlayerCharacter->EnableRhythmMode(GameplayController, false);
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		const FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	const FString LevelToLoad = "DefaultMap";
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelToLoad));
}

void ARhythmGameplayLevel::SaveLevelResults(int32 TotalScore, int32 MaxCombo, const FString& Rank) {
	if (const URhythmGameInstance* GameInst = Cast<URhythmGameInstance>(GetGameInstance()); !GameInst) {
		UE_LOG(LogTemp, Error, TEXT("Failed to get game instance"));
		return;
	}

	// Here you would save the player's results
	// For example:
	// GameInst->SaveLevelScore(CurrentLevel.LevelName, TotalScore, MaxCombo, Rank);

	// Unlock achievements based on score/rank
	if (Rank == "S") {
		// Unlock "Perfect Performance" achievement
		UE_LOG(LogTemp, Log, TEXT("Unlocked 'Perfect Performance' achievement"));
	} else if (MaxCombo >= 50) {
		// Unlock "Combo Master" achievement
		UE_LOG(LogTemp, Log, TEXT("Unlocked 'Combo Master' achievement"));
	}

	// Reward the player with in-game currency based on performance
	int32 CoinsEarned = 0;
	if (Rank == "S")
		CoinsEarned = 1000;
	else if (Rank == "A")
		CoinsEarned = 750;
	else if (Rank == "B")
		CoinsEarned = 500;
	else if (Rank == "C")
		CoinsEarned = 300;
	else if (Rank == "D")
		CoinsEarned = 150;
	else
		CoinsEarned = 50; // Rank F

	// Add coins to player's total
	// GameInst->AddCoins(CoinsEarned);

	UE_LOG(LogTemp, Log, TEXT("Earned %d coins for performance"), CoinsEarned);
}
