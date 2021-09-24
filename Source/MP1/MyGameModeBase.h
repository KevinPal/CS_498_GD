// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
//#include "GameFramework/AGameModeBase/DefaultPawnClass.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MP1_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
    public:
        AMyGameModeBase();

        //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attributes);
        //TSubclassOf<APawn> starter_pawn;
	
	
};
