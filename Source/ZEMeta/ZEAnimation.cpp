//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAnimation.cpp
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

#include "ZEAnimation.h"
#include "ZEClass.h"
#include "ZEFile/ZEResourceFile.h"
#include "ZETypes.h"
#include "ZEError.h"
#include "ZESerialization/ZESerializer.h"
#include "ZESerialization/ZEUnserializer.h"

#include <string.h>

#define	ZE_ANIF_ANIMATION_CHUNKID					((ZEDWORD)'ANIM')
#define	ZE_ANIF_PROPERTY_ANIMATION_CHUNKID			((ZEDWORD)(ZE_ANIF_ANIMATION_CHUNKID + (ZEDWORD)'PRPT'))
#define	ZE_ANIF_PROPERTY_ANIMATION_KEY_CHUNKID		((ZEDWORD)(ZE_ANIF_ANIMATION_CHUNKID + (ZEDWORD)'KEY '))

struct ZEPropertyAnimationKeyChunk
{
	ZEDWORD							ChunkId;
	float							Time;
};

struct ZEPropertyAnimationChunk
{
	ZEDWORD							ChunkId;
	int								PropertyId;
	bool							Interpolate;
	ZEDWORD							ValueType;
	ZEDWORD							KeyCount;
};

struct ZEAnimationChunk
{
	ZEDWORD							ChunkId;
	char							Name[ZE_MAX_NAME_SIZE];
	ZEDWORD							FrameCount;
	float							FramePerSecond;
	ZEDWORD							PropertyAnimationCount;
};

bool ZEAnimation::WriteToFile(ZESerializer* Serializer, ZEAnimation* Animation)
{
	ZEAnimationChunk AnimationChunk;
	AnimationChunk.ChunkId = ZE_ANIF_ANIMATION_CHUNKID;
	AnimationChunk.FramePerSecond = Animation->FramePerSecond;
	AnimationChunk.FrameCount = Animation->FrameCount;
	AnimationChunk.PropertyAnimationCount = Animation->PropertyAnimations.GetCount();
	strncpy(AnimationChunk.Name, Animation->Name, ZE_MAX_NAME_SIZE);

	Serializer->Write(&AnimationChunk, sizeof(ZEAnimationChunk), 1);	

	AnimationChunk.PropertyAnimationCount = Animation->PropertyAnimations.GetCount();
	for (size_t I = 0; I < AnimationChunk.PropertyAnimationCount; I++)
	{
		ZEPropertyAnimation* PropertyAnimation = &Animation->PropertyAnimations[I];

		ZEPropertyAnimationChunk PropertyAnimationChunk;
		PropertyAnimationChunk.ChunkId = ZE_ANIF_PROPERTY_ANIMATION_CHUNKID;
		PropertyAnimationChunk.Interpolate = PropertyAnimation->Interpolate;
		PropertyAnimationChunk.PropertyId = PropertyAnimation->PropertyId;
		PropertyAnimationChunk.ValueType = PropertyAnimation->ValueType;
		PropertyAnimationChunk.KeyCount = PropertyAnimation->Keys.GetCount();
		Serializer->Write(&PropertyAnimationChunk, sizeof(ZEPropertyAnimationChunk), 1);
		
		for (size_t I = 0; I < PropertyAnimationChunk.KeyCount;I++)
		{
			ZEPropertyAnimationKeyChunk PropertyAnimationKeyChunk;
			PropertyAnimationKeyChunk.ChunkId = ZE_ANIF_PROPERTY_ANIMATION_KEY_CHUNKID;
			PropertyAnimationKeyChunk.Time = PropertyAnimation->Keys[I].Time;
			Serializer->Write(&PropertyAnimationKeyChunk, sizeof(ZEPropertyAnimationKeyChunk), 1);
			
			PropertyAnimation->Keys[I].Value.Serialize(Serializer);	
		}
	}
	return Animation;
}

