// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"

#include "TestCharacter.generated.h"

UCLASS()
class MP1_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attributes);
    int forward_speed = 2000;

    UPROPERTY(EditAnywhere)
    class UBoxComponent* collider = NULL;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	bool can_dmg;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	UCurveFloat* health_curve;

    UFUNCTION(BlueprintPure, Category="Health")
	FText GetHealthText();

    UFUNCTION(BlueprintPure, Category="Health")
    bool PlayFlash();

    UFUNCTION(BlueprintCallable, BlueprintPure,  Category="Health")
    float GetHealth();

	FTimeline health_timeline;
    float prev_health;
    float target_health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Score")
	int score;

    UFUNCTION(BlueprintPure, Category="Score")
	FText GetScoreText();

    UFUNCTION()
    void MoveUp(float value);

    UFUNCTION()
    void MoveRight(float value);

	UFUNCTION()
    void SetHealth();

    UFUNCTION()
    void SetCanDmg();

	FTimeline cooldown_timeline;
    float prev_cooldown;
    float target_cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	bool can_shoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	float cooldown;

    UFUNCTION()
    void SetCooldown();

    UFUNCTION()
    void SetCanShoot();

    UFUNCTION(BlueprintCallable, BlueprintPure,  Category="Cooldown")
    float GetCooldown();

    UFUNCTION(Category="Cooldown")
    void OnShoot();

    bool should_flash;

	void ReceiveDamage(float Damage, AActor* Damager);

    UFUNCTION(BlueprintCallable)
    void Restart();

    UFUNCTION(BlueprintCallable)
    bool IsGameOver();



	
};
