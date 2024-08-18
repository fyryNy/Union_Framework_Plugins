namespace GOTHIC_NAMESPACE
{
		auto Hook_zCAIPlayer_CheckPhysics = ::Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x004FE140, 0x00511520, 0x0050AE90, 0x0050DC40)), &zCAIPlayer::CheckPhysics_Union);
		void zCAIPlayer::CheckPhysics_Union()
		{
				oCNpc* npc = zDYNAMIC_CAST<oCNpc>(this->vob);

				if (player == this->vob)
				{
						(this->*Hook_zCAIPlayer_CheckPhysics)();
						return;
				}

				static int ZS_MM_ATTACK = parser->GetIndex("ZS_MM_ATTACK");
				if (ZS_MM_ATTACK == -1
						|| !(this->model && !this->model->isFlying && this->aboveFloor > 5.0f)
						|| !npc
						|| !npc->IsAIState(ZS_MM_ATTACK)
						|| !npc->GetEM()->IsEmpty(TRUE)
						|| npc->fight_waitForAniEnd != -1
						|| !npc->enemy
						)
				{
						(this->*Hook_zCAIPlayer_CheckPhysics)();
						return;
				}

				switch (npc->GetCurrentFightMove())
				{
				case oCNpc::NPC_FIGHTMOVE_TURN:
				case oCNpc::NPC_FIGHTMOVE_WAIT:
				case oCNpc::NPC_FIGHTMOVE_RUNBACK:

#if ENGINE >= Engine_G2
				case oCNpc::NPC_FIGHTMOVE_WAIT_EXT:
				case oCNpc::NPC_FIGHTMOVE_WAIT_LONG:
#endif
						break;

				default:
						(this->*Hook_zCAIPlayer_CheckPhysics)();
						return;
				}

				if (npc->bbox3D.mins[VY] <= npc->enemy->bbox3D.mins[VY]
						|| !(this->aboveFloor > 45.0f || !npc->IsSameHeight(npc->enemy) || !npc->enemy->IsSameHeight(npc))
						)
				{
						(this->*Hook_zCAIPlayer_CheckPhysics)();
						return;
				}

				auto aboveFlor_prev = this->aboveFloor;
				this->aboveFloor = this->config.zMV_STEP_HEIGHT + 1.0f;
				(this->*Hook_zCAIPlayer_CheckPhysics)();
				this->aboveFloor = aboveFlor_prev;
		}

		auto SlopeAddr = zSwitch(0x53d2b9, 0x55494b, 0x54c6e1, 0x551901);
		void __fastcall zCCollObjectCharacter_TestHardCollisions_Slope(::Union::Registers& reg)
		{
				zCCollObjectCharacter* This = nullptr;

#if ENGINE >= Engine_G2
				This = reinterpret_cast<zCCollObjectCharacter*>(reg.ebx);
#else
				This = reinterpret_cast<zCCollObjectCharacter*>(reg.ebp);
#endif

				if (!This)
				{
						return;
				}
				
				auto vob = static_cast<zCVob*>(This->m_pvClientData);
				if (vob == player)
				{
						return;
				}

				int EndOffset = 0;
#if ENGINE >= Engine_G2
				EndOffset = 0x1a4;
#elif ENGINE == Engine_G1
				EndOffset = 0x133;
#else
				EndOffset = 0x146;
#endif
				reg.eip = SlopeAddr + EndOffset;
		}
		auto PartialHook_zCCollObjectCharacter_TestHardCollisions_Slope = ::Union::CreatePartialHook(reinterpret_cast<void*>(SlopeAddr), &zCCollObjectCharacter_TestHardCollisions_Slope);

		auto AbyssAddr = zSwitch(0x53d4d0, 0x554b8a, 0x54c96d, 0x551b8d);
		void __fastcall zCCollObjectCharacter_TestHardCollisions_Abyss(::Union::Registers& reg)
		{
				zCCollObjectCharacter* This = nullptr;

#if ENGINE >= Engine_G2
				This = reinterpret_cast<zCCollObjectCharacter*>(reg.ebx);
#else
				This = reinterpret_cast<zCCollObjectCharacter*>(reg.ebp);
#endif

				if (!This)
				{
						return;
				}

				auto vob = static_cast<zCVob*>(This->m_pvClientData);
				if (vob == player)
				{
						return;
				}

				int EndOffset = 0;
#if ENGINE >= Engine_G2
				EndOffset = 0x41;
#else
				EndOffset = 0x40;
#endif
				reg.eip = AbyssAddr + EndOffset;
		}
		auto PartialHook_zCCollObjectCharacter_TestHardCollisions_Abyss = ::Union::CreatePartialHook(reinterpret_cast<void*>(AbyssAddr), &zCCollObjectCharacter_TestHardCollisions_Abyss);

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