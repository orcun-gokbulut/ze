//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxModelActions.cpp
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

#include "ZE3dsMaxModelActions.h"
#include <maxscrpt/maxscrpt.h>
#include "ZE3dsMaxModelScriptMesh.ms.h"
#include "ZE3dsMaxModelScriptBone.ms.h"
#include "ZE3dsMaxModelScriptPhysicalBody.ms.h"


int ZE3dsMaxModelActionAddMeshAttributes::GetId()
{
	return ZEModelMeshAttributesAdd_Action_ID;
}

BOOL ZE3dsMaxModelActionAddMeshAttributes::ExecuteAction()
{
	ZE3dsMaxModelScriptMesh_ms MeshScript;
	return ExecuteMAXScriptScript((char*)MeshScript.GetData());
}

void ZE3dsMaxModelActionAddMeshAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = "Add ZEModel Mesh Attributes";
}

void ZE3dsMaxModelActionAddMeshAttributes::GetMenuText(MSTR& menuText)
{
	menuText = "Add ZEModel Mesh Attributes";
}

void ZE3dsMaxModelActionAddMeshAttributes::GetDescriptionText(MSTR& descText)
{
	descText = "Add ZEModel Mesh Attributes";
}

void ZE3dsMaxModelActionAddMeshAttributes::GetCategoryText(MSTR& catText)
{
	catText = "Zinek Engine";
}

BOOL ZE3dsMaxModelActionAddMeshAttributes::IsChecked()
{
	return FALSE;
}

BOOL ZE3dsMaxModelActionAddMeshAttributes::IsItemVisible()
{
	return TRUE;
}

BOOL ZE3dsMaxModelActionAddMeshAttributes::IsEnabled()
{
	return TRUE;
}

MaxIcon* ZE3dsMaxModelActionAddMeshAttributes::GetIcon()
{
	if (MeshActionIcon == NULL)
		MeshActionIcon = new MaxBmpFileIcon("ZEToolbarIcons", 1);

	return MeshActionIcon;
}

void ZE3dsMaxModelActionAddMeshAttributes::DeleteThis()
{
	delete MeshActionIcon;
}

ZE3dsMaxModelActionAddMeshAttributes::ZE3dsMaxModelActionAddMeshAttributes()
{
	MeshActionIcon = NULL;
}

int ZE3dsMaxModelActionAddBoneAttributes::GetId()
{
	return ZEModelBoneAttributesAdd_Action_ID;
}

BOOL ZE3dsMaxModelActionAddBoneAttributes::ExecuteAction()
{
	ZE3dsMaxModelScriptBone_ms BoneScript;
	return ExecuteMAXScriptScript((char*)BoneScript.GetData());
}

void ZE3dsMaxModelActionAddBoneAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = "Add ZEModel Bone Attributes";
}

void ZE3dsMaxModelActionAddBoneAttributes::GetMenuText(MSTR& menuText)
{
	menuText = "Add ZEModel Bone Attributes";
}

void ZE3dsMaxModelActionAddBoneAttributes::GetDescriptionText(MSTR& descText)
{
	descText = "Add ZEModel Bone Attributes";
}

void ZE3dsMaxModelActionAddBoneAttributes::GetCategoryText(MSTR& catText)
{
	catText = "Zinek Engine";
}

BOOL ZE3dsMaxModelActionAddBoneAttributes::IsChecked()
{
	return FALSE;
}

BOOL ZE3dsMaxModelActionAddBoneAttributes::IsItemVisible()
{
	return TRUE;
}

BOOL ZE3dsMaxModelActionAddBoneAttributes::IsEnabled()
{
	return TRUE;
}

MaxIcon* ZE3dsMaxModelActionAddBoneAttributes::GetIcon()
{
	if (BoneActionIcon == NULL)
		BoneActionIcon = new MaxBmpFileIcon("ZEToolbarIcons", 2);

	return BoneActionIcon;
}

void ZE3dsMaxModelActionAddBoneAttributes::DeleteThis()
{
	delete BoneActionIcon;
}

ZE3dsMaxModelActionAddBoneAttributes::ZE3dsMaxModelActionAddBoneAttributes()
{
	BoneActionIcon = NULL;
}

int ZE3dsMaxModelActionAddPhysicalBodyAttributes::GetId()
{
	return ZEModelPhysicalBodyAttributesAdd_Action_ID;
}

BOOL ZE3dsMaxModelActionAddPhysicalBodyAttributes::ExecuteAction()
{
	ZE3dsMaxModelScriptPhysicalBody_ms PhysicalBodyScript;
	return ExecuteMAXScriptScript((char*)PhysicalBodyScript.GetData());
}

void ZE3dsMaxModelActionAddPhysicalBodyAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = "Add ZEModel Physical Body Attributes";
}

void ZE3dsMaxModelActionAddPhysicalBodyAttributes::GetMenuText(MSTR& menuText)
{
	menuText = "Add ZEModel Physical Body Attributes";
}

void ZE3dsMaxModelActionAddPhysicalBodyAttributes::GetDescriptionText(MSTR& descText)
{
	descText = "Add ZEModel Physical Body Attributes";
}

void ZE3dsMaxModelActionAddPhysicalBodyAttributes::GetCategoryText(MSTR& catText)
{
	catText	= "Zinek Engine";
}

BOOL ZE3dsMaxModelActionAddPhysicalBodyAttributes::IsChecked()
{
	return FALSE;
}

BOOL ZE3dsMaxModelActionAddPhysicalBodyAttributes::IsItemVisible()
{
	return TRUE;
}

BOOL ZE3dsMaxModelActionAddPhysicalBodyAttributes::IsEnabled()
{
	return TRUE;
}

MaxIcon* ZE3dsMaxModelActionAddPhysicalBodyAttributes::GetIcon()
{
	if (PhysicalBodyActionIcon == NULL)
		PhysicalBodyActionIcon = new MaxBmpFileIcon("ZEToolbarIcons", 3);

	return PhysicalBodyActionIcon;
}

void ZE3dsMaxModelActionAddPhysicalBodyAttributes::DeleteThis()
{
	delete PhysicalBodyActionIcon;
}

ZE3dsMaxModelActionAddPhysicalBodyAttributes::ZE3dsMaxModelActionAddPhysicalBodyAttributes()
{
	PhysicalBodyActionIcon = NULL;
}
