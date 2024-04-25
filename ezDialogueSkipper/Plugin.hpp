namespace GOTHIC_NAMESPACE
{
		int skipHotkey = KEY_LSHIFT;

		//int __fastcall CGameManager_HandleEvent(CGameManager* _this, void* vtable, int key);
		auto Hook_CGameManager_HandleEvent = ::Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x004293C0, 0x0042C030, 0x0042AA30, 0x0042AD50)), &CGameManager::HandleEvent_Union);
		int CGameManager::HandleEvent_Union(int Key)
		{
				if (!this || skipHotkey == -1)
				{
						return (this->*Hook_CGameManager_HandleEvent)(Key);
				}

				auto skipHotkeyPressed = zinput->KeyPressed(skipHotkey);

				auto result = (this->*Hook_CGameManager_HandleEvent)(Key);

				if (!result
						|| !skipHotkeyPressed
#if ENGINE <= Engine_G1A
						|| Key != 2052
#else
						|| Key != MOUSE_BUTTONRIGHT
#endif
						|| oCInformationManager::GetInformationManager().HasFinished()
						)
				{
						return result;
				}

				std::vector<oCNpc*> npcs{ player, player->talkOther, player->GetFocusNpc() };
				for (auto npc : npcs)
				{
						if (!npc)
						{
								continue;
						}

						auto& msgList = npc->GetEM()->messageList;
						for (int i = 0; i < msgList.GetNum(); i++)
						{
								auto msg = msgList[i];
								if (!msg)
								{
										continue;
								}

								auto msgConv = zDYNAMIC_CAST<oCMsgConversation>(msg);
								if (!msgConv)
								{
										continue;
								}

								if (msgConv->subType == oCMsgConversation::EV_OUTPUT
										|| msgConv->subType == oCMsgConversation::EV_OUTPUTSVM
										|| msgConv->subType == oCMsgConversation::EV_OUTPUTSVM_OVERLAY
										|| msgConv->subType == oCMsgConversation::EV_PLAYANI_NOOVERLAY)
								{
										msgConv->Delete();
										result++;
								}
						}
				}

				return result;
		}

		void ApplySettings()
		{
				skipHotkey = GetKey(zoptions->ReadString("ezDialogueSkipper", "HotKey", "KEY_LSHIFT").ToChar());
		}

		void Game_Entry()
		{
				//GAME ENTRY
		}

		void Game_Init()
		{
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