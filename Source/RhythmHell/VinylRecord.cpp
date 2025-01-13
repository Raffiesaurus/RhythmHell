// Raffiesaurus, 2025


#include "VinylRecord.h"
#include "RhythmHellCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

// Sets default values
AVinylRecord::AVinylRecord() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VinylMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VinylMesh"));
	RootComponent = VinylMesh;

	InfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidgetComponent"));
	InfoWidgetComponent->SetupAttachment(RootComponent);
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	InfoWidgetComponent->SetDrawSize(FVector2D(200, 50));
	InfoWidgetComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void AVinylRecord::BeginPlay() {
	Super::BeginPlay();

	if (InfoWidgetComponent && InfoWidgetComponent->GetUserWidgetObject()) {
		UUserWidget* Widget = Cast<UUserWidget>(InfoWidgetComponent->GetUserWidgetObject());
		if (Widget) {
			UTextBlock* TextBlock = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("SongNameText")));
			if (TextBlock) {
				TextBlock->SetText(FText::FromString(SongName));
			}
		}
	}
}

// Called every frame
void AVinylRecord::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (InfoWidgetComponent) {
		APlayerController* Player = GetWorld()->GetFirstPlayerController();
		if (Player && Player->GetPawn()) {
			FVector PlayerLocation = Player->GetPawn()->GetActorLocation();
			FVector WidgetLocation = InfoWidgetComponent->GetComponentLocation();

			FRotator LookAtRotation = (PlayerLocation - WidgetLocation).Rotation();

			InfoWidgetComponent->SetWorldRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));
		}
	}
}

void AVinylRecord::OnInteract_Implementation(AActor* Interactor) {
	if (ARhythmHellCharacter* Character = Cast<ARhythmHellCharacter>(Interactor)) {
		//UE_LOG(LogTemp, Warning, TEXT("Vinyl %s has been picked up by player."), *SongName);
	}
}

void AVinylRecord::Highlight_Implementation(bool bEnable) {

	if (bHighlighted == bEnable) {
		return;
	}

	bHighlighted = bEnable;

	if (VinylMesh) {
		VinylMesh->SetRenderCustomDepth(bEnable);
	}

	if (InfoWidgetComponent) {
		InfoWidgetComponent->SetVisibility(bEnable);
	}
}