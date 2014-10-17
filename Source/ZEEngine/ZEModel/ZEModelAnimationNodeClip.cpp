//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNodeClip.cpp
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

#include "ZEModelAnimationNodeClip.h"
#include "ZEModel.h"

void ZEModelAnimationNodeClip::ApplyLimits()
{
	if (AnimationClip == NULL)
		return;

	if (AnimationClip->Frames.GetCount() == 0)
		return;

	if (!LimitsEnabled)
	{
		EffectiveStartFrame = 0;
		EffectiveEndFrame = (ZEUInt)AnimationClip->Frames.GetCount() - 1;
	}
	else
	{
		EffectiveStartFrame = StartFrame % (ZEUInt)AnimationClip->Frames.GetCount();	
		EffectiveEndFrame = EndFrame % (ZEUInt)AnimationClip->Frames.GetCount();	
	}
}

ZEModelAnimationNodeClip::ZEModelAnimationNodeClip()
{
	AnimationClip = NULL;

	CurrentFrame	= 0.0f;
	StartFrame		= 0;
	EndFrame		= 0;

	EffectiveStartFrame = 0;
	EffectiveEndFrame = 0;
	LimitsEnabled = false;

	Speed = 30.0f;
	Looping = false;
}

ZEModelAnimationNodeClip::~ZEModelAnimationNodeClip()
{

}

void ZEModelAnimationNodeClip::SetAnimationClip(ZEModelAnimationClip* animationClip)
{
	if(animationClip == NULL)
		return;

	if (AnimationClip != NULL)
	{
		AnimationClip->RemoveReference();
		AnimationClip = NULL;
	}

	AnimationClip = animationClip;
	AnimationClip->AddReference();

	ApplyLimits();
}

const ZEModelAnimationClip* ZEModelAnimationNodeClip::GetAnimationClip()
{
	return AnimationClip;
}

void ZEModelAnimationNodeClip::SetSequence(ZESize index)
{
	if (AnimationClip == NULL)
		return;

	if (index >= AnimationClip->GetSequenceCount())
		return;

	StartFrame = AnimationClip->Sequences[index].StartFrame;
	EndFrame = AnimationClip->Sequences[index].EndFrame;

	ApplyLimits();
}

