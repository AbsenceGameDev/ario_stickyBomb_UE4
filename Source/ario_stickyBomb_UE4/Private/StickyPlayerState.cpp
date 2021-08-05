/**
 * @author  Ario Amin
 * @file    /StickyPlayerState.cpp
 * @class   AStickyPlayerState
 * @brief   Networked Player-state implementation
 * @details Keeps track of the state of the player, in regards to kills and ammo.
 * @todo    Redesign and remove extraneous code
 */
#include "StickyPlayerState.h"

#include "StickyGameMode.h"
#include "UI/StickyHUD.h"

// Engine Classes/Types
#include <Engine/Engine.h>
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

/* ================================ */
/* Public Methods: Client interface */
void	AStickyPlayerState::AddUserScore(float deltaScore) { SetScore(GetScore() + deltaScore); }
float AStickyPlayerState::GetUserScore() const { return GetScore(); }
void	AStickyPlayerState::AddKill(int32 Kill) { Kills += Kill; }
int32 AStickyPlayerState::GetKills() const { return Kills; }
void	AStickyPlayerState::SetAmmo(int32 AmmoCount)
{
	Ammo									= AmmoCount;
	AStickyHUD* PlayerHud = Cast<AStickyHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	if (PlayerHud) {
		PlayerHud->UpdateAmmo();
	}
}
int32 AStickyPlayerState::GetAmmo() const { return Ammo; }

/* ======================================== */
/* Protected Methods: Server/Client Actions */
void AStickyPlayerState::OnRep_Kills()
{
	AStickyHUD* PlayerHud = Cast<AStickyHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	if (PlayerHud) PlayerHud->UpdateTotalKills();
}

void AStickyPlayerState::OnRep_Ammo()
{
	AStickyHUD* PlayerHud = Cast<AStickyHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	if (PlayerHud) {
		PlayerHud->UpdateAmmo();
	}
}

void AStickyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStickyPlayerState, Kills);
	DOREPLIFETIME(AStickyPlayerState, Ammo);
}