bool ZEAnimation::ReadFromFile(ZEUnserializer* Unserializer, ZEAnimation* Animation)
{
	ZEAnimationChunk AnimationChunk;
	Unserializer->Read(&AnimationChunk, sizeof(ZEAnimationChunk), 1);
	if (AnimationChunk.ChunkId != ZE_ANIF_ANIMATION_CHUNKID)
	{
		zeError("Animation System", "Corrupted file. Animation chunk id does not match.");
		return false;
	}

	Animation->FramePerSecond = AnimationChunk.FramePerSecond;
	Animation->FrameCount = AnimationChunk.FrameCount;
	strncpy(Animation->Name, AnimationChunk.Name, ZE_MAX_NAME_SIZE);
	
	Animation->PropertyAnimations.SetCount(AnimationChunk.PropertyAnimationCount);
	for (size_t I = 0; I < Animation->PropertyAnimations.GetCount(); I++)
	{
		ZEPropertyAnimationChunk PropertyAnimationChunk;
		Unserializer->Read(&PropertyAnimationChunk, sizeof(ZEPropertyAnimationChunk), 1);
		
		if (PropertyAnimationChunk.ChunkId != ZE_ANIF_PROPERTY_ANIMATION_CHUNKID)
		{
			zeError("Animation System", "Corrupted file. Property animation chunk id does not match.");
			return false;
		}
		
		ZEPropertyAnimation* PropertyAnimation = &Animation->PropertyAnimations[I];
		PropertyAnimation->Interpolate = PropertyAnimationChunk.Interpolate;
		PropertyAnimation->PropertyId = PropertyAnimationChunk.PropertyId;
		PropertyAnimation->ValueType = (ZEVariantType)PropertyAnimationChunk.ValueType;
		PropertyAnimation->Keys.SetCount(PropertyAnimationChunk.KeyCount);
		for (size_t I = 0; I < PropertyAnimation->Keys.GetCount();I++)
		{
			ZEPropertyAnimationKeyChunk PropertyAnimationKeyChunk;
			Unserializer->Read(&PropertyAnimationKeyChunk, sizeof(ZEPropertyAnimationKeyChunk), 1);
			
			if (PropertyAnimationKeyChunk.ChunkId != ZE_ANIF_PROPERTY_ANIMATION_KEY_CHUNKID)
			{
				zeError("Animation System", "Corrupted file. Property animation key chunk id does not match.");
				return false;
			}
			PropertyAnimation->Keys[I].Time = PropertyAnimationKeyChunk.Time;
			PropertyAnimation->Keys[I].Value.Unserialize(Unserializer);
		}
	}
	return Animation;
}

void ZEAnimationController::SetOwner(ZEClass* Owner)
{
	this->Owner = Owner;	
}

ZEClass* ZEAnimationController::GetOwner()
{
	return Owner;
}

void ZEAnimationController::SetLooping(bool Looping)
{
	this->Looping = Looping;
}

bool ZEAnimationController::GetLooping()
{
	return Looping;
}

void ZEAnimationController::SetCurrentFrame(float Frame)
{
	CurrentFrame = Frame;
}

float ZEAnimationController::GetCurrentFrame()
{
	return CurrentFrame;
}

void ZEAnimationController::SetStartFrame(float Frame)
{
	StartFrame = Frame;
}

float ZEAnimationController::GetStartFrame()
{
	return StartFrame;
}

void ZEAnimationController::SetEndFrame(float Frame)
{
	EndFrame = Frame;
}

float ZEAnimationController::GetEndFrame()
{
	return EndFrame;
}

void ZEAnimationController::SetAnimationSpeed(float Speed)
{
	this->Speed = Speed;
}

float ZEAnimationController::GetAnimationSpeed()
{
	return Speed;
}

ZEAnimationState ZEAnimationController::GetAnimationState()
{
	return State;
}

void ZEAnimationController::SetAnimation(ZEAnimation* Animation)
{
	this->Animation = Animation;

	if (Animation == NULL)
	{
		StartFrame = 0.0f;
		EndFrame = 0.0f;
		CurrentFrame = 0.0f;
		State = ZE_AS_NONE;
	}
	else
	{
		StartFrame = 0.0f;
		EndFrame = Animation->FrameCount;
		CurrentFrame = 0.0f;
		State = ZE_AS_NONE;
	}
}

ZEAnimation* ZEAnimationController::GetAnimation()
{
	return Animation;
}

void ZEAnimationController::PlayAnimation()
{
	if (Animation != NULL)
	{
		CurrentFrame = StartFrame;
		State = ZE_AS_PLAYING;
	}
}

void ZEAnimationController::PauseAnimation()
{
	if (Animation != NULL)
		State = ZE_AS_PAUSED;
}

void ZEAnimationController::StopAnimation()
{
	if (Animation != NULL)
	{
		State = ZE_AS_STOPPED;
		CurrentFrame = StartFrame;
	}
}

void ZEAnimationController::ResumeAnimation()
{
	State = ZE_AS_PLAYING;
}

