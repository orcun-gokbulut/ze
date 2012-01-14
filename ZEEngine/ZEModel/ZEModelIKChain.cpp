//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelIKChain.cpp
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

#include "ZEModelIKChain.h"
#include "ZEModelBone.h"
#include "ZEModel.h"

const ZEString& ZEModelIKChain::GetName()
{
	return Name;
}
void ZEModelIKChain::SetName(ZEString Name)
{
	this->Name = Name;
}

const ZEVector3& ZEModelIKChain::GetEffectorPosition()
{
	return EffectorPosition;
}

void ZEModelIKChain::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEModelIKChain::GetEnabled()
{
	return Enabled;
}

void ZEModelIKChain::SetEffectorPosition(const ZEVector3& Position)
{
	EffectorPosition = Position;
}

const ZEQuaternion& ZEModelIKChain::GetEffectorRotation()
{
	return EffectorRotation;
}

void ZEModelIKChain::SetEffectorRotation(const ZEQuaternion& Rotation)
{
	EffectorRotation = Rotation;
}

void ZEModelIKChain::SetMaxIterationCount(unsigned int Value)
{
	MaxIterationCount = Value;
}

unsigned int ZEModelIKChain::GetMaxIterationCount()
{
	return MaxIterationCount;
}

void ZEModelIKChain::SetErrorThreshold(float Value)
{
	ErrorThreshold = Value;
}

float ZEModelIKChain::GetErrorThreshold()
{
	return ErrorThreshold;
}

void ZEModelIKChain::Process()
{
	if (!Enabled)
		return;

	for (unsigned int I = 0; I < MaxIterationCount; I++)
	{	
		if (ZEVector3::LengthSquare(Nodes[Nodes.GetCount() - 1].Bone->GetLocalPosition() - EffectorPosition) < ErrorThreshold * ErrorThreshold)
			return;
		Iterate();

	}
}

void ZEModelIKChain::Iterate()
{
	// Get Direction from bone root to the effector
	ZEModelBone* Knob = Nodes[Nodes.GetCount() - 1].Bone;

	for (int I = Nodes.GetCount() - 2; I >= 0 ; I--)
	{
		ZEModelIKChainNode& CurrentNode = Nodes[I];

		if (CurrentNode.EnabledRotation)
		{
			// Calculate Effector Direction
			ZEVector3 EffectorDirection = EffectorPosition - CurrentNode.Bone->GetLocalPosition();
			EffectorDirection.NormalizeSelf();

			// Calculate Knob Direction
			ZEVector3 KnobDirection = Knob->GetLocalPosition() - CurrentNode.Bone->GetLocalPosition();
			KnobDirection.NormalizeSelf();

			// Calculate Normal
			ZEVector3 Normal;
			ZEVector3::CrossProduct(Normal, KnobDirection, EffectorDirection);
			Normal.NormalizeSelf();

			// Calculate Rotation
			float Dot = ZEVector3::DotProduct(EffectorDirection, KnobDirection);
			if (Dot > 1.0f)
				Dot = 1.0f;
			else if (Dot < -1.0)
				Dot = -1.0f;
		
			float Angle = acosf(Dot);
			if (Angle != Angle)
				return;

			if (Angle > RotationLimit)
				Angle = RotationLimit;
			else if (Angle < -RotationLimit)
				Angle = -RotationLimit;

			if (!Normal.IsValid())
				return;

			ZEQuaternion Rotation = ZEQuaternion(Angle, Normal); //Local Space

			Rotation *= CurrentNode.Bone->GetLocalRotation();
			Rotation.NormalizeSelf();
			CurrentNode.Bone->SetLocalRotation(Rotation);
			
			// Limit Angles
			if (CurrentNode.LimitRotation)
			{
				ZEVector3 Angles;			
				ZEQuaternion::ConvertToEulerAngles(Angles, CurrentNode.Bone->GetRelativeRotation());
				ZEQuaternion::CreateFromEuler(Rotation, Angles.Clamp(CurrentNode.MinRotationAngle, CurrentNode.MaxRotationAngle));
				CurrentNode.Bone->SetRelativeRotation(Rotation);
			}
		}
	}
}

ZEModelIKChain::ZEModelIKChain()
{
	Enabled = true;
	ErrorThreshold = 0.1f;
	MaxIterationCount = 10;
	RotationLimit = 15.0f;
}
