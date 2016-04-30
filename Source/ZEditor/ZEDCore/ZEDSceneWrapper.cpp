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

#include "ZEMath/ZEViewVolume.h"
#include "ZEDEntityWrapper.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGame/ZEScene.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNView.h"

void ZEDSceneWrapper::PreRenderEntity(ZEDEntityWrapper* EntityWrapper, const ZERNPreRenderParameters* Parameters)
{
	if (!EntityWrapper->GetVisible())
		return;

	EntityWrapper->PreRender(Parameters);

	const ZEArray<ZEDObjectWrapper*>& ChildWrappers = EntityWrapper->GetChildWrappers();
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		PreRenderEntity(static_cast<ZEDEntityWrapper*>(ChildWrappers[I]), Parameters);
}

void ZEDSceneWrapper::RayCastEntity(ZEDEntityWrapper* Wrapper, ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	Wrapper->RayCast(Report, Parameters);
	if (Report.CheckDone())
		return;

	const ZEArray<ZEDObjectWrapper*> ChildWrappers = Wrapper->GetChildWrappers();
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		if (!Parameters.Filter(ChildWrappers[I]))
			continue;

		RayCastEntity(static_cast<ZEDEntityWrapper*>(ChildWrappers[I]), Report, Parameters);

		if (Report.CheckDone())
			break;
	}
}

void ZEDSceneWrapper::SetObject(ZEObject* Object)
{
	if (Object == NULL)
		return;

	if (!ZEClass::IsDerivedFrom(ZEScene::Class(), Object->GetClass()))
		return;

	ZEScene* Scene = static_cast<ZEScene*>(Object);
	ZEDObjectWrapper::SetObject(Scene);

	const ZESmartArray<ZEEntity*>& Entities = Scene->GetEntities();
	ChildWrappers.SetCount(Entities.GetCount());

	for (ZESize I = 0; I < Entities.GetCount(); I++)
	{
		ZEClass* TargetClass = Entities[I]->GetClass();

		/*Requires implementation with ZEMeta, making wrappers class identifiers identical with ZEEntity and derivations' class identifiers, 
		thus returning the appropriate wrapper class if it exists, if it does not exist return it's parent's and so on.*/

		/*const ZEArray<ZEClass*>& WrapperTypes = ZEDCore::GetInstance()->GetWrapperTypes();
		
		for (ZESize I = 0; I < WrapperTypes.GetCount(); I++)
		{
			if (WrapperTypes[]) //WrapperTypes[I]->GetAttributes()->Values*
			{
				ZEDObjectWrapper* Wrapper = (ZEDObjectWrapper*)WrapperTypes[I]->CreateInstance();
				Wrapper->SetObject(Object);
			}
		}*/

		ZEDEntityWrapper* Temp = ZEDEntityWrapper::CreateInstance();
		Temp->SetObject(Entities[I]);
		ChildWrappers[I] = Temp;
	}
}

void ZEDSceneWrapper::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		if (!Parameters.Filter(Wrappers[I]))
			continue;;

		Wrappers[I]->RayCast(Report, Parameters);

		if (Report.CheckDone())
			break;
	}
}

ZEDSceneWrapper* ZEDSceneWrapper::CreateInstance()
{
	return new ZEDSceneWrapper();
}

const ZEArray<ZEDObjectWrapper*>& ZEDSceneWrapper::GetChildWrappers()
{
	return *reinterpret_cast<ZEArray<ZEDObjectWrapper*>*>(&Wrappers);
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

	if (Wrappers.Exists(static_cast<ZEDEntityWrapper*>(Wrapper)))
		return;

	ZEScene* Scene = static_cast<ZEScene*>(Object);
	Scene->AddEntity(static_cast<ZEEntity*>(Wrapper->GetObject()));

	Wrappers.Add(static_cast<ZEDEntityWrapper*>(Wrapper));
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

	ZEScene* Scene = static_cast<ZEScene*>(Object);
	Scene->RemoveEntity((ZEEntity*)Wrapper->GetObject());
	
	Wrappers.RemoveValue(static_cast<ZEDEntityWrapper*>(Wrapper));
}

void ZEDSceneWrapper::PreRender(ZERNRenderer* Renderer)
{
	ZERNPreRenderParameters Parameters;
	Parameters.Renderer = Renderer;
	Parameters.View = &Renderer->GetView();

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		Wrappers[I]->PreRender(&Parameters);

		const ZEArray<ZEDObjectWrapper*>& Children = Wrappers[I]->GetChildWrappers();
		for (ZESize J = 0; J < Children.GetCount(); J++)
			Children[J]->PreRender(&Parameters);
	}
}
