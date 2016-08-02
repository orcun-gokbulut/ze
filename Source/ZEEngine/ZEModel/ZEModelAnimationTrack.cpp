//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationTrack.cpp
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

#include "ZEModelAnimationTrack.h"

#include "ZEModel.h"
#include "ZEModelBone.h"
#include "ZEModelMesh.h"
#include "ZEMDResource.h"
#include "ZEMDResourceAnimation.h"
#include "ZEMDResourceBone.h"
#include "ZEMDResourceMesh.h"

#include "ZEMath/ZEMath.h"

#include <string.h>

void ZEModelAnimationTrack::SetState(ZEModelAnimationState State)
{
	if (this->State == State)
		return;

	this->State = State;

	if (GetModel() != NULL)
		GetModel()->AnimationStateChanged();
}

void ZEModelAnimationTrack::UpdateAnimation()
{
	if (GetState() == ZE_MAS_PLAYING && !(BlendFactor == 0.0f)) // && !(LOD != -1 && Model->ActiveLOD > LOD) //ActiveLOD Removed from Model.
	{
		UpdateMeshesAndBones();
	}
}

void ZEModelAnimationTrack::UpdateMeshesAndBones()
{
	if (Model == NULL)
		return;

	// Check whether this track is the root track
	bool RootAnimation = (Model->AnimationTracks[0] == this);

	// Calculate interpolation value between two frames (current and next frame)
	float Interpolation = CurrentFrame - ZEMath::Floor(CurrentFrame);

	// Find next frame id
	ZESize NextFrameId = (ZESize)ZEMath::Ceil(CurrentFrame);
	if (NextFrameId >= Animation->GetFrames().GetCount())
	{
		NextFrameId = (ZESize)EffectiveStartFrame + (ZESize)ZEMath::Mod(CurrentFrame, (float)(EffectiveEndFrame - EffectiveStartFrame));
	}

	// Get frames
	const ZEMDResourceAnimationFrame* Frame = &Animation->GetFrames()[(ZESize)ZEMath::Floor(CurrentFrame)];
	const ZEMDResourceAnimationFrame* NextFrame = &Animation->GetFrames()[NextFrameId]; //Looping bugu olabilir arrayden circular ile alımmalımı looping ise?

	// Update Bones 
	for (ZESize I = 0; I < Frame->BoneKeys.GetCount(); I++)
	{
		const ZEMDResourceAnimationKey* Key = &Frame->BoneKeys[I];
		const ZEMDResourceAnimationKey* NextKey = &NextFrame->BoneKeys[I];

		ZESize ItemId = (ZESize)Key->ItemId;

		// Linear interpolate position of the two frames (Current and next frame)
		ZEVector3 Position;
		ZEVector3::Lerp(Position, Key->Position, NextKey->Position, Interpolation);

		// Spherical linear interpolate rotation of the two frames (Current and next frame
		ZEQuaternion Rotation;
		ZEQuaternion::Slerp(Rotation, Key->Rotation, NextKey->Rotation, Interpolation);

		// Check that whether animation will be written directly or blended with previous track
		if (RootAnimation || BlendMode == ZE_MABM_OVERWRITE)
		{
			// Write the calculated value with out blending
			Model->Bones[ItemId]->SetPosition(Position);
			Model->Bones[ItemId]->SetRotation(Rotation);
		}
		else
		{
			ZEVector3 PositionBlend;
			ZEQuaternion RotationBlend;

			// Select blending mode
			switch(BlendMode)
			{
				case ZE_MABM_INTERPOLATE:
					ZEVector3::Lerp(PositionBlend, Model->Bones[ItemId]->GetPosition(), Position, BlendFactor);
					ZEQuaternion::Slerp(RotationBlend, Model->Bones[ItemId]->GetRotation(), Rotation, BlendFactor);
					break;

				case ZE_MABM_ADDITIVE:		
					PositionBlend = Model->Bones[ItemId]->GetPosition() + (Position - Model->Resource->GetBones()[ItemId]->GetPosition()) * BlendFactor;
					RotationBlend = Model->Bones[ItemId]->GetRotation() * (Model->Resource->GetBones()[ItemId]->GetRotation().Conjugate() * Rotation);
					break;
			}

			// Update bone
			Model->Bones[ItemId]->SetPosition(PositionBlend);
			Model->Bones[ItemId]->SetRotation(RotationBlend);
		}
	}

	// Update model meshes.
	// Mechanism is same as above (Update Bones)
	for (ZESize I = 0; I < Frame->MeshKeys.GetCount(); I++)
	{
		const ZEMDResourceAnimationKey* Key = &Frame->MeshKeys[I];
		const ZEMDResourceAnimationKey* NextKey = &NextFrame->MeshKeys[I];

		ZESize ItemId = (ZESize)Key->ItemId;

		ZEVector3 Position;
		ZEVector3::Lerp(Position, Key->Position, NextKey->Position, Interpolation);
		Model->Meshes[ItemId]->SetPosition(Position);

		ZEQuaternion Rotation;
		ZEQuaternion::Slerp(Rotation, Key->Rotation, NextKey->Rotation, Interpolation);

		ZEVector3 Scale;
		ZEVector3::Lerp(Scale, Key->Scale, NextKey->Scale, Interpolation);

		if (RootAnimation || BlendMode == ZE_MABM_OVERWRITE)
		{
			Model->Meshes[ItemId]->SetPosition(Position);
			Model->Meshes[ItemId]->SetRotation(Rotation);
			Model->Meshes[ItemId]->SetScale(Scale);
		}
		else
		{
			ZEVector3 PositionBlend;
			ZEQuaternion RotationBlend;
			ZEVector3 ScaleBlend;

			switch(BlendMode)
			{
			case ZE_MABM_INTERPOLATE:
				ZEVector3::Lerp(PositionBlend, Model->Meshes[ItemId]->GetPosition(), Position, BlendFactor);
				ZEQuaternion::Slerp(RotationBlend, Model->Meshes[ItemId]->GetRotation(), Rotation, BlendFactor);
				ZEVector3::Lerp(ScaleBlend, Model->Meshes[ItemId]->GetScale(), Scale, BlendFactor);
				break;

			case ZE_MABM_ADDITIVE:
				PositionBlend = Model->Meshes[ItemId]->GetPosition() + Position * BlendFactor;
				RotationBlend = Model->Meshes[ItemId]->GetRotation() * Rotation;
				ScaleBlend = Model->Meshes[ItemId]->GetScale() + Scale * BlendFactor;
				break;
			}

			Model->Meshes[ItemId]->SetPosition(PositionBlend);
			Model->Meshes[ItemId]->SetRotation(RotationBlend);
			Model->Meshes[ItemId]->SetScale(ScaleBlend);
		}
	}
}

