//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNodeAdd.cpp
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

#include "ZEModelAnimationNodeAdd.h"
#include "ZEModelAnimationController.h"

ZEModelAnimationNodeAdd::ZEModelAnimationNodeAdd()
{
	SetInputNodeCount(2);
	InputCheckFlags.Value = ZE_MAN_ADD_INPUT_NONE;
	Weight = 1.0f;
}

ZEModelAnimationNodeAdd::~ZEModelAnimationNodeAdd()
{

}

bool ZEModelAnimationNodeAdd::SetBaseInputNode(ZEModelAnimationNode* input)
{
	if (!SetInputNode(0, input))
		return false;

	InputCheckFlags.RaiseFlags(ZE_MAN_ADD_INPUT_BASE);

	return true;
}

ZEModelAnimationNode* ZEModelAnimationNodeAdd::GetBaseInputNode()
{
	return InputNodes[0];
}

bool ZEModelAnimationNodeAdd::SetAdditiveInputNode(ZEModelAnimationNode* input)
{
	if (!SetInputNode(1, input))
		return false;

	InputCheckFlags.RaiseFlags(ZE_MAN_ADD_INPUT_ADD);

	return true;
}

ZEModelAnimationNode* ZEModelAnimationNodeAdd::GetAdditiveInputNode()
{
	return InputNodes[1];
}

void ZEModelAnimationNodeAdd::SetWeight(float weight)
{
	if (weight > 1.0f)
		Weight = 1.0f;
	else if (weight < 0.0f)
		Weight = 0.0f;
	else
		Weight = weight;
}

float ZEModelAnimationNodeAdd::GetWeight()
{
	return Weight;
}

void ZEModelAnimationNodeAdd::ProcessSelf(float elapsedTime)
{
	if (InputNodes[0] == NULL)
		InputCheckFlags.UnraiseFlags(ZE_MAN_ADD_INPUT_BASE);
	
	if (InputNodes[1] == NULL)
		InputCheckFlags.UnraiseFlags(ZE_MAN_ADD_INPUT_ADD);
}

bool ZEModelAnimationNodeAdd::GenerateOutput(ZEModelAnimationFrame& output)
{

	if (InputCheckFlags.GetFlags(ZE_MAN_ADD_INPUT_ALL))
	{
		ZEModelAnimationFrame outputBase;
		if (InputNodes[0]->GetOutput(outputBase))
			return false;

		ZEModelAnimationFrame outputAdd;
		if (InputNodes[1]->GetOutput(outputAdd))
			return false;

		ZESize outputBoneKeyCount = outputBase.BoneKeys.GetCount();
		output.BoneKeys.SetCount(outputBoneKeyCount);

		for (ZESize I = 0; I < outputBoneKeyCount; I++)
		{
			const ZEModelAnimationKey* keyBase = &outputBase.BoneKeys[I];
			const ZEModelAnimationKey* keyAdd = &outputAdd.BoneKeys[I];
			ZEModelAnimationKey* keyOutput = &output.BoneKeys[I];

			keyOutput->ItemId = keyBase->ItemId;

			keyOutput->Position = keyBase->Position + (keyAdd->Position * Weight);

			ZEQuaternion weightedRotation;
			ZEQuaternion::Nlerp(weightedRotation, ZEQuaternion::Identity, keyAdd->Rotation, Weight);
			keyOutput->Rotation = keyBase->Rotation * weightedRotation;

			keyOutput->Scale = ZEVector3::One;
		}

		ZESize outputMeshKeyCount = outputBase.MeshKeys.GetCount();
		output.MeshKeys.SetCount(outputMeshKeyCount);

		for (ZESize I = 0; I < outputMeshKeyCount; I++)
		{
			const ZEModelAnimationKey* keyBase = &outputBase.MeshKeys[I];
			const ZEModelAnimationKey* keyAdd = &outputAdd.MeshKeys[I];
			ZEModelAnimationKey* keyOutput = &output.MeshKeys[I];

			keyOutput->ItemId = keyBase->ItemId;

			keyOutput->Position = keyBase->Position + (keyAdd->Position * Weight);

			ZEQuaternion weightedRotation;
			ZEQuaternion::Nlerp(weightedRotation, ZEQuaternion::Identity, keyAdd->Rotation, Weight);
			keyOutput->Rotation = keyBase->Rotation * weightedRotation;

			keyOutput->Scale = keyBase->Scale + (keyAdd->Scale * Weight);
		}

	}
	else if (InputCheckFlags.GetFlags(ZE_MAN_ADD_INPUT_BASE))
	{
		return InputNodes[0]->GetOutput(output);
	}
	else
	{
		return false;
	}

}

ZEModelAnimationNodeAdd* ZEModelAnimationNodeAdd::CreateInstance()
{
	return new ZEModelAnimationNodeAdd();
}
