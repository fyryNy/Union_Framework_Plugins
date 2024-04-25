namespace GOTHIC_NAMESPACE
{
		int KeyToSlot(int Key)
		{
				switch (Key)
				{
				case KEY_F1:
						return 1;
				case KEY_F2:
						return 2;
				case KEY_F3:
						return 3;
				case KEY_F4:
						return 4;
				case KEY_F5:
						return 5;
				case KEY_F6:
						return 6;
				case KEY_F7:
						return 7;
				case KEY_F8:
						return 8;
				case KEY_F9:
						return 9;
				case KEY_F10:
						return 10;
				case KEY_F11:
						return 11;
				case KEY_F12:
						return 12;
				default:
						return -1;
				}
		}

		int KeyToPreset(int Key)
		{
				switch (Key)
				{
				case KEY_F1:
						return 1;
				case KEY_F2:
						return 2;
				case KEY_F3:
						return 3;
				case KEY_F4:
						return 4;
				default:
						return -1;
				}
		}

		::Union::StringANSI GetArchivePath()
		{
				return ::Union::StringANSI::Format("{0}{1}", zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES).ToChar(), slInfo.GetSaveSlotName(slInfo.Slot));
		}
}