// Fill out your copyright notice in the Description page of Project Settings.


#include "Tunnel.h"
#include "TestCharacter.h"
#include "Bullet.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABullet::ABullet() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    this->mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bullet_mesh"));
    RootComponent = this->mesh;
    const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
    mesh->SetStaticMesh(MeshObj.Object);
    this->SetActorScale3D(FVector(0.75, 0.75, 0.75));

    this->mesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    this->mesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::TriggerEnter);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ABullet::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
    this->ApplyWorldOffset(DeltaTime * GetActorForwardVector() * speed, false);
}

void ABullet::TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {

    if(dynamic_cast<ATestCharacter*>(OtherActor) == NULL) {
        UE_LOG(LogTemp, Warning, TEXT("Hit %s [%s]"), *OtherActor->GetName(), *OtherComp->GetName());

        ATunnel* tunnel = dynamic_cast<ATunnel*>(OtherActor);

        if(tunnel != NULL) {

            if(OtherComp == tunnel->enemy_mesh) {
                tunnel->enemy_mesh->DestroyComponent();
                tunnel->enemy_mesh = NULL;

                ATestCharacter* actual_character = dynamic_cast<ATestCharacter*>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
                if(actual_character) {
                    actual_character->score++;
                } else {
                    UE_LOG(LogTemp, Error, TEXT("Failed to increment score"));
                }
                this->GetWorld()->DestroyActor(this);
            } else if(OtherComp != tunnel->trigger) {
                this->GetWorld()->DestroyActor(this);
            }
            

        }

    }
}
