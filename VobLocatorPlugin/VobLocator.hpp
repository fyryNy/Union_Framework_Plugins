namespace GOTHIC_NAMESPACE
{
		static constexpr std::string_view PluginName = "VobLocator";

		struct ScreenPos
		{
				int X;
				int Y;

				ScreenPos(int X = 0, int Y = 0) : X(X), Y(Y) {}
		};

		enum class Icons : int
		{
				ICON_NONE,
				ICON_1H,
				ICON_2H,
				ICON_AMULET,
				ICON_ARMOR,
				ICON_BOW,
				ICON_CHEST,
				ICON_CORPSE,
				ICON_CROSSBOW,
				ICON_DOC,
				ICON_DRINK,
				ICON_FOOD,
				ICON_FOOD2,
				ICON_GOLD,
				ICON_HELMET,
				ICON_KEY,
				ICON_LEVER,
				ICON_LOCKPICK,
				ICON_MAP,
				ICON_MINING,
				ICON_MISC,
				ICON_MONSTER_HOSTILE,
				ICON_MONSTER_NEUTRAL,
				ICON_NPC_HOSTILE,
				ICON_NPC_PEACE,
				ICON_PLANT,
				ICON_POTION,
				ICON_QUIVER,
				ICON_RING,
				ICON_RUNE,
				ICON_SCROLL,
				ICON_SHIELD,
				ICON_STONEPLATE,
				ICON_TELEPORT,
				ICON_TORCH,
				ICON_TREASURE,
				ICON_TROPHY,
				ICON_UNKNOWN
		};

		enum class AdditionalIcons : int
		{
				ICON_QUEST,
				ICON_TRADER,
				ICON_PICKPOCKET
		};

		class VobLocator
		{
		public:
				VobLocator()
				{
						this->indexCanStealNpcAST = parser->GetIndex("RX_CanStealNpcAST");

						this->UpdateSettings();
				}

				~VobLocator() {}

				void Loop()
				{
						if (!this->CanBeActive())
						{
								return;
						}

						this->HandleInput();

						if (!this->VobLocatorActive || !ogame->game_drawall || !ogame->GetShowPlayerStatus())
						{
								return;
						}

						this->RestoreViewport();
						this->PrintText();
						this->SearchVobs();
				}

				void UpdateSettings()
				{
						this->ShowText = zoptions->ReadBool(PluginName.data(), "ShowText", true);
						this->Text = zoptions->ReadString(PluginName.data(), "Text", "VobLocator ON");
						this->IconSize = zoptions->ReadInt(PluginName.data(), "IconSize", 25);
						this->AdditionalIconSize = (this->IconSize / 3) * 2;
						this->HotKey = GetKey(zoptions->ReadString(PluginName.data(), "HotKey", "KEY_LBRACKET").ToChar());
						this->MaxDistance = zoptions->ReadInt(PluginName.data(), "MaxDistance", -1);

						this->ShowChests = zoptions->ReadBool(PluginName.data(), "ShowChests", true);
						this->ShowCorpses = zoptions->ReadBool(PluginName.data(), "ShowCorpses", true);

						this->ShowHostileNpcs = zoptions->ReadBool(PluginName.data(), "ShowHostileNpcs", true);
						this->ShowNeutralNpcs = zoptions->ReadBool(PluginName.data(), "ShowNeutralNpcs", true);
						this->ShowTraders = zoptions->ReadBool(PluginName.data(), "ShowTraders", true);
						this->CheckTraderCondition = zoptions->ReadBool(PluginName.data(), "CheckTraderCondition", true);
						this->ShowPickPockets = zoptions->ReadBool(PluginName.data(), "ShowPickPockets", true);
						this->ShowAmbientPickPockets = zoptions->ReadBool(PluginName.data(), "ShowAmbientPickPockets", false);

						this->ShowArmors = zoptions->ReadBool(PluginName.data(), "ShowArmors", true);
						this->ShowDocuments = zoptions->ReadBool(PluginName.data(), "ShowDocuments", true);
						this->ShowFood = zoptions->ReadBool(PluginName.data(), "ShowFood", true);
						this->ShowMagicItems = zoptions->ReadBool(PluginName.data(), "ShowMagicItems", true);
						this->ShowMeleeWeapons = zoptions->ReadBool(PluginName.data(), "ShowMeleeWeapons", true);
						this->ShowMiscItems = zoptions->ReadBool(PluginName.data(), "ShowMiscItems", true);
						this->ShowMunition = zoptions->ReadBool(PluginName.data(), "ShowMunition", true);
						this->ShowOres = zoptions->ReadBool(PluginName.data(), "ShowOres", true);
						this->ShowPlants = zoptions->ReadBool(PluginName.data(), "ShowPlants", true);
						this->ShowPotions = zoptions->ReadBool(PluginName.data(), "ShowPotions", true);
						this->ShowRangedWeapons = zoptions->ReadBool(PluginName.data(), "ShowRangedWeapons", true);
						this->ShowSpells = zoptions->ReadBool(PluginName.data(), "ShowSpells", true);
						this->ShowStoneplates = zoptions->ReadBool(PluginName.data(), "ShowStoneplates", true);
						this->ShowSwitches = zoptions->ReadBool(PluginName.data(), "ShowSwitches", true);
						this->ShowTreasures = zoptions->ReadBool(PluginName.data(), "ShowTreasures", true);
						this->ShowTrophies = zoptions->ReadBool(PluginName.data(), "ShowTrophies", true);
				}

				void GetTraders()
				{
						this->traderInfos.clear();

						auto list = ogame->GetInfoManager()->infoList.next;
						while (list) {
								auto info = list->data;
								list = list->next;

								if (!info->pd.trade)
								{
										continue;
								}

								traderInfos.push_back(info);
						}
				}

				void GetPickPockets()
				{
						this->pickpocketInfos.clear();

						auto list = ogame->GetInfoManager()->infoList.next;
						while (list) {
								auto info = list->data;
								list = list->next;

								auto infoName = ::Union::StringANSI(info->name).MakeUpper();
								if (!infoName.Contains("PICKPOCKET") && !infoName.Contains("_STEAL") && !infoName.Contains("PICKME"))
								{
										continue;
								}

								if (infoName.Contains("_DOIT") || infoName.Contains("_TRY"))
								{
										continue;
								}

								if (parser->GetIndex(info->name + "_DOIT") == -1 && parser->GetIndex(info->name + "_TRY") == -1)
								{
										continue;
								}

								pickpocketInfos.push_back(info);
						}
				}

		private:
				bool CanBeActive()
				{
						if (!ogame || !player)
						{
								return false;
						}

						if (ogame->singleStep
								|| ogame->pause_screen
								|| ogame->load_screen
								|| ogame->save_screen
								|| ogame->singleStep
								|| player->attribute[NPC_ATR_HITPOINTS] <= 0
								|| zcon->IsVisible()
								|| player->inventory2.IsOpen()
								|| !ogame->GetShowPlayerStatus()
								|| !ogame->game_drawall
								)
						{
								return false;
						}

						return true;
				}

				void HandleInput()
				{
						if (zinput->KeyToggled(this->HotKey))
						{
								this->VobLocatorActive = !this->VobLocatorActive;
						}
				}

				void PrintIcon(ScreenPos screenPos, Icons icon, std::vector<AdditionalIcons> additionals)
				{
						auto iconTopLeft = zVEC2(static_cast<float>(screenPos.X), static_cast<float>(screenPos.Y));
						auto iconBottomRight = zVEC2(static_cast<float>(screenPos.X + this->IconSize), static_cast<float>(screenPos.Y + this->IconSize));
						auto iconTopRight = zVEC2(static_cast<float>(screenPos.X + this->IconSize), static_cast<float>(screenPos.Y));

						auto iconTexName = this->GetIconName(icon);
						zCTexture* iconTex = iconTex->Load(iconTexName, true);
						zrenderer->DrawTile(iconTex, iconTopLeft, iconBottomRight, zSwitch(0.9f, 0.9f, 1.1f, 1.1f), zVEC2(0.0f, 0.0f), zVEC2(1.0f, 1.0f), GFX_WHITE);
						iconTex->Release();

						iconTopRight[VX] = iconTopRight[VX] - ((this->AdditionalIconSize * additionals.size()) / 2);
						auto additionalIconSizeHalf = zVEC2(static_cast<float>(this->AdditionalIconSize / 2), static_cast<float>(this->AdditionalIconSize / 2));
						for (auto additional : additionals)
						{
								auto additionalTopLeft = iconTopRight - additionalIconSizeHalf;
								auto additionalBottomRight = iconTopRight + additionalIconSizeHalf;

								auto additionalTexName = this->GetAdditionalIconName(additional);
								zCTexture* additionalTex = additionalTex->Load(additionalTexName, true);
								zrenderer->DrawTile(additionalTex, additionalTopLeft, additionalBottomRight, zSwitch(0.8f, 0.8f, 1.0f, 1.0f), zVEC2(0.0f, 0.0f), zVEC2(1.0f, 1.0f), GFX_WHITE);
								additionalTex->Release();

								iconTopRight[VX] = iconTopRight[VX] + this->AdditionalIconSize;
						}
				}

				void PrintText()
				{
						if (!this->ShowText)
						{
								return;
						}

						auto prevColor = screen->GetColor();

						int x = 8192 - screen->FontSize(this->Text) - 200;
						int y = (8192 / screen->FontY()) * 5;

						screen->SetFontColor(GFX_GREEN);
						screen->Print(x, y, this->Text);
						screen->SetFontColor(prevColor);
				}

				void SearchVobs()
				{
						int maxDist = this->MaxDistance;
						if (this->MaxDistance == -1)
						{
								maxDist = static_cast<int>(oCSpawnManager::GetRemoveRange());
						}

						zCArray<zCVob*> listVobs;
						player->CreateVobList(listVobs, static_cast<float>(maxDist));

						size_t listVobsMax = static_cast<size_t>(listVobs.GetNumInList());
						for (size_t i = 0; i < listVobsMax; i++)
						{
								auto vob = listVobs[i];

								if (!vob->IsValidVob_Union())
								{
										continue;
								}

								int vobDistance = static_cast<int>(player->GetDistanceToVob(*vob));
								if (vobDistance > maxDist)
								{
										continue;
								}

								auto vobPos = vob->bbox3D.GetCenter();

								if (vob->GetVobType() == zVOB_TYPE_NSC)
								{
										vobPos[VY] = vob->bbox3D.maxs[VY] + 25.0f;
								}

								ScreenPos screenPos;
								if (!this->World2Screen(vobPos, screenPos))
								{
										continue;
								}

								screenPos.X -= (this->IconSize / 2);
								screenPos.Y -= (this->IconSize / 2);

								Icons icon = this->GetIcon(vob);
								std::vector<AdditionalIcons> additionals = this->GetAdditionalIcon(vob);

								auto isTrader = std::find(additionals.begin(), additionals.end(), AdditionalIcons::ICON_TRADER) != additionals.end();
								auto isPickPocketable = std::find(additionals.begin(), additionals.end(), AdditionalIcons::ICON_PICKPOCKET) != additionals.end();

								switch (icon)
								{
								case Icons::ICON_UNKNOWN:
										break;
								case Icons::ICON_1H:
								case Icons::ICON_2H:
										if (this->ShowMeleeWeapons)
										{
												break;
										}
										continue;
								case Icons::ICON_BOW:
								case Icons::ICON_CROSSBOW:
										if (this->ShowRangedWeapons)
										{
												break;
										}
										continue;
								case Icons::ICON_QUIVER:
										if (this->ShowMunition)
										{
												break;
										}
										continue;
								case Icons::ICON_HELMET:
								case Icons::ICON_ARMOR:
								case Icons::ICON_SHIELD:
										if (this->ShowArmors)
										{
												break;
										}
										continue;
								case Icons::ICON_CHEST:
										if (this->ShowChests)
										{
												break;
										}
										continue;
								case Icons::ICON_CORPSE:
										if (this->ShowCorpses)
										{
												break;
										}
										continue;
								case Icons::ICON_MAP:
								case Icons::ICON_DOC:
										if (this->ShowDocuments)
										{
												break;
										}
										continue;
								case Icons::ICON_DRINK:
								case Icons::ICON_FOOD:
								case Icons::ICON_FOOD2:
										if (this->ShowFood)
										{
												break;
										}
										continue;
								case Icons::ICON_GOLD:
								case Icons::ICON_KEY:
								case Icons::ICON_LOCKPICK:
								case Icons::ICON_TORCH:
								case Icons::ICON_MISC:
										if (this->ShowMiscItems)
										{
												break;
										}
										continue;
								case Icons::ICON_MONSTER_HOSTILE:
								case Icons::ICON_NPC_HOSTILE:
										if (this->ShowHostileNpcs || (this->ShowTraders && isTrader) || (this->ShowPickPockets && isPickPocketable))
										{
												break;
										}
										continue;
								case Icons::ICON_NPC_PEACE:
								case Icons::ICON_MONSTER_NEUTRAL:
										if (this->ShowNeutralNpcs || (this->ShowTraders && isTrader) || (this->ShowPickPockets && isPickPocketable))
										{
												break;
										}
										continue;
								case Icons::ICON_PLANT:
										if (this->ShowPlants)
										{
												break;
										}
										continue;
								case Icons::ICON_POTION:
										if (this->ShowPotions)
										{
												break;
										}
										continue;
								case Icons::ICON_AMULET:
								case Icons::ICON_RING:
										if (this->ShowMagicItems)
										{
												break;
										}
										continue;
								case Icons::ICON_RUNE:
								case Icons::ICON_SCROLL:
										if (this->ShowSpells)
										{
												break;
										}
										continue;
								case Icons::ICON_STONEPLATE:
										if (this->ShowStoneplates)
										{
												break;
										}
										continue;
								case Icons::ICON_LEVER:
								case Icons::ICON_TELEPORT:
										if (this->ShowSwitches)
										{
												break;
										}
										continue;
								case Icons::ICON_MINING:
										if (this->ShowOres)
										{
												break;
										}
										continue;
								case Icons::ICON_TREASURE:
										if (this->ShowTreasures)
										{
												break;
										}
										continue;
								case Icons::ICON_TROPHY:
										if (this->ShowTrophies)
										{
												break;
										}
										continue;
								default:
										continue;
								}

								this->PrintIcon(screenPos, icon, additionals);
						}
				}

				void RestoreViewport()
				{
						int ScreenX, ScreenY, ScreenSX, ScreenSY;
						screen->GetViewport(ScreenX, ScreenY, ScreenSX, ScreenSY);
						zrenderer->SetViewport(ScreenX, ScreenY, ScreenSX, ScreenSY);
				}

				bool World2Screen(const zVEC3& worldPos, ScreenPos& screenPos)
				{
						zCCamera* cam = ogame->GetCamera();

						zVEC3 trafoPos = cam->GetTransform(zCAM_TRAFO_VIEW) * worldPos;
						if (trafoPos[VZ] < cam->nearClipZ)
						{
								return false;
						}

						int x, y;
						cam->Project(&trafoPos, x, y);

						if (x < 0 || x > 8192 || y < 0 || y > 8192)
						{
								return false;
						}

						screenPos.X = x;
						screenPos.Y = y;

						return true;
				}

				zSTRING GetIconName(Icons icon)
				{
						switch (icon)
						{
						case Icons::ICON_1H:
								return "ICON_1H.TGA";
						case Icons::ICON_2H:
								return "ICON_2H.TGA";
						case Icons::ICON_AMULET:
								return "ICON_AMULET.TGA";
						case Icons::ICON_ARMOR:
								return "ICON_ARMOR.TGA";
						case Icons::ICON_BOW:
								return "ICON_BOW.TGA";
						case Icons::ICON_CHEST:
								return "ICON_CHEST.TGA";
						case Icons::ICON_CORPSE:
								return "ICON_CORPSE.TGA";
						case Icons::ICON_CROSSBOW:
								return "ICON_CROSSBOW.TGA";
						case Icons::ICON_DOC:
								return "ICON_DOC.TGA";
						case Icons::ICON_DRINK:
								return "ICON_DRINK.TGA";
						case Icons::ICON_FOOD:
								return "ICON_FOOD.TGA";
						case Icons::ICON_FOOD2:
								return "ICON_FOOD2.TGA";
						case Icons::ICON_GOLD:
								return "ICON_GOLD.TGA";
						case Icons::ICON_HELMET:
								return "ICON_HELMET.TGA";
						case Icons::ICON_KEY:
								return "ICON_KEY.TGA";
						case Icons::ICON_LEVER:
								return "ICON_LEVER.TGA";
						case Icons::ICON_LOCKPICK:
								return "ICON_LOCKPICK.TGA";
						case Icons::ICON_MAP:
								return "ICON_MAP.TGA";
						case Icons::ICON_MINING:
								return "ICON_MINING.TGA";
						case Icons::ICON_MISC:
								return "ICON_MISC.TGA";
						case Icons::ICON_MONSTER_HOSTILE:
								return "ICON_MONSTER_HOSTILE.TGA";
						case Icons::ICON_MONSTER_NEUTRAL:
								return "ICON_MONSTER_NEUTRAL.TGA";
						case Icons::ICON_NPC_HOSTILE:
								return "ICON_NPC_HOSTILE.TGA";
						case Icons::ICON_NPC_PEACE:
								return "ICON_NPC_PEACE.TGA";
						case Icons::ICON_PLANT:
								return "ICON_PLANT.TGA";
						case Icons::ICON_POTION:
								return "ICON_POTION.TGA";
						case Icons::ICON_QUIVER:
								return "ICON_QUIVER.TGA";
						case Icons::ICON_RING:
								return "ICON_RING.TGA";
						case Icons::ICON_RUNE:
								return "ICON_RUNE.TGA";
						case Icons::ICON_SCROLL:
								return "ICON_SCROLL.TGA";
						case Icons::ICON_SHIELD:
								return "ICON_SHIELD.TGA";
						case Icons::ICON_STONEPLATE:
								return "ICON_STONEPLATE.TGA";
						case Icons::ICON_TELEPORT:
								return "ICON_TELEPORT.TGA";
						case Icons::ICON_TORCH:
								return "ICON_TORCH.TGA";
						case Icons::ICON_TREASURE:
								return "ICON_TREASURE.TGA";
						case Icons::ICON_TROPHY:
								return "ICON_TROPHY.TGA";
						case Icons::ICON_UNKNOWN:
								return "ICON_UNKNOWN.TGA";
						default:
								return "";
						}
				}

				zSTRING GetAdditionalIconName(AdditionalIcons additional)
				{
						switch (additional)
						{
						case AdditionalIcons::ICON_PICKPOCKET:
								return "ICON_PICKPOCKET.TGA";
						case AdditionalIcons::ICON_TRADER:
								return "ICON_TRADER.TGA";
						case AdditionalIcons::ICON_QUEST:
								return "ICON_QUEST.TGA";
						default:
								return "";
						}
				}

				Icons GetIcon(zCVob* vob)
				{
						auto vobType = vob->GetVobType();

						if (vobType == zVOB_TYPE_ITEM)
						{
								auto item = static_cast<oCItem*>(vob);

								auto itemInstanceName = ::Union::StringANSI(item->GetInstanceName()).MakeUpper();
								auto itemVisualName = ::Union::StringANSI(item->visual->GetVisualName()).MakeUpper();

								if (item->HasFlag(ITM_FLAG_TORCH) || itemInstanceName.StartsWith("ITLSTORCH"))
								{
										return Icons::ICON_TORCH;
								}
								else if (itemInstanceName.Contains("STONEPLATE") || itemVisualName.Contains("_STONEPLATE_"))
								{
										return Icons::ICON_STONEPLATE;
								}
								else if (item->HasFlag(ITM_FLAG_SHIELD))
								{
										return Icons::ICON_SHIELD;
								}
								else if (item->mainflag & ITM_CAT_ARMOR)
								{
										if (item->wear == ITM_WEAR_HEAD)
										{
												return Icons::ICON_HELMET;
										}

										return Icons::ICON_ARMOR;
								}
								else if (item->mainflag & ITM_CAT_FOOD)
								{
										auto scemeName = item->scemeName;
										scemeName.Upper();
										if (scemeName == "POTIONFAST" || scemeName == "POTION")
										{
												return Icons::ICON_DRINK;
										}
										else if (item->scemeName == "RICE")
										{
												return Icons::ICON_FOOD2;
										}
										else if (itemInstanceName.StartsWith("ITPL") || itemInstanceName.Contains("PLANT") || itemInstanceName.Contains("HERB") || itemInstanceName.Contains("MUSHROOM"))
										{
												return Icons::ICON_PLANT;
										}

										return Icons::ICON_FOOD;
								}
								else if (item->mainflag & ITM_CAT_DOCS)
								{
										if (itemInstanceName.StartsWith("ITWR") && itemInstanceName.Contains("MAP"))
										{
												return Icons::ICON_MAP;
										}

										return Icons::ICON_DOC;
								}
								else if (item->mainflag & ITM_CAT_NF)
								{
										if (item->HasFlag(ITM_FLAG_2HD_AXE) || item->HasFlag(ITM_FLAG_2HD_SWD))
										{
												return Icons::ICON_2H;
										}

										return Icons::ICON_1H;
								}
								else if (item->mainflag & ITM_CAT_FF)
								{
										if (item->HasFlag(ITM_FLAG_CROSSBOW))
										{
												return Icons::ICON_CROSSBOW;
										}
										else if (item->HasFlag(ITM_FLAG_BOW))
										{
												return Icons::ICON_BOW;
										}

										return Icons::ICON_QUIVER;
								}
								else if (item->mainflag & ITM_CAT_MUN)
								{
										return Icons::ICON_QUIVER;
								}
								else if (item->mainflag & ITM_CAT_POTION)
								{
										return Icons::ICON_POTION;
								}
								else if (item->mainflag & ITM_CAT_MAGIC && item->flags & ITM_FLAG_RING)
								{
										return Icons::ICON_RING;
								}
								else if (item->mainflag & ITM_CAT_MAGIC && item->flags & ITM_FLAG_AMULET)
								{
										return Icons::ICON_AMULET;
								}
								else if (item->mainflag & ITM_CAT_RUNE)
								{
										if (itemInstanceName.StartsWith("ITRU"))
										{
												return Icons::ICON_RUNE;
										}

										return Icons::ICON_SCROLL;
								}
								else if (item->mainflag & ITM_CAT_NONE)
								{
										if (itemInstanceName.Contains("LOCKPICK"))
										{
												return Icons::ICON_LOCKPICK;
										}
										else if (itemInstanceName == "ITMI_GOLD")
										{
												return Icons::ICON_GOLD;
										}
										else if (itemInstanceName.StartsWith("ITKE"))
										{
												return Icons::ICON_KEY;
										}
										else if (itemInstanceName.StartsWith("ITAT"))
										{
												return Icons::ICON_TROPHY;
										}

										return Icons::ICON_MISC;
								}
								else
								{
										return Icons::ICON_UNKNOWN;
								}
						}
						else if (vobType == zVOB_TYPE_NSC)
						{
								auto npc = static_cast<oCNpc*>(vob);

								if ((npc->attribute[NPC_ATR_HITPOINTS] <= 0 || npc->IsUnconscious()) && npc->CanBeLooted_Union())
								{
										return Icons::ICON_CORPSE;
								}
								else if (npc->attribute[NPC_ATR_HITPOINTS] > 0)
								{
										if (npc->guild > NPC_GIL_HUMANS)
										{
												if (npc->attitude == NPC_ATT_HOSTILE || npc->attitude == NPC_ATT_ANGRY)
												{
														return Icons::ICON_MONSTER_HOSTILE;
												}
												else
												{
														return Icons::ICON_MONSTER_NEUTRAL;
												}
										}
										else
										{
												if (npc->attitude == NPC_ATT_HOSTILE)
												{
														return Icons::ICON_NPC_HOSTILE;
												}
												else
												{
														return Icons::ICON_NPC_PEACE;
												}
										}
								}
						}
						else if (vobType == zVOB_TYPE_MOB)
						{
								if (auto container = zDYNAMIC_CAST<oCMobContainer>(vob); container && !container->IsEmpty_Union())
								{
										return Icons::ICON_CHEST;
								}
								else if (auto inter = zDYNAMIC_CAST<oCMobInter>(vob))
								{
										auto swtch = zDYNAMIC_CAST<oCMobSwitch>(vob);
										auto wheel = zDYNAMIC_CAST<oCMobWheel>(vob);
										auto funcName = ::Union::StringANSI(inter->onStateFuncName).MakeUpper();
										auto objName = ::Union::StringANSI(inter->objectName);

										if ((swtch || wheel))
										{
												if (funcName.StartsWith("RX_RUNESTONE_") || funcName.StartsWith("EVT_TP_"))
												{
														return Icons::ICON_TELEPORT;
												}
												else
												{
														return Icons::ICON_LEVER;
												}
										}
										else if ((funcName == "IRONHACKEN" ||
												funcName == "OREHACKEN" ||
												funcName == "GOLDHACKEN") &&
												!objName.IsEmpty() && objName.IsDigit()
												)
										{
												auto num = objName.ConvertToInt();

												if (num > 0)
												{
														return Icons::ICON_MINING;
												}
										}
										else if (funcName.StartsWith("MINING") || funcName == "GOLDHACKEN_ARAXOSMINE" || funcName.Contains("HACKEN"))
										{
												return Icons::ICON_MINING;
										}
										else if (funcName.StartsWith("HIDDENFIND") || funcName == "B_SCGETTREASURE")
										{
												return Icons::ICON_TREASURE;
										}
								}
						}

						return Icons::ICON_NONE;
				}

				std::vector<AdditionalIcons> GetAdditionalIcon(zCVob* vob)
				{
						std::vector<AdditionalIcons> additionals = { };
						auto vobType = vob->GetVobType();

						if (vobType == zVOB_TYPE_ITEM)
						{
								auto item = static_cast<oCItem*>(vob);

								if (item->flags & ITM_FLAG_MI)
								{
										//additionals.push_back(AdditionalIcons::ICON_QUEST);
								}
						}
						else if (vobType == zVOB_TYPE_NSC)
						{
								auto npc = static_cast<oCNpc*>(vob);

								if (this->ShowPickPockets && this->CanBePickPocketed(npc))
								{
										additionals.push_back(AdditionalIcons::ICON_PICKPOCKET);
								}
								if (this->ShowTraders && this->CanTrade(npc))
								{
										additionals.push_back(AdditionalIcons::ICON_TRADER);
								}
						}

						return additionals;
				}

				bool CanTrade(oCNpc* npc)
				{
						if (npc->attribute[NPC_ATR_HITPOINTS] <= 0)
						{
								return false;
						}

						for (auto info : this->traderInfos)
						{
								if (info->GetNpcID() != npc->GetInstance())
								{
										continue;
								}

								if (!this->CheckTraderCondition)
								{
										return true;
								}

								parser->SetInstance("SELF", npc);
								parser->SetInstance("OTHER", player);

								if (info->InfoConditions())
								{
										return true;
								}
						}

						return false;
				}

				bool CanBePickPocketed(oCNpc* npc)
				{
						if (npc->attribute[NPC_ATR_HITPOINTS] <= 0)
						{
								return false;
						}

						for (auto info : this->pickpocketInfos) {
								if (info->GetNpcID() != npc->GetInstance())
								{
										continue;
								}

								parser->SetInstance("SELF", npc);
								parser->SetInstance("OTHER", player);

								if (info->InfoConditions())
								{
										return true;
								}
						}

						//For Gothic 2: New Balance
						if (this->indexCanStealNpcAST != -1)
						{
								//NPC_GIL_HUMANS == GIL_TPL in New Balance
								if (npc->guild > NPC_GIL_HUMANS)
								{
										return false;
								}

								static auto argumentSymbol = [_this = this]() -> zCPar_Symbol*
										{
												if (parser->GetSymbol(_this->indexCanStealNpcAST)->ele != 1)
												{
														return nullptr;
												}

												auto symbol = parser->GetSymbol(_this->indexCanStealNpcAST + 1);
												if (!symbol || symbol->type != zPAR_TYPE_INSTANCE)
												{
														return nullptr;
												}

												return symbol;
										}();

										if (argumentSymbol)
										{
												const auto pos = parser->GetSymbol(this->indexCanStealNpcAST)->single_intdata;
												parser->datastack.Clear();

												argumentSymbol->offset = reinterpret_cast<int>(npc);
												parser->datastack.Push(this->indexCanStealNpcAST + 1);
												parser->DoStack(pos);

												auto ret = parser->PopDataValue();

												if (ret)
												{
														return true;
												}
										}
						}
						//For pickpocketing ambient npcs with ezPickPockets etc
						else if (this->ShowAmbientPickPockets && player->GetTalent_Union("npc_talent_pickpocket") > 0 &&
								(npc->npcType == 0
#if ENGINE < Engine_G2
										|| npc->npcType == 4
										|| npc->npcType == 6
#else
										|| npc->npcType == 3
										|| npc->npcType == 5
										|| npc->npcType == 6
#endif
										)
								&& npc->guild <= NPC_GIL_HUMANS)
						{
								if (!npc->GetAivar_Union(zSTRING{ "aiv_playerhaspickedmypocket" }))
								{
										return true;
								}
						}

						return false;
				}

				bool VobLocatorActive;
				bool ShowText;
				zSTRING Text;
				int IconSize;
				int AdditionalIconSize;
				int MaxDistance;
				int HotKey;
				std::vector<oCInfo*> traderInfos;
				std::vector<oCInfo*> pickpocketInfos;
				int indexCanStealNpcAST;

				bool ShowChests;
				bool ShowCorpses;

				bool ShowHostileNpcs;
				bool ShowNeutralNpcs;
				bool ShowTraders;
				bool CheckTraderCondition;
				bool ShowPickPockets;
				bool ShowAmbientPickPockets;

				bool ShowArmors;
				bool ShowDocuments;
				bool ShowFood;
				bool ShowMagicItems;
				bool ShowMeleeWeapons;
				bool ShowMiscItems;
				bool ShowMunition;
				bool ShowOres;
				bool ShowPlants;
				bool ShowPotions;
				bool ShowRangedWeapons;
				bool ShowSpells;
				bool ShowStoneplates;
				bool ShowSwitches;
				bool ShowTreasures;
				bool ShowTrophies;
		};
		std::unique_ptr<VobLocator> vobLocator;
}