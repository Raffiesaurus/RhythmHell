// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BeatMapEditorHUD.generated.h"

/**
 *
 */
UCLASS()
class RHYTHMHELL_API ABeatMapEditorHUD : public AHUD
{
	GENERATED_BODY()

public:
	ABeatMapEditorHUD();

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	float TimelineZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	float TimelineScroll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int32 SelectedNoteIndex;

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void DrawTimeline();

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void DrawNotes();

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void DrawGrid();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void HandleTimelineLeftClick(FVector2D MousePosition);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void HandleTimelineRightClick(FVector2D MousePosition, float ElapsedSeconds);

	UFUNCTION(BlueprintCallable, Category = "Input")
	int32 GetNoteAtPosition(FVector2D Position);

	TObjectPtr<UCanvas> PlaybackCanvas;

private:
	float TimeToScreenX(float Time);
	float ScreenXToTime(float X);
	int32 ScreenYToLane(float Y);
};
