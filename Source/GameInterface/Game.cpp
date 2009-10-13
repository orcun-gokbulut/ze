//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Game.cpp
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

#include "Game.h"
#include "Core/Core.h"
#include "Core/Error.h"

ZEGameDescription* ZEGame::GetGameDescription()
{
	return NULL;
}

ZEScene* ZEGame::GetScene()
{
	return Scene;
}

bool ZEGame::RegisterEntityDescription(ZEEntityDescription* EntityDescription)
{
#ifdef ZEDEBUG_ENABLED
	for (int I = 0; I < EntityDescriptions.GetCount(); I++)
		if (stricmp(EntityDescriptions[I]->GetTypeName(), EntityDescription->GetTypeName()) == 0)
		{
			zeError("Entity Provider", "There is an already registered entity declaration. (Entity Description Name : \"%s\")", EntityDescription->GetTypeName());
			return false;
		}
#endif
	EntityDescriptions.Add(EntityDescription);
	return true;
}

void ZEGame::UnregisterEntityDescription(ZEEntityDescription* EntityDescription)
{
	EntityDescriptions.DeleteValue(EntityDescription);
}

const ZEArray<ZEEntityDescription*> ZEGame::GetEntityDescriptions()
{
	return EntityDescriptions;
}

ZEEntityDescription* ZEGame::GetEntityDescription(size_t Index)
{
	return EntityDescriptions[Index];
}

ZEEntityDescription* ZEGame::GetEntityDescription(const char* EntityTypeName)
{
	for (size_t I = 0; I < EntityDescriptions.GetCount(); I++)
		if (stricmp(EntityTypeName, EntityDescriptions[I]->GetTypeName()) == 0)
			return EntityDescriptions[I];
	return NULL;
}


ZEEntity* ZEGame::CreateEntityInstance(size_t Index)
{
	ZEEntityDescription* Description = GetEntityDescription(Index);
	if (Description == NULL)
	{
		zeError("Game", "Game can not create entity becouse it is not registered. (Entity Index : %d).", Index);
		return NULL;
	}
	else
	{
		ZEEntity* Entity = Description->CreateInstance();
		if (Entity == NULL)
		{
			zeError("Game", "Game can not create instance of entity.  (Entity Index : %d).",  Description->GetTypeName());
			return NULL;
		}
		else
			return Entity;
	}
}

ZEEntity* ZEGame::CreateEntityInstance(const char* EntityTypeName)
{
	ZEEntityDescription* Description = GetEntityDescription(EntityTypeName);
	if (Description == NULL)
	{
		zeError("Game", "Game can not create entity becouse it is not registered. (Entity Index : %d).",  Description->GetTypeName());
		return NULL;
	}
	else
	{
		ZEEntity* Entity = Description->CreateInstance();
		if (Entity == NULL)
		{
			zeError("Game", "Game can not create instance of entity. (Entity Type Name : \"%s\").",  Description->GetTypeName());
			return NULL;
		}
		else
			return Entity;
	}
}

bool ZEGame::Initialize()
{
	if (Scene != NULL)
	{
		Scene->Deinitialize();
		delete Scene;
	}

	Scene = new ZEScene();
	return Scene->Initialize();
}

bool ZEGame::Deinitialize()
{
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
}

void ZEGame::Tick(float ElapsedTime)
{
	Scene->Tick(ElapsedTime);
}

#include "Player.h"
#include "LightBrush.h"
#include "ModelBrush.h"
#include "SkyBrush.h"
ZEGame::ZEGame()
{
	Scene = NULL;
	RegisterEntityDescription(ZEPlayer::EntityDescription());
	RegisterEntityDescription(ZELightBrush::EntityDescription());
	RegisterEntityDescription(ZEModelBrush::EntityDescription());
	RegisterEntityDescription(ZESkyBrush::EntityDescription());
}

ZEGame::~ZEGame()
{
	Deinitialize();
}

ZEGame* ZEGame::GetInstance()
{
	return zeCore->GetGame();
}
