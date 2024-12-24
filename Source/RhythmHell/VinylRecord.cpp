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
	UE_LOG(LogTemp, Warning, TEXT("HELP"));
	if (ARhythmHellCharacter* Character = Cast<ARhythmHellCharacter>(Interactor)) {
		UE_LOG(LogTemp, Warning, TEXT("Picked up vinyl: %s"), *SongName);
	}
}
