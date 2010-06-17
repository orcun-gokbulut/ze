//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundDebugModule.cpp
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

#include "ZESoundDebugModule.h"

#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGame\ZEGame.h"
#include "ZEGame\ZEPlayer.h"
#include "ZEGame\ZEScene.h"
#include "ZESound\ZESoundSource3D.h"

bool ZESoundDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	// Create the player
	if (Player == NULL)
	{
		Player = (ZEPlayer*)zeGame->CreateEntityInstance("ZEPlayer");
		Player->SetPosition(ZEVector3(0.0f, 5.0f, 0.0f));
		Player->SetRotation(ZEQuaternion::Identity);
		Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
		Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
		Scene->SetActiveCamera(Player->GetCamera());
		Scene->AddEntity(Player);
	}

	if (SoundSource == NULL)
	{
		ZESoundSource3D* SoundSource = ZESoundSource3D::CreateInstance();

		SoundSource->SetSoundResource(ZESoundResource::LoadResource("test.wav"));
		SoundSource->SetCurrentPositionTime(50);
		SoundSource->SetStartPositionTime(0);
		/*SoundSource->SetEndPositionTime(70.5f);*/
		SoundSource->SetPlaybackSpeed(1.0f);
		SoundSource->SetLooping(true);
		SoundSource->Play();
		SoundSource->SetSoundSourceType(ZE_SST_MUSIC);
	}

	return true;
}

void ZESoundDebugModule::Deinitialize()
{
	if (Player != NULL)
	{
		Player->Destroy();
		Player = NULL;
	}

	if (SoundSource != NULL)
	{
		SoundSource->Destroy();
		SoundSource = NULL;
	}
}

ZESoundDebugModule::ZESoundDebugModule()
{
	SoundSource = NULL;
	Player = NULL;
}

ZESoundDebugModule::~ZESoundDebugModule()
{
	Deinitialize();
}



