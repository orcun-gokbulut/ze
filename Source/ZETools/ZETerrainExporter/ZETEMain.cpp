//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEMain.cpp
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

#include "ZETEPatchDatabase.h"
#include "ZETEProcessor.h"
#include "ZETEPatchFile.h"
#include "ZETEPatchGDAL.h"
#include "ZEFile/ZEPathManager.h"

#include <FreeImage.h>
#include <gdal.h>

void main()
{
	GDALAllRegister();
	FreeImage_Initialise();

	ZEPathManager::GetInstance()->SetAccessControl(false);

	ZETEProcessor Procesor;
	Procesor.SetDebugDump(true);
	Procesor.SetRegenerate(true);

	ZETEPatchDatabase PatchDatabase;
	PatchDatabase.SetBlockSize(1024);
	PatchDatabase.SetPixelType(ZETE_PT_COLOR);
	PatchDatabase.SetPath("C:/Users/orcun.gokbulut/Desktop/ZinekEngine/ZE/branches/v0.6.1-NewTerrain/RunDir/Resources/Terrain/Patches");
	Procesor.SetPatchDatabase(&PatchDatabase);

	/*ZETEBlockDatabase BlockDatabase;
	BlockDatabase.SetBlockSize(1024);
	BlockDatabase.SetPixelType(ZETE_PT_ELEVATION);
	BlockDatabase.SetPath("C:/Users/orcun.gokbulut/Desktop/ZinekEngine/ZE/branches/v0.6.1-NewTerrain/RunDir/Resources/Terrain/Layers/Elevation");
	Procesor.SetBlockDatabase(&BlockDatabase);
	
	ZETEPatch* Patch;
	Patch = new ZETEPatchFile();
	Patch->SetSource("C:/Users/orcun.gokbulut/Desktop/Elevation.png");
	Patch->SetPriority(0);
	Patch->SetPositionX(0);
	Patch->SetPositionY(0);
	Patch->SetEndX(11171);
	Patch->SetEndY(9542);
	PatchDatabase.AddPatch(Patch);*/

	ZETEBlockDatabase BlockDatabase;
	BlockDatabase.SetBlockSize(1024);
	BlockDatabase.SetPixelType(ZETE_PT_COLOR);
	BlockDatabase.SetPath("C:/Users/orcun.gokbulut/Desktop/ZinekEngine/ZE/branches/v0.6.1-NewTerrain/RunDir/Resources/Terrain/Layers/Color");
	Procesor.SetBlockDatabase(&BlockDatabase);

	ZETEPatch* Patch;
	Patch = new ZETEPatchFile();
	Patch->SetSource("C:/Users/orcun.gokbulut/Desktop/elevation.png");
	Patch->SetPriority(0);
	Patch->SetPositionX(0);
	Patch->SetPositionY(0);
	Patch->SetEndX(11171);
	Patch->SetEndY(9542);
	PatchDatabase.AddPatch(Patch);

	Procesor.Generate();

	BlockDatabase.SaveDatabase();
	
	FreeImage_DeInitialise();
}