void ZEAnimationController::AdvanceAnimation(float TimeElapsed)
{
	if (State == ZE_AS_PLAYING)
	{
		// Normalize Time
		CurrentFrame += TimeElapsed * (Speed / Animation->FramePerSecond);
		if (CurrentFrame > EndFrame)
			if (Looping)
				CurrentFrame = StartFrame + fmodf(CurrentFrame, EndFrame - StartFrame);
			else
			{
				// Initial values;
				State = ZE_AS_STOPPED;
			}
			
		// Animate PRoperties
		for (size_t I = 0; I < Animation->PropertyAnimations.GetCount(); I++)
		{
			ZEPropertyAnimation* PropertyAnimation = &Animation->PropertyAnimations[I];

			if(PropertyAnimation->Interpolate && (PropertyAnimation->ValueType == ZE_VRT_INTEGER || 
												  PropertyAnimation->ValueType == ZE_VRT_FLOAT || 
												  PropertyAnimation->ValueType == ZE_VRT_QUATERNION || 
												  PropertyAnimation->ValueType == ZE_VRT_VECTOR2 ||
												  PropertyAnimation->ValueType == ZE_VRT_VECTOR3 ||
												  PropertyAnimation->ValueType == ZE_VRT_VECTOR4))
			{
				// Get prev and next key keys
				int PrevKeyIndex = -1;
				int NextKeyIndex = -1;
				for (size_t N = 0; N < PropertyAnimation->Keys.GetCount(); N++)
				{
					if (CurrentFrame > PropertyAnimation->Keys[N].Time)
						PrevKeyIndex = N;
					
					if (CurrentFrame <= PropertyAnimation->Keys[N].Time)
					{
						NextKeyIndex = N;
						break;
					}
				}

				if (PrevKeyIndex == -1 && NextKeyIndex == -1)
					break;

				if (NextKeyIndex == -1)
					Owner->SetProperty(PropertyAnimation->PropertyId, PropertyAnimation->Keys[PrevKeyIndex].Value);
				else
				{
					// Interpolate
					ZEVariant* PrevKeyValue;
					float PrevKeyTime;

					ZEVariant* NextKeyValue = &PropertyAnimation->Keys[NextKeyIndex].Value;
					float NextKeyTime = PropertyAnimation->Keys[NextKeyIndex].Time;

					if (PrevKeyIndex == -1)
					{
						Owner->SetProperty(PropertyAnimation->PropertyId, *NextKeyValue);
						break;
					}
					else
					{
						PrevKeyTime = PropertyAnimation->Keys[PrevKeyIndex].Time;
						PrevKeyValue = &PropertyAnimation->Keys[PrevKeyIndex].Value;
					}
		
					float Interpolation = (CurrentFrame - PrevKeyTime) / (NextKeyTime - PrevKeyTime);

					if (PropertyAnimation->ValueType == ZE_VRT_VECTOR2)
					{
						ZEVector2 Value;
						ZEVector2::Lerp(Value, PrevKeyValue->GetVector2(), NextKeyValue->GetVector2() ,Interpolation);
						Owner->SetProperty(PropertyAnimation->PropertyId, Value);
					}
					else if (PropertyAnimation->ValueType == ZE_VRT_VECTOR3)
					{
						ZEVector3 Value;
						ZEVector3::Lerp(Value, PrevKeyValue->GetVector3(), NextKeyValue->GetVector3() ,Interpolation);
						Owner->SetProperty(PropertyAnimation->PropertyId, Value);
					}
					else if (PropertyAnimation->ValueType == ZE_VRT_VECTOR4)
					{
						ZEVector4 Value;
						ZEVector4::Lerp(Value, PrevKeyValue->GetVector4(), NextKeyValue->GetVector4() ,Interpolation);
						Owner->SetProperty(PropertyAnimation->PropertyId, Value);
					}
					else if (PropertyAnimation->ValueType == ZE_VRT_QUATERNION)
					{
						ZEQuaternion Value;
						ZEQuaternion::Slerp(Value, PrevKeyValue->GetQuaternion(), NextKeyValue->GetQuaternion() ,Interpolation);
						Owner->SetProperty(PropertyAnimation->PropertyId, Value);
					}
					else if (PropertyAnimation->ValueType == ZE_VRT_FLOAT)
					{
						float V1 = PrevKeyValue->GetFloat() , V2 = NextKeyValue->GetFloat();
						int Value = V1 + (V2 - V1) * Interpolation;
						Owner->SetProperty(PropertyAnimation->PropertyId, Value);
					}
					else if (PropertyAnimation->ValueType == ZE_VRT_INTEGER)
					{
						int V1 = PrevKeyValue->GetInteger() , V2 = NextKeyValue->GetInteger();
						int Value = V1 + (V2 - V1) * Interpolation;
						Owner->SetProperty(PropertyAnimation->PropertyId, Value);
					}
				}
			}
			else
			{
				bool Found = false;
				for (int N = PropertyAnimation->Keys.GetCount() - 1; N >= 0; N--)
					if (CurrentFrame >= PropertyAnimation->Keys[N].Time)
					{
						Owner->SetProperty(PropertyAnimation->PropertyId, PropertyAnimation->Keys[N].Value);
						Found = true;
						break;
					}

				/*if (!Found)
					Owner->SetProperty(PropertyAnimation->PropertyId, PropertyAnimation->InitialValue);*/
			}
		}
	}
}

ZEAnimationController::ZEAnimationController()
{
	Owner = NULL;
	Animation = NULL;
	StartFrame = 0.0f;
	EndFrame = 0.0f;
	CurrentFrame = 0.0f;
	State = ZE_AS_NONE;
	Speed = 1.0f;
	Looping = false;
}

ZEAnimationController::~ZEAnimationController()
{
}
