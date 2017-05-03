//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTISectorWrapper.cpp
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

#include "ZEDTISectorWrapper.h"

#include "ZEMath/ZEViewVolume.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGame/ZEScene.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNView.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEApplications/ZETrainIG/ZETISector.h"

void ZEDTISectorWrapper::SetObject(ZEObject* Object)
{
	if (Object == NULL)
		return;

	if (!ZEClass::IsDerivedFrom(ZETISector::Class(), Object->GetClass()))
		return;

	ZEDObjectWrapper3D::SetObject(Object);
}

ZEString ZEDTISectorWrapper::GetName() const
{
	return "Sector";
}

ZETISector* ZEDTISectorWrapper::GetSector() const
{
	return static_cast<ZETISector*>(GetObject());
}

bool ZEDTISectorWrapper::CheckChildrenClass(ZEClass* Class)
{
	return ZEClass::IsDerivedFrom(ZEEntity::Class(), Class);
}

bool ZEDTISectorWrapper::Load(const ZEString& FileName)
{
	if (GetSector() == NULL)
		return false;

	GetSector()->SetSectorFile(FileName);
	GetSector()->SetIsPopulated(true);
	GetSector()->Load();

	Update();

	return true;
}

bool ZEDTISectorWrapper::Save(const ZEString& FileName)
{
	if (GetSector() == NULL)
		return false;

	bool AccessControl = ZEPathManager::GetInstance()->GetAccessControl();
	ZEPathManager::GetInstance()->SetAccessControl(false);
	return GetSector()->Save(FileName);
	ZEPathManager::GetInstance()->SetAccessControl(AccessControl);
}

ZEDTISectorWrapper* ZEDTISectorWrapper::CreateInstance()
{
	return new ZEDTISectorWrapper();
}

bool ZEDTISectorWrapper::AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	return ZEDEntityWrapper::AddChildWrapper(Wrapper, Update);
}

bool ZEDTISectorWrapper::RemoveChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	return ZEDEntityWrapper::RemoveChildWrapper(Wrapper, Update);
}

void ZEDTISectorWrapper::PreRender(const ZERNPreRenderParameters* Parameters)
{
	ZETISector* Sector = static_cast<ZETISector*>(GetObject());
	
	if (Sector->GetScene() != NULL)
		Sector->GetScene()->PreRender(Parameters);
	
	ZEDEntityWrapper::PreRender(Parameters);
}
