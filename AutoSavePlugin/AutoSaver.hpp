namespace GOTHIC_NAMESPACE
{
		static constexpr std::string_view PluginName = "AutoSaver";

		class AutoSaver
		{
		public:
				AutoSaver()
				{
						this->ResetTimer();
						this->UpdateSettings();
				}

				~AutoSaver() { }

				bool CanDoWork()
				{
						if (!this->autoSaverEnabled)
						{
								return false;
						}

						if (!ogame
								|| ogame->pause_screen
								|| ogame->load_screen
								|| ogame->save_screen
								|| ogame->singleStep
								|| this->gameSaveLoadBusy
								)
						{
								return false;
						}

						if (!player)
						{
								return false;
						}

						return true;
				}

				bool CanSaveGame()
				{
						if (player->GetAnictrl()->state != zCAIPlayer::zMV_STATE_STAND
								|| player->GetAnictrl()->GetWaterLevel() > 0
								|| player->HasBodyStateModifier(BS_MOD_TRANSFORMED)
								|| player->bodyState == BS_TAKEITEM
								|| player->bodyState == BS_MOBINTERACT
								|| ((player->bodyState & BS_FLAG_INTERRUPTABLE) && !(player->bodyState & BS_FLAG_FREEHANDS))
								|| !oCInformationManager::GetInformationManager().HasFinished()
								|| player->inventory2.IsOpen()
								|| this->IsPlayerInDanger()
								)
						{
								return false;
						}

						static const zSTRING saveMenu = gameMan->menu_save_savegame ? gameMan->menu_save_savegame->name : zSTRING{ "MENU_SAVEGAME_SAVE" };

						for (int i = 0; i < zCMenuItem::itemList.GetNum(); i++)
						{
								auto item = zCMenuItem::itemList[i];
								if (item->m_parOnSelAction_S[0] == saveMenu)
								{
										if ((item->m_parItemFlags & IT_DISABLED) || (item->m_parItemFlags & IT_ONLY_OUT_GAME))
										{
												return false;
										}
								}
						}

						return true;
				}

				void CheckTimer()
				{
						if (!this->CanDoWork())
						{
								return;
						}

						if (!this->CanSaveGame())
						{
								if (this->timerTotalSeconds > this->autoSaverTimeMin)
								{
										this->saveDelay = true;
										this->timerTotalSecondsDelayed = timerTotalSeconds + 3;
								}
								this->saveBlocked = true;
								return;
						}

						this->saveBlocked = false;
						if (this->timerTotalSeconds <= this->autoSaverTimeMin)
						{
								return;
						}

						if (this->timerTotalSecondsDelayed > 0 && this->timerTotalSeconds <= this->timerTotalSecondsDelayed)
						{
								return;
						}

						this->DoSaveGame();
				}

				void DoSaveGame()
				{
						this->ResetTimer();
						this->gameSaveLoadBusy = true;
						auto status = ogame->GetShowPlayerStatus();
						ogame->SetShowPlayerStatus(FALSE);

#if ENGINE == Engine_G1A
						ogame->WriteSavegame(this->autoSaverSlot, true);
#else
						gameMan->Write_Savegame(this->autoSaverSlot);
#endif

						oCSavegameInfo* info = gameMan->savegameManager->GetSavegame(this->autoSaverSlot);
						info->m_Name = zSTRING{ std::format("AutoSave D:{}, {:02}:{:02}", info->m_TimeDay + 1, info->m_TimeHour, info->m_TimeMin).c_str() };
						gameMan->savegameManager->SetAndWriteSavegame(this->autoSaverSlot, info);

						this->gameSaveLoadBusy = false;
						ogame->SetShowPlayerStatus(status);
				}

				void IncreaseTimer()
				{
						if (!this->CanDoWork())
						{
								return;
						}

						this->timerFrame += ztimer->frameTimeFloat / ztimer->factorMotion;
						if (this->timerFrame < 1000.0f)
						{
								return;
						}

						this->timerFrame -= 1000.0f;
						this->timerTotalSeconds += 1;
				}

				bool IsPlayerAttacked()
				{
						static int ZS_ATTACK = parser->GetIndex("ZS_ATTACK");
						static int ZS_MM_ATTACK = parser->GetIndex("ZS_MM_ATTACK");

						if (ZS_ATTACK == -1 || ZS_MM_ATTACK == -1)
						{
								return false;
						}

						ogame->GetHeroStatus();
						static auto enemyList = reinterpret_cast<zCArray<zCVob*>*>(zSwitch(0x008D88A4, 0x0091E8B8, 0x0097E418, 0x00AAD648));
						if (!enemyList && enemyList->IsEmpty())
						{
								return false;
						}

						for (int id = 0; id < enemyList->GetNum(); id++)
						{
								auto npc = static_cast<oCNpc*>(enemyList->GetSafe(id));
								if (!npc)
								{
										continue;
								}

								if (npc->enemy == player && (npc->state.IsInState(ZS_ATTACK) || npc->state.IsInState(ZS_MM_ATTACK)))
								{
										return true;
								}
						}

						return false;
				}

				bool IsPlayerInDanger()
				{

						if (player->IsUnconscious()
								|| player->attribute[NPC_ATR_HITPOINTS] <= 0
								|| zCCSCamera::playing
								|| player->GetWeaponMode() != NPC_WEAPON_NONE
								|| (this->IsPlayerAttacked())
								)
						{
								return true;
						}

						return false;
				}

				void Loop()
				{
						this->IncreaseTimer();
						this->CheckTimer();
						this->PrintIcon();
				}

				void PrintIcon()
				{
						if (!this->CanDoWork())
						{
								return;
						}

						if (!ogame->GetShowPlayerStatus()
								|| !ogame->game_drawall)
						{
								return;
						}

						auto diff = this->autoSaverTimeMin - this->timerTotalSeconds;
						if (diff > 30)
						{
								return;
						}

						int psize = 32;
						int px1 = screen->nax(8192 / 2) - (psize / 2);
						int py1 = screen->nay(8192 / 5) - (psize / 2);
						zSTRING texName = zSTRING{ "ICON_AUTOSAVE" };
						zCTexture* tex = tex->Load(texName, true);
						zrenderer->DrawTile(tex, zVEC2(static_cast<float>(px1), static_cast<float>(py1)), zVEC2(static_cast<float>(px1 + psize), static_cast<float>(py1 + psize)), 1.2f, { 0.0f, 0.0f }, { 1.0f, 1.0f }, this->saveBlocked ? GFX_RED : GFX_GREEN);
						tex->Release();

						if (diff > 0)
						{
								zSTRING msg{ diff };
								screen->Print((8192 / 2) - (screen->FontSize(msg) / 2), screen->any(py1 + psize), msg);
						}
				}

				void ResetTimer()
				{
						timerFrame = 0;
						timerTotalSeconds = 0;
						saveDelay = false;
						timerTotalSecondsDelayed = 0;
				};

				void UpdateSettings()
				{
						this->autoSaverEnabled = zoptions->ReadBool(PluginName.data(), "Enabled", TRUE);
						this->autoSaverSlot = zoptions->ReadInt(PluginName.data(), "Slot", 8);
						int TimeInterval = zoptions->ReadInt(PluginName.data(), "TimeInterval", 5);
						this->autoSaverTimeMin = TimeInterval > 0 ? 60 * TimeInterval : 300;
				}

				bool gameSaveLoadBusy;
				bool worldChange;

		private:
				float timerFrame;
				int timerTotalSeconds;

				bool autoSaverEnabled;
				int autoSaverSlot;
				int autoSaverTimeMin;

				bool saveBlocked;
				bool saveDelay;
				int timerTotalSecondsDelayed;
		};
		std::unique_ptr<AutoSaver> autoSaver;
}