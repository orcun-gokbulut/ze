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

#include "ZEDebug\ZEPhysicsDebugModule.h"
#include "ZEDebug\ZEModelAnimationDebugModule.h"
#include "ZEDebug\ZEGraphicsDebugModule.h"
#include "ZEDebug\ZESoundDebugModule.h"
#include "ZEDebug\ZEMetaDebugModule.h"

extern HINSTANCE ApplicationInstance;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//MessageBox(NULL, "Attach it while you can !", "Zinek Engine", MB_OK); 
	_set_SSE2_enable(1); 
	ApplicationInstance = hInstance;
	
	ZEModelAnimationDebugModule DebugModule;
	//zeCore->SetDebugComponent(&DebugModule);

	ZEGraphicsDebugModule GraphicsDebugModule;
	zeCore->SetDebugComponent(&GraphicsDebugModule);

	ZEPhysicsDebugModule PhysicsDebugModule;
	//zeCore->SetDebugComponent(&PhysicsDebugModule);

	ZESoundDebugModule SoundDebugComponent;
	//zeCore->SetDebugComponent(&SoundDebugComponent);

	ZEMetaDebugModule MetaDebugComponent;
	//zeCore->SetDebugComponent(&MetaDebugComponent);

	zeCore->GetOptions()->Load("options.ini");
	zeCore->GetOptions()->ResetChanges();
	zeCore->SetGraphicsModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_GRAPHICS));
	zeCore->SetSoundModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_SOUND));
	zeCore->SetInputModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_INPUT));
	zeCore->SetPhysicsModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_PHYSICS));
	ZEConsoleWindow ConsoleWindow;
	zeCore->GetConsole()->SetConsoleInterface(&ConsoleWindow);
	zeCore->GetWindow()->SetWindowType(zeCore->GetOptions()->GetOption("Graphics", "Fullscreen")->GetValue().GetBoolean() ? ZE_WT_FULLSCREEN : ZE_WT_RESIZABLE);
	zeCore->GetWindow()->SetWindowSize(zeCore->GetOptions()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInteger(), zeCore->GetOptions()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInteger());

 	if (zeCore->StartUp())
		zeCore->Run(); 
}





