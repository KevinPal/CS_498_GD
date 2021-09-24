// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCharacter.h"
#include "Tunnel.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->GravityScale = 0;
    health = 1;
    can_dmg = true;
    should_flash = false;

    const ConstructorHelpers::FObjectFinder<UCurveFloat> HealthCurveObj(TEXT("CurveFloat'/Game/InfiniteMatrix/health_float_curve.health_float_curve'"));
    health_curve = HealthCurveObj.Object;

}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("new YEETING Start"));

    // Setup tunnels
    //FVector new_pos = FVector(old_pos.X + 2500, old_pos.Y, old_pos.Z);
    for(int i = 0; i < 3; i++) {
        this->GetWorld()->SpawnActor<ATunnel>(this->GetActorLocation() + FVector(2500 * i, 0, 0), this->GetActorRotation());
    }

    // Setup HUD and health stuff
    //
    if(health_curve) {
        FOnTimelineFloat update_callback;
        FOnTimelineEventStatic finished_callback;

        update_callback.BindUFunction(this, FName("SetHealth"));
        finished_callback.BindUFunction(this, FName("SetCanDmg"));

        health_timeline.AddInterpFloat(health_curve, update_callback);
        health_timeline.SetTimelineFinishedFunc(finished_callback);
    } else {
        UE_LOG(LogTemp, Error, TEXT("No health curve found"));
    }
	
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime) {
    if(DeltaTime < 0.5) {
        Super::Tick(DeltaTime);
        AddActorWorldOffset(FVector(forward_speed * DeltaTime, 0, 0), true, NULL,  ETeleportType::None);
        this->health_timeline.TickTimeline(DeltaTime);
    }

}

void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("Up", this, &ATestCharacter::MoveUp);
    PlayerInputComponent->BindAxis("Right", this, &ATestCharacter::MoveRight);
}

void ATestCharacter::MoveUp(float val) {
    AddActorWorldOffset(FVector(0, 0, 500 * val * GetWorld()->GetDeltaSeconds()), true, NULL,  ETeleportType::None);
}

void ATestCharacter::MoveRight(float val) {
    AddActorWorldOffset(FVector(0, 500 * val * GetWorld()->GetDeltaSeconds(), 0), true, NULL,  ETeleportType::None);
}

FText ATestCharacter::GetHealthText() {
    int32 HP = FMath::RoundHalfFromZero(health * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

float ATestCharacter::GetHealth() {
    return this->health;
}

void ATestCharacter::SetHealth() {
    if(health_curve) {
        float timeline_val = health_timeline.GetPlaybackPosition();
        float curve_val = health_curve->GetFloatValue(timeline_val);
        this->health = prev_health * (1 - curve_val) + (curve_val) * target_health;
    }
}

void ATestCharacter::SetCanDmg() {
    this->can_dmg = true;
}

void ATestCharacter::ReceiveDamage(float damage, AActor* damager) {


    UE_LOG(LogTemp, Error, TEXT("Got dmg"));
    
    if(this->can_dmg) {
        prev_health = health;
        target_health = health - damage;
        this->can_dmg = false;
        this->should_flash = true;
        health_timeline.PlayFromStart();
    }
}

bool ATestCharacter::PlayFlash() {
    if(should_flash) {
        should_flash = false;
        return true;
    } else {
        return false;
    }
}

