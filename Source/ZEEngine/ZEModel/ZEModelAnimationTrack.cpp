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
#include "ZEModelResource.h"
#include "ZEModelBone.h"
#include "ZEModelMesh.h"
#include "ZEModel.h"
#include <string.h>
		
void ZEModelAnimationTrack::SetOwner(ZEModel* Model)
{
	Owner = Model;
}

ZEModel* ZEModelAnimationTrack::GetOwner()
{
	return Owner;
}

void ZEModelAnimationTrack::SetState(ZEModelAnimationState State)
{
	if (Animation == NULL)
		State = ZE_MAS_STOPPED;
	else
		this->State = State;
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

void ZEModelAnimationTrack::SetAnimation(const ZEModelAnimation* Animation)
{
	this->Animation = Animation;
}

const ZEModelAnimation* ZEModelAnimationTrack::GetAnimation()
{
	return Animation;
}

void ZEModelAnimationTrack::SetAnimationByName(const char* AnimationName)
{
	Animation = NULL;

	if (Owner->ModelResource == NULL)
		return;

	for (ZESize I = 0; I < Owner->ModelResource->GetAnimations().GetCount(); I++)
		if (strnicmp(AnimationName, Owner->ModelResource->GetAnimations()[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			Animation = &Owner->ModelResource->GetAnimations()[I];

			// Recalculate Start and End Frame
			return;
		}
}	

const char* ZEModelAnimationTrack::GetAnimationName()
{
	if (Animation != NULL)
		return Animation->Name;
	else
		return "";
}

void ZEModelAnimationTrack::SetCurrentFrame(float Frame)
{
	CurrentFrame = Frame;
	UpdateMeshesAndBones();
}

float ZEModelAnimationTrack::GetCurrentFrame()
{
	return CurrentFrame;
}

void ZEModelAnimationTrack::SetStartFrame(ZEUInt Frame)
{
	// Recalculate Effective Start Frame
	StartFrame = Frame;
}

ZEUInt ZEModelAnimationTrack::GetStartFrame()
{
	return StartFrame;
}

void ZEModelAnimationTrack::SetEndFrame(ZEUInt Frame)
{
	// Recalculate Effective End Frame
	EndFrame = Frame;
}

ZEUInt ZEModelAnimationTrack::GetEndFrame()
{
	return EndFrame;
}

void ZEModelAnimationTrack::SetLooping(bool Enabled)
{
	Looping = Enabled;
}

bool ZEModelAnimationTrack::GetLooping()
{
	return Looping;
}
void ZEModelAnimationTrack::SetBlendFactor(float Factor)
{
	BlendFactor = Factor;
}

float ZEModelAnimationTrack::GetBlendFactor()
{
	return BlendFactor;
}

void ZEModelAnimationTrack::SetSpeed(float Factor)
{
	Speed = Factor;
}

float ZEModelAnimationTrack::GetSpeed()
{
	return Speed;
}

void ZEModelAnimationTrack::Play()
{
	if (State == ZE_MAS_PLAYING)
		return;

	if (Animation == NULL)
	{
		State = ZE_MAS_STOPPED;
		return;
	}

	if ((ZESize)this->StartFrame > Animation->Frames.GetCount())
	{
		this->StartFrame = (ZEUInt)Animation->Frames.GetCount() - 1;
	}

	CurrentFrame = (float)StartFrame;

	if ((ZESize)this->EndFrame > Animation->Frames.GetCount())
	{
		this->EndFrame = (ZEUInt)Animation->Frames.GetCount() - 1;
	}
	
	State = ZE_MAS_PLAYING;
}

void ZEModelAnimationTrack::Play(ZEUInt StartFrame, ZEUInt EndFrame)
{
	if (Animation == NULL)
	{
		State = ZE_MAS_STOPPED;
		return;
	}

	if ((ZESize)StartFrame > Animation->Frames.GetCount())
	{
		this->StartFrame = (ZEUInt)Animation->Frames.GetCount() - 1;
		CurrentFrame = (float)StartFrame;
	}
	else
	{
		CurrentFrame = (float)StartFrame;
		this->StartFrame = StartFrame;
	}

	if ((ZESize)this->EndFrame > Animation->Frames.GetCount())
	{
		this->EndFrame = (ZEUInt)Animation->Frames.GetCount() - 1;
	}
	else
	{
		this->EndFrame = (EndFrame == 0 ? (ZEUInt)Animation->Frames.GetCount() - 1 : EndFrame);
	}
	
	State = ZE_MAS_PLAYING;
}

void ZEModelAnimationTrack::Resume()
{
	if (Animation == NULL)
	{
		State = ZE_MAS_STOPPED;
		return;
	}

	if (CurrentFrame >= Animation->Frames.GetCount())
	{
		CurrentFrame = (float)(Animation->Frames.GetCount() - 1);
	}

	if ((ZESize)this->EndFrame > Animation->Frames.GetCount())
	{
		this->EndFrame = (ZEUInt)Animation->Frames.GetCount() - 1;
	}
	else
	{
		this->EndFrame = (EndFrame == 0 ? (ZEUInt)Animation->Frames.GetCount() - 1 : EndFrame);
	}

	State = ZE_MAS_PLAYING;
}

void ZEModelAnimationTrack::Pause()
{
	State = ZE_MAS_PAUSED;
}

void ZEModelAnimationTrack::Stop()
{
	State = ZE_MAS_STOPPED;
	Tick(0.0f);
}

void ZEModelAnimationTrack::UpdateMeshesAndBones()
{
	// Check whether this track is the root track
	bool RootAnimation = (&Owner->AnimationTracks[0] == this);

	// Calculate interpolation value between two frames (current and next frame)
	float Interpolation = CurrentFrame - ZEMath::Floor(CurrentFrame);

	// Find next frame id
	ZESize NextFrameId = (ZESize)ZEMath::Ceil(CurrentFrame);
	if (NextFrameId >= Animation->Frames.GetCount())
	{
		NextFrameId = (ZESize)StartFrame + (ZESize)ZEMath::Mod(CurrentFrame, (float)(EndFrame - StartFrame));
	}

	// Get frames
	const ZEModelResourceAnimationFrame* Frame = &Animation->Frames[(ZESize)ZEMath::Floor(CurrentFrame)];
	const ZEModelResourceAnimationFrame* NextFrame = &Animation->Frames[NextFrameId];

	// Update Bones 
	for (ZESize I = 0; I < Frame->BoneKeys.GetCount(); I++)
	{
		const ZEModelResourceAnimationKey* Key = &Frame->BoneKeys[I];
		const ZEModelResourceAnimationKey* NextKey = &NextFrame->BoneKeys[I];

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
			Owner->Bones[ItemId].SetRelativePosition(Position);
			Owner->Bones[ItemId].SetRelativeRotation(Rotation);
		}
		else
		{
			ZEVector3 PositionBlend;
			ZEQuaternion RotationBlend;

			// Select blending mode
			switch(BlendMode)
			{
				case ZE_MABM_INTERPOLATE:
					ZEVector3::Lerp(PositionBlend, Owner->Bones[ItemId].GetRelativePosition(), Position, BlendFactor);
					ZEQuaternion::Slerp(RotationBlend, Owner->Bones[ItemId].GetRelativeRotation(), Rotation, BlendFactor);
					break;
				case ZE_MABM_ADDAPTIVE:		
					PositionBlend = Owner->Bones[ItemId].GetRelativePosition() + (Position - Owner->ModelResource->GetBones()[ItemId].RelativePosition) * BlendFactor;
					RotationBlend = Owner->Bones[ItemId].GetRelativeRotation() * (Owner->ModelResource->GetBones()[ItemId].RelativeRotation.Conjugate() * Rotation);
					break;
			}

			// Update bone
			Owner->Bones[ItemId].SetRelativePosition(PositionBlend);
			Owner->Bones[ItemId].SetRelativeRotation(RotationBlend);
		}
	}

	// Update model meshes.
	// Mechanism is same as above (Update Bones)
	for (ZESize I = 0; I < Frame->MeshKeys.GetCount(); I++)
	{
		const ZEModelResourceAnimationKey* Key = &Frame->MeshKeys[I];
		const ZEModelResourceAnimationKey* NextKey = &NextFrame->MeshKeys[I];

		ZESize ItemId = (ZESize)Key->ItemId;

		ZEVector3 Position;
		ZEVector3::Lerp(Position, Key->Position, NextKey->Position, Interpolation);
		Owner->Meshes[ItemId].SetLocalPosition(Position);

		ZEQuaternion Rotation;
		ZEQuaternion::Slerp(Rotation, Key->Rotation, NextKey->Rotation, Interpolation);

		ZEVector3 Scale;
		ZEVector3::Lerp(Scale, Key->Scale, NextKey->Scale, Interpolation);

		if (RootAnimation || BlendMode == ZE_MABM_OVERWRITE)
		{
			Owner->Meshes[ItemId].SetLocalPosition(Position);
			Owner->Meshes[ItemId].SetLocalRotation(Rotation);
			Owner->Meshes[ItemId].SetLocalScale(Scale);
		}
		else
		{
			ZEVector3 PositionBlend;
			ZEQuaternion RotationBlend;
			ZEVector3 ScaleBlend;

			switch(BlendMode)
			{
			case ZE_MABM_INTERPOLATE:
				ZEVector3::Lerp(PositionBlend, Owner->Meshes[ItemId].GetLocalPosition(), Position, BlendFactor);
				ZEQuaternion::Slerp(RotationBlend, Owner->Meshes[ItemId].GetLocalRotation(), Rotation, BlendFactor);
				ZEVector3::Lerp(ScaleBlend, Owner->Meshes[ItemId].GetLocalScale(), Scale, BlendFactor);
				break;

			case ZE_MABM_ADDAPTIVE:
				PositionBlend = Owner->Meshes[ItemId].GetLocalPosition() + Position * BlendFactor;
				RotationBlend = Owner->Meshes[ItemId].GetLocalRotation() * Rotation;
				ScaleBlend = Owner->Meshes[ItemId].GetLocalScale() + Scale * BlendFactor;
				break;
			}

			Owner->Meshes[ItemId].SetLocalPosition(PositionBlend);
			Owner->Meshes[ItemId].SetLocalRotation(RotationBlend);
			Owner->Meshes[ItemId].SetLocalScale(ScaleBlend);
		}
	}
}

void ZEModelAnimationTrack::Tick(float ElapsedTime)
{
	if (State == ZE_MAS_PLAYING)
	{
		// Check LOD status if Model's current LOD is lower than current track do not calculate it
		if (LOD != -1 && Owner->ActiveLOD > LOD)
		{
			return;
		}

		// Check animation limits
		if (CurrentFrame >= (float)EndFrame)
		{	
			if (Looping)
			{
				// Recalculate current frame
				CurrentFrame -= (float)EndFrame;
				CurrentFrame = (float)StartFrame + ZEMath::Mod(CurrentFrame, (float)(EndFrame - StartFrame));
			}
			else
			{
				// End Animation
				CurrentFrame = (float)EndFrame;
				State = ZE_MAS_STOPPED;
				return;
			}
		}
		
		if (BlendFactor == 0.0f)
		{
			return;
		}

		UpdateMeshesAndBones();

		CurrentFrame += Speed * ElapsedTime;
	}
}

ZEModelAnimationTrack::ZEModelAnimationTrack()
{
	Owner			= NULL;
	Animation		= NULL;	
	State			= ZE_MAS_STOPPED;
	CurrentFrame	= 0.0f;
	StartFrame		= 0;
	EndFrame		= 0;
	Speed			= 1.0f;
	BlendFactor		= 0.5f;
	Looping			= false;
	BlendMode		= ZE_MABM_INTERPOLATE;
	LOD				= 0;
}
