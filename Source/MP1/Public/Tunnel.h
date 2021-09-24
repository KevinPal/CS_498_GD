// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "Tunnel.generated.h"

UCLASS(Blueprintable)
class MP1_API ATunnel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATunnel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);    

    UFUNCTION()
    void TriggerExit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void OnWallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


    UPROPERTY(EditAnywhere)
    class UStaticMeshComponent* mesh = NULL;
    UPROPERTY(EditAnywhere)
    class UStaticMeshComponent* wall_mesh = NULL;
    UPROPERTY(EditAnywhere)
    class UBoxComponent* trigger = NULL;

    UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> wall_dmg;
	
	
};
