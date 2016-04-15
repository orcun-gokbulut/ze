//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDScene.cpp
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

#include "ZEDScene.h"

#include "ZEDCore.h"
#include "ZEDEntityWrapper.h"
#include "ZEMeta/ZEObject.h"
#include "ZEDTransformationManager.h"
#include "ZEDGizmo.h"

void ZEDScene::Tick(ZEDObjectWrapper* Wrapper, float ElapsedTime)
{
	if (!Wrapper->GetObjectEnabled())
		return;

	Wrapper->Tick(ElapsedTime);

	const ZEArray<ZEDObjectWrapper*>& SubEntities = Wrapper->GetChildWrappers();
	for (ZESize N = 0; N < SubEntities.GetCount(); N++)
		Tick(SubEntities[N], ElapsedTime);
}

void ZEDScene::AddWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Wrapper == NULL)
		return;

	if (Wrappers.Exists(Wrapper))
		return;

	Wrappers.Add(Wrapper);
}

void ZEDScene::RemoveWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Wrapper == NULL)
		return;

	if (!Wrappers.Exists(Wrapper))
		return;

	Wrappers.RemoveValue(Wrapper);
}

const ZEArray<ZEDObjectWrapper*>& ZEDScene::GetWrappers()
{
	return Wrappers;
}

ZEArray<ZEDObjectWrapper*> ZEDScene::GetWrappers(ZEClass* Class)
{
	ZEArray<ZEDObjectWrapper*> ResultWrappers;
	ZEDObjectWrapper* CurrentWrapper = NULL;
	ResultWrappers.Clear();

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{	
		CurrentWrapper = Wrappers[I];
		if (ZEClass::IsDerivedFrom(Class, CurrentWrapper->GetClass()))
			ResultWrappers.Add(CurrentWrapper);
	}

	return ResultWrappers;
}

ZEDObjectWrapper* ZEDScene::GetWrapper(ZEObject* Object)
{
	if (Object == NULL)
		return NULL;

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{	
		if (Wrappers[I]->GetObject() == Object)
			return Wrappers[I];
	}

	return NULL;
}

void ZEDScene::Tick(float ElapsedTime)
{
	if (!GetEnabled())
		return;

	ZEScene::Tick(ElapsedTime);

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
		Tick(Wrappers[I], ElapsedTime);
}

void ZEDScene::Render(float ElapsedTime)
{
	/*if (GetActiveCamera() == NULL)
		return;

	ZEScene::Render(ElapsedTime);

	ZEDrawParameters* Parameters = GetRenderer()->GetDrawParameters();

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		Wrappers[I]->Draw(Parameters);

		const ZEArray<ZEDObjectWrapper*>& Children = Wrappers[I]->GetChildWrappers();
		for (ZESize J = 0; J < Children.GetCount(); J++)
			Children[J]->Draw(Parameters);
	}

	ZEDTransformationManager::GetInstance()->GetGizmo()->Draw(Parameters);*/
}

bool ZEDScene::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	// DO raycast from wrappers

	bool Result = false;
	if (!Parameters.FilterFunction.IsNull())
	{
		for (ZESize I = 0; I < Wrappers.GetCount(); I++)
		{
			if (Parameters.FilterFunction(static_cast<ZEEntity*>(Wrappers[I]->GetObject()), Parameters.FilterFunctionParameter))
				Result |= Wrappers[I]->RayCast(Report, Parameters);
		}
	}
	else
	{
		for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			Result |= Wrappers[I]->RayCast(Report, Parameters);
	}

	return Result;
}

bool ZEDScene::Initialize()
{
	if (!ZEScene::Initialize())
		return false;

	return true;
}

void ZEDScene::Deinitialize()
{
	ZEScene::Deinitialize();
}

ZEDScene* ZEDScene::CreateInstance()
{
	return new ZEDScene();
}
