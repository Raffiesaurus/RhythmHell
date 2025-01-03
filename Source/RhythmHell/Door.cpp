// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Kismet/GameplayStatics.h"
#include "RhythmHellCharacter.h"

// Sets default values
ADoor::ADoor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay() {
	Super::BeginPlay();

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