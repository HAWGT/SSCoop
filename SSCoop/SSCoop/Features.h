#pragma once
#include <random>

#include "Helpers.h"
#include "CppSDK/SDK/Engine_classes.hpp"
#include "CppSDK/SDK/GM_Singleplayer_classes.hpp"
#include "CppSDK/SDK/PAWN_SystemShockCharacter_classes.hpp"
#include "CppSDK/SDK/CoreGame_classes.hpp"
#include "CppSDK/SDK/Con_Hacker_classes.hpp"
#include "CppSDK/SDK/PAWN_Hacker_Implant_classes.hpp"
#include "CppSDK/SDK/PAWN_PlayerGhost_classes.hpp"
#include "CppSDK/SDK/GM_SinglePlayer_parameters.hpp"

inline const char ConfigFileName[] = ".\\SystemShock\\Binaries\\Win64\\SystemShock.ini";

inline SDK::ACON_Hacker_C* MainHacker;

void HookGame();
void MainLoop();

void SpawnNewPlayer(SDK::ACON_Hacker_C* Hacker);
void PossessRandomPawn(SDK::ACON_Hacker_C* Hacker);

void hk_UGameViewportClient_PostRender(SDK::UGameViewportClient Instance, SDK::UCanvas* Canvas);
typedef void(__fastcall* UGameViewportClient_PostRender_t)(SDK::UGameViewportClient Instance, SDK::UCanvas* Canvas);
inline UGameViewportClient_PostRender_t Orig_UGameViewportClient_PostRender;

void __fastcall hk_AActor_ProcessEvent(SDK::AActor* Class, SDK::UFunction* Function, void* Parms);
typedef void(__fastcall* AActor_ProcessEvent_t)(SDK::AActor* Class, SDK::UFunction* Function, void* Parms);
inline AActor_ProcessEvent_t Orig_AActor_ProcessEvent;

typedef SDK::AActor* (__fastcall* UWorld_SpawnActor_t)(SDK::UWorld* Instance, struct SDK::UClass* Class, struct SDK::FTransform* Transform, void* SpawnInfo);
inline UWorld_SpawnActor_t Orig_UWorld_SpawnActor;

void __fastcall hk_UGameplayStatics_OpenLevel(const SDK::UObject* WorldContextObject, SDK::FName LevelName, bool bAbsolute, SDK::FString Options);
typedef void(__fastcall* UGameplayStatics_OpenLevel_t)(const SDK::UObject* WorldContextObject, SDK::FName LevelName, bool bAbsolute, SDK::FString Options);
inline UGameplayStatics_OpenLevel_t Orig_UGameplayStatics_OpenLevel;

typedef bool(__fastcall* UWorld_ServerTravel_t)(SDK::UWorld* Instance, const struct SDK::FString* FURL, bool bAbsolute, bool bShouldSkipGameNotify);
inline UWorld_ServerTravel_t Orig_UWorld_ServerTravel;
