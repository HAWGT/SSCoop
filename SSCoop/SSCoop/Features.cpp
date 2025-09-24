#include "Features.h"

void HookGame()
{
	//TODO: 
	//enable console

	bool bHooked = false;

	while (!bHooked)
	{
		auto World = SDK::UWorld::GetWorld();
		auto Engine = SDK::UEngine::GetEngine();

		if (!World || !Engine || !Engine->GameViewport)
		{
			continue;
		}

		Orig_UGameViewportClient_PostRender = reinterpret_cast<UGameViewportClient_PostRender_t>(VTHook(reinterpret_cast<const void**>(reinterpret_cast<SDK::UGameViewportClient*>(Engine->GameViewport)->VTable), 0x63, hk_UGameViewportClient_PostRender));

		Orig_AActor_ProcessEvent = reinterpret_cast<AActor_ProcessEvent_t>(TrampHook64(PatternScan("48 89 5C 24 10 48 89 6C 24 18 57 48 83 EC ? F7 82 B0 00 00 00 ? ? ? ?"), (BYTE*)hk_AActor_ProcessEvent, 15));

		Orig_UWorld_SpawnActor = reinterpret_cast<UWorld_SpawnActor_t>(PatternScan("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 08 FF FF FF 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 40"));

		BYTE ForceListenPatch[] = { 0x90, 0x90 };

		BYTE* ListenOptionCheck = PatternScan("74 ? 49 8B 8D 80 02 00 00 49 8B D7");

		Patch(ForceListenPatch, ListenOptionCheck, sizeof(ForceListenPatch));

		Orig_UGameplayStatics_OpenLevel = reinterpret_cast<UGameplayStatics_OpenLevel_t>(TrampHook64(PatternScan("48 89 54 24 10 55 53 56 41 56 48 8D 6C 24 C1"), (BYTE*)hk_UGameplayStatics_OpenLevel, 15));

		Orig_UWorld_ServerTravel = reinterpret_cast<UWorld_ServerTravel_t>(PatternScan("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 56 48 83 EC ? 48 8B B9 18 01 00 00"));

		bHooked = true;

		
		/*

		for (int i = 0; i < SDK::UObject::GObjects->Num(); i++)
		{
			SDK::UObject* Obj = SDK::UObject::GObjects->GetByIndex(i);

			if (!Obj)
				continue;

			if (Obj->IsDefaultObject())
				continue;

			if (Obj->IsA(SDK::AActor::StaticClass()))
			{
				SDK::AActor* Actor = static_cast<SDK::AActor*>(Obj);

				Actor->bReplicates = true;
				Actor->bReplicateMovement = true;
			}
		}

		*/
	}
}

void MainLoop()
{
}

void SpawnNewPlayer(SDK::ACON_Hacker_C* Hacker)
{
	std::cout << "SpawnNewPlayer" << "\n";

	auto HackerImplant = SDK::UCoreGameFunctionLibrary::SpawnActorInWorld(SDK::APAWN_Hacker_Implant_C::StaticClass(), Hacker->GetTransform(), true, SDK::UWorld::GetWorld());
	Hacker->Possess(static_cast<SDK::APawn*>(HackerImplant));
}

void PossessRandomPawn(SDK::ACON_Hacker_C* Hacker)
{
	std::cout << "PossessRandomPawn" << "\n";

	std::vector<SDK::APawn*> Pawns;

	auto World = SDK::UWorld::GetWorld();
	auto Level = World->PersistentLevel;
	SDK::TArray<SDK::AActor*>& Actors = Level->Actors;

	for (const auto& Actor : Actors)
	{
		if (Actor->IsA(SDK::APawn::StaticClass()) && !Actor->IsA(SDK::APAWN_Hacker_Implant_C::StaticClass()) && !Actor->IsA(SDK::APAWN_PlayerGhost_C::StaticClass()))
		{
			Pawns.push_back(static_cast<SDK::APawn*>(Actor));
		}
	}

	if (Pawns.size() < 1)
	{
		SpawnNewPlayer(Hacker);
		return;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, Pawns.size() - 1);

	int randomValue = distrib(gen);

	std::cout << randomValue << "\n";

	Hacker->Possess(Pawns[randomValue]);
}

void hk_UGameViewportClient_PostRender(SDK::UGameViewportClient Instance, SDK::UCanvas* Canvas)
{
	MainLoop();

	return Orig_UGameViewportClient_PostRender(Instance, Canvas);
}

void __fastcall hk_AActor_ProcessEvent(SDK::AActor* Class, SDK::UFunction* Function, void* Parms)
{
	Orig_AActor_ProcessEvent(Class, Function, Parms);

	if (Class->IsA(SDK::AGM_SinglePlayer_C::StaticClass()) && !Function->GetName().compare("K2_PostLogin"))
	{
		std::cout << "K2_PostLogin" << "\n";

		auto Hacker = static_cast<SDK::ACON_Hacker_C*>(static_cast<SDK::Params::GM_SinglePlayer_C_K2_PostLogin*>(Parms)->NewPlayer);

		if (!Hacker->bIsLocalPlayerController)
		{
			PossessRandomPawn(Hacker);
		}
		else
		{
			MainHacker = Hacker;

			auto World = SDK::UWorld::GetWorld();
			auto Level = World->PersistentLevel;
			SDK::TArray<SDK::AActor*>& Actors = Level->Actors;

			for (const auto& Actor : Actors)
			{
				Actor->SetReplicates(true);
				Actor->SetReplicateMovement(true);
			}
		}
	}

	if (!Function->GetName().compare("K2_DestroyActor") && Class->IsA(SDK::APawn::StaticClass()) && !Class->IsA(SDK::APAWN_Hacker_Implant_C::StaticClass()) && !Class->IsA(SDK::APAWN_PlayerGhost_C::StaticClass()))
	{
		std::cout << "K2_DestroyActor" << "\n";

		auto Pawn = static_cast<SDK::APawn*>(Class);
		auto Controller = Pawn->Controller;

		if (!Controller->IsA(SDK::ACON_Hacker_C::StaticClass()))
		{
			return;
		}

		auto Hacker = static_cast<SDK::ACON_Hacker_C*>(Controller);

		if (Controller != MainHacker)
		{
			PossessRandomPawn(Hacker);
		}
	}
}

void __fastcall hk_UGameplayStatics_OpenLevel(const SDK::UObject* WorldContextObject, SDK::FName LevelName, bool bAbsolute, SDK::FString Options)
{
	auto World = SDK::UWorld::GetWorld();
	if (World)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, LevelName.ToString().c_str(), (int)LevelName.ToString().size(), NULL, 0);
		std::wstring wLevelNameString(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, LevelName.ToString().c_str(), (int)LevelName.ToString().size(), &wLevelNameString[0], size_needed);
		const wchar_t* wLevelNameChar = wLevelNameString.c_str();

		SDK::FString FURL = SDK::FString(wLevelNameChar);

		Orig_UWorld_ServerTravel(World, &FURL, false, false);
	}
	else
	{
		Orig_UGameplayStatics_OpenLevel(WorldContextObject, LevelName, bAbsolute, Options);
	}
}
