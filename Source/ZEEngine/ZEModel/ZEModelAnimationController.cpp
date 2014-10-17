//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationController.cpp
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

#include "ZEModelAnimationController.h"
#include "ZEModelAnimationNode.h"
#include "ZEModel.h"

ZEModelAnimationController::ZEModelAnimationController()
{
	State = ZE_MAS_STOPPED;
	OutputNode = NULL;
}

void ZEModelAnimationController::Initialize(ZEModel* Owner)
{
	this->Owner = Owner;
}

void ZEModelAnimationController::Deinitialize()
{
	AnimationTree.SetCount(0);
}

ZEModelAnimationController::~ZEModelAnimationController()
{

}

void ZEModelAnimationController::SetAnimationOutputNode(ZEModelAnimationNode* Node)
{
	OutputNode = Node;
}

ZEModelAnimationNode* ZEModelAnimationController::GetAnimationOutputNode()
{
	return OutputNode;
}

void ZEModelAnimationController::AddAnimationNode(ZEModelAnimationNode* Node)
{
	if (AnimationTree.Exists(Node))
		return;

	//Node->SetOwner(this);
	AnimationTree.Add(Node);
}

void ZEModelAnimationController::RemoveAnimationNode(ZEModelAnimationNode* Node)
{
	if (!AnimationTree.Exists(Node))
		return;

	AnimationTree.DeleteValue(Node);
	//Node->SetOwner(NULL);
}

ZEModelAnimationNode* ZEModelAnimationController::GetAnimationNode(const char* Name)
{
	for (ZEInt I = 0; I < AnimationTree.GetCount(); I++)
	{
		if (strcmp(AnimationTree[I]->GetName(), Name) == 0)
			return AnimationTree[I];
	}

	return NULL;
}

void ZEModelAnimationController::Play()
{
	State = ZE_MAS_PLAYING;
}

void ZEModelAnimationController::Resume()
{
	State = ZE_MAS_PLAYING;
}

void ZEModelAnimationController::Pause()
{
	State = ZE_MAS_PAUSED;
}

void ZEModelAnimationController::Stop()
{
	State = ZE_MAS_STOPPED;
}

void ZEModelAnimationController::ApplyOutput()
{
	if(OutputNode == NULL)
		return;

	ZEModelAnimationFrame ResultFrame;

	OutputNode->GetOutput(ResultFrame);

	// Update Bones 
	for (ZESize I = 0; I < ResultFrame.BoneKeys.GetCount(); I++)
	{
		const ZEModelAnimationKey* Key = &ResultFrame.BoneKeys[I];

		ZESize ItemId = (ZESize)Key->ItemId;

		Owner->Bones[ItemId].SetPosition(Key->Position);
		Owner->Bones[ItemId].SetRotation(Key->Rotation);
		
	}

	// Update model meshes.
	// Mechanism is same as above (Update Bones)
	for (ZESize I = 0; I < ResultFrame.MeshKeys.GetCount(); I++)
	{
		const ZEModelAnimationKey* Key = &ResultFrame.MeshKeys[I];

		ZESize ItemId = (ZESize)Key->ItemId;

		Owner->Meshes[ItemId].SetLocalPosition(Key->Position);
		Owner->Meshes[ItemId].SetLocalRotation(Key->Rotation);
		Owner->Meshes[ItemId].SetLocalScale(Key->Scale);

	}
}

void ZEModelAnimationController::Tick(float ElapsedTime)
{
	if (State == ZE_MAS_PLAYING)
	{
		for (ZEInt I = 0; I < AnimationTree.GetCount(); I++)
		{
			AnimationTree[I]->Process(ElapsedTime);
		}
	}

}
