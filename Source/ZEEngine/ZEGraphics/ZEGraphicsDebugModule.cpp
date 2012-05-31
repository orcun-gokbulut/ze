//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDebugModule.cpp
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


#include "ZEGraphicsDebugModule.h"
#include "ZEGame\ZEScene.h"
#include "ZEGame\ZEGizmo.h"
#include "ZEGame\ZEGrid.h"
#include "ZEGame\ZEGame.h"
#include "ZEGame\ZEPlayer.h"
#include "ZEGraphics\ZECamera.h"
#include "ZEMath\ZEAngle.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEMath\ZEVector.h"


bool ZEGraphicsDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	Grid = ZEGrid::CreateInstance();
	Grid->SetPosition(ZEVector3(0.01f, 0.01f, 0.01f));
	Scene->AddEntity(Grid);

	Gizmo = ZEGizmo::CreateInstance();
	Gizmo->SetPosition(ZEVector3(-0.01f, -0.01f, -0.01f));
	Scene->AddEntity(Gizmo);

	PlayerSteering = new ZESteeringPlayerFree();

	Player = ZEPlayer::CreateInstance();
	Player->SetName("TestPlayer1");
	Player->SetEnabled(true);
	Player->SetVisible(true);
	Player->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Player->SetRotation(ZEQuaternion::Identity);
	Player->GetCamera()->SetNearZ(0.01f);
	Player->GetCamera()->SetFarZ(200.0f);
	Player->GetCamera()->SetFOV(ZE_PI / 4.0f);
	Player->AddSteering(PlayerSteering);
	Player->SetMaxLinearVelocity(20.0f);
	Player->SetMaxLinearAcceleration(20.0f);
	Scene->SetActiveCamera(Player->GetCamera());
	Scene->AddEntity(Player);

	return true;
}

void ZEGraphicsDebugModule::Deinitialize()
{
	if (Grid != NULL)
	{
		Grid->Destroy();
		Grid = NULL;
	}
	if (Gizmo != NULL)
	{
		Gizmo->Destroy();
		Gizmo = NULL;
	}
	if (Player != NULL)
	{
		Player->Destroy();
		Player = NULL;
	}
	if (PlayerSteering != NULL)
	{
		delete PlayerSteering;
		PlayerSteering = NULL;
	}
}

void ZEGraphicsDebugModule::Process(float ElapsedTime)
{

}

ZEGraphicsDebugModule::ZEGraphicsDebugModule()
{
	Grid = NULL;
	Gizmo = NULL;
	Player = NULL;
	PlayerSteering = NULL;
}

ZEGraphicsDebugModule::~ZEGraphicsDebugModule()
{
	Deinitialize();
}
