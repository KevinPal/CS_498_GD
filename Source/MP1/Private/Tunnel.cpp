// Fill out your copyright notice in the Description page of Project Settings.


#include "Tunnel.h"
#include "TestCharacter.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Math/UnrealMathUtility.h"

// Sets default values
ATunnel::ATunnel() {

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //OnActorBeginOverlap.AddDynamic(this, &ATunnel::OnOverlapBegin);

    // Setup Mesh
    this->mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("tunnel_mesh"));
    RootComponent = this->mesh;

    const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/InfiniteMatrix/Meshes/SM_Tunnel"));
    mesh->SetStaticMesh(MeshObj.Object);

    // Create Wall
    //
    int rot = FMath::RandRange(0, 360);
    FString string = FString::Printf( TEXT("Rand rotation %d"), rot);

    this->wall_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("tunnel_wall_mesh"));
    this->wall_mesh->AddLocalRotation(FRotator(0, 0, rot));
    this->wall_mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

    //const ConstructorHelpers::FObjectFinder<UStaticMesh> WallMeshObj(TEXT("/Game/InfiniteMatrix/Meshes/SM_Hole_03"));
    const ConstructorHelpers::FObjectFinder<UStaticMesh> WallMeshObj(*FString::Printf(TEXT("StaticMesh'/Game/InfiniteMatrix/Meshes/SM_Hole_0%d'"), FMath::RandRange(1, 4)));
    this->wall_mesh->SetStaticMesh(WallMeshObj.Object);

    this->wall_mesh->OnComponentHit.AddDynamic(this, &ATunnel::OnWallHit);


    // Create Trigger
    this->trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("tunnel_trigger"));
    this->trigger->ApplyWorldOffset(FVector(2516, 0, 0), false);
    this->trigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    this->trigger->SetBoxExtent(FVector(32, 500, 500), false);

    this->trigger->OnComponentBeginOverlap.AddDynamic(this, &ATunnel::TriggerEnter);
    this->trigger->OnComponentEndOverlap.AddDynamic(this, &ATunnel::TriggerExit);



}

// Called when the game starts or when spawned
void ATunnel::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ATunnel::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    FRotator NewRotation = FRotator(0, 0, 30 * DeltaTime);
	FQuat QuatRotation = FQuat(NewRotation);
    if(this->wall_mesh) {
        this->wall_mesh->AddLocalRotation(FRotator(0, 0, 30 * DeltaTime));
    } else {
        UE_LOG(LogTemp, Error, TEXT("Wall mesh was deleted, not ticking"));
    }
}

void ATunnel::TriggerEnter(
        class UPrimitiveComponent* HitComp, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult & SweepResult) {

    if(dynamic_cast<ATestCharacter*>(OtherActor) != NULL) {

        this->GetWorld()->SpawnActor<ATunnel>(this->GetActorLocation() + FVector(2500 * 3, 0, 0), this->GetActorRotation());
    }
}

void ATunnel::TriggerExit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

    if(dynamic_cast<ATestCharacter*>(OtherActor) != NULL) {
        if(! this->GetWorld()->DestroyActor(this)) {
            UE_LOG(LogTemp, Error, TEXT("Failed to delete"));
        }
    }
}


void ATunnel::OnWallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
    ATestCharacter* character = dynamic_cast<ATestCharacter*>(OtherActor);
    if(character != NULL) {

        UE_LOG(LogTemp, Warning, TEXT("Hit wall"));
        character->ReceiveDamage(0.1, this);
        wall_mesh->DestroyComponent();
        wall_mesh = NULL;
    }
}
