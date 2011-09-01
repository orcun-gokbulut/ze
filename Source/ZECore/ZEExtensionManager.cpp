//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEExtensionManager.cpp
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

#include "ZEExtensionManager.h"
#include "ZECore/ZEError.h"
#include "ZEOptionManager.h"
#include "ZEExtensionDescription.h"
#include "ZEModuleDescription.h"
#include "ZECore/ZECore.h"

#include <string.h>

ZEOptionSection ZEExtensionManager::ExtensionManagerOptions;

ZEExtensionDescription* ZEExtensionManager::GetExtensionDescription(const char* Name)
{
	for (size_t I = 0; I < ExtensionList.GetCount(); I++)
		if (_stricmp(ExtensionList[I]->GetName(), Name) == 0)
			return ExtensionList[I];

	return NULL;
}

const ZEArray<ZEExtensionDescription*>& ZEExtensionManager::GetExtensionDescriptions()
{
	return ExtensionList;
}

ZEArray<ZEExtensionDescription*> ZEExtensionManager::GetExtensionDescriptions(ZEExtensionDescription* ParentExtension)
{
	if (ParentExtension == NULL)
		return ExtensionList;

	ZEArray<ZEExtensionDescription*> List;
	for (size_t I = 0; I < ExtensionList.GetCount(); I++)
	{
		ZEExtensionDescription* Current = ExtensionList[I]->GetParent();
		
		while(Current != NULL)
		{
			if (ParentExtension == Current)
			{
				List.Add(ExtensionList[I]);
				break;
			}

			Current = Current->GetParent();
		}
	}

	return List;
}


bool ZEExtensionManager::RegisterExtension(ZEExtensionDescription* ExtensionDescription)
{
	char ExtensionVersion[255];
	if (ExtensionDescription->GetOptions() != NULL)
	{
		bool Result;
		Result = ZEOptionManager::GetInstance()->RegisterSection(ExtensionDescription->GetOptions());
		if (!Result)
		{
			ExtensionDescription->GetVersion().GetShortString(ExtensionVersion);
			zeError("Module Manager", "Can not register extensions's option section. Extension Name : \"%s\", Extension Version : %s)", 
				ExtensionDescription->GetName(), ExtensionVersion);
		}
	}

	ExtensionList.Add(ExtensionDescription);
	return true;
}

void ZEExtensionManager::UnregisterExtension(ZEExtensionDescription* ExtensionDescription)
{
	ExtensionList.DeleteValue(ExtensionDescription);
}

class ZEExtensionManager* ZEExtensionManager::GetInstance()
{
	return ZECore::GetInstance()->GetExtensionManager();
}

#include "ZEInput/ZEVRPNInput/ZEVRPNInputDeviceExtensionDescription.h"
#include "ZEInput/ZEFreeSpaceInput/ZEFreespaceInputDeviceExtensionDescription.h"
ZEExtensionManager::ZEExtensionManager()
{
	RegisterExtension(new ZEVRPNInputDeviceExtensionDescription());
	RegisterExtension(new ZEFreespaceInputDeviceExtensionDescription());
}

ZEExtensionManager::~ZEExtensionManager()
{

}
