namespace GOTHIC_NAMESPACE
{
		/*void __fastcall oCNpc_Equip(oCNpc* _this, oCItem* item);
		auto Hook_oCNpc_Equip = ::Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x006968F0, 0x006C8440, 0x006DB550, 0x00739C90)), &oCNpc_Equip);
		void __fastcall oCNpc_Equip(oCNpc* _this, oCItem* item)
		{
				if (!item)
				{
						return;
				}

				if (_this != player)
				{
						Hook_oCNpc_Equip(_this, item);
						return;
				}

				Hook_oCNpc_Equip(_this, item);
		}*/

		auto Hook_oCGame_HandleEvent = ::Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x0065EEE0, 0x0068A300, 0x0069E980, 0x006FC170)), &oCGame::HandleEvent_Union);
		int oCGame::HandleEvent_Union(int Key)
		{
				if (this->GetWorld()->csPlayer->GetPlayingGlobalCutscene() || !player || player->inventory2.IsOpen())
				{
						return FALSE;
				}

				if (!quickSlots->HandleInput(Key, false))
				{
						return (this->*Hook_oCGame_HandleEvent)(Key);
				}

				return TRUE;
		}

		auto Hook_oCNpcInventory_HandleEvent = ::Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x0066E390, 0x0069BBC0, 0x006B0520, 0x0070E040)), &oCNpcInventory::HandleEvent_Union);
		int oCNpcInventory::HandleEvent_Union(int Key)
		{
				if (!this->IsActive() || this->owner != player || !player->GetEM()->IsEmpty(TRUE) || player->interactItemCurrentState != -1)
				{
						return FALSE;
				}

				if (!quickSlots->HandleInput(Key, true))
				{
						return (this->*Hook_oCNpcInventory_HandleEvent)(Key);
				}

				return TRUE;
		}

		void Game_LoadEnd()
		{
				quickSlots->Unarchive();
		}

		void Game_Entry()
		{
				//GAME ENTRY
		}

		void Game_Init()
		{
				quickSlots = std::make_unique<QuickSlots>();
		}

		void Game_ApplyOptions()
		{
				//GAME APPLY OPTIONS
		}

		void Game_PreLoop()
		{
				//GAME PRE LOOP
		}

		void Game_Loop()
		{
				quickSlots->Loop();
		}

		void Game_PostLoop()
		{
				//GAME POST LOOP
		}

		void Game_MenuLoop()
		{
				//GAME MENU LOOP
		}

		void Game_Exit()
		{
				quickSlots.release();
		}

		void Game_Pause()
		{
				//GAME PAUSE
		}

		void Game_Unpause()
		{
				//GAME UNPAUSE
		}

		void Game_SaveBegin()
		{
				//GAME SAVE BEGIN
		}

		void Game_SaveEnd()
		{
				quickSlots->Archive();
		}

		void Game_LoadBegin_NewGame()
		{
				//GAME LOAD BEGIN NEW GAME
		}

		void Game_LoadEnd_NewGame()
		{
				Game_LoadEnd();
		}

		void Game_LoadBegin_SaveGame()
		{
				//GAME LOAD BEGIN SAVE GAME
		}

		void Game_LoadEnd_SaveGame()
		{
				Game_LoadEnd();
		}

		void Game_LoadBegin_ChangeLevel()
		{
				//GAME LOAD BEGIN CHANGE LEVEL
		}
		void Game_LoadEnd_ChangeLevel()
		{
				Game_LoadEnd();
		}

		void Game_LoadBegin_Trigger()
		{
				//GAME LOAD BEGIN TRIGGER
		}

		void Game_LoadEnd_Trigger()
		{
				//GAME LOAD END TRIGGER
		}

		void Game_DefineExternals()
		{
				//GAME DEFINE EXTERNALS
		}
}