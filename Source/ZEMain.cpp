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

#include "ZEGraphics\ZEDirect3D9\ZED3D9TextureResizer.h"

extern HINSTANCE ApplicationInstance;

#include <stdio.h>
#include <string>
#include <string.h>

#define FREEIMAGE_LIB
#include "ZEGraphics\ZEBitmap.h"
#include "FreeImage.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
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

 	zeCore->StartUp();
	
	//////////////////////////////////////////
	///// ZED3D9TextureResizer TEST Start ////
	//////////////////////////////////////////

	ZEBitmap Input; 
	Input.Load("TextureResizeTest\\Inputs\\CarterDiffuse.tga");
	ZEBitmapSamplingOptions Options;
	
	unsigned int SrcWidth = Input.GetWidth();
	unsigned int SrcHeight = Input.GetHeight();
	unsigned int SrcPitch = Input.GetPitch();

	unsigned char* InputBuffer =  new unsigned char[SrcHeight * SrcPitch];
	Input.CopyTo(InputBuffer, SrcPitch, SrcWidth, SrcHeight);
	Input.Release();

	unsigned int DestWidth = SrcWidth / 2;
	unsigned int DestHeight = SrcHeight /*/ 2*/;
	unsigned int DestPitch = SrcPitch / 2;

	unsigned char* OutputBuffer = new unsigned char[SrcHeight * DestPitch];

	ZED3D9TextureResizer	Resizer;
	Resizer.Initialize(OutputBuffer, DestPitch, DestWidth, DestHeight, 
						InputBuffer, SrcPitch, SrcWidth, SrcHeight);
	Resizer.SetAutoFitMode(ZE_D3D9_FPO2_AUTO);
	Resizer.SetResizeFilter(ZE_D3D9_RF_KAISER);
	Resizer.Process();

	Resizer.Deinitialize();

	ZEBitmap Output;
	Output.Create(DestWidth, SrcHeight, 4);
	Output.CopyFrom(OutputBuffer, DestPitch, DestWidth, SrcHeight);
	Output.Save("TextureResizeTest\\2xDownsampled\\CarterDiffuse.tga", ZE_BFF_TGA);
	Output.Release();

	//////////////////////////////////////////
	////// ZED3D9TextureResizer TEST End /////
	//////////////////////////////////////////

	zeCore->Run();
}
