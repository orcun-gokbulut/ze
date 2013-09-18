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

#include "ZEMaterial.h"
#include "ZERenderCommand.h"
#include "ZEGraphics/ZEConstantBuffer.h"

#include <memory.h>

/********************************************************************/
/*					Render Command Base Classes						*/
/********************************************************************/
_ZERenderCommandDefaultBase::_ZERenderCommandDefaultBase()
{
	FirstVertex = 0;
	PrimitiveCount = 0;
	PrimitiveType = ZE_PT_NONE;
	
	FirstVertex = 0;
	VertexLayout = NULL;
	memset(VertexBuffers, NULL, sizeof(ZEVertexBuffer*) * ZE_MAX_VERTEX_BUFFER_SLOT);
}

_ZERenderCommandDefaultBase::~_ZERenderCommandDefaultBase()
{

}

_ZERenderCommandIndexedBase::_ZERenderCommandIndexedBase()
{
	BaseVertex = 0;
	FirstIndex = 0;
	IndexCount = 0;
	IndexBuffer = NULL;
}

_ZERenderCommandIndexedBase::~_ZERenderCommandIndexedBase()
{

}

_ZERenderCommandInstancedBase::_ZERenderCommandInstancedBase()
{
	InstanceCount = 0;
	FirstInstance = 0;
}

_ZERenderCommandInstancedBase::~_ZERenderCommandInstancedBase()
{

}

_ZERenderCommandIndexedInstancedBase::_ZERenderCommandIndexedInstancedBase()
{

}

_ZERenderCommandIndexedInstancedBase::~_ZERenderCommandIndexedInstancedBase()
{

}

/********************************************************************/
/*						Render Command Classes						*/
/********************************************************************/

static ZEUInt32	NextId = 0;

ZERenderCommand::ZERenderCommand(ZESize CommandSize, ZERenderCommandType CommandType)
{
	Id = NextId++;
	Size = CommandSize;
	Type = CommandType;

	Order = 3.0f;
	Priority = 3;

	Material = NULL;

	Skinned = false;
	SkinningBuffer = NULL;

	WorldTransform = ZEMatrix4x4::Identity;	
}

ZERenderCommand::~ZERenderCommand()
{

}

ZEUInt32 ZERenderCommand::GetId() const
{
	return Id;
}

ZESize ZERenderCommand::GetSize() const
{
	return Size;
}

ZERenderCommandType ZERenderCommand::GetType() const
{
	return Type;
}

ZERenderCommandDefault::ZERenderCommandDefault()
	: ZERenderCommand(sizeof(ZERenderCommandDefault), ZE_RCT_DEFAULT)
{

}

ZERenderCommandDefault::~ZERenderCommandDefault()
{

}

ZERenderCommandIndexed::ZERenderCommandIndexed()
	: ZERenderCommand(sizeof(ZERenderCommandIndexed), ZE_RCT_INDEXED)
{

}

ZERenderCommandIndexed::~ZERenderCommandIndexed()
{

}

ZERenderCommandInstanced::ZERenderCommandInstanced()
	: ZERenderCommand(sizeof(ZERenderCommandInstanced), ZE_RCT_INSTANCED)
{

}

ZERenderCommandInstanced::~ZERenderCommandInstanced()
{

}

ZERenderCommandIndexedInstanced::ZERenderCommandIndexedInstanced()
	: ZERenderCommand(sizeof(ZERenderCommandIndexedInstanced), ZE_RCT_INDEXED_INSTANCED)
{

}

ZERenderCommandIndexedInstanced::~ZERenderCommandIndexedInstanced()
{

}
