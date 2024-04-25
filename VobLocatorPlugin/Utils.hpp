namespace GOTHIC_NAMESPACE
{
		bool oCMobContainer::IsEmpty_Union()
		{
				auto contInv = this->containList.next;

				while (contInv)
				{
						auto item = contInv->GetData();
						contInv = contInv->next;

						if (item->instanz < 0)
						{
								continue;
						}

						return false;
				}

				return true;
		}

		bool zCVob::IsValidVob_Union()
		{
				auto vobType = this->GetVobType();

				if (vobType == zVOB_TYPE_NSC)
				{
						auto npc = static_cast<oCNpc*>(this);

						if (npc->instanz < 0)
						{
								return false;
						}

						if (npc == player)
						{
								return false;
						}

						if (npc->name->IsEmpty())
						{
								return false;
						}

						if (!npc->GetHomeWorld())
						{
								return false;
						}

#if ENGINE >= Engine_G2
						if (npc->noFocus)
						{
								return false;
						}
#endif

						return true;
				}
				else if (vobType == zVOB_TYPE_ITEM)
				{
						auto item = static_cast<oCItem*>(this);

						if (item->instanz < 0)
						{
								return false;
						}

						if (!item->GetVisual())
						{
								return false;
						}

						if (item->flags & ITM_FLAG_NFOCUS)
						{
								return false;
						}

						if (!item->GetHomeWorld())
						{
								return false;
						}

						if (item->name.IsEmpty() && item->description.IsEmpty())
						{
								return false;
						}

						return true;
				}
				else if (vobType == zVOB_TYPE_MOB)
				{
						if (auto inter = zDYNAMIC_CAST<oCMobInter>(this))
						{
								return true;
						}
						/*if (auto container = zDYNAMIC_CAST<oCMobContainer>(this))
						{
							if (container->IsEmpty_Union())
							{
								return false;
							}

							return true;
						}

						if (auto inter = zDYNAMIC_CAST<oCMobInter>(this))
						{
							if (inter->onStateFuncName.IsEmpty())
							{
								return false;
							}

							return true;
						}*/
				}

				return false;
		}

		bool oCNpc::CanBeLooted_Union()
		{
#if ENGINE <= Engine_G1A
				for (int i = INV_NONE; i < INV_MAX; i++)
				{
						if (!this->inventory2.packString[i].IsEmpty())
						{
								return true;
						}
				}
#else
				if (!this->inventory2.packString.IsEmpty())
				{
						return true;
				}
#endif

#if ENGINE <= Engine_G1A
				for (int i = INV_NONE; i < INV_MAX; i++)
				{
						auto npcInv = this->inventory2.inventory[i].next;
						while (npcInv)
						{
								auto item = npcInv->GetData();
								npcInv = npcInv->next;

								if (item->instanz < 0)
								{
										continue;
								}

								if (item->HasFlag(ITM_CAT_ARMOR))
								{
										continue;
								}

								return true;
						}
				}
#else
				auto npcInv = this->inventory2.inventory.next;
				while (npcInv)
				{
						auto item = npcInv->GetData();
						npcInv = npcInv->next;

						if (item->instanz < 0)
						{
								continue;
						}

						if (item->HasFlag(ITM_FLAG_ACTIVE))
						{
								continue;
						}

						return true;
				}
#endif

				return false;
		}

		bool IsInRange(int low, int high, int x)
		{
				return (low <= x && x <= high);
		}

		int oCNpc::GetAivar_Union(const zSTRING& aivar)
		{
				auto sym = parser->GetSymbol(aivar);
				if (!sym)
				{
						return -1;
				}

				return this->aiscriptvars[sym->single_intdata];
		}

		int oCNpc::GetTalent_Union(const zSTRING& talent)
		{
				auto sym = parser->GetSymbol(talent);
				if (!sym)
				{
						return -1;
				}

				return this->GetTalentSkill(sym->single_intdata);
		}
}