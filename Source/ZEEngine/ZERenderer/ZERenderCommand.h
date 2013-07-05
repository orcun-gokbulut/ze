//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderCommand.h
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

#pragma once
#ifndef	__ZE_RENDER_COMMAND_H__
#define __ZE_RENDER_COMMAND_H__

#include "ZETypes.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEGraphics/ZEVertexLayout.h"
#include "ZEGraphics/ZERasterizerState.h"
#include "ZEGraphics/ZEGraphicsDeviceState.h"

#define ZE_RCT_NONE						0
#define ZE_RCT_DEFAULT					1
#define ZE_RCT_INDEXED					2
#define ZE_RCT_INSTANCED				4
#define ZE_RCT_INDEXED_INSTANCED		8
typedef ZEFlagsBase<ZEUInt32>			ZERenderCommandType;

struct ZESkinningBuffer
{
	ZEMatrix4x4	BoneMatrices[58];
};

struct ZETransformationBuffer
{
	ZEMatrix4x4	WorldView;
	ZEMatrix4x4	WorldViewProjection;
};

/********************************************************************/
/*					Render Command Base Classes						*/
/********************************************************************/
class ZEMaterial;
class ZEVertexLayout;
class ZEVertexBuffer;
class ZEConstantBuffer;

class _ZERenderCommandDefaultBase
{
	public:
		ZEPrimitiveType			PrimitiveType;
		ZEUInt32				PrimitiveCount;

		ZEUInt32				FirstVertex;
		ZEVertexLayout*			VertexLayout;
		ZEVertexBuffer*			VertexBuffers[ZE_MAX_VERTEX_BUFFER_SLOT];

	protected:
								_ZERenderCommandDefaultBase();
								~_ZERenderCommandDefaultBase();
};

class ZEIndexBuffer;

class _ZERenderCommandIndexedBase
{
	public:
		ZEUInt32				BaseVertex;
		ZEUInt32				FirstIndex;
		ZEUInt32				IndexCount;
		ZEIndexBuffer*			IndexBuffer;

	protected:
								_ZERenderCommandIndexedBase();
								~_ZERenderCommandIndexedBase();
};

class _ZERenderCommandInstancedBase
{
	public:
		ZEUInt32				InstanceCount;
		ZEUInt32				FirstInstance;

	protected:
								_ZERenderCommandInstancedBase();
								~_ZERenderCommandInstancedBase();
};

class _ZERenderCommandIndexedInstancedBase : public _ZERenderCommandIndexedBase, 
											 public _ZERenderCommandInstancedBase
{
	protected:
								_ZERenderCommandIndexedInstancedBase();
								~_ZERenderCommandIndexedInstancedBase();
};

/********************************************************************/
/*						Render Command Classes						*/
/********************************************************************/

class ZERenderCommand
{
	friend class ZERenderer;
	friend class ZERenderStage;
	friend class ZECommandBuffer;

	protected:
		ZEUInt32				Id;
		ZESize					Size;
		ZERenderCommandType		Type;

								ZERenderCommand(ZESize CommandSize, ZERenderCommandType CommandType);
								~ZERenderCommand();

	public:
		float					Order;
		ZEInt32					Priority;
		
		ZEMaterial*				Material;
		
		bool					Skinned;
		ZEConstantBuffer*		SkinningBuffer;
		ZEConstantBuffer*		TransformationBuffer;

};

class ZERenderCommandDefault :	public ZERenderCommand,
								public _ZERenderCommandDefaultBase
{
	public:
								ZERenderCommandDefault();
								~ZERenderCommandDefault();
};

class ZERenderCommandIndexed :	public ZERenderCommand, 
								public _ZERenderCommandDefaultBase, 
								public _ZERenderCommandIndexedBase
{
	public:
								ZERenderCommandIndexed();
								~ZERenderCommandIndexed();
};

class ZERenderCommandInstanced : public ZERenderCommand,
								 public _ZERenderCommandDefaultBase,
								 public _ZERenderCommandInstancedBase
{
	public:
								ZERenderCommandInstanced();
								~ZERenderCommandInstanced();
};

class ZERenderCommandIndexedInstanced : public ZERenderCommand,
										public _ZERenderCommandDefaultBase,
										public _ZERenderCommandIndexedInstancedBase
{
	public:
								ZERenderCommandIndexedInstanced();
								~ZERenderCommandIndexedInstanced();
};

#endif
