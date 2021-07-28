
#include "StickyPlayerState.h"

#include "StickyGameMode.h"
#include "UI/StickyHUD.h"

#include <Engine/Engine.h>
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

/*

	Persistent information for the player. Player controller only exists
	on your machine and server.

	Hierarchy in ue4 for player state

	UObjectBase
	UObjectBaseUtility
	UObject
	AActor
	AInfo
	APlayerState
*/

// Score is Replicated : Direct change to server
void AStickyPlayerState::AddUserScore(float deltaScore)
{
  SetScore(GetScore() + deltaScore);
}

float AStickyPlayerState::GetUserScore() const
{
  return GetScore();
}

void AStickyPlayerState::AddKill(int32 Kill)
{
  Kills += Kill;
}

int32 AStickyPlayerState::GetKills() const
{
  return Kills;
}

// Notify HUD
void AStickyPlayerState::OnRep_Kills()
{
  AStickyHUD* PlayerHud = Cast<AStickyHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
  if (PlayerHud)
	PlayerHud->UpdateTotalKills();
}

void AStickyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);
  DOREPLIFETIME(AStickyPlayerState, Kills);
}
