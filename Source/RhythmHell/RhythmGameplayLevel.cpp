// Raffiesaurus, 2025

#include "RhythmGameplayLevel.h"

#include "RhythmGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

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

void ARhythmGameplayLevel::StartGameplay() {}
void ARhythmGameplayLevel::PauseGameplay() {}
void ARhythmGameplayLevel::ResumeGameplay() {}
void ARhythmGameplayLevel::EndGameplay() {}

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

	URhythmGameInstance* GameInst = Cast<URhythmGameInstance>(GetGameInstance());
	LevelJSONPath = GameInst->GetVinylJSON();
	
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
			GameplayWidget->SetVisibility(ESlateVisibility::Hidden);
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

	return true;
}

void ARhythmGameplayLevel::SetupPlayerCharacter() {}
void ARhythmGameplayLevel::OnLevelComplete(int32 TotalScore, int32 MaxCombo, const FString& Rank) {}
