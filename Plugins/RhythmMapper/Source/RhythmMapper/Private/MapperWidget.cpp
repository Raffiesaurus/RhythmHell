#include "MapperWidget.h"

#include "Kismet/GameplayStatics.h"

UMapperWidget::UMapperWidget(): PlayPauseButton(nullptr), PauseButton(nullptr), SoundSelectorView(nullptr),
                                SoundToUse(nullptr),
                                SoundThumbnailWidget(nullptr), AudioPlayer(nullptr), ConstructCount(0),
                                EditorWorld(nullptr), PlayText(nullptr) {}

UMapperWidget::~UMapperWidget() {}

void UMapperWidget::CreateSelectorView(UPanelWidget* RootPanel) {
	SoundSelectorView = WidgetTree->ConstructWidget<USinglePropertyView>(
		USinglePropertyView::StaticClass());

	RootPanel->AddChild(SoundSelectorView);
	UCanvasPanelSlot* SelectorViewSlot = Cast<UCanvasPanelSlot>(SoundSelectorView->Slot);
	SelectorViewSlot->SetSize(FVector2D(500, 100));
	SelectorViewSlot->SetPosition(FVector2D(0, 0));
	SelectorViewSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 0.0f));
	SelectorViewSlot->SetOffsets(FMargin(0, 0, 0, 70));
	SoundSelectorView->SetObject(this);
	SoundSelectorView->SetPropertyName(FName("SoundToUse"));
}

void UMapperWidget::CreateThumbnailWidget(UPanelWidget* RootPanel) {
	SoundThumbnailWidget = WidgetTree->ConstructWidget<UAssetThumbnailWidget>(
		UAssetThumbnailWidget::StaticClass());
	SoundThumbnailWidget->SetResolution(FIntPoint(650, 70));
	RootPanel->AddChild(SoundThumbnailWidget);
	UCanvasPanelSlot* ThumbnailSlot = Cast<UCanvasPanelSlot>(SoundThumbnailWidget->Slot);
	ThumbnailSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 0.0f));
	ThumbnailSlot->SetOffsets(FMargin(0, 100, 0, 70));

	if (SoundToUse) {
		SoundThumbnailWidget->SetAssetByObject(SoundToUse);
	}
}

void UMapperWidget::CreatePlayText() {
	PlayText = WidgetTree->ConstructWidget<UTextBlock>(
		UTextBlock::StaticClass());
	PlayPauseButton->AddChild(PlayText);
	FSlateFontInfo FontToUse = PlayText->GetFont();
	FontToUse.Size = 12;
	PlayText->SetText(FText::FromString("PLAY"));
	PlayText->SetFont(FontToUse);
	UButtonSlot* PlayTextSlot = Cast<UButtonSlot>(PlayText->Slot);
	PlayTextSlot->SetHorizontalAlignment(HAlign_Center);
	PlayTextSlot->SetVerticalAlignment(VAlign_Center);
}

void UMapperWidget::CreatePlayButton(UPanelWidget* RootPanel) {
	PlayPauseButton = WidgetTree->ConstructWidget<UEditorUtilityButton>(
		UEditorUtilityButton::StaticClass());

	RootPanel->AddChild(PlayPauseButton);
	UCanvasPanelSlot* PlayButtonSlot = Cast<UCanvasPanelSlot>(PlayPauseButton->Slot);
	PlayButtonSlot->SetAnchors(FAnchors(0.4f, 0.0f, 0.6f, 0.0f));
	PlayButtonSlot->SetOffsets(FMargin(0, 200, 0, 30));
	CreatePlayText();
	PlayPauseButton->OnClicked.AddDynamic(this, &UMapperWidget::OnPlayPauseButtonClick);
}

void UMapperWidget::OnPlayPauseButtonClick() {
	OnPlayAudio();
}

void UMapperWidget::OnPlayAudio() {
	if (AudioPlayer) {
		if (AudioPlayer->bIsPaused && SoundToUse == AudioPlayer->Sound) {
			AudioPlayer->SetPaused(false);
			return;
		}
		if (AudioPlayer->IsPlaying()) {
			OnPauseAudio();
			return;
		}
	} else {
		AudioPlayer = UGameplayStatics::SpawnSound2D(EditorWorld, SoundToUse, 1, 1, 0, nullptr, true, false);
	}
	AudioPlayer->SetSound(SoundToUse);
	AudioPlayer->Play();
	PlayText->SetText(FText::FromString("PAUSE"));
}

void UMapperWidget::OnStopAudio() {
	if (AudioPlayer) {
		AudioPlayer->Stop();
		PlayText->SetText(FText::FromString("PLAY"));
	}
}

void UMapperWidget::OnPauseAudio() {
	if (AudioPlayer) {
		if (AudioPlayer->IsPlaying()) {
			AudioPlayer->SetPaused(true);
			PlayText->SetText(FText::FromString("RESUME"));
		}
	}
}

void UMapperWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent) {
	Super::NativeOnFocusLost(InFocusEvent);
	UE_LOG(LogTemp, Warning, TEXT("Goodbye :("));
}

void UMapperWidget::NativePreConstruct() {
	Super::NativePreConstruct();

	UWidget* RootWidget = WidgetTree->RootWidget;
	UPanelWidget* RootPanel = Cast<UPanelWidget>(RootWidget);

	if (WidgetTree && ConstructCount == 0) {
		ConstructCount++;
		EditorWorld = GEditor->GetEditorWorldContext().World();
		CreateSelectorView(RootPanel);
		CreateThumbnailWidget(RootPanel);
		CreatePlayButton(RootPanel);
	}
}

void UMapperWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UMapperWidget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(
		UMapperWidget, SoundToUse)) {
		if (SoundToUse) {
			SoundThumbnailWidget->SetAssetByObject(SoundToUse);
			if (AudioPlayer) {
				if (AudioPlayer->IsPlaying()) {
					AudioPlayer->Stop();
					AudioPlayer->SetSound(SoundToUse);
				}
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("No sound selected."));
		}
	}
}

void UMapperWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMapperWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
}
