namespace GOTHIC_NAMESPACE
{
		int TEXT_FONT_INVENTORY;

		void CleanUp()
		{
				if (player->GetSpellBook())
				{
						player->GetSpellBook()->Close(TRUE);
				}

				if (player->inventory2.IsOpen())
				{
						player->inventory2.Close();
				}

				if (auto container = zDYNAMIC_CAST<oCMobContainer>(player->GetInteractMob()))
				{
						container->Close(player);
						container->Reset();
				}
		}

		void __fastcall oCItemContainer_Draw(::Union::Registers& reg)
		{
				if (player->GetInstanceName() != "PC_HERO")
				{
						return;
				}

#if ENGINE == Engine_G1
				auto item = (oCItem*)(reg.ebx);
				auto textView = *(zCView**)(reg.esi + 0x5C);
#elif ENGINE == Engine_G1A
				auto item = (oCItem*)(reg.esi);
				auto textView = *(zCView**)(reg.ebp + 0x60);
#elif ENGINE == Engine_G2
				auto item = (oCItem*)(reg.ebx);
				auto textView = *(zCView**)(reg.ebp + 0x80);
#elif ENGINE == Engine_G2A
				auto item = (oCItem*)(reg.ebx);
				auto textView = *(zCView**)(reg.ebp + 0x80);
#endif

				if (textView && item && item->HasFlag(ITM_FLAG_ACTIVE))
				{
						auto col = zCOLOR(255, 0, 0);

						auto hotkey = -1;

						if (item->mainflag == ITM_CAT_FF)
						{
								hotkey = 3;
						}
						else if (item->mainflag == ITM_CAT_NF && (item->HasFlag(ITM_FLAG_SWD) || item->HasFlag(ITM_FLAG_AXE)))
						{
								hotkey = 1;
						}
						else if (item->mainflag == ITM_CAT_NF && (item->HasFlag(ITM_FLAG_2HD_SWD) || item->HasFlag(ITM_FLAG_2HD_AXE)))
						{
								hotkey = 2;
						}

						if (hotkey != -1)
						{
								auto fontName = zSTRING{ "FONT_DEFAULT.TGA" };
								if (TEXT_FONT_INVENTORY != -1)
								{
										auto sym = parser->GetSymbol(TEXT_FONT_INVENTORY);
										if (sym)
										{
												sym->GetValue(fontName, 0);
										}
								}
								auto s = zSTRING{ hotkey };
								int px = textView->FontSize(s);
								int py = textView->FontY();
								textView->PrintTimed(px + 100, 4100 - (py / 2), s, -2, &col);
						}
				}

#if ENGINE == Engine_G1
				reg.eip = 0x00667D4B;
#elif ENGINE == Engine_G1A
				reg.eip = 0x00695858;
#elif ENGINE == Engine_G2
				reg.eip = 0x006AA89D;
#elif ENGINE == Engine_G2A
				reg.eip = 0x0070838D;
#endif
		}
		auto PartialHook_oCItemContainer_Draw = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x00667B97, 0x006956AA, 0x006AA54D, 0x0070803D)), &oCItemContainer_Draw);

		auto Hook_oCNpc_EquipWeapon = ::Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x00696C20, 0x006C8770, 0x006DB870, 0x0073A030)), &oCNpc::EquipWeapon_Union);
		void oCNpc::EquipWeapon_Union(oCItem* item)
		{
				if (this != player || this->GetInstanceName() != "PC_HERO")
				{
						(this->*Hook_oCNpc_EquipWeapon)(item);
						return;
				}

				if (item->HasFlag(ITM_FLAG_ACTIVE))
				{
						this->UnequipItem(item);
						return;
				}

				if (!this->CanUse(item)) {
						this->DisplayCannotUse();
						return;
				}

				if ((item->HasFlag(ITM_FLAG_2HD_SWD)) || (item->HasFlag(ITM_FLAG_2HD_AXE))) {
						this->UnequipItem(this->GetSlotItem(NPC_NODE_LONGSWORD));
				}
				else if ((item->HasFlag(ITM_CAT_NF)) && (!item->HasFlag(ITM_FLAG_SHIELD))) {
						this->UnequipItem(this->GetSlotItem(NPC_NODE_SWORD));
				}
				else if (item->HasFlag(ITM_FLAG_SHIELD)) {
						this->UnequipItem(this->GetSlotItem(NPC_NODE_SHIELD));
				}
				this->EquipItem(item);
		}

		auto Hook_oCNpc_GetNextWeaponMode = ::Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x00696640, 0x006C81B0, 0x006DB2F0, 0x00739A30)), &oCNpc::GetNextWeaponMode_Union);
		int oCNpc::GetNextWeaponMode_Union(int from, int direction, int ignoreCond)
		{
				if (this != player || this->GetInstanceName() != "PC_HERO")
				{
						return (this->*Hook_oCNpc_GetNextWeaponMode)(from, direction, ignoreCond);
				}

				oCItem* w1h = this->GetSlotItem(NPC_NODE_SWORD);
				oCItem* w2h = this->GetSlotItem(NPC_NODE_LONGSWORD);
				oCItem* wranged = this->GetEquippedRangedWeapon();

				int check = from;

				while (TRUE)
				{
						check += direction;
						if (check > NPC_WEAPON_MAG)
						{
								check = NPC_WEAPON_FIST;
						}
						if (check <= NPC_WEAPON_NONE)
						{
								check = NPC_WEAPON_MAG;
						}

						switch (check)
						{
						case NPC_WEAPON_FIST:
								if (ignoreCond)
								{
										return check;
								}
								else if (!w1h && !w2h)
								{
										return check;
								}
								break;
						case NPC_WEAPON_DAG:
								if (w1h && w1h->HasFlag(ITM_FLAG_DAG))
								{
										return check;
								}
								break;
						case NPC_WEAPON_1HS:
								if (w1h && (w1h->HasFlag(ITM_FLAG_AXE) || w1h->HasFlag(ITM_FLAG_SWD)))
								{
										return check;
								}
								break;
						case NPC_WEAPON_2HS:
								if (w2h && (w2h->HasFlag(ITM_FLAG_2HD_AXE) || w2h->HasFlag(ITM_FLAG_2HD_SWD)))
								{
										return check;
								}
								break;
						case NPC_WEAPON_BOW:
								if (wranged && wranged->HasFlag(ITM_FLAG_BOW) && this->IsMunitionAvailable(wranged))
								{
										return check;
								}
								break;
						case NPC_WEAPON_CBOW:
								if (wranged && wranged->HasFlag(ITM_FLAG_CROSSBOW) && this->IsMunitionAvailable(wranged))
								{
										return check;
								}
								break;
						case NPC_WEAPON_MAG:
								if (this->GetNumberOfSpells() > 0)
								{
										return check;
								}
								break;
						default:
								break;
						}
				}

				return -1;
		}

		auto Hook_oCGame_HandleEvent = ::Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x0065EEE0, 0x0068A300, 0x0069E980, 0x006FC170)), &oCGame::HandleEvent_Union);
		int oCGame::HandleEvent_Union(int Key)
		{
				if (this->GetWorld()->csPlayer->GetPlayingGlobalCutscene() || !player || player->inventory2.IsOpen())
				{
						return FALSE;
				}

				if (player->GetInstanceName() != "PC_HERO")
				{
						return (this->*Hook_oCGame_HandleEvent)(Key);
				}

				auto weaponMode = player->GetWeaponMode();

				switch (Key)
				{
				case KEY_GRAVE:
				{
						if (!player->CanDrawWeapon())
						{
								break;
						}

						CleanUp();

						if (weaponMode != NPC_WEAPON_NONE)
						{
								player->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0), player);

								if (weaponMode == NPC_WEAPON_FIST)
								{
										break;
								}
						}

						player->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_DRAWWEAPON, NPC_WEAPON_FIST, 1), player);
				}
				break;

				case KEY_1:
				{
						if (!player->CanDrawWeapon())
						{
								break;
						}

						CleanUp();

						if (weaponMode != NPC_WEAPON_NONE)
						{
								player->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0), player);

								if (weaponMode == NPC_WEAPON_1HS || weaponMode == NPC_WEAPON_DAG)
								{
										break;
								}
						}

						auto w1h = player->GetSlotItem(NPC_NODE_SWORD);
						if (w1h)
						{
								player->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_DRAWWEAPON, NPC_WEAPON_DAG, 0), player);
						}
				}
				break;

				case KEY_2:
				{
						if (!player->CanDrawWeapon())
						{
								break;
						}

						CleanUp();

						if (weaponMode != NPC_WEAPON_NONE)
						{
								player->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0), player);

								if (weaponMode == NPC_WEAPON_2HS)
								{
										break;
								}
						}

						auto w2h = player->GetSlotItem(NPC_NODE_LONGSWORD);
						if (w2h)
						{
								player->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_DRAWWEAPON, NPC_WEAPON_2HS, 0), player);
						}
				}
				break;

				case KEY_3:
				{
						if (!player->CanDrawWeapon())
						{
								break;
						}

						CleanUp();

						if (weaponMode != NPC_WEAPON_NONE)
						{
								player->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0), player);

								if (weaponMode == NPC_WEAPON_BOW || weaponMode == NPC_WEAPON_CBOW)
								{
										break;
								}
						}

						oCItem* wranged = player->GetEquippedRangedWeapon();
						if (wranged && player->IsMunitionAvailable(wranged))
						{
								player->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_DRAWWEAPON, NPC_WEAPON_BOW, 0), player);
						}
				}
				break;

				default:
						return (this->*Hook_oCGame_HandleEvent)(Key);
				}

				return TRUE;
		}

		void Game_Entry()
		{
				//GAME ENTRY
		}

		void Game_Init()
		{
				TEXT_FONT_INVENTORY = parser->GetIndex("TEXT_FONT_INVENTORY");
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