//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDRelocateOperation.cpp
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

#include "ZEDRelocateOperation.h"

#include "ZEDEditor.h"
#include "ZEDObjectWrapper.h"
#include "ZEDObjectWrapper3D.h"

bool ZEDRelocateOperation::Apply()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		ZEDRelocatedItem& Item = Items[I];
		ZEDObjectWrapper* Wrapper = Item.Wrapper;

		if (Wrapper == NULL)
			continue;
		
		ZEDObjectWrapper3D* Wrapper3D = ZEClass::Cast<ZEDObjectWrapper3D>(Wrapper);
		if (Wrapper3D != NULL)
		{
			ZEVector3 Position = Wrapper3D->GetPosition();
			ZEQuaternion Rotation = Wrapper3D->GetRotation();
			ZEVector3 Scale = Wrapper3D->GetScale();

			if (Item.OldParent != NULL)
				Item.OldParent->RemoveChildWrapper(Wrapper);

			if (Item.NewParent != NULL)
				Item.NewParent->AddChildWrapper(Wrapper);

			Wrapper3D->SetPosition(Position);
			Wrapper3D->SetRotation(Rotation);
			Wrapper3D->SetScale(Scale);
		}
		else
		{
			if (Item.OldParent != NULL)
				Item.OldParent->RemoveChildWrapper(Wrapper);

			if (Item.NewParent != NULL)
				Item.NewParent->AddChildWrapper(Wrapper);
		}
	}

	return true;
}

bool ZEDRelocateOperation::Revert()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		ZEDRelocatedItem& Item = Items[I];
		ZEDObjectWrapper* Wrapper = Item.Wrapper;
		
		ZEDObjectWrapper3D* Wrapper3D = ZEClass::Cast<ZEDObjectWrapper3D>(Wrapper);
		if (Wrapper3D != NULL)
		{
			ZEVector3 Position = Wrapper3D->GetPosition();
			ZEQuaternion Rotation = Wrapper3D->GetRotation();
			ZEVector3 Scale = Wrapper3D->GetScale();

			if (Item.NewParent != NULL)
				Item.NewParent->RemoveChildWrapper(Wrapper3D);

			if (Item.OldParent != NULL)
				Item.OldParent->AddChildWrapper(Wrapper3D);

			Wrapper3D->SetPosition(Position);
			Wrapper3D->SetRotation(Rotation);
			Wrapper3D->SetScale(Scale);
		}
		else
		{

			if (Item.NewParent != NULL)
				Item.NewParent->RemoveChildWrapper(Wrapper);

			if (Item.OldParent != NULL)
				Item.OldParent->AddChildWrapper(Wrapper);
		}
	}

	return true;
}

ZEDRelocateOperation::ZEDRelocateOperation()
{

}

ZEDRelocateOperation* ZEDRelocateOperation::Create(ZEDObjectWrapper* Destination, const ZEArray<ZEDObjectWrapper*>& Wrappers)
{
	ZEDRelocateOperation* Operation = new ZEDRelocateOperation();

	Operation->Items.SetCount(Wrappers.GetCount());
	for (ZESize I = 0; I < Operation->Items.GetCount(); I++)
	{
		Operation->Items[I].Wrapper = Wrappers[I];
		Operation->Items[I].NewParent = Destination;
		Operation->Items[I].OldParent = Wrappers[I]->GetParent();
	}

	Operation->SetText("Relocate");

	return Operation;
}
