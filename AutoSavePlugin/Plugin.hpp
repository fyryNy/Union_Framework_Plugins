namespace GOTHIC_NAMESPACE
{
		void ApplySettings()
		{
				autoSaver->UpdateSettings();
		}

		void Game_Entry()
		{
				//GAME ENTRY
		}

		void Game_Init()
		{
				autoSaver = std::make_unique<AutoSaver>();
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
				autoSaver->Loop();
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
				autoSaver.release();
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
				autoSaver->gameSaveLoadBusy = true;
		}

		void Game_SaveEnd()
		{
				autoSaver->gameSaveLoadBusy = false;
				autoSaver->gameSaveLoadBusy = false;
		}

		void Game_LoadBegin_NewGame()
		{
				autoSaver->gameSaveLoadBusy = true;
		}

		void Game_LoadEnd_NewGame()
		{
				autoSaver->ResetTimer();
				autoSaver->gameSaveLoadBusy = false;
		}

		void Game_LoadBegin_SaveGame()
		{
				autoSaver->gameSaveLoadBusy = true;
		}

		void Game_LoadEnd_SaveGame()
		{
				autoSaver->ResetTimer();
				autoSaver->gameSaveLoadBusy = false;
		}

		void Game_LoadBegin_ChangeLevel()
		{
				autoSaver->gameSaveLoadBusy = true;
		}
		void Game_LoadEnd_ChangeLevel()
		{
				autoSaver->gameSaveLoadBusy = false;
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