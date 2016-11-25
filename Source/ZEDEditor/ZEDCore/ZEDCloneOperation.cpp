//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDCloneOperation.cpp
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

#include "ZEDCloneOperation.h"

#include "ZEDEditor.h"
#include "ZEDObjectWrapper.h"
#include "ZEDObjectManager.h"

bool ZEDCloneOperation::Apply()
{
	Clones.SetCount(Originals.GetCount());
	for (ZESize I = 0; I < Originals.GetCount(); I++)
		Clones[I] = Originals[I]->Clone();

	for (ZESize I = 0; I < Clones.GetCount(); I++)
	{
		ZESSize ParentIndex = -1;
		ZEDObjectWrapper* Parent = Originals[I]->GetParent();
		while(Parent != NULL && Parent->GetParent() != NULL)
		{
			ParentIndex = Originals.FindIndex(Parent);
			if (ParentIndex != -1)
			{
				Clones[ParentIndex]->AddChildWrapper(Clones[I]);
				break;
			}
		}

		if (ParentIndex == -1)
			GetEditor()->GetObjectManager()->GetRootWrapper()->AddChildWrapper(Clones[I]);
	}

	return true;
}

bool ZEDCloneOperation::Revert()
{
	for (ZESize I = 0; I < Clones.GetCount(); I++)
	{
		ZESSize ParentIndex = -1;
		ZEDObjectWrapper* Parent = Clones[I]->GetParent();
		while(Parent != NULL && Parent->GetParent() != NULL)
		{
			ParentIndex = Clones.FindIndex(Parent);
			if (ParentIndex != -1)
			{
				Clones[ParentIndex]->RemoveChildWrapper(Clones[I]);
				break;
			}
		}

		if (ParentIndex == -1)
			GetEditor()->GetObjectManager()->GetRootWrapper()->RemoveChildWrapper(Clones[I]);
	}

	for (ZESize I = 0; I < Clones.GetCount(); I++)
		Clones[I]->GetClass()->Destroy(Clones[I]);

	Clones.Clear();
	return true;
}

ZEDCloneOperation::ZEDCloneOperation()
{

}

ZEDCloneOperation::~ZEDCloneOperation()
{

}

ZEDCloneOperation* ZEDCloneOperation::Create(const ZEArray<ZEDObjectWrapper*>& Wrappers)
{
	ZEDCloneOperation* Operation = new ZEDCloneOperation();

	Operation->Originals = Wrappers;
	Operation->SetText("Clone Objects");

	return Operation;
}