ZEModel* ZEModelAnimationTrack::GetModel()
{
	return Model;
}

ZEModelAnimationState ZEModelAnimationTrack::GetState()
{
	return State;
}

void ZEModelAnimationTrack::SetLOD(ZEUInt LOD)
{
	this->LOD = LOD;
}

ZEUInt ZEModelAnimationTrack::GetLOD()
{
	return LOD;
}

void ZEModelAnimationTrack::SetResource(ZERSHolder<const ZEMDResourceAnimation> Resource)
{
	if (this->Resource == Resource)
		return;

	Stop();

	this->Resource = Resource;
	Animation = Resource;

	ApplyLimits();
}

ZERSHolder<const ZEMDResourceAnimation> ZEModelAnimationTrack::GetResource()
{
	return Resource;
}

void ZEModelAnimationTrack::SetCurrentFrame(float Frame)
{
	if (Animation == NULL)
		return;

	if (Frame < 0.0f)
		CurrentFrame = (float)EffectiveStartFrame;
	else if (Frame > (float)EffectiveEndFrame)
		CurrentFrame = (float)EffectiveEndFrame;
	else
		CurrentFrame = Frame;

	UpdateMeshesAndBones();
}

void ZEModelAnimationTrack::SetCurrentFrameByTime(float Seconds)
{
	if (Animation == NULL)
		return;

	if (Animation->GetFrames().GetCount() == 0)
		return;

	float TempCurrentFrameValue = (GetFrameRate() * Seconds);

	if (TempCurrentFrameValue > (float)Animation->GetFrames().GetCount() - 1.0f)
	{
		SetCurrentFrame((float)Animation->GetFrames().GetCount() - 1.0f);
	}
	else
	{
		SetCurrentFrame(TempCurrentFrameValue);
	}
}

void ZEModelAnimationTrack::SetCurrentFrameByPercentage(float Percentage)
{
	if (Animation == NULL)
		return;

	if (Animation->GetFrames().GetCount() == 0)
		return;

	if (Percentage < 0.0f)
	{
		SetCurrentFrame(0.0f);
	}
	else
	{
		SetCurrentFrame((((float)Animation->GetFrames().GetCount() - 1.0f) / 100.0f) * Percentage);
	}

}

