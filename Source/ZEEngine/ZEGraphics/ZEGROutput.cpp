//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGROutput.cpp
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

#include "ZEGROutput.h"

#include "ZEGRGraphicsModule.h"
#include "ZEGRWindow.h"
#include "ZEGRTexture.h"

bool ZEGROutput::Initialize(const ZEGRWindow* Window, ZEGRFormat Format)
{
	this->Window = Window;
	SetFormat(Format);

	return true;
}

void ZEGROutput::Deinitialize()
{
	Window = NULL;
	
	Texture.Release();
}

ZEGROutput::ZEGROutput()
{
	Window = NULL;

	Register();
}

ZEGROutput::~ZEGROutput()
{
	Unregister();
}

ZEGRResourceType ZEGROutput::GetResourceType() const
{
	return ZEGR_RT_OUTPUT;
}

const ZEGRWindow* ZEGROutput::GetWindow() const
{
	return Window;
}

ZEGRTexture* ZEGROutput::GetTexture() const
{
	return Texture;
}

ZEHolder<ZEGROutput> ZEGROutput::CreateInstance(const ZEGRWindow* Window, ZEGRFormat Format)
{
	zeCheckError(Window == NULL, NULL, "Window cannot be NULL.");
	zeDebugCheck(Window->GetWidth() == 0 || Window->GetHeight() == 0, "Width or height of window cannot be null.");
	zeDebugCheck(Format == ZEGR_TF_NONE, "Format cannot be none");

	ZEHolder<ZEGROutput> Output = ZEGRGraphicsModule::GetInstance()->CreateOutput();
	if (Output == NULL)
		return NULL;

	if (!Output->Initialize(Window, Format))
		return NULL;

	return Output;
}
