//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPlugInManager.cpp
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

#include "ZEDPlugInManager.h"
#include "ZEDSoundExtensionsPlugIn/ZEDSoundExtensionsPlugIn.h"
#include "ZEDImageExtensionsPlugIn/ZEDImageExtensionsPlugIn.h"

QList<ZEDPlugIn*> ZEDPlugInManager::PlugIns = QList<ZEDPlugIn*>();
QList<ZEDExtension*> ZEDPlugInManager::Extensions = QList<ZEDExtension*>();

void ZEDPlugInManager::ReadPlugInsFromFile()
{
	//Implement DLL Reading
}

QList<ZEDExtension*> ZEDPlugInManager::GetExtensions()
{
	return Extensions;
}

bool ZEDPlugInManager::RegisterPlugIn(ZEDPlugIn* NewPlugIn)
{
	bool Exists = false;

	for (ZEInt I = 0; I < PlugIns.count(); I++)
	{
		if (strcmp(NewPlugIn->GetName(), PlugIns[I]->GetName()) == 0)
			Exists = true;
	}

	if(!Exists)
	{
		PlugIns.append(NewPlugIn);

		for (ZESize I = 0; I < NewPlugIn->GetExtensionCount(); I++)
		{
			ZEDExtensionDescription* Desc = NewPlugIn->GetExtensionDescriptions()[I];
			Extensions.append(Desc->CreateInstance());
		}

		return true;
	}

	else
		return false;
}

void ZEDPlugInManager::LoadInternalPlugIns()
{
	RegisterPlugIn(new ZEDSoundExtensionsPlugIn());
	RegisterPlugIn(new ZEDImageExtensionsPlugIn());
}

ZEDPlugInManager::ZEDPlugInManager()
{
	PlugIns.clear();
	Extensions.clear();

	LoadInternalPlugIns();
	ReadPlugInsFromFile();
}

ZEDPlugInManager::~ZEDPlugInManager()
{

}

ZEDPlugInManager* ZEDPlugInManager::GetInstance()
{
	static ZEDPlugInManager Manager;
	return &Manager;
}
