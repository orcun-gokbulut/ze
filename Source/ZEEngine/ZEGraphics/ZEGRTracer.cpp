//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTracer.cpp
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

#include "ZEGRTracer.h"

#include "ZEError.h"
#include "ZEGRGraphicsModule.h"

ZEGRTracer::ZEGRTracer()
{
	Enabled = true;
	EventCount = 0;
}

ZEGRTracer::~ZEGRTracer()
{

}

void ZEGRTracer::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEGRTracer::GetEnabled()
{
	return Enabled;
}

void ZEGRTracer::StartFrame()
{
	zeDebugCheck(EventCount != 0, "There are still tracer events left open in previous frame.");
}

void ZEGRTracer::StartEvent(const char* EventName)
{
	EventCount++;
}

void ZEGRTracer::Mark(const char* MakerName)
{

}

void ZEGRTracer::EndEvent()
{
	zeDebugCheck(EventCount == 0, "No started event available.");
	EventCount--;
}

void ZEGRTracer::EndFrame()
{
	zeDebugCheck(EventCount != 0, "There are still tracer events left open in this frame.");
}

ZEGRTracer* ZEGRTracer::GetInstance()
{
	return ZEGRGraphicsModule::GetInstance()->GetTracer();
}
