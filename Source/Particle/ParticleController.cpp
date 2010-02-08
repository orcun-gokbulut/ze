//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ParticleController.cpp
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

#include "ParticleController.h"


ZEParticleControllerNode::ZEParticleControllerNode()
{
}

void ZEParticleControllerNode::SetTime(float Time)
{
	this->Time = Time;
}

float ZEParticleControllerNode::GetTime()
{
	return this->Time;
}

void ZEParticleControllerNode::SetValue(float Value)
{
	this->Value = Value;
}

float ZEParticleControllerNode::GetValue()
{
	return this->Value;
}

ZEParticleControllerNode::ZEParticleControllerNode(float Time, float Value)
{
	this->Time = Time;
	this->Value = Value;
}




void ZEParticleController::SetName(const ZEString& ControllerName)
{
	Name = ControllerName;
}

const ZEString& ZEParticleController::GetName()
{
	return Name;
}

void ZEParticleController::SetOwner(ZEParticleEmitter* ControllerOwner)
{
	this->Owner = ControllerOwner;
}

ZEParticleEmitter* ZEParticleController::GetOwner()
{
	return Owner;
}

void ZEParticleController::AddKeyFrame(float Time, float Value)
{
	Nodes.Add(ZEParticleControllerNode(Time, Value));
}

void ZEParticleController::RemoveKeyFrame(float Time)
{
	for(unsigned int I = 0; I < Nodes.GetCount(); I++)
	{
		if(Nodes[I].GetTime() == Time)
			Nodes.DeleteAt(I);
	}
}

void ZEParticleController::SetKeyFrame(float Time, float Value)
{
	for(unsigned int I = 0; I < Nodes.GetCount(); I++)
	{
		if(Nodes[I].GetTime() == Time)
			Nodes[I].SetValue(Value);
	}
}

float ZEParticleController::GetKeyframe(float Time)
{
	for(unsigned int I = 0; I < Nodes.GetCount(); I++)
	{
		if(Nodes[I].GetTime() == Time)
			return Nodes[I].GetValue();
	}

	return -1.0f;
}

ZEParticleController::ZEParticleController()
{
	Owner = NULL;
}

ZEParticleController::~ZEParticleController()
{
}
