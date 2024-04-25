namespace GOTHIC_NAMESPACE
{
		zCArray<unsigned short> jumpKeys;

		void GetKeyMapping()
		{
				zinput->GetBinding(GAME_SMOVE, jumpKeys);
		}

		bool IsJumpKeyPressed()
		{
				for (int i = 0; i < jumpKeys.GetNum(); i++)
				{
						if (zinput->KeyPressed(jumpKeys[i]))
						{
								return true;
						}
				}

				return false;
		}

		void __fastcall zCInput_Win32_ProcessInputEvents(zCInput_Win32* _this);
		auto Hook_zCInput_Win32_ProcessInputEvents = ::Union::CreateHook(reinterpret_cast<void*>(0x004C8BA0), &zCInput_Win32_ProcessInputEvents);
		void __fastcall zCInput_Win32_ProcessInputEvents(zCInput_Win32* _this)
		{
				Hook_zCInput_Win32_ProcessInputEvents(_this);

				if (!_this)
				{
						return;
				}

				if (jumpKeys.IsEmpty())
				{
						GetKeyMapping();
				}

				int state = static_cast<int>(_this->GetState(GAME_SMOVE));
				if (state != 0 && zinput->GetMouseButtonPressedRight() && !IsJumpKeyPressed())
				{
						_this->SetState(GAME_SMOVE, 0);
				}
		}

		void Game_Entry()
		{
				//GAME ENTRY
		}

		void Game_Init()
		{
				//GAME INIT
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
				//GAME LOOP
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
				//GAME EXIT
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
				//GAME SAVE END
		}

		void Game_LoadBegin_NewGame()
		{
				//GAME LOAD BEGIN NEW GAME
		}

		void Game_LoadEnd_NewGame()
		{
				//GAME LOAD END NEW GAME
		}

		void Game_LoadBegin_SaveGame()
		{
				//GAME LOAD BEGIN SAVE GAME
		}

		void Game_LoadEnd_SaveGame()
		{
				//GAME LOAD END SAVE GAME
		}

		void Game_LoadBegin_ChangeLevel()
		{
				//GAME LOAD BEGIN CHANGE LEVEL
		}
		void Game_LoadEnd_ChangeLevel()
		{
				//GAME LOAD END CHANGE LEVEL
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