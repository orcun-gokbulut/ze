//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGame.cpp
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

#include "ZEGame.h"
#include "ZEError.h"
#include "ZECore/ZECore.h"
#include "ZEUI/ZEUIManager.h"
#include "ZERenderer/ZECamera.h"
#include "ZEMath/ZEAngle.h"

#include <string.h>

ZEScene* ZEGame::GetScene()
{
	return Scene;
}

#include "ZEUI/ZEFontResource.h"

bool ZEGame::Initialize()
{
	if (UIManager == NULL)
	{
		UIManager = ZEUIManager::CreateInstance();
		UIManager->Initialize();
	}

	if (Scene != NULL)
	{
		Scene->Deinitialize();
		delete Scene;
	}

	if (Scene == NULL)
		Scene = new ZEScene();
	
	return Scene->Initialize();
}

bool ZEGame::Deinitialize()
{
	if (UIManager != NULL)
	{
		UIManager->Destroy();
		UIManager = NULL;
	}

	if (Scene != NULL)
	{
		Scene->Destroy();
		Scene = NULL;
	}

	return true;
}

void ZEGame::Reset()
{

}

void ZEGame::Destroy()
{
	Deinitialize();
	delete this;
}

void ZEGame::Render(float ElapsedTime)
{
	Scene->Render(ElapsedTime);
	UIManager->Render(Scene->GetRenderer());
	Scene->GetRenderer()->Render(ElapsedTime);
	Scene->GetRenderer()->ClearLists();
}

void ZEGame::Tick(float ElapsedTime)
{	
	Scene->Tick(ElapsedTime);
	UIManager->Tick(ElapsedTime);
}

ZEGame::ZEGame()
{
	Scene = NULL;
	UIManager = NULL;
}

ZEGame::~ZEGame()
{
	Deinitialize();
}

ZEGame* ZEGame::GetInstance()
{
	return zeCore->GetGame();
}
