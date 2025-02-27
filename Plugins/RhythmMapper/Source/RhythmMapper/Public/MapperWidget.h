#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "EditorUtilityWidgetComponents.h"
#include "Components/SinglePropertyView.h"
#include "Components/AssetThumbnailWidget.h"
#include "Components/AudioComponent.h"
#include "Components/TextBlock.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "MapperWidget.generated.h"

UCLASS(BlueprintType)
class RHYTHMMAPPER_API UMapperWidget : public UEditorUtilityWidget {
	GENERATED_BODY()

public:
	UMapperWidget();
	virtual ~UMapperWidget() override;
	
	void CreateSelectorView(UPanelWidget* RootPanel);
	void CreateThumbnailWidget(UPanelWidget* RootPanel);
	void CreatePlayText();
	void CreatePlayButton(UPanelWidget* RootPanel);

	UFUNCTION()
	void OnPlayPauseButtonClick();
	void OnPlayAudio();
	void OnStopAudio();
	void OnPauseAudio();

	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void NativeOnInitialized() override;
	
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(SkipSerialization=false), Category="Buttons")
	UEditorUtilityButton* PlayPauseButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(SkipSerialization=false), Category="Buttons")
	UEditorUtilityButton* PauseButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(SkipSerialization=false), Category="Properties")
	USinglePropertyView* SoundSelectorView;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Sound")
	USoundWave* SoundToUse;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(SkipSerialization=false), Category="Properties")
	UAssetThumbnailWidget* SoundThumbnailWidget;

	UPROPERTY()
	UAudioComponent* AudioPlayer;

private:
	int ConstructCount;

	UPROPERTY()
	UWorld* EditorWorld;

	UPROPERTY()
	UTextBlock* PlayText;
};
