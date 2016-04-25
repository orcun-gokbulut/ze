//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSceneWrapper.cpp
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

#include "ZEDSceneWrapper.h"

#include "ZEGame/ZEEntity.h"
#include "ZEDEntityWrapper.h"

void ZEDSceneWrapper::PreRender(ZERNRenderer* Renderer, ZEDEntityWrapper* Wrapper)
{
	/*for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		Wrappers[I].PreRender();
	}*/
}

void ZEDSceneWrapper::SetObject(ZEObject* Object)
{
	if (Object == NULL)
		return;

	if (!ZEClass::IsDerivedFrom(ZEDScene::Class(), Object->GetClass()))
		return;

	ZEDScene* Scene = (ZEDScene*)Object;

	ZEDObjectWrapper::SetObject(Scene);

	const ZESmartArray<ZEEntity*>& Entities = Scene->GetEntities();
	ChildWrappers.SetCount(Entities.GetCount());

	for (ZESize I = 0; I < Entities.GetCount(); I++)
	{
		ZEClass* TargetClass = Entities[I]->GetClass();

		/*Requires implementation with ZEMeta, making wrappers class identifiers identical with ZEEntity and derivations' class identifiers, 
		thus returning the appropriate wrapper class if it exists, if it does not exist return it's parent's and so on.*/

// 		const ZEArray<ZEClass*>& WrapperTypes = ZEDCore::GetInstance()->GetWrapperTypes();
// 
// 		for (ZESize I = 0; I < WrapperTypes.GetCount(); I++)
// 		{
// 			if (WrapperTypes[]) //WrapperTypes[I]->GetAttributes()->Values*
// 			{
// 				ZEDObjectWrapper* Wrapper = (ZEDObjectWrapper*)WrapperTypes[I]->CreateInstance();
// 				Wrapper->SetObject(Object);
// 			}
// 		}

		ZEDEntityWrapper* Temp = ZEDEntityWrapper::CreateInstance();
		Temp->SetObject(Entities[I]);
		ChildWrappers[I] = Temp;
	}
}

bool ZEDSceneWrapper::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	if (Object == NULL)
		return false;

	return ((ZEDScene*)Object)->RayCast(Report, Parameters);
}

ZEDSceneWrapper* ZEDSceneWrapper::CreateInstance()
{
	return new ZEDSceneWrapper();
}

const ZEArray<ZEDObjectWrapper*>& ZEDSceneWrapper::GetChildWrappers()
{
	if (Object == NULL)
		return ZEDObjectWrapper::GetChildWrappers();

	return ((ZEDScene*)Object)->GetWrappers();
}

void ZEDSceneWrapper::AddChildWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Object == NULL)
		return;

	if (Wrapper == NULL)
		return;

	if (Wrapper->GetObject() == NULL)
		return;

	if (!ZEClass::IsDerivedFrom(ZEDEntityWrapper::Class(), Wrapper->GetClass()))
		return;

	ZEDScene* Scene = (ZEDScene*)Object;

	Scene->AddEntity((ZEEntity*)Wrapper->GetObject());
	Scene->AddWrapper(Wrapper);
}

void ZEDSceneWrapper::RemoveChildWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Object == NULL)
		return;

	if (Wrapper == NULL)
		return;

	if (Wrapper->GetObject() == NULL)
		return;

	if (!ZEClass::IsDerivedFrom(ZEDEntityWrapper::Class(), Wrapper->GetClass()))
		return;

	ZEDScene* Scene = (ZEDScene*)Object;

	Scene->RemoveEntity((ZEEntity*)Wrapper->GetObject());
	Scene->RemoveWrapper(Wrapper);
}

void ZEDSceneWrapper::PreRender(ZERNRenderer* Renderer)
{
	
}