float ZEModelAnimationTrack::GetCurrentFrame()
{
	return CurrentFrame;
}

float ZEModelAnimationTrack::GetCurrentFrameByTime()
{
	if (Animation == NULL)
		return 0.0f;

	if (Animation->GetFrames().GetCount() == 0)
		return 0.0f;

	return CurrentFrame / GetFrameRate();

}

float ZEModelAnimationTrack::GetCurrentFrameByPercentage()
{
	if (Animation == NULL)
		return 0.0f;

	if (Animation->GetFrames().GetCount() == 0)
		return 0.0f;

	return (CurrentFrame / ((float)Animation->GetFrames().GetCount() - 1.0f)) * 100.0f;

}

void ZEModelAnimationTrack::SetStartFrame(ZEUInt Frame)
{
	StartFrame = Frame;

	ApplyLimits();
}

void ZEModelAnimationTrack::SetStartFrameByTime(float Seconds)
{
	if (Animation == NULL)
		return;

	if (Animation->GetFrames().GetCount() == 0)
		return;

	ZEUInt TempStartFrameValue = (ZEUInt)(GetFrameRate() * Seconds);

	if (TempStartFrameValue > (ZEUInt)Animation->GetFrames().GetCount() - 1)
	{
		SetStartFrame((ZEUInt)Animation->GetFrames().GetCount() - 1);
	}
	else
	{
		SetStartFrame(TempStartFrameValue);
	}
}

void ZEModelAnimationTrack::SetStartFrameByPercentage(float Percentage)
{
	if (Animation == NULL)
		return;
	
	if (Animation->GetFrames().GetCount() == 0)
		return;

	if (Percentage < 0.0f)
	{
		SetStartFrame(0);
	}
	else
	{
		SetStartFrame((ZEUInt)((((float)Animation->GetFrames().GetCount() - 1.0f) / 100.0f) * Percentage));
	}
}

ZEUInt ZEModelAnimationTrack::GetStartFrame()
{
	return StartFrame;
}

float ZEModelAnimationTrack::GetStartFrameByTime()
{
	if (Animation == NULL)
		return 0.0f;

	if (Animation->GetFrames().GetCount() == 0)
		return 0.0f;

	return (float)StartFrame / GetFrameRate();
}

float ZEModelAnimationTrack::GetStartFrameByPercentage()
{
	if (Animation == NULL)
		return 0.0f;

	if (Animation->GetFrames().GetCount() == 0)
		return 0.0f;

	return ((float)StartFrame / ((float)Animation->GetFrames().GetCount() - 1.0f)) * 100.0f;

}

void ZEModelAnimationTrack::SetEndFrame(ZEUInt Frame)
{
	EndFrame = Frame;

	ApplyLimits();
}

void ZEModelAnimationTrack::SetEndFrameByTime(float Seconds)
{
	if (Animation == NULL)
		return;

	if (Animation->GetFrames().GetCount() == 0)
		return;

	ZEUInt TempEndFrameValue = (ZEUInt)(GetFrameRate() * Seconds);

	if (TempEndFrameValue > (ZEUInt)Animation->GetFrames().GetCount() - 1)
	{
		SetEndFrame((ZEUInt)Animation->GetFrames().GetCount() - 1);
	}
	else
	{
		SetEndFrame(TempEndFrameValue);
	}
}

void ZEModelAnimationTrack::SetEndFrameByPercentage(float Percentage)
{
	if (Animation == NULL)
		return;

	if (Animation->GetFrames().GetCount() == 0)
		return;

	if (Percentage < 0.0f)
	{
		SetEndFrame(0);
	}
	else
	{
		SetEndFrame((ZEUInt)((((float)Animation->GetFrames().GetCount() - 1.0f) / 100.0f) * Percentage));
	}

}

ZEUInt ZEModelAnimationTrack::GetEndFrame()
{
	return EndFrame;
}

float ZEModelAnimationTrack::GetEndFrameByTime()
{
	if (Animation == NULL)
		return 0.0f;

	if (Animation->GetFrames().GetCount() == 0)
		return 0.0f;

	return (float)EndFrame / GetFrameRate();
}

float ZEModelAnimationTrack::GetEndFrameByPercentage()
{
	if (Animation == NULL)
		return 0.0f;

	if (Animation->GetFrames().GetCount() == 0)
		return 0.0f;

	return ((float)EndFrame / ((float)Animation->GetFrames().GetCount() - 1.0f)) * 100.0f;
}

