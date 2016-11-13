//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMenuManager.cpp
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

#include "ZEDMenuManager.h"

#include "ZEDMenu2.h"
#include "ZEML/ZEMLReader.h"

ZEDMenuManager::ZEDMenuManager()
{

}

ZEDMenuManager::~ZEDMenuManager()
{
	for (ZESize I = 0; I < Menus.GetCount(); I++)
		Menus[I]->Destroy();
	Menus.Clear();
}

const ZEArray<ZEDMenu2*>& ZEDMenuManager::GetMenus()
{
	return Menus;
}

ZEDMenu2* ZEDMenuManager::GetMenu(const ZEString& Name)
{
	for (ZESize I = 0; I < Menus.GetCount(); I++)
	{
		if (Menus[I]->GetName() == Name)
			return Menus[I];
	}

	return NULL;
}

bool ZEDMenuManager::AddMenu(ZEDMenu2* Menu)
{
	zeCheckError(Menu == NULL, false, "Cannot add menu. Menu is NULL.");
	zeCheckError(Menus.Exists(Menu), false, "Cannot add menu. Menu has been already added. Menu Name: \"%s\".", Menu->GetName().ToCString());

	for (ZESize I = 0; I < Menus.GetCount(); I++)
	{
		if (Menus[I]->GetName() == Menu->GetName())
		{
			zeError("Cannot add menu. Another menu with the same name has been already added. Menu Name: \"%s\".", Menu->GetName().ToCString());
			return false;
		}
	}

	Menu->Manager = this;
	Menus.Add(Menu);

	return true;
}

bool ZEDMenuManager::RemoveMenu(ZEDMenu2* Menu)
{
	zeCheckError(Menu == NULL, false, "Cannot remove menu. Menu is NULL.");
	zeCheckError(!Menus.Exists(Menu), false, "Cannot remove menu. Menu is not added. Menu Name: \"%s\".", Menu->GetName().ToCString());

	Menu->Manager = NULL;
	Menus.Add(Menu);

	return true;
}

bool ZEDMenuManager::Load(const ZEString& ConfigurationFile)
{
	ZEMLReader Reader;
	if (!Reader.Open(ConfigurationFile))
	{
		zeError("Cannor load configuration. File Name: \"%s\".", ConfigurationFile.ToCString());
		return false;
	}

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (!RootNode.IsValid() || RootNode.GetName() != "ZEDMenuManager")
	{
		zeError("Cannor load configuration. Unknown structure. File Name: \"%s\".", ConfigurationFile.ToCString());
		return false;
	}

	ZEMLReaderNode MenusNode = RootNode.GetNode("Menus");
	if (MenusNode.IsValid())
	{
		ZESize MenuNodeCount = MenusNode.GetNodeCount("Menu");
		for (ZESize I = 0; I < MenuNodeCount; I++)
		{
			ZEMLReaderNode& MenuNode = MenusNode.GetNode("Menu", I);
			ZEDMenu2* NewMenu = ZEDMenu2::CreateInstance();
			NewMenu->Load(&MenuNode);
			AddMenu(NewMenu);
		}
	}

	return true;
}

bool ZEDMenuManager::Save(const ZEString& ConfigurationFile)
{
	return false;
}

void ZEDMenuManager::Update()
{
	for (ZESize I = 0; I < Menus.GetCount(); I++)
		Menus[I]->Update();
}

void ZEDMenuManager::Destroy()
{
	delete this;
}

ZEDMenuManager* ZEDMenuManager::CreateInstance()
{
	return new ZEDMenuManager;
}
