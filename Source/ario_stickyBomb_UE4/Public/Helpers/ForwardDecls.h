#pragma once

// Engine classes
class AController;
class UDamageType;
class UPrimitiveComponent;

class USphereComponent;
class UProjectileMovementComponent;

class USceneComponent;
class UCameraComponent;
class UInputComponent;
class UMotionControllerComponent;

class USkeletalMeshComponent;
class UAnimMontage;
class USoundBase;

struct FAttachmentTransformRules;

/** Actors **/
class AStickyProjectile;
class AStickyGameMode;
class AStickyGameState;
class AStickyPlayerState;
class ABaseShooter;

/** Components **/
class UHealthComp;
class UAmmoComp;
class UStickyGunSkeletalComp;
class UStickyLinetraceComp;

/** Forward decl. UI
 * @todo Have not made this widget yet, mid-priority
 **/
class SHealthWidget;

/** Forward decl. Basic Slate UI */
class STextBlock;

/** Forward decl. custom UI */
class AStickyHUD;
class SAmmoWidget;
class SKillWidget;
class SSlideInText;
class SKillContentContainer;