void ZEModelAnimationTrack::SetLooping(bool Enabled)
{
	Looping = Enabled;
}

bool ZEModelAnimationTrack::GetLooping()
{
	return Looping;
}

void ZEModelAnimationTrack::ApplyLimits()
{
	if (Animation == NULL)
		return;

	if (Animation->GetFrames().GetCount() == 0)
		return;

	if (!LimitsEnabled)
	{
		EffectiveStartFrame = 0;
		EffectiveEndFrame = (ZEUInt)Animation->GetFrames().GetCount() - 1;
	}
	else
	{
		EffectiveStartFrame = StartFrame % (ZEUInt)Animation->GetFrames().GetCount();	
		EffectiveEndFrame = EndFrame % (ZEUInt)Animation->GetFrames().GetCount();	
	}
}

void ZEModelAnimationTrack::SetLimitsEnabled(bool Enabled)
{
	LimitsEnabled = Enabled;

	ApplyLimits();
}

bool ZEModelAnimationTrack::GetLimitsEnabled()
{
	return LimitsEnabled;
}

void ZEModelAnimationTrack::SetBlendFactor(float Factor)
{
	BlendFactor = Factor;
}

float ZEModelAnimationTrack::GetBlendFactor()
{
	return BlendFactor;
}

void ZEModelAnimationTrack::SetSpeed(float Speed)
{
	this->Speed = Speed;
}

float ZEModelAnimationTrack::GetSpeed()
{
	return Speed;
}

float ZEModelAnimationTrack::GetFrameRate()
{
	if (Animation != NULL)
		return Speed / Animation->GetFramesPerSecond();
	else
		return Speed / 30.0f;
}

void ZEModelAnimationTrack::SetBlendMode(ZEModelAnimationBlendMode Mode)
{
	this->BlendMode = Mode;
}

ZEModelAnimationBlendMode ZEModelAnimationTrack::GetBlendMode()
{
	return this->BlendMode;
}

void ZEModelAnimationTrack::Play()
{
	if (GetState() == ZE_MAS_PLAYING)
		return;

	if (Animation == NULL)
	{
		SetState(ZE_MAS_STOPPED);
		return;
	}

	CurrentFrame = (float)EffectiveStartFrame;
	
	SetState(ZE_MAS_PLAYING);
}

void ZEModelAnimationTrack::Play(ZEUInt StartFrame, ZEUInt EndFrame)
{
	if (Animation == NULL)
		return;

	LimitsEnabled = true;

	this->StartFrame = StartFrame;
	this->EndFrame = EndFrame;

	ApplyLimits();

	CurrentFrame = (float)EffectiveStartFrame;
	
	SetState(ZE_MAS_PLAYING);
}

void ZEModelAnimationTrack::Resume()
{
	if (Animation == NULL)
		return;

	ApplyLimits();

	if (CurrentFrame > (float)EffectiveEndFrame)
		CurrentFrame = (float)EffectiveEndFrame;

	SetState(ZE_MAS_PLAYING);
}

void ZEModelAnimationTrack::Pause()
{
	SetState(ZE_MAS_PAUSED);
}

void ZEModelAnimationTrack::Stop()
{
	Tick(0.0f);
	SetState(ZE_MAS_STOPPED);
}

void ZEModelAnimationTrack::Tick(float ElapsedTime)
{
	if (BlendFactor == 0.0f)
		return;

	if (GetState() == ZE_MAS_PLAYING)
	{
		// Check LOD status if Model's current LOD is lower than current track do not calculate it
// 		if (LOD != -1 && Owner->ActiveLOD > LOD)
// 		{
// 			return;
// 		}

		//Update CurrentFrame
		CurrentFrame += GetFrameRate() * ElapsedTime;

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
				SetState(ZE_MAS_STOPPED);
				return;
			}
		}
	}
}

ZEModelAnimationTrack::ZEModelAnimationTrack() : ModelLink(this)
{
	Model = NULL;
	Animation = NULL;	
	State = ZE_MAS_STOPPED;

	EffectiveStartFrame = 0;
	EffectiveEndFrame = 0;
	LimitsEnabled = false;

	CurrentFrame = 0.0f;
	StartFrame = 0;
	EndFrame = 0;

	Speed = 30.0f;
	BlendFactor	= 0.5f;
	Looping = false;
	BlendMode = ZE_MABM_INTERPOLATE;
	LOD = 0;
}
