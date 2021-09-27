// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCharacter.h"
#include <iostream>
#include "Tunnel.h"
#include "Bullet.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATestCharacter::ATestCharacter() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->GravityScale = 0;
    health = 1;
    can_dmg = true;
    should_flash = false;

    can_shoot = true;
    cooldown = 1;

    const ConstructorHelpers::FObjectFinder<UCurveFloat> HealthCurveObj(TEXT("CurveFloat'/Game/InfiniteMatrix/health_float_curve.health_float_curve'"));
    health_curve = HealthCurveObj.Object;

    this->SetActorScale3D(FVector(1, 1, 0.33));

    // Create Collider
    try {
        this->collider = CreateDefaultSubobject<UBoxComponent>(TEXT("player_trigger"));
        this->collider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        this->collider->SetBoxExtent(FVector(32 * 2, 32, 32 * 3), false);
        UE_LOG(LogTemp, Error, TEXT("Created collider with id %d"), this->collider->GetUniqueID());
    } catch(std::exception const & ex) {
        UE_LOG(LogTemp, Error, TEXT("Error creating collider %s"), ex.what());
    }


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

    APlayerController* PC = (APlayerController*) GetController();

    if (PC) {
        PC->bShowMouseCursor = true;
        //PC->bEnableClickEvents = true;
        //PC->bEnableMouseOverEvents = true;
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

    if(health_curve) {
        FOnTimelineFloat update_callback;
        FOnTimelineEventStatic finished_callback;

        update_callback.BindUFunction(this, FName("SetCooldown"));
        finished_callback.BindUFunction(this, FName("SetCanShoot"));

        cooldown_timeline.AddInterpFloat(health_curve, update_callback);
        cooldown_timeline.SetTimelineFinishedFunc(finished_callback);
    } else {
        UE_LOG(LogTemp, Error, TEXT("No health curve found"));
    }

	
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime) {
    this->forward_speed = 2000 + 50 * score;
    if(health <= 0) {
        this->forward_speed = 0;
        this->can_shoot = false;
    }

    if(DeltaTime < 0.5) {
        Super::Tick(DeltaTime);
        AddActorWorldOffset(FVector(forward_speed * DeltaTime, 0, 0), true, NULL,  ETeleportType::None);
        this->health_timeline.TickTimeline(DeltaTime);
        this->cooldown_timeline.TickTimeline(DeltaTime);
    }
}

void ATestCharacter::Restart() {
    if(health <= 0) {
        UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
    }
}

bool ATestCharacter::IsGameOver() {
    return health <= 0;
}

void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("Up", this, &ATestCharacter::MoveUp);
    PlayerInputComponent->BindAxis("Right", this, &ATestCharacter::MoveRight);

    PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ATestCharacter::OnShoot);

}

void ATestCharacter::OnShoot() {

    if(can_shoot) {
        FVector mouse_loc, mouse_dir;
        APlayerController* controller = (APlayerController*) GetWorld()->GetFirstPlayerController();
        controller->DeprojectMousePositionToWorld(mouse_loc, mouse_dir);
        
        ABullet* bullet = this->GetWorld()->SpawnActor<ABullet>(this->GetActorLocation(), mouse_dir.Rotation());
        if(bullet) {
            bullet->speed = 2 * this->forward_speed;
        }
        can_shoot = false;

        prev_cooldown = 0;
        target_cooldown = 1;

        cooldown_timeline.PlayFromStart();
    }
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

FText ATestCharacter::GetScoreText() {
	FString score_str = FString(TEXT("Score: ")) + FString::FromInt(score);
	return FText::FromString(score_str);
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

    if(health_timeline.IsPlaying()) {
        health_timeline.Stop();
        health = target_health;
    }

    if(!this->can_dmg && damage > 0) {
        damage = 0;
        can_dmg = true;
    }

    prev_health = health;
    target_health = health - damage;

    if(target_health >= 1) {
        target_health = 1;
    } else if(target_health <= 0) {
        target_health = 0;
    }

    if(damage > 0) {
        this->should_flash = true;
        this->can_dmg = false;
    }
    health_timeline.PlayFromStart();
}

bool ATestCharacter::PlayFlash() {
    if(should_flash) {
        should_flash = false;
        return true;
    } else {
        return false;
    }
}

// Cooldown

void ATestCharacter::SetCooldown() {
    if(health_curve) {
        float timeline_val = cooldown_timeline.GetPlaybackPosition();
        float curve_val = health_curve->GetFloatValue(timeline_val);
        this->cooldown = prev_cooldown * (1 - curve_val) + (curve_val) * target_cooldown;
    }
}

void ATestCharacter::SetCanShoot() {
    this->can_shoot = true;
    cooldown = 1;
}

float ATestCharacter::GetCooldown() {
    return this->cooldown;
}
