namespace GOTHIC_NAMESPACE
{
		bool Bjorned;
		float BjornTimeFrame;
		float BjornAngle;

		static inline bool IsInScreenBoundaries(zTRndSimpleVertex* tileVert)
		{
				for (int i = 0; i < 4; i++)
				{
						if ((tileVert[i].pos[0] >= 0 && tileVert[i].pos[0] <= zrenderer->vid_xdim)
								&& (tileVert[i].pos[1] >= 0 && tileVert[i].pos[1] <= zrenderer->vid_ydim)
								)
						{
								return true;
						}
				}

				return false;
		}

		void zCRenderer::DrawTile_Rotated(zCTexture* texture, zVEC2 const& posMin, zVEC2 const& posMax, float zValue, zVEC2 const& uvMin, zVEC2 const& uvMax, zCOLOR color, float rotation)
		{
				//skip if there's no texture at all
				if (!texture)
				{
						return;
				}

				static zTRndSimpleVertex tileVert[4];

				//I'm not sure what exactly it does 
				texture->CacheIn(-1.0f);

				//Create vertices
				tileVert[0].pos = posMin;
				tileVert[0].uv = uvMin;
				tileVert[0].z = zValue;
				tileVert[0].color = color;

				tileVert[2].pos = posMax;
				tileVert[2].uv = uvMax;
				tileVert[2].z = zValue;
				tileVert[2].color = color;

				tileVert[3].pos[0] = tileVert[2].pos[0];
				tileVert[3].pos[1] = tileVert[0].pos[1];
				tileVert[3].uv[0] = tileVert[2].uv[0];
				tileVert[3].uv[1] = tileVert[0].uv[1];
				tileVert[3].z = zValue;
				tileVert[3].color = color;

				tileVert[1].pos[0] = tileVert[0].pos[0];
				tileVert[1].pos[1] = tileVert[2].pos[1];
				tileVert[1].uv[0] = tileVert[0].uv[0];
				tileVert[1].uv[1] = tileVert[2].uv[1];
				tileVert[1].z = zValue;
				tileVert[1].color = color;

				//convert rotation from degrees to radian and calculate cos and sin
				double radian = rotation * 3.14159 / 180;
				auto s = std::sin(radian);
				auto c = std::cos(radian);

				//calculate pivot point
				zVEC2 TextureCenter = zVEC2{ (posMax[0] + posMin[0]) / 2, (posMax[1] + posMin[1]) / 2 };

				//calculate rotated x and y points for every vert
				for (int i = 0; i < 4; i++)
				{
						float x = static_cast<float>(c * (tileVert[i].pos[0] - TextureCenter[0]) - s * (tileVert[i].pos[1] - TextureCenter[1]) + TextureCenter[0]);
						float y = static_cast<float>(s * (tileVert[i].pos[0] - TextureCenter[0]) + c * (tileVert[i].pos[1] - TextureCenter[1]) + TextureCenter[1]);
						tileVert[i].pos[0] = x;
						tileVert[i].pos[1] = y;
				}

				//skip if rotated texture is outside the screen dimensions
				if (!IsInScreenBoundaries(tileVert))
				{
						return;
				}

				DrawPolySimple(texture, &tileVert[0], 4);
		}

		void HandleResultString_Union(zSTRING Str);
		auto Hook_HandleResultString = ::Union::CreateHook(reinterpret_cast<void*>(0x004301F0, 0x00433D80, 0x004329E0, 0x00432EC0), &HandleResultString_Union);
		void HandleResultString_Union(zSTRING Str)
		{
				auto str = Str;
				str.Upper();

				if (str == "BJORN")
				{
						Bjorned = !Bjorned;
						BjornTimeFrame = 0.0f;

						if (Bjorned)
						{
								ogame->array_view[oCGame::oEGameDialogView::GAME_VIEW_SCREEN]->PrintTimed(100, 300, "YOU'VE BEEN BJORNED!!!", 10000, 0);
						}
				}

				Hook_HandleResultString(Str);
		}

		void Bjorn()
		{
				if (!Bjorned)
				{
						return;
				}

				BjornTimeFrame += ztimer->frameTimeFloat / ztimer->factorMotion;

				if (BjornTimeFrame >= 50.0f)
				{
						BjornTimeFrame -= 50.0f;
						BjornAngle += 5.0f;
				}

				if (BjornAngle >= 360.0f)
				{
						BjornAngle = 0.0f;
				}

				zSTRING texName{ "ICON_TEST" };
				zCTexture* tex = tex->Load(texName, TRUE);

				int x = screen->nax(100);
				int y = screen->nay(300 + screen->FontY());
				int sx = x + 75;
				int sy = y + 75;

				zVEC2 TopLeft{ static_cast<float>(x), static_cast<float>(y) };
				zVEC2 BottomRight{ static_cast<float>(sx), static_cast<float>(sy) };

				zrenderer->DrawTile_Rotated(tex, TopLeft, BottomRight, 1.2f, zVEC2(0.0f, 0.0f), zVEC2(1.0f, 1.0f), GFX_WHITE, BjornAngle);
				tex->Release();
		}

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
				Bjorn();
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