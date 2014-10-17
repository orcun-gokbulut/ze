//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNodeBlend.cpp
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

#include "ZEModelAnimationNodeBlend.h"
#include "ZEModelAnimationController.h"

ZEModelAnimationNodeBlend::ZEModelAnimationNodeBlend()
{
	SetInputNodeCount(2);
	InputCheckFlags.Value = ZE_MAN_BLEND_INPUT_NONE;
	BlendFactor = 0.5f;
}

ZEModelAnimationNodeBlend::~ZEModelAnimationNodeBlend()
{

}

bool ZEModelAnimationNodeBlend::SetInputNodeA(ZEModelAnimationNode* input)
{
	if (!SetInputNode(0, input))
		return false;

	InputCheckFlags.RaiseFlags(ZE_MAN_BLEND_INPUT_A);

	return true;

}

const ZEModelAnimationNode* ZEModelAnimationNodeBlend::GetInputNodeA() const
{
	return InputNodes[0];
}

bool ZEModelAnimationNodeBlend::SetInputNodeB(ZEModelAnimationNode* input)
{
	if (!SetInputNode(1, input))
		return false;

	InputCheckFlags.RaiseFlags(ZE_MAN_BLEND_INPUT_B);

	return true;
}

const ZEModelAnimationNode* ZEModelAnimationNodeBlend::GetInputNodeB() const
{
	return InputNodes[1];
}

void ZEModelAnimationNodeBlend::SetBlendFactor(float factor)
{
	if (factor > 1.0f)
		BlendFactor = 1.0f;
	else if (factor < 0.0f)
		BlendFactor = 0.0f;
	else
		BlendFactor = factor;
}

float ZEModelAnimationNodeBlend::GetBlendFactor() const
{
	return BlendFactor;
}

void ZEModelAnimationNodeBlend::ProcessSelf(float elapsedTime)
{
	if (InputNodes[0] == NULL)
		InputCheckFlags.UnraiseFlags(ZE_MAN_BLEND_INPUT_A);
	
	if (InputNodes[1] == NULL)
		InputCheckFlags.UnraiseFlags(ZE_MAN_BLEND_INPUT_B);
}

bool ZEModelAnimationNodeBlend::GenerateOutput(ZEModelAnimationFrame& output)
{

	if (InputCheckFlags.GetFlags(ZE_MAN_BLEND_INPUT_ALL))
	{
		ZEModelAnimationFrame outputA;
		
		if (!InputNodes[0]->GetOutput(outputA))
			return false;

		ZEModelAnimationFrame outputB;

		if (!InputNodes[1]->GetOutput(outputB))
			return false;

		ZESize outputBoneKeyCount = outputA.BoneKeys.GetCount();
		output.BoneKeys.SetCount(outputBoneKeyCount);

		for (ZESize I = 0; I < outputBoneKeyCount; I++)
		{
			const ZEModelAnimationKey* keyA = &outputA.BoneKeys[I];
			const ZEModelAnimationKey* keyB = &outputB.BoneKeys[I];
			ZEModelAnimationKey* keyBlend = &output.BoneKeys[I];

			keyBlend->ItemId = keyA->ItemId;
			ZEVector3::Lerp(keyBlend->Position, keyA->Position, keyB->Position, BlendFactor);
			ZEQuaternion::Nlerp(keyBlend->Rotation, keyA->Rotation, keyB->Rotation, BlendFactor);
			keyBlend->Scale = ZEVector3::One;
		}

		ZESize outputMeshKeyCount = outputA.MeshKeys.GetCount();
		output.MeshKeys.SetCount(outputMeshKeyCount);

		for (ZESize I = 0; I < outputMeshKeyCount; I++)
		{
			const ZEModelAnimationKey* keyA = &outputA.MeshKeys[I];
			const ZEModelAnimationKey* keyB = &outputB.MeshKeys[I];
			ZEModelAnimationKey* keyBlend = &output.MeshKeys[I];

			keyBlend->ItemId = keyA->ItemId;
			ZEVector3::Lerp(keyBlend->Position, keyA->Position, keyB->Position, BlendFactor);
			ZEQuaternion::Nlerp(keyBlend->Rotation, keyA->Rotation, keyB->Rotation, BlendFactor);
			ZEVector3::Lerp(keyBlend->Scale, keyA->Scale, keyB->Scale, BlendFactor);
		}

	}
	else if (InputCheckFlags.GetFlags(ZE_MAN_BLEND_INPUT_A))
	{
		return InputNodes[0]->GetOutput(output);
	}
	else if (InputCheckFlags.GetFlags(ZE_MAN_BLEND_INPUT_B))
	{
		return InputNodes[1]->GetOutput(output);
	}
	else
	{
		return false;
	}

}

ZEModelAnimationNodeBlend* ZEModelAnimationNodeBlend::CreateInstance()
{
	return new ZEModelAnimationNodeBlend();
}
