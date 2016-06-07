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

	ZEDObjectWrapper::SetObject(Object);
}

ZEString ZEDSceneWrapper::GetName()
{
	return "Scene";
}

ZEScene* ZEDSceneWrapper::GetScene()
{
	return static_cast<ZEScene*>(GetObject());
}

void ZEDSceneWrapper::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ZEArray<ZEDObjectWrapper*> Wrappers = GetChildWrappers();
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		if (!Parameters.Filter(Wrappers[I]))
			continue;

		Wrappers[I]->RayCast(Report, Parameters);

		if (Report.CheckDone())
			break;
	}
}

void ZEDSceneWrapper::Update()
{
	ZEScene* Scene = GetScene();
	const ZESmartArray<ZEEntity*>& Entities = Scene->GetEntities();

	// Remove
	const ZEArray<ZEDObjectWrapper*>& Wrappers = GetChildWrappers();

	for (ZESSize I = Wrappers.GetCount() - 1; I >= 0; I--)
	{
		bool Found = false;
		for (ZESize N = 0; N < Entities.GetCount(); N++)
		{
			if (Wrappers[I]->GetObject() == Entities[N])
			{
				Found = true;
				break;
			}
		}

		if (!Found)
			Wrappers[I]->Destroy();
	}

	// Add
	for (ZESize I = 0; I < Entities.GetCount(); I++)
	{
		ZEDEntityWrapper* Wrapper = NULL;
		const ZEArray<ZEDObjectWrapper*>& Wrappers = GetChildWrappers();
		for (ZESize N = 0; N < Wrappers.GetCount(); N++)
		{
			if (Wrappers[N]->GetObject() == Entities[I])
			{
				Wrapper = static_cast<ZEDEntityWrapper*>(Wrappers[N]);
				break;
			}
		}

		if (Wrapper == NULL)
		{
			Wrapper = ZEDEntityWrapper::CreateInstance();
			Wrapper->SetObject(Entities[I]);
			AddChildWrapper(Wrapper, true);
		}
	}
}

ZEDSceneWrapper* ZEDSceneWrapper::CreateInstance()
{
	return new ZEDSceneWrapper();
}

bool ZEDSceneWrapper::AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	if (Wrapper != NULL && !ZEClass::IsDerivedFrom(ZEDEntityWrapper::Class(), Wrapper->GetClass()))
		return false;

	if (!ZEDObjectWrapper::AddChildWrapper(Wrapper, Update))
		return false;

	if (!Update)
	{
		if (GetScene() != NULL)
			GetScene()->AddEntity(static_cast<ZEEntity*>(Wrapper->GetObject()));
	}

	return true;
}

bool ZEDSceneWrapper::RemoveChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	if (!ZEDObjectWrapper::RemoveChildWrapper(Wrapper, Update))
		return false;

	if (!Update)
	{
		if (GetScene() != NULL)
			GetScene()->RemoveEntity(static_cast<ZEEntity*>(Wrapper->GetObject()));
	}

	return true;
}

void ZEDSceneWrapper::PreRender(const ZERNPreRenderParameters* Parameters)
{
	Parameters->Renderer->StartScene(NULL);

	GetScene()->PreRender(Parameters);
	ZEArray<ZEDObjectWrapper*> Wrappers = GetChildWrappers();
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
		PreRenderEntity(static_cast<ZEDEntityWrapper*>(Wrappers[I]), Parameters);

	Parameters->Renderer->EndScene();
}
