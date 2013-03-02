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

enum ZERenderPipeline
{
	ZE_RP_NONE		= 0,
	ZE_RP_3D		= 1,
	ZE_RP_2D		= 2
};

#define ZE_RCT_DEFAULT						1
#define ZE_RCT_INDEXED						2
#define ZE_RCT_INSTANCED					4
#define ZE_RCT_SKINNED						8
#define ZE_RCT_LIGHT						16
#define ZE_RCT_SHADOW						32
#define ZE_RCT_TERRAIN						64
#define ZE_RCT_PARTICLE						128
typedef ZEUInt32 ZERenderCommandType;

#define	ZE_RCF_NONE							0
#define	ZE_RCF_Z_CULL						1
#define	ZE_RCF_NO_Z_WRITE					2
#define	ZE_RCF_WORLD_TRANSFORM				4
#define ZE_RCF_VIEW_TRANSFORM				8
#define ZE_RCF_PROJECTION_TRANSFORM			16
#define	ZE_RCF_VIEW_PROJECTION_TRANSFORM	(ZE_RCF_VIEW_TRANSFORM | ZE_RCF_PROJECTION_TRANSFORM)
typedef ZEUInt32 ZERenderCommandFlags;

class ZELight;
class ZEMaterialNew;
class ZEMaterial;
class ZEIndexBuffer;
class ZEVertexBuffer;
class ZEVertexLayout;
class ZERenderTarget;


/************************************************************************/
/*                        RENDER COMMAND TEST                           */
/************************************************************************/
class ZERenderCommandTest
{
	public:
		ZESize					Size;

		ZERenderCommandType		Type;
		float					Order;
		ZEInt32					Priority;

		ZEMaterialNew*			Material;

		ZEPrimitiveType			PrimitiveType;
		ZEUInt32				PrimitiveCount;

		ZEUInt32				FirstVertex;
		ZEVertexLayout*			VertexLayout;
		ZEVertexBuffer*			VertexBuffers[ZE_MAX_VERTEX_BUFFER_SLOT];

								ZERenderCommandTest();
								~ZERenderCommandTest();
};

class ZERenderCommandIndexedTest : public ZERenderCommandTest
{
	public:
		ZEUInt32				BaseVertex;
		ZEUInt32				FirstIndex;
		ZEUInt32				IndexCount;
		ZEIndexBuffer*			IndexBuffer;

								ZERenderCommandIndexedTest();
								~ZERenderCommandIndexedTest();
};

class ZERenderCommandInstancedTest : public ZERenderCommandTest
{
	public:
		ZEUInt32				InstanceCount;
		ZEUInt32				FirstInstance;

								ZERenderCommandInstancedTest();
								~ZERenderCommandInstancedTest();
};

/************************************************************************/
/*                        OLD RENDER COMMAND                            */
/************************************************************************/
class ZERenderCommand
{
	public:
		ZESize					Size;

		float					Order;
		ZEInt					Priority;

		ZERenderCommandType		Type;
		ZERenderCommandFlags	Flags;
		ZERenderPipeline		Pipeline;

		ZEPrimitiveType			PrimitiveType;
		ZEUInt					PrimitiveCount;
		
		ZEUInt					FirstVertex;
		ZEVertexLayout			VertexLayout;
		ZEVertexBuffer*			VertexBuffers[ZE_MAX_VERTEX_BUFFER_SLOT];

		ZEMaterial*				Material;
		ZEMatrix4x4				WorldMatrix;
		ZEMatrix4x4				LocalMatrix;

								ZERenderCommand();
								~ZERenderCommand();
};

class ZERenderCommandSkinned : public ZERenderCommand
{
	public:
		ZEArray<ZEMatrix4x4>	BoneTransforms;

								ZERenderCommandSkinned();
								~ZERenderCommandSkinned();
};

class ZERenderCommandIndexed : public ZERenderCommand
{
	public:
		ZEUInt					BaseVertex;
		ZEUInt					FirstIndex;
		ZEUInt					IndexCount;
		ZEIndexBuffer*			IndexBuffer;

								ZERenderCommandIndexed();
								~ZERenderCommandIndexed();
};

class  ZERenderCommandInstanced : public ZERenderCommand
{
	public:
		ZEUInt					InstanceCount;
		ZEUInt					FirstInstance;

								ZERenderCommandInstanced();
								~ZERenderCommandInstanced();
};

#endif
