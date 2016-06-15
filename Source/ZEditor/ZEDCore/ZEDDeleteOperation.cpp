//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDDeleteOperation.cpp
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

#include "ZEDDeleteOperation.h"

#include "ZEDObjectWrapper.h"
#include "ZEDObjectEvent.h"

#include "ZEDModule.h"

bool ZEDDeleteOperation::Apply()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		if (Items[I].Parent != NULL)
		{
			Items[I].Parent->RemoveChildWrapper(Items[I].Wrapper);
		
			ZEDObjectEvent Event;
			Event.SetType(ZED_OCET_REMOVED);
			Event.SetWrapper(Items[I].Wrapper);
			GetModule()->DistributeEvent(&Event);
		}

	}

	return true;
}

bool ZEDDeleteOperation::Revert()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		if (Items[I].Parent != NULL)
			Items[I].Parent->AddChildWrapper(Items[I].Wrapper);

		ZEDObjectEvent Event;
		Event.SetType(ZED_OCET_ADDED);
		Event.SetWrapper(Items[I].Wrapper);
		GetModule()->DistributeEvent(&Event);
	}

	return true;
}

ZEDDeleteOperation::ZEDDeleteOperation()
{

}

ZEDDeleteOperation::~ZEDDeleteOperation()
{
	if (GetStatus() != ZED_OS_NOT_DONE)
		return;

	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		if (Items[I].Wrapper != NULL)
			Items[I].Wrapper->Destroy();
	}
}

ZEDDeleteOperation* ZEDDeleteOperation::Create(const ZEArray<ZEDObjectWrapper*>& Wrappers)
{
	ZEDDeleteOperation* Operation = new ZEDDeleteOperation();

	Operation->Items.SetCount(Wrappers.GetCount());
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		Operation->Items[I].Wrapper = Wrappers[I];
		Operation->Items[I].Parent = Wrappers[I]->GetParent();
	}

	Operation->SetText("Delete");

	return Operation;
}
