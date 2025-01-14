// Raffiesaurus, 2025


#include "Door.h"
#include "Kismet/GameplayStatics.h"
#include "RhythmHellCharacter.h"
#include "Helpers.h"

// Sets default values
ADoor::ADoor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));

	RootComponent = DoorMesh;

	TriggerBox->SetupAttachment(DoorMesh);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	TriggerBox->SetRelativeLocation(FVector::ZeroVector);

}

// Called when the game starts or when spawned
void ADoor::BeginPlay() {
	Super::BeginPlay();

	if (DoorMesh && TriggerBox) {
		FVector Origin, BoxExtent;
		DoorMesh->GetLocalBounds(Origin, BoxExtent);

		TriggerBox->SetBoxExtent(BoxExtent);
		TriggerBox->SetRelativeLocation(Origin);
	}
}

// Called every frame
void ADoor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ADoor::OnInteract_Implementation(AActor* Interactor) {
	if (ARhythmHellCharacter* Player = Cast<ARhythmHellCharacter>(Interactor)) {
		UE_LOG(LogTemp, Warning, TEXT("Door has been activated."));
		//	if (LevelToLoad != NAME_None) {
		//		UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
		//	}
	}
}

void ADoor::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);

	ARhythmHellCharacter* Player = Cast<ARhythmHellCharacter>(OtherActor);
	if (Player) {
		if (Player->bIsCarryingVinyl) {
			if (!LevelToLoad.IsNone()) {
				UE_LOG(LogTemp, Warning, TEXT("Loading: %s"), *LevelToLoad.ToString());
				UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Empty Level"));
			}
		} else {
			DisplyPickupPrompt();
		}
	}

}

void ADoor::DisplyPickupPrompt() {

	UE_LOG(LogTemp, Warning, TEXT("Go pickup something idiot"));

}
