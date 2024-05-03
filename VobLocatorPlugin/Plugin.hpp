namespace GOTHIC_NAMESPACE
{
		void ApplySettings()
		{
				vobLocator->UpdateSettings();
		}

		void LoadEnd()
		{
				vobLocator->GetTraders();
				vobLocator->GetPickPockets();
		}

		void Game_Entry()
		{
				//GAME ENTRY
		}

		void Game_Init()
		{
				vobLocator = std::make_unique<VobLocator>();
				ApplySettings();
		}

		void Game_ApplyOptions()
		{
				ApplySettings();
		}

		void Game_PreLoop()
		{
				//GAME PRE LOOP
		}

		void Game_Loop()
		{
				vobLocator->Loop();
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
				vobLocator.release();
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
				LoadEnd();
		}

		void Game_LoadBegin_SaveGame()
		{
				//GAME LOAD BEGIN SAVE GAME
		}

		void Game_LoadEnd_SaveGame()
		{
				LoadEnd();
		}

		void Game_LoadBegin_ChangeLevel()
		{
				//GAME LOAD BEGIN CHANGE LEVEL
		}
		void Game_LoadEnd_ChangeLevel()
		{
				LoadEnd();
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