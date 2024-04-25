namespace GOTHIC_NAMESPACE
{
		int VirtualPosX;
		int VirtualPosY;
		int PixelSize;

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

		void DrawCompass()
		{
				if (!ogame || !player || !ogame->GetShowPlayerStatus() || !ogame->game_drawall)
				{
						return;
				}

				if (player->movlock || player->attribute[NPC_ATR_HITPOINTS] <= 0)
				{
						return;
				}

				//position of the north
				zVEC3 vecNorth;
				//in archolos world is rotated
				if (::Union::StringANSI(ogame->GetGameWorld()->GetWorldName()).Contains("ARCHOLOS_ENDGAME") || ::Union::StringANSI(ogame->GetGameWorld()->GetWorldName()).Contains("ARCHOLOS_MAINLAND"))
				{
						vecNorth = player->GetPositionWorld() + zVEC3{ -1.0f, 0.0f, 0.0f };
				}
				else
				{
						vecNorth = player->GetPositionWorld() + zVEC3{ 0.0f, 0.0f, 1.0f };
				}

				//get the angle to the north
				float azimuth, elevation;
				player->GetAngles(vecNorth, azimuth, elevation);

				zSTRING texNameBack{ "COMPASS_BACK" };
				zSTRING texNameNeedle{ "COMPASS_NEEDLE" };
				zCTexture* texBack = texBack->Load(texNameBack, TRUE);
				zCTexture* texNeedle = texNeedle->Load(texNameNeedle, TRUE);

				float TopLeftX = static_cast<float>(screen->nax(VirtualPosX));
				float TopLeftY = static_cast<float>(screen->nay(VirtualPosY));

				zVEC2 TopLeft{ TopLeftX, TopLeftY };
				zVEC2 BottomRight{ TopLeftX + PixelSize, TopLeftY + PixelSize };

				auto filter = zrenderer->GetBilerpFilterEnabled();
				auto alphaBlend = zrenderer->GetAlphaBlendFunc();

				zrenderer->SetBilerpFilterEnabled(TRUE);
				zrenderer->SetAlphaBlendFunc(zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_BLEND);
				zrenderer->DrawTile(texBack, TopLeft, BottomRight, zSwitch(1.0f, 1.0f, 1.2f, 1.2f), zVEC2(0.0f, 0.0f), zVEC2(1.0f, 1.0f), GFX_WHITE);
				zrenderer->DrawTile_Rotated(texNeedle, TopLeft, BottomRight, zSwitch(0.95f, 0.95f, 1.15f, 1.15f), zVEC2(0.0f, 0.0f), zVEC2(1.0f, 1.0f), GFX_WHITE, azimuth);
				zrenderer->SetBilerpFilterEnabled(filter);
				zrenderer->SetAlphaBlendFunc(alphaBlend);

				texBack->Release();
				texNeedle->Release();
		}

		void ApplySettings()
		{
				VirtualPosX = zoptions->ReadInt("CompassPlugin", "VirtualPosX", 100);
				VirtualPosY = zoptions->ReadInt("CompassPlugin", "VirtualPosY", 500);
				PixelSize = zoptions->ReadInt("CompassPlugin", "PixelSize", 128);
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
				DrawCompass();
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