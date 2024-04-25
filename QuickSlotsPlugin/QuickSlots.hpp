namespace GOTHIC_NAMESPACE
{
		struct QSPreset
		{
				std::map<size_t, std::string> Slots;
				std::vector<std::string> EquippedItems;

				QSPreset(std::map<size_t, std::string> s, std::vector<std::string> ei) :
						Slots(s), EquippedItems(ei) {};
		};

		struct QS
		{
				size_t slot;
				oCItem* item;
				bool active;

				QS(size_t sl, oCItem* it, bool ac) :
						slot(sl), item(it), active(ac) {};
		};

		class QuickSlots
		{
		public:
				QuickSlots()
				{
						this->cellSize = zoptions->ReadInt("INTERFACE", "InventoryCellSize", 70);
						this->cellSizeX = screen->anx(this->cellSize);
						this->cellSizeY = screen->any(this->cellSize);

						this->itemView = new zCView(0, 0, this->cellSizeX, this->cellSizeY);
						this->itemView->InsertBack(zSTRING{ "INV_SLOT_HIGHLIGHTED.TGA" });
						this->itemView->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
						this->itemView->SetTransparency(255);

						this->itemViewActive = new zCView(0, 0, this->cellSizeX, this->cellSizeY);
						this->itemViewActive->InsertBack(zSTRING{ "INV_SLOT_EQUIPPED_HIGHLIGHTED.TGA" });
						this->itemViewActive->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
						this->itemViewActive->SetTransparency(255);

						this->itemViewEmpty = new zCView(0, 0, this->cellSizeX, this->cellSizeY);
						this->itemViewEmpty->InsertBack(zSTRING{ "INV_SLOT.TGA" });
						this->itemViewEmpty->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
						this->itemViewEmpty->SetTransparency(255);

						this->textView = new zCView(0, 0, this->cellSizeX, this->cellSizeY);
						this->textView->SetTransparency(255);
						this->textView->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);

						this->itemWorld = zfactory->CreateWorld();
#if ENGINE > Engine_G2
						this->itemWorld->m_bIsInventoryWorld = TRUE;
#endif
				}

				~QuickSlots()
				{
						this->Clear();

						if (this->textView)
						{
								screen->RemoveItem(this->textView);
								delete this->textView;
						}

						if (this->itemView)
						{
								screen->RemoveItem(this->itemView);
								delete this->itemView;
						}

						if (this->itemViewActive)
						{
								screen->RemoveItem(this->itemViewActive);
								delete this->itemViewActive;
						}

						if (this->itemViewEmpty)
						{
								screen->RemoveItem(this->itemViewEmpty);
								delete this->itemViewEmpty;
						}

						if (this->itemWorld)
						{
								this->itemWorld->Release();
						}
				}

				void Archive()
				{
						if (slInfo.Slot < 0)
						{
								return;
						}

						zCArchiver* ar = zarcFactory->CreateArchiverWrite(::Union::StringANSI::Format("{0}\\QuickSlots.sav", GetArchivePath()).ToChar(), zARC_MODE_ASCII, 0, 0);
						if (!ar)
						{
								return;
						}

						//DO STUFF
						ar->WriteInt("QSCount", this->CurrentSlots.size());
						for (auto& slot : this->CurrentSlots)
						{
								ar->WriteInt("QS", slot.first);
								ar->WriteString("QS", slot.second.c_str());
						}

						ar->WriteInt("PresetsCount", this->Presets.size());
						for (auto& PresetIt : this->Presets)
						{
								ar->WriteInt("PresetID", PresetIt.first);
								ar->WriteInt("PresetQSCount", PresetIt.second.Slots.size());
								for (auto& slot : PresetIt.second.Slots)
								{
										ar->WriteInt("PresetQS", slot.first);
										ar->WriteString("PresetQS", slot.second.c_str());
								}
								ar->WriteInt("PresetEquippedCount", PresetIt.second.EquippedItems.size());
								for (auto& equipped : PresetIt.second.EquippedItems)
								{
										ar->WriteString("PresetEquipped", equipped.c_str());
								}
						}

						ar->Close();
						ar->Release();
				}

				void Unarchive()
				{
						if (slInfo.Slot < 0)
						{
								return;
						}

						this->Clear();

						zCArchiver* ar = zarcFactory->CreateArchiverRead(::Union::StringANSI::Format("{0}\\QuickSlots.sav", GetArchivePath()).ToChar(), 0);
						if (!ar)
						{
								return;
						}

						auto QSCount = ar->ReadIntSafe("QSCount");

						for (int i = 0; i < QSCount; i++)
						{
								auto slotNr = ar->ReadInt("QS");
								auto slotInstanceName = ar->ReadString("QS");

								if (!slotInstanceName.IsEmpty() && slotNr > 0)
								{
										this->CurrentSlots.insert({ slotNr, slotInstanceName.ToChar() });
								}
						}

						auto PresetsCount = ar->ReadIntSafe("PresetsCount");
						for (int i = 0; i < PresetsCount; i++)
						{
								auto PresetID = ar->ReadInt("PresetID");

								auto PresetQSCount = ar->ReadInt("PresetQSCount");
								std::map<size_t, std::string> Slots;
								for (auto ii = 0; ii < PresetQSCount; ii++)
								{
										auto slotNr = ar->ReadInt("PresetQS");
										auto slotInstanceName = ar->ReadString("PresetQS");
										Slots.insert({ slotNr,slotInstanceName.ToChar() });
								}

								auto PresetEquippedCount = ar->ReadIntSafe("PresetEquippedCount");
								std::vector<std::string> Equipped;
								for (auto ii = 0; ii < PresetEquippedCount; ii++)
								{
										Equipped.push_back(ar->ReadString("PresetEquipped").ToChar());
								}

								this->Presets.insert({ PresetID, QSPreset(Slots, Equipped) });
						}

						ar->Close();
						ar->Release();
				}

				bool HandleInput(int Key, bool Inv)
				{
						if (ogame->game_testmode)
						{
								return false;
						}

						switch (Key)
						{
#if ENGINE >= Engine_G2
						case KEY_1:
						{
								if (zinput->KeyPressed(KEY_LSHIFT) && player->CanDrawWeapon())
								{
										this->DrawFist();

										return true;
								}
								return false;
						}
#endif
						case KEY_F1:
						case KEY_F2:
						case KEY_F3:
						case KEY_F4:
						case KEY_F5:
						case KEY_F6:
						case KEY_F7:
						case KEY_F8:
						case KEY_F9:
						case KEY_F10:
						case KEY_F11:
						case KEY_F12:
						{
								auto PresetNr = KeyToPreset(Key);
								auto SlotNr = KeyToSlot(Key);

								if (zinput->KeyPressed(KEY_LSHIFT) && PresetNr != -1)
								{
										if (Inv)
										{
												this->RegisterPreset(PresetNr);
										}
										else
										{
												this->SwitchPreset(PresetNr);
										}
										return true;
								}

								if (Inv)
								{
										this->RegisterItem(SlotNr, player->inventory2.GetSelectedItem());
								}
								else
								{
										this->UseItemFromSlot(SlotNr);
								}

								return true;
						}

						default:
								return false;
						}
				}

				void Loop()
				{
						this->DrawQuickSlots();
				}

		private:
				void Clear()
				{
						this->ClearPresets();
						this->ClearSlots();
				}

				void ClearSlots()
				{
						this->CurrentSlots.clear();
				}

				void ClearPresets()
				{
						this->Presets.clear();
				}

				void DrawFist()
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

						switch (player->GetWeaponMode())
						{
						case NPC_WEAPON_NONE:
								break;
						case NPC_WEAPON_FIST:
						{
								oCMsgWeapon* msg = new oCMsgWeapon(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0);
								player->GetEM()->OnMessage(msg, player);
						}
						return;
						default:
						{
								oCMsgWeapon* msg = new oCMsgWeapon(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0);
								player->GetEM()->OnMessage(msg, player);
						}
						break;
						}

						{
								oCMsgWeapon* msg = new oCMsgWeapon(oCMsgWeapon::EV_DRAWWEAPON, NPC_WEAPON_FIST, 1);
								player->GetEM()->OnMessage(msg, player);
						}
				}

				void DrawQuickSlots()
				{
						if (!ogame || ogame->singleStep || !ogame->game_drawall || !ogame->GetShowPlayerStatus() || ogame->game_testmode)
						{
								return;
						}

						if (!this->itemView || !this->itemWorld)
						{
								return;
						}

						if (player->inventory2.IsOpen() && player->inventory2.GetNextContainerLeft(&player->inventory2))
						{
								return;
						}

						std::vector<QS*> qs;

						for (size_t pos = 1; pos <= 12; pos++)
						{
								auto it = this->CurrentSlots.find(pos);
								if (it == this->CurrentSlots.end())
								{
										if (showEmptySlots || player->inventory2.IsOpen())
										{
												qs.push_back({ new QS(pos, nullptr, false) });
										}
										continue;
								}

								auto item = player->IsInInv(zSTRING{ it->second.c_str() }, 1);

								auto itemActive = [item]() {
										if (!item)
										{
												return false;
										}

										if (item->HasFlag(ITM_FLAG_ACTIVE))
										{
												return true;
										}

										if (item->mainflag != ITM_CAT_MUN)
										{
												return false;
										}

										if (player->GetEquippedRangedWeapon() && player->GetEquippedRangedWeapon()->munition == item->instanz)
										{
												return true;
										}

										if (player->GetWeapon() && player->GetWeapon()->munition == item->instanz)
										{
												return true;
										}

										return false;
										}();

										if (item)
										{
												qs.push_back({ new QS(it->first, item, itemActive) });
										}
										else if (showEmptySlots || player->inventory2.IsOpen())
										{
												qs.push_back({ new QS(pos, nullptr, false) });
										}
						}

						if (!qs.size())
						{
								return;
						}

						size_t maxCellsInRow;
						int startPosX;
						int startPosY;
						int inverse;

						if (player->inventory2.IsOpen())
						{
								int InvPosX, InvPosY, InvSizeX, InvSizeY;
								player->inventory2.GetPosition(InvPosX, InvPosY);
								player->inventory2.GetSize(InvSizeX, InvSizeY);
								startPosX = (InvPosX - this->cellSizeX) / 2;
								startPosY = this->cellSizeY;
								maxCellsInRow = (InvPosX - this->cellSizeX) / this->cellSizeX;
								inverse = 1;
						}
						else
						{
								startPosX = 8192 / 2;
								startPosY = 8192 - (this->cellSizeY * 2);
								maxCellsInRow = (8192 - this->cellSizeX) / this->cellSizeX;
								inverse = -1;
						}

						size_t rowsCount = (qs.size() / maxCellsInRow) + 1;
						size_t offset = 0;

						for (size_t iRow = 0; iRow < rowsCount; iRow++)
						{
								size_t cells = qs.size() - offset;
								cells = cells > maxCellsInRow ? maxCellsInRow : cells;
								int rowStartPosX = startPosX - ((cells * this->cellSizeX) / 2);
								int rowStartPosY = startPosY + (iRow * this->cellSizeY) * inverse;
								for (size_t iCell = 0; iCell < cells; iCell++)
								{
										int curPosX = rowStartPosX + (iCell * this->cellSizeX);
										auto it = qs[offset];

										zCView* currentView = nullptr;
										if (it->item)
										{
												currentView = it->active ? this->itemViewActive : this->itemView;
										}
										else
										{
												currentView = this->itemViewEmpty;
										}

										/// +Item
										screen->InsertItem(currentView);

										currentView->SetPos(curPosX, rowStartPosY);
										currentView->SetSize(this->cellSizeX, this->cellSizeY);
										currentView->Blit();

										if (it->item)
										{
												auto prevLighting = oCItem::GetLightingSwell();
												oCItem::SetLightingSwell(TRUE);
												it->item->RenderItem(this->itemWorld, currentView, 0.0f);
												oCItem::SetLightingSwell(prevLighting);
										}

										screen->RemoveItem(currentView);
										/// -Item

										/// +Text
										screen->InsertItem(this->textView);

										this->textView->SetPos(curPosX, rowStartPosY);
										this->textView->SetSize(this->cellSizeX, this->cellSizeY);
										this->textView->ClrPrintwin();

										this->textView->SetFontColor(GFX_GOLD);
										auto slotNr = zSTRING{ "F" } + zSTRING{ static_cast<int>(it->slot) };
										this->textView->Print(1000, 1000, slotNr);
										if (it->item && it->item->amount > 1)
										{
												this->textView->SetFontColor(GFX_WHITE);
												auto itemAmount = zSTRING{ it->item->amount };
												this->textView->Print(8192 - this->textView->FontSize(itemAmount) - 1000, 8192 - this->textView->FontY() - 1000, itemAmount);
										}

										this->textView->Blit();

										screen->RemoveItem(this->textView);
										/// -Text

										offset++;
								}
						}

						qs.clear();

						int x, y, sx, sy;
						screen->GetViewport(x, y, sx, sy);
						zrenderer->SetViewport(x, y, sx, sy);
				}

				void HideWeapon()
				{
						if (player->GetWeaponMode() != NPC_WEAPON_NONE)
						{
								player->GetEM()->OnMessage(new oCMsgWeapon(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0), player);
						}

						if (player->GetSpellBook())
						{
								player->GetSpellBook()->Close(TRUE);
						}
				}

				void RegisterItem(size_t SlotNr, oCItem* Item)
				{
						if (!Item)
						{
								return;
						}

						if (SlotNr == 0)
						{
								return;
						}

						std::string instanceName = Item->GetInstanceName().ToChar();
						auto it = this->CurrentSlots.find(SlotNr);
						if (it != this->CurrentSlots.end())
						{
								auto slotInstanceName = it->second;
								this->CurrentSlots.erase(it);
								if (slotInstanceName == instanceName)
								{
										return;
								}
						}

						std::erase_if(this->CurrentSlots, [SlotNr, instanceName](const auto& slot) {
								auto const& [key, value] = slot;
								return key == SlotNr || value == instanceName;
								});

						this->CurrentSlots.insert({ SlotNr, instanceName });
				}

				void RegisterPreset(size_t PresetNr)
				{
						std::vector<std::string> presetItems;

#if ENGINE < Engine_G2
						for (int cat = 0; cat < INV_MAX; cat++)
						{
								player->inventory2.UnpackCategory(cat);
								auto playerInv = player->inventory2.inventory[cat].next;
								while (playerInv)
								{
										auto item = playerInv->GetData();
										playerInv = playerInv->next;

										if (item->instanz < 0)
										{
												continue;
										}

										if (!item->HasFlag(ITM_FLAG_ACTIVE))
										{
												continue;
										}

										std::string itemInstance = item->GetInstanceName().ToChar();
										presetItems.push_back(itemInstance);
								}
						}
#else
						player->inventory2.UnpackCategory();
						auto playerInv = player->inventory2.inventory.next;
						while (playerInv)
						{
								auto item = playerInv->GetData();
								playerInv = playerInv->next;

								if (item->instanz < 0)
								{
										continue;
								}

								if (!item->HasFlag(ITM_FLAG_ACTIVE))
								{
										continue;
								}

								std::string itemInstance = item->GetInstanceName().ToChar();
								presetItems.push_back(itemInstance);
						}
#endif

						zSTRING msg = zSTRING{ "Saving preset: " } + zSTRING{ static_cast<int>(PresetNr) };
						ogame->GetTextView()->Printwin(msg);

						auto preset = QSPreset(CurrentSlots, presetItems);

						auto it = this->Presets.find(PresetNr);
						if (it == this->Presets.end())
						{
								this->Presets.insert({ PresetNr, preset });
								return;
						}

						it->second = preset;
				}

				void SwitchPreset(size_t PresetNr)
				{
						if (player->IsUnconscious()
								|| player->attribute[NPC_ATR_HITPOINTS] <= 0
								|| !player->GetAnictrl()->IsStanding()
								|| player->GetBodyState() == BS_SWIM
								|| player->GetBodyState() == BS_DIVE)
						{
								return;
						}

						this->UnequipAll();
						this->CurrentSlots.clear();

						zSTRING msg = zSTRING{ "Switching preset: " } + zSTRING{ static_cast<int>(PresetNr) };
						ogame->GetTextView()->Printwin(msg);

						auto it = this->Presets.find(PresetNr);
						if (it == this->Presets.end())
						{
								return;
						}

						for (auto presetItem : it->second.EquippedItems)
						{
								auto item = player->IsInInv(zSTRING{ presetItem.c_str() }, 1);
								if (item)
								{
										player->Equip(item);
								}
						}

						this->CurrentSlots = it->second.Slots;
				}

				void UnequipAll()
				{
#if ENGINE < Engine_G2
						for (int cat = 0; cat < INV_MAX; cat++)
						{
								player->inventory2.UnpackCategory(cat);
								auto playerInv = player->inventory2.inventory[cat].next;
								while (playerInv)
								{
										auto item = playerInv->GetData();
										playerInv = playerInv->next;

										if (item->instanz < 0)
										{
												continue;
										}

										if (!item->HasFlag(ITM_FLAG_ACTIVE))
										{
												continue;
										}

										player->Equip(item);
								}
						}
#else
						player->inventory2.UnpackCategory();
						auto playerInv = player->inventory2.inventory.next;
						while (playerInv)
						{
								auto item = playerInv->GetData();
								playerInv = playerInv->next;

								if (item->instanz < 0)
								{
										continue;
								}

								if (!item->HasFlag(ITM_FLAG_ACTIVE))
								{
										continue;
								}

								player->Equip(item);
						}
#endif
				}

				void UseItem(oCItem* item)
				{
						player->GetEM()->OnMessage(new oCMsgManipulate(oCMsgManipulate::EV_EQUIPITEM, item), item);
				}

				void UseItemFromSlot(size_t SlotNr)
				{
						if (player->IsUnconscious()
								|| player->attribute[NPC_ATR_HITPOINTS] <= 0
								|| !player->GetAnictrl()->IsStanding()
								|| player->GetBodyState() == BS_SWIM
								|| player->GetBodyState() == BS_DIVE
								|| player->interactItemCurrentState != -1
								)
						{
								return;
						}

						auto it = this->CurrentSlots.find(SlotNr);
						if (it == this->CurrentSlots.end())
						{
								return;
						}

						auto item = player->IsInInv(zSTRING{ it->second.c_str() }, 1);
						if (!item)
						{
								return;
						}

						this->HideWeapon();

						if (!item->GetSchemeName().IsEmpty())
						{
								this->UseItemToState(item);
								return;
						}

						this->UseItem(item);
				}

				void UseItemToState(oCItem* item)
				{
						player->GetEM()->OnMessage(new oCMsgManipulate(oCMsgManipulate::EV_USEITEMTOSTATE, item, player->interactItemCurrentState + 1), item);
				}

		private:
				int cellSize = 70;
				int cellSizeX;
				int cellSizeY;
				bool showEmptySlots = false;
				std::map<size_t, std::string> CurrentSlots;
				std::map<size_t, QSPreset> Presets;
				zCView* textView;
				zCView* itemView;
				zCView* itemViewActive;
				zCView* itemViewEmpty;
				zCWorld* itemWorld;
		};
		std::unique_ptr<QuickSlots> quickSlots;
}