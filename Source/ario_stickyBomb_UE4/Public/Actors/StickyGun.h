// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StickyGun.generated.h"

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStickyGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
