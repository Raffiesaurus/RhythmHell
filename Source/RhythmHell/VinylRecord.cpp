// Fill out your copyright notice in the Description page of Project Settings.


#include "VinylRecord.h"
#include "RhythmHellCharacter.h"

// Sets default values
AVinylRecord::AVinylRecord() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VinylMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VinylMesh"));
	RootComponent = VinylMesh;
}

// Called when the game starts or when spawned
void AVinylRecord::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AVinylRecord::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AVinylRecord::OnInteract_Implementation(AActor* Interactor) {
	if (ARhythmHellCharacter* Character = Cast<ARhythmHellCharacter>(Interactor)) {
		//UE_LOG(LogTemp, Warning, TEXT("Vinyl %s has been picked up by player."), *SongName);
	}
}

void AVinylRecord::Highlight_Implementation(bool bEnable) {
	if (VinylMesh) {
		VinylMesh->SetRenderCustomDepth(bEnable);
	}
}