// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float health;

    float prev_health;
    float target_health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	bool can_dmg;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	UCurveFloat* health_curve;

	FTimeline health_timeline;
	//float curve_float_val;
	//float timeline_val;
	FTimerHandle timer_handle;

    UFUNCTION(BlueprintPure, Category="Health")
	FText GetHealthText();

    UFUNCTION()
    void MoveUp(float value);

    UFUNCTION()
    void MoveRight(float value);

	UFUNCTION()
    void SetHealth();

    UFUNCTION(BlueprintCallable, BlueprintPure,  Category="Health")
    float GetHealth();

    UFUNCTION()
    void SetCanDmg();

    UFUNCTION(BlueprintPure, Category="Health")
    bool PlayFlash();

    bool should_flash;

	void ReceiveDamage(float Damage, AActor* Damager);




	
};
