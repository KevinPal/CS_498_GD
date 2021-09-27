// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class MP1_API AGameHUD : public AHUD {

	GENERATED_BODY()
	
    public:
        AGameHUD();

        virtual void DrawHUD() override;

        virtual void BeginPlay() override;

    private:
        UPROPERTY(EditAnywhere, Category = "Health")
        TSubclassOf<class UUserWidget> health_widget_class;

        UPROPERTY(EditAnywhere, Category = "Health")
        class UUserWidget* health_widget;
	
};
