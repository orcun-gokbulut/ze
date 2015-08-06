//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDModule.cpp
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

#include "ZEDModule.h"
#include "ZEDScene.h"
#include "ZEDViewPort.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEGrid.h"
#include "ZEModel/ZEModel.h"
#include "ZEDTransformationManager.h"
#include "ZEDCore.h"
#include "ZEDGizmo.h"

ZEDScene* ZEDModule::GetScene()
{
	return Scene;
}

ZEDViewPort* ZEDModule::GetViewPort()
{
	return Viewport;
}

void ZEDModule::Tick(float ElapsedTime)
{
	Scene->Tick(ElapsedTime);
	Viewport->Tick(ElapsedTime);
}

void ZEDModule::Render(float ElapsedTime)
{
	Scene->Render(ElapsedTime);
	Scene->GetRenderer()->Render(ElapsedTime);
	Scene->GetRenderer()->ClearLists();
}

void ZEDModule::StartUp()
{
	if (Scene == NULL)
	{
		Scene = ZEDScene::CreateInstance();
		Scene->Initialize();
	}

	Viewport->Initialize();

	Grid = ZEGrid::CreateInstance();
	Scene->AddEntity(Grid);
	Scene->GetWrapper(Grid)->SetObjectSelectable(false);

	ZEModel* Trial = ZEModel::CreateInstance();
	Trial->SetBoundingBox(ZEAABBox(ZEVector3(-1.0f, -1.0f, -1.0f),ZEVector3(1.0f, 1.0f, 1.0f)));
	Trial->SetUserDefinedBoundingBoxEnabled(true);
	Scene->AddEntity(Trial);

	ZEModel* Trial2 = ZEModel::CreateInstance();
	Trial2->SetBoundingBox(ZEAABBox(ZEVector3(-1.0f, -1.0f, -1.0f),ZEVector3(1.0f, 1.0f, 1.0f)));
	Trial2->SetUserDefinedBoundingBoxEnabled(true);
	Trial2->SetPosition(ZEVector3(5.0f, 0.0f, 5.0f));
	Scene->AddEntity(Trial2);

	Scene->AddEntity(ZEDCore::GetInstance()->GetTransformationManager()->GetGizmo());
	Scene->GetWrapper(ZEDCore::GetInstance()->GetTransformationManager()->GetGizmo())->SetObjectSelectable(false);

	ZEDCore::GetInstance()->GetTransformationManager()->SetTransformType(ZED_TT_TRANSLATE);
	ZEDCore::GetInstance()->GetTransformationManager()->GetGizmo()->SetMode(ZED_GM_MOVE);
}

void ZEDModule::ShutDown()
{
	Viewport->Deinitialize();

	if (Scene != NULL)
	{
		Scene->Deinitialize();
		Scene->Destroy();
		Scene = NULL;
	}
}

ZEDModule::ZEDModule()
{
	Scene = NULL;
	Viewport = new ZEDViewPort();
}

ZEDModule::~ZEDModule()
{

}

