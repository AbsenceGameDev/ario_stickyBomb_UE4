// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Components/SceneComponent.h>

#include "StickyLinetraceComp.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UStickyLinetraceComp : public USceneComponent
{
	GENERATED_BODY()

	public:
	// Sets default values for this component's properties
	UStickyLinetraceComp();

	protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Basic Params")
	float LookAtDistance = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Basic Params")
	float TickInterval = 0.5f;

	UWorld*				WorldPtr		= nullptr;
	ABaseShooter* BaseChar		= nullptr;
	FHitResult*		HitResult		= new FHitResult();
	bool					bValidOwner = false;
};
