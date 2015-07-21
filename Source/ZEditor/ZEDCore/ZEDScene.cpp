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

bool ZEDScene::ExportZEDScene()
{
	const ZESmartArray<ZEEntity*>& Wrappers = GetEntities();
	ZESize Count = Wrappers.GetCount();

	for (ZESize I = 0; I < Count; I++)
	{
		ZEDObjectWrapper* CurrentWrapper = (ZEDObjectWrapper*)Wrappers[I];
		CurrentWrapper->GetObject();



	}
}

bool ZEDScene::ImportZEDScene()
{

}

void ZEDScene::AddWrapper(ZEObject* Object)
{
	ZEClass* TargetClass = Object->GetClass();

	/*Requires implementation with ZEMeta, making wrappers class identifiers identical with ZEEntity and derivations' class identifiers, 
	thus returning the appropriate wrapper class if it exists, if it does not exist return it's parent's and so on.*/

	const ZEArray<ZEClass*>& WrapperTypes = ZEDCore::GetInstance()->GetWrapperTypes();

	for (ZESize I = 0; I < WrapperTypes.GetCount(); I++)
	{
		if (false)//WrapperTypes[I]->GetAttributes()->Values*
		{
			ZEDObjectWrapper* Wrapper = (ZEDObjectWrapper*)WrapperTypes[I]->CreateInstance();
			Wrapper->SetObject(Object);
		}
	}

	if (ZEClass::IsDerivedFrom(ZEEntity::Class(), TargetClass))
	{
		AddEntity((ZEEntity*)Object);
	}

}

void ZEDScene::RemoveWrapper(ZEObject* Object)
{
	
}

const ZESmartArray<ZEDObjectWrapper*>& ZEDScene::GetWrappers()
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

void ZEDScene::Tick(ZEDObjectWrapper* Wrapper, float ElapsedTime)
{
	if (!Wrapper->GetObjectEnabled())
		return;

	Wrapper->Tick(ElapsedTime);

	const ZEArray<ZEDObjectWrapper*>& Components = Wrapper->GetComponentWrappers();
	for (ZESize N = 0; N < Components.GetCount(); N++)
		Tick(Components[N], ElapsedTime);


	const ZEArray<ZEDObjectWrapper*>& SubEntities = Wrapper->GetChildWrappers();
	for (ZESize N = 0; N < SubEntities.GetCount(); N++)
		Tick(SubEntities[N], ElapsedTime);
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

}

bool ZEDScene::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	// DO raycast from wrappers
}

bool ZEDScene::Initialize()
{
	if (!ZEScene::Initialize())
		return false;



	return true;
}

void ZEDScene::Deinitialize()
{


	return ZEDScene::Deinitialize();
}

ZEDScene::ZEDScene()
{

}

ZEDScene* ZEDScene::CreateInstance()
{
	return new ZEDScene();
}
