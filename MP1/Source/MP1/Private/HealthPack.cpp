// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPack.h"


// Sets default values
AHealthPack::AHealthPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    this->mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("health_mesh"));
    RootComponent = this->mesh;

    const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/InfiniteMatrix/Meshes/health_pack_sm.health_pack_sm'"));
    mesh->SetStaticMesh(MeshObj.Object);

}

// Called when the game starts or when spawned
void AHealthPack::BeginPlay()
{
	Super::BeginPlay();

    
	
}

// Called every frame
void AHealthPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

