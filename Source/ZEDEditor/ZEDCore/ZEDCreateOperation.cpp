//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDCreateOperation.cpp
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

#include "ZEDCreateOperation.h"

#include "ZEDEditor.h"
#include "ZEDObjectWrapper.h"
#include "ZEDObjectManager.h"

bool ZEDCreateOperation::Apply()
{
	if (Parent == NULL)
		return false;

	if (!Parent->CheckChildrenClass(TargetClass))
		return false;

	ZEClass* WrapperClass = GetEditor()->GetObjectManager()->FindWrapperClass(TargetClass);
	if (WrapperClass == NULL)
		return false;

	ZEObject* Object = TargetClass->CreateInstance();
	if (Object == NULL)
		return false;

	if (Wrapper == NULL)
		Wrapper = static_cast<ZEDObjectWrapper*>(WrapperClass->CreateInstance());

	if (Wrapper == NULL)
		return false;

	Wrapper->SetObject(Object);

	if (!Parent->AddChildWrapper(Wrapper))
		return false;

	return true;
}

bool ZEDCreateOperation::Revert()
{
	if (Parent == NULL)
		return false;
	
	if (Wrapper == NULL)
		return false;

	if (!Parent->RemoveChildWrapper(Wrapper))
		return false;

	return true;
}

ZEDCreateOperation::ZEDCreateOperation()
{
	TargetClass = NULL;
	Wrapper = NULL;
	Parent = NULL;
}

ZEDCreateOperation::~ZEDCreateOperation()
{
	if (GetStatus() != ZED_OS_DONE)
	{
		if (Wrapper->GetObject() != NULL)
			delete Wrapper->GetObject();

		if (Wrapper != NULL)
			Wrapper->Destroy();
	}
}

ZEDCreateOperation* ZEDCreateOperation::Create(ZEClass* Class, ZEDObjectWrapper* Parent)
{
	ZEDCreateOperation* Operation = new ZEDCreateOperation();

	Operation->TargetClass = Class;
	Operation->Wrapper = NULL;
	Operation->Parent = Parent;
	Operation->SetText("Create Object");

	return Operation;
}