void ZEModelAnimationNodeClip::SetSequence(const char* name)
{
	if (AnimationClip == NULL)
		return;

	ZESize sequenceCount = AnimationClip->GetSequenceCount();

	for (ZESize I = 0; I < sequenceCount; I++)
	{
		if (strnicmp(name, AnimationClip->Sequences[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			StartFrame = AnimationClip->Sequences[I].StartFrame;
			EndFrame = AnimationClip->Sequences[I].EndFrame;

			ApplyLimits();

			return;
		}
	}
}

void ZEModelAnimationNodeClip::SetSpeed(float fPS)
{
	Speed = fPS;
}

float ZEModelAnimationNodeClip::GetSpeed()
{
	return Speed;
}

void ZEModelAnimationNodeClip::SetLooping(bool enabled)
{
	Looping = enabled;
}

bool ZEModelAnimationNodeClip::GetLooping()
{
	return Looping;
}

void ZEModelAnimationNodeClip::SetLimitsEnabled(bool enabled)
{
	LimitsEnabled = enabled;

	ApplyLimits();
}

bool ZEModelAnimationNodeClip::GetLimitsEnabled()
{
	return LimitsEnabled;
}

void ZEModelAnimationNodeClip::SetStartFrame(ZEUInt frame)
{
	StartFrame = frame;

	ApplyLimits();
}

void ZEModelAnimationNodeClip::SetStartFrameByTime(float seconds)
{
	if (AnimationClip == NULL)
		return;

	if (AnimationClip->Frames.GetCount() == 0)
		return;

	ZEUInt TempStartFrameValue = (ZEUInt)(Speed * seconds);

	if (TempStartFrameValue > (ZEUInt)AnimationClip->Frames.GetCount() - 1)
	{
		SetStartFrame((ZEUInt)AnimationClip->Frames.GetCount() - 1);
	}
	else
	{
		SetStartFrame(TempStartFrameValue);
	}
}

void ZEModelAnimationNodeClip::SetStartFrameByPercentage(float percentage)
{
	if (AnimationClip == NULL)
		return;
	
	if (AnimationClip->Frames.GetCount() == 0)
		return;

	if (percentage < 0.0f)
	{
		SetStartFrame(0);
	}
	else
	{
		SetStartFrame((ZEUInt)((((float)AnimationClip->Frames.GetCount() - 1.0f) / 100.0f) * percentage));
	}
}

ZEUInt ZEModelAnimationNodeClip::GetStartFrame()
{
	return StartFrame;
}

float ZEModelAnimationNodeClip::GetStartFrameByTime()
{
	if (AnimationClip == NULL)
		return 0.0f;

	if (AnimationClip->Frames.GetCount() == 0)
		return 0.0f;

	return (float)StartFrame / Speed;
}

float ZEModelAnimationNodeClip::GetStartFrameByPercentage()
{
	if (AnimationClip == NULL)
		return 0.0f;

	if (AnimationClip->Frames.GetCount() == 0)
		return 0.0f;

	return ((float)StartFrame / ((float)AnimationClip->Frames.GetCount() - 1.0f)) * 100.0f;

}

void ZEModelAnimationNodeClip::SetEndFrame(ZEUInt Frame)
{
	EndFrame = Frame;

	ApplyLimits();
}

void ZEModelAnimationNodeClip::SetEndFrameByTime(float Seconds)
{
	if (AnimationClip == NULL)
		return;

	if (AnimationClip->Frames.GetCount() == 0)
		return;

	ZEUInt TempEndFrameValue = (ZEUInt)(Speed * Seconds);

	if (TempEndFrameValue > (ZEUInt)AnimationClip->Frames.GetCount() - 1)
	{
		SetEndFrame((ZEUInt)AnimationClip->Frames.GetCount() - 1);
	}
	else
	{
		SetEndFrame(TempEndFrameValue);
	}
}

void ZEModelAnimationNodeClip::SetEndFrameByPercentage(float percentage)
{
	if (AnimationClip == NULL)
		return;

	if (AnimationClip->Frames.GetCount() == 0)
		return;

	if (percentage < 0.0f)
	{
		SetEndFrame(0);
	}
	else
	{
		SetEndFrame((ZEUInt)((((float)AnimationClip->Frames.GetCount() - 1.0f) / 100.0f) * percentage));
	}

}

ZEUInt ZEModelAnimationNodeClip::GetEndFrame()
{
	return EndFrame;
}

float ZEModelAnimationNodeClip::GetEndFrameByTime()
{
	if (AnimationClip == NULL)
		return 0.0f;

	if (AnimationClip->Frames.GetCount() == 0)
		return 0.0f;

	return (float)EndFrame / Speed;
}

float ZEModelAnimationNodeClip::GetEndFrameByPercentage()
{
	if (AnimationClip == NULL)
		return 0.0f;

	if (AnimationClip->Frames.GetCount() == 0)
		return 0.0f;

	return ((float)EndFrame / ((float)AnimationClip->Frames.GetCount() - 1.0f)) * 100.0f;
}

void ZEModelAnimationNodeClip::ProcessSelf(float elapsedTime)
{
	//Update CurrentFrame
	CurrentFrame += Speed * elapsedTime;

	// Check animation limits
	if (CurrentFrame >= (float)EffectiveEndFrame)
	{	
		if (Looping)
		{
			// Recalculate current frame
			CurrentFrame -= (float)EffectiveEndFrame;
			CurrentFrame = (float)EffectiveStartFrame + ZEMath::Mod(CurrentFrame, (float)(EffectiveEndFrame - EffectiveStartFrame));
		}
		else
		{
			// End Animation
			CurrentFrame = (float)EffectiveEndFrame;
			return;
		}
	}
}

bool ZEModelAnimationNodeClip::GenerateOutput(ZEModelAnimationFrame& output)
{
	// Calculate interpolation value between two frames (current and next frame)
	float Interpolation = CurrentFrame - ZEMath::Floor(CurrentFrame);

	// Find next frame id
	ZESize NextFrameId = (ZESize)ZEMath::Ceil(CurrentFrame);
	if (NextFrameId >= AnimationClip->Frames.GetCount())
	{
		NextFrameId = (ZESize)EffectiveStartFrame + (ZESize)ZEMath::Mod(CurrentFrame, (float)(EffectiveEndFrame - EffectiveStartFrame));
	}

	// Get frames
	const ZEModelAnimationFrame* Frame = AnimationClip->Frames[(ZESize)ZEMath::Floor(CurrentFrame)];
	const ZEModelAnimationFrame* NextFrame = AnimationClip->Frames[NextFrameId]; //Looping bugu olabilir arrayden circular ile al�mmal�m� looping ise?

	ZESize OutputBoneKeyCount = Frame->BoneKeys.GetCount();
	output.BoneKeys.SetCount(OutputBoneKeyCount);
	ZESize OutputMeshKeyCount = Frame->MeshKeys.GetCount();
	output.MeshKeys.SetCount(OutputMeshKeyCount);

	for (ZESize I = 0; I < Frame->BoneKeys.GetCount(); I++)
	{
		const ZEModelAnimationKey* Key = &Frame->BoneKeys[I];
		const ZEModelAnimationKey* NextKey = &NextFrame->BoneKeys[I];

		output.BoneKeys[I].ItemId = Key->ItemId;

		// Linear interpolate position of the two frames (Current and next frame)
		ZEVector3 Position;
		ZEVector3::Lerp(Position, Key->Position, NextKey->Position, Interpolation);
		output.BoneKeys[I].Position = Position;

		// Spherical linear interpolate rotation of the two frames (Current and next frame
		ZEQuaternion Rotation;
		ZEQuaternion::Slerp(Rotation, Key->Rotation, NextKey->Rotation, Interpolation);
		output.BoneKeys[I].Rotation = Rotation;

		output.BoneKeys[I].Scale = ZEVector3::One;
		
	}

	for (ZESize I = 0; I < Frame->MeshKeys.GetCount(); I++)
	{
		const ZEModelAnimationKey* Key = &Frame->MeshKeys[I];
		const ZEModelAnimationKey* NextKey = &NextFrame->MeshKeys[I];

		output.MeshKeys[I].ItemId = Key->ItemId;

		ZEVector3 Position;
		ZEVector3::Lerp(Position, Key->Position, NextKey->Position, Interpolation);
		output.MeshKeys[I].Position = Position;

		ZEQuaternion Rotation;
		ZEQuaternion::Slerp(Rotation, Key->Rotation, NextKey->Rotation, Interpolation);
		output.MeshKeys[I].Rotation = Rotation;

		ZEVector3 Scale;
		ZEVector3::Lerp(Scale, Key->Scale, NextKey->Scale, Interpolation);
		
		output.MeshKeys[I].Scale = Scale;
	}

	return true;
}

ZEModelAnimationNodeClip* ZEModelAnimationNodeClip::CreateInstance()
{
	return new ZEModelAnimationNodeClip();
}
