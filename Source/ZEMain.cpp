//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMain.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "ZECore\ZECore.h"

#define NOMINMAX
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#include "ZECore\ZECore.h"
#include "ZECore\ZEConsoleWindow.h"
#include "ZECore\ZEModuleManager.h"
#include "ZECore\ZEOptionManager.h"
#include "ZECore\ZEWindow.h"

#include "ZEPhysics\ZEPhysicsDebugModule.h"
#include "ZEModel\ZEModelDebugModule.h"
#include "ZEGraphics\ZEGraphicsDebugModule.h"
#include "ZESound\ZESoundDebugModule.h"
#include "ZEMeta\ZEMetaDebugModule.h"
#include "ZEUI\ZEUIDebugModule.h"

extern HINSTANCE ApplicationInstance;

#include "ZEDS/ZEFileCache.h"
#include <stdio.h>

class ZEStringCacheIdentifier : public ZECacheChunkIdentifier
{
	public:
		const char* String;
		virtual ZEDWORD GetHash() const
		{
			int Hash = 0;
			int I = 0;
			while (String[I] != NULL)
			{
				Hash += String[I];
				I++;
			}

			return Hash;
		}

		virtual size_t Write(void* File) const
		{
			ZEDWORD Count = strlen(String) + 1;
			fwrite(&Count, sizeof(ZEDWORD), 1, (FILE*)File);
			fwrite(String, sizeof(const char), strlen(String) + 1, (FILE*)File);
			return Count + sizeof(ZEDWORD);
		}

		virtual bool Equal(void* File) const
		{
			const size_t BufferSize = 1024;
			char Buffer[BufferSize];
			
			ZEDWORD TotalBytes;
			fread(&TotalBytes, sizeof(ZEDWORD), 1, (FILE*)File);
			
			size_t ReadBytes = 0;
			while (ReadBytes < TotalBytes)
			{
				size_t BytesToRead = (TotalBytes - ReadBytes < BufferSize ? TotalBytes - ReadBytes : BufferSize);
				fread(Buffer, sizeof(const char), BytesToRead, (FILE*)File);
				for (size_t I = 0; I < BytesToRead; I++)
					if (Buffer[I] != String[ReadBytes + I])
						return false;

				ReadBytes += BytesToRead;
			}
			
			return true;
		}

		ZEStringCacheIdentifier(const char* String)
		{
			this->String = String;
		}
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
/*	char CacheItem1[] = "This is a cache item 1. So Blabalbalsfedw fewwerewrew End Of File";
	char CacheItem2[] = "ard bla bldfaskfjşl sad şlaksfj aşskldf sşalkjf şaslkdjf şsldakj dra";
	
	ZEFileCache Cache;

	Cache.OpenCache("c:\\test.zeCache");
//	Cache.AddChunk(&ZEStringCacheIdentifier("Orcun"), CacheItem1, sizeof(CacheItem1));
//	Cache.AddChunk(&ZEStringCacheIdentifier("Cengiz"), CacheItem1, sizeof(CacheItem2));

	char Buffer[1024];
	Cache.GetChunkData(&ZEStringCacheIdentifier("dfg"), NULL, 0, 0);
	Cache.CloseCache();*/
	
	//MessageBox(NULL, "Attach it while you can !", "Zinek Engine", MB_OK); 
	_set_SSE2_enable(1); 
	ApplicationInstance = hInstance;
	
	ZEModelAnimationDebugModule DebugModule;
	//zeCore->SetDebugComponent(&DebugModule);

	ZEGraphicsDebugModule GraphicsDebugModule;
	zeCore->SetApplicationModule(&GraphicsDebugModule);

	ZEPhysicsDebugModule PhysicsDebugModule;
	//zeCore->SetDebugComponent(&PhysicsDebugModule);

	ZESoundDebugModule SoundDebugComponent;
	//zeCore->SetDebugComponent(&SoundDebugComponent);

	ZEMetaDebugModule MetaDebugComponent;
	//zeCore->SetDebugComponent(&MetaDebugComponent);

	ZEUIDebugModule UIDebugModule;
	//zeCore->SetDebugComponent(&UIDebugModule);


	zeCore->GetOptions()->Load("options.ini");
	zeCore->GetOptions()->ResetChanges();
	ZEConsoleWindow ConsoleWindow;
	zeCore->GetConsole()->SetConsoleInterface(&ConsoleWindow);
	zeCore->GetWindow()->SetWindowType(zeCore->GetOptions()->GetOption("Graphics", "Fullscreen")->GetValue().GetBoolean() ? ZE_WT_FULLSCREEN : ZE_WT_RESIZABLE);
	zeCore->GetWindow()->SetWindowSize(zeCore->GetOptions()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInteger(), zeCore->GetOptions()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInteger());

 	if (zeCore->StartUp())
		zeCore->Run();
}





