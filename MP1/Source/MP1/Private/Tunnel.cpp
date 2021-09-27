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
    int rot = FMath::RandRange(0, 360);
    FString string = FString::Printf( TEXT("Rand rotation %d"), rot);

    this->wall_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("tunnel_wall_mesh"));
    this->wall_mesh->AddLocalRotation(FRotator(0, 0, rot));
    this->wall_mesh->ApplyWorldOffset( FVector(2500, 0, 0), false);
    this->wall_mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    const ConstructorHelpers::FObjectFinder<UStaticMesh> WallMeshObj(*FString::Printf(TEXT("StaticMesh'/Game/InfiniteMatrix/Meshes/SM_Hole_0%d'"), FMath::RandRange(1, 4)));
    this->wall_mesh->SetStaticMesh(WallMeshObj.Object);

    this->wall_mesh->OnComponentBeginOverlap.AddDynamic(this, &ATunnel::OnWallHit);

    wall_speed = FMath::RandRange(-50, 50);

    // Do health pack
    if(FMath::RandRange(0, 100) < 25) {
        this->health_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("health_mesh"));

        float hp_theta = FMath::RandRange(0, 360);
        float hp_r = FMath::RandRange(0, 400);
        float hp_h = FMath::RandRange(500, 2000);

        float hp_sin, hp_cos;
        FMath::SinCos(&hp_sin, &hp_cos, hp_theta);

        this->health_mesh->ApplyWorldOffset(
                FVector(hp_h, hp_cos * hp_r, hp_sin * hp_r),
                    false);
        this->health_mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

        const ConstructorHelpers::FObjectFinder<UStaticMesh> HealthMeshObj(TEXT("StaticMesh'/Game/InfiniteMatrix/Meshes/health_pack_sm.health_pack_sm'"));
        this->health_mesh->SetStaticMesh(HealthMeshObj.Object);

        this->health_mesh->OnComponentBeginOverlap.AddDynamic(this, &ATunnel::OnHealthHit);

        this->enemy_mesh = NULL;
    } else  {
        // Do enemy
        this->health_mesh = NULL;

        this->enemy_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("enemy_mesh"));

        enemy_theta = FMath::RandRange(0, 360);
        enemy_r = FMath::RandRange(0, 400);
        enemy_h = FMath::RandRange(500, 2000);
        enemy_speed = FMath::RandRange(-5, 5);

        float enemy_sin, enemy_cos;
        FMath::SinCos(&enemy_sin, &enemy_cos, enemy_theta);

        this->enemy_mesh->ApplyWorldOffset(
                FVector(enemy_h, enemy_cos * enemy_r, enemy_sin * enemy_r),
                    false);
        this->enemy_mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

        const ConstructorHelpers::FObjectFinder<UStaticMesh> EnemyMeshObj(TEXT("StaticMesh'/Game/InfiniteMatrix/Meshes/enemy_sm.enemy_sm'"));
        this->enemy_mesh->SetStaticMesh(EnemyMeshObj.Object);
        this->enemy_mesh->SetWorldScale3D(FVector(0.75, 0.75, 0.75));

        this->enemy_mesh->OnComponentBeginOverlap.AddDynamic(this, &ATunnel::OnEnemyHit);

        this->enemy_movement = 100 * FMath::VRand();
        this->enemy_movement.X = 0;
    }
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

    if(this->wall_mesh) {
        this->wall_mesh->AddLocalRotation(FRotator(0, 0, wall_speed * DeltaTime));
    } else {
        //UE_LOG(LogTemp, Error, TEXT("Wall mesh was deleted, not ticking"));
    }

    if(this->health_mesh) {
        this->health_mesh->AddLocalRotation(FRotator(0, 100 * DeltaTime, 0));
    }

    if(this->enemy_mesh) {
        //this->enemy_mesh->AddLocalOffset(DeltaTime * this->enemy_movement);
        float d_theta = enemy_speed * DeltaTime;

        float enemy_sin, enemy_cos;
        FMath::SinCos(&enemy_sin, &enemy_cos, enemy_theta);
        enemy_theta += d_theta;

        this->enemy_mesh->AddLocalOffset(
                FVector(0, -enemy_r * enemy_sin * d_theta, enemy_r * enemy_cos * d_theta),
                    false);
    }
}

void ATunnel::TriggerEnter(
        class UPrimitiveComponent* HitComp, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult & SweepResult) {

    ATestCharacter* character = dynamic_cast<ATestCharacter*>(OtherActor);
    if(character != NULL) {
        if(character->collider == OtherComp) {
            //UE_LOG(LogTemp, Warning, TEXT("Hit %s on %s, creating wall"), *OtherActor->GetName(), *OtherComp->GetName());
            this->GetWorld()->SpawnActor<ATunnel>(this->GetActorLocation() + FVector(2500 * 3, 0, 0), this->GetActorRotation());
        }

    }
}

void ATunnel::TriggerExit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

    ATestCharacter* character = dynamic_cast<ATestCharacter*>(OtherActor);
    if(character) {
        if(character->collider == OtherComp) {
            if(this->wall_mesh != NULL) {
                character->score++;
            }
        }
        if(! this->GetWorld()->DestroyActor(this)) {
            UE_LOG(LogTemp, Error, TEXT("Failed to delete"));
        }

    }
}


void ATunnel::OnWallHit(
        class UPrimitiveComponent* HitComp, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult & SweepResult) {

    ATestCharacter* character = dynamic_cast<ATestCharacter*>(OtherActor);
    if(character != NULL) {

        character->ReceiveDamage(0.1, this);
        wall_mesh->DestroyComponent();
        wall_mesh = NULL;
    }
}

void ATunnel::OnEnemyHit(
        class UPrimitiveComponent* HitComp, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult & SweepResult) {

    ATestCharacter* character = dynamic_cast<ATestCharacter*>(OtherActor);
    if(character != NULL) {

        character->ReceiveDamage(0.1, this);
        enemy_mesh->DestroyComponent();
        enemy_mesh = NULL;
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Enemy hit by %s %s"), *OtherActor->GetName(), *OtherComp->GetName());
    }
}

void ATunnel::OnHealthHit(
        class UPrimitiveComponent* HitComp, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult & SweepResult) {

    ATestCharacter* character = dynamic_cast<ATestCharacter*>(OtherActor);
    if(character != NULL) {

        character->ReceiveDamage(-0.1, this);
        health_mesh->DestroyComponent();
        health_mesh = NULL;
    }
}
