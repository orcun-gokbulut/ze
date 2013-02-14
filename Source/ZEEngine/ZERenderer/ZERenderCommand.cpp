//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderCommand.cpp
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

#include "ZERenderCommand.h"
#include <memory.h>

ZERenderCommand::ZERenderCommand()
{
	Order = 0.0f;
	Priority = 0;

	Flags = ZE_RCF_NONE;
	Pipeline = ZE_RP_NONE;

	PrimitiveType = ZE_PT_NONE;
	PrimitiveCount = 0;
	FirstVertex = 0;
	Material = NULL;

	memset(VertexBuffers, 0, sizeof(ZEVertexBuffer*) * 16);
}

ZERenderCommand::~ZERenderCommand()
{
	
}

ZERenderCommandSkinned::ZERenderCommandSkinned()
{
	Flags |= ZE_RCF_SKINNED;
}

ZERenderCommandSkinned::~ZERenderCommandSkinned()
{

}

ZERenderCommandIndexed::ZERenderCommandIndexed()
{
	Flags |= ZE_RCF_INDEXED;
	
	BaseVertex = 0;
	FirstIndex = 0;
	IndexCount = 0;
	IndexBuffer = NULL;
}

ZERenderCommandIndexed::~ZERenderCommandIndexed()
{

}

ZERenderCommandInstanced::ZERenderCommandInstanced()
{
	Flags |= ZE_RCF_INSTANCED;

	InstanceCount = 0;
	FirstInstance = 0;
}

ZERenderCommandInstanced::~ZERenderCommandInstanced()
{

}
