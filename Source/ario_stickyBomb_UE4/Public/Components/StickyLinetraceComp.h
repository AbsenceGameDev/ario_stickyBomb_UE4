// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"

// Helpers
#include "Helpers/ForwardDecls.h"
#include "Helpers/Macros.h"

#include <Components/SceneComponent.h>

#include "StickyLinetraceComp.generated.h"

/**
 * @author  Ario Amin
 * @file    Components/StickyLinetraceComponent
 * @class   UStickyLinetraceComp
 * @brief   Not much more than a linetracer. Almost a functor,
 * @details It is not quite a functor, but it is very small and serves only one function, the tick-component function.
 *          It runs a tick on 0.2 second period, in which it shoots a linetrace about 2.5 meters adjusted from Unreal Units.
 *          It is designed in a way that it is attached to the camera of ABaseShooter (and derived) actors and will activate the
 *          component by pressing the designated key for it,
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UStickyLinetraceComp : public USceneComponent
{
	GENERATED_BODY()

	public:
	/**
	 * @brief   Construct a new UStickyLinetraceComp object
	 *
	 */
	UStickyLinetraceComp();

	protected:
	/**
	 * @brief   Called when the game starts
	 *
	 */
	virtual void BeginPlay() override;

	public:
	/**
	 * @brief   This components tick function.
	 * @details It fires a line trace in direction of the components normal vector.
	 *          Tick interval is set to 0.5 seconds for this component
	 *
	 * @param   DeltaTime
	 * @param   TickType
	 * @param   ThisTickFunction
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Basic Params")
	float LookAtDistance = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Basic Params")
	float TickInterval = 0.5f;

#ifdef STICKY_DEBUG
	UWorld*				WorldPtr		= nullptr;
	ABaseShooter* BaseChar		= nullptr;
	bool					bValidOwner = false;
#endif		// STICKY_DEBUG

	FHitResult* HitResult = new FHitResult();
};
