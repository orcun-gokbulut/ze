//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SoundSource3D.cpp
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

#include "SoundModule.h"
#include "SoundSource3D.h"

bool ZESoundSource3D::Is3D()
{
	return true;
}

void ZESoundSource3D::SetPosition(const ZEPoint3& NewPosition)
{
 	Changed3D = true;
	ZEComponent::SetLocalPosition(NewPosition);
}

void ZESoundSource3D::SetRotation(const ZEQuaternion& NewRotation)
{
	ZEComponent::SetLocalRotation(NewRotation);
	Changed3D = true;
}

float ZESoundSource3D::GetMinDistance() 
{
	return MinDistance;
}

void ZESoundSource3D::SetMinDistance(float  NewMinDistance) 
{
	MinDistance = NewMinDistance;
	Changed3D = true;
}		

float ZESoundSource3D::GetMaxDistance() 
{
	return MaxDistance;
}

void ZESoundSource3D::SetMaxDistance(float  NewMaxDistance)
{
	MaxDistance = NewMaxDistance;
	Changed3D = true;
}		

unsigned int ZESoundSource3D::GetInsideConeAngle() 	
{
	return InsideAngle;

}

unsigned int ZESoundSource3D::GetOutsideConeAngle() 	
{
	return OutsideAngle;

}

void ZESoundSource3D::SetConeAngles (unsigned int NewInsideAngle, unsigned int NewOutsideAngle)
{
	InsideAngle = NewInsideAngle;
	OutsideAngle = NewOutsideAngle;
	Changed3D = true;
}		

unsigned int ZESoundSource3D::GetConeOutsideVolume()
{
	return ConeOutsideVolume;
}

void ZESoundSource3D::SetConeOutsideVolume(unsigned int NewOutsideVolume)
{
	ConeOutsideVolume = NewOutsideVolume;
	Changed3D = true;
}

void ZESoundSource3D::Tick(float Time)
{
	ZESoundSource::Update();
	if (SoundState != SOUNDSTATE_NOBUFFER && Changed3D)
	{
		zeSound->UpdateSource3D(this);
		Changed3D = false;
	}
}

ZESoundSource3D::ZESoundSource3D()
{
	Changed3D = false;
}

