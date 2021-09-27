// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

AGameHUD::AGameHUD() {
    const ConstructorHelpers::FClassFinder<UUserWidget> HealthWidgetObj(TEXT("/Game/InfiniteMatrix/UI/health_bar_widget"));
    health_widget_class = HealthWidgetObj.Class;
}

void AGameHUD::DrawHUD() {
    Super::DrawHUD();
}

void AGameHUD::BeginPlay() {
    Super::BeginPlay();

    if(health_widget_class != NULL) {
        health_widget = CreateWidget<UUserWidget>(GetWorld(), health_widget_class);
        if(health_widget) {
            health_widget->AddToViewport();
        } else {
            UE_LOG(LogTemp, Error, TEXT("Found widget class but could not create widget"));
        }
    } else {
        UE_LOG(LogTemp, Error, TEXT("No widget class found, skipping widget"));
    }
}
