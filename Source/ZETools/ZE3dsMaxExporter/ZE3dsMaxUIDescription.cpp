//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxUIDescription.cpp
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

#include "ZE3dsMaxUIDescription.h"
#include "ZE3dsMaxUI.h"
#include "ZE3dsMaxInteriorActions.h"
#include "ZE3dsMaxModelActions.h"
#include "ZEDS/ZEString.h"

#define ZE_3DS_MAX_UI_CLASS_ID Class_ID(0x9b240b2, 0x2c757d96)

INT ZE3dsMaxUIDescription::IsPublic()
{ 
	return TRUE; 
}

void* ZE3dsMaxUIDescription::Create(BOOL Loading) 
{ 
	return new ZE3dsMaxUI(); 
}

const TCHAR* ZE3dsMaxUIDescription::ClassName()
{ 
	return ZEString("ZE3dsMaxUI"); 
}

SClass_ID ZE3dsMaxUIDescription::SuperClassID()
{ 
	return GUP_CLASS_ID; 
}

Class_ID ZE3dsMaxUIDescription::ClassID()
{ 
	return ZE_3DS_MAX_UI_CLASS_ID; 
}

const TCHAR* ZE3dsMaxUIDescription::Category()
{
	return ZEString("Zinek");
}

const TCHAR* ZE3dsMaxUIDescription::InternalName()
{ 
	return ZEString("Zinek Engine UI");
}

HINSTANCE ZE3dsMaxUIDescription::HInstance()
{ 
	return hInstance; 
}

int ZE3dsMaxUIDescription::NumActionTables()
{
	return 1;
}

ActionTable* ZE3dsMaxUIDescription::GetActionTable(int i)
{
	ActionTable* ZinekActionTable = new ActionTable(ZE3dsMax_Action_Table_ID.PartA(), kActionMainUIContext, MSTR::FromCStr("Zinek Engine"));
	ZinekActionTable->AppendOperation(new ZE3dsMaxModelActionAddMeshAttributes());
	ZinekActionTable->AppendOperation(new ZE3dsMaxModelActionAddBoneAttributes());
	ZinekActionTable->AppendOperation(new ZE3dsMaxModelActionAddPhysicalBodyAttributes());
	ZinekActionTable->AppendOperation(new ZE3dsMaxModelActionAddBoundingBoxAttributes());
	ZinekActionTable->AppendOperation(new ZE3dsMaxInteriorActionAddRoomAttributes());
	ZinekActionTable->AppendOperation(new ZE3dsMaxInteriorActionAddDoorAttributes());
	ZinekActionTable->AppendOperation(new ZE3dsMaxCommonUtilsActionAddHelperAttributes());
	ZinekActionTable->AppendOperation(new ZE3dsMaxCommonUtilsRemoveAttributes());

	ZE3dsMaxActionCallback* ActionTableCallback = new ZE3dsMaxActionCallback(ZinekActionTable);
	ZinekActionTable->SetCallback(ActionTableCallback);

	return ZinekActionTable;
}

ClassDesc2* ZE3dsMaxUIDescription::GetInstance()
{
	static ZE3dsMaxUIDescription Desc;
	return &Desc;
}
