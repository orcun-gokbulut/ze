//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRShaderMeta.h
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

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFlags.h"
#include "ZEGRTexture.h"
#include "ZEGRVertexLayout.h"
#include "ZEGRDefinitions.h"
#include "ZEGRShaderCompileOptions.h"

typedef ZEGRColorMask	ZEShaderRegisterMask;

ZE_EXPORT_ZEENGINE ZE_ENUM(ZEGRShaderRegisterType)
{
	ZEGR_SRT_FLOAT_32			= 0,
	ZEGR_SRT_UNSIGNED_INT_32	= 1,
	ZEGR_SRT_SIGNED_INT_32		= 2,
	ZEGR_SRT_NONE				= 3,
};

ZE_EXPORT_ZEENGINE ZE_ENUM(ZEGRShaderSystemSemantic)
{
	ZEGR_SSS_NONE						= 0,
	ZEGR_SSS_POSITION					= 1,
	ZEGR_SSS_CLIP_DISTANCE				= 2,
	ZEGR_SSS_CULL_DISTANCE				= 3,
	ZEGR_SSS_RENDER_TARGET_ARRAY_INDEX	= 4,
	ZEGR_SSS_VIEWPORT_ARRAY_INDEX		= 5,
	ZEGR_SSS_VERTEX_ID					= 6,
	ZEGR_SSS_PRIMITIVE_ID				= 7,
	ZEGR_SSS_INSTANCE_ID				= 8,
	ZEGR_SSS_IS_FRONT_FACE				= 9,
	ZEGR_SSS_SAMPLE_INDEX				= 10,
	ZEGR_SSS_TARGET						= 11,
	ZEGR_SSS_DEPTH						= 12,
	ZEGR_SSS_COVERAGE					= 13
};

ZE_EXPORT_ZEENGINE ZE_ENUM(ZEGRShaderConstantBufferType)
{
	ZEGR_SCBT_NONE		= 0,
	ZEGR_SCBT_C_BUFFER	= 1,
	ZEGR_SCBT_T_BUFFER	= 2
};

ZE_EXPORT_ZEENGINE ZE_ENUM(ZEGRShaderConstantType)
{
	ZEGR_SCDT_VOID		= 0,
    ZEGR_SCDT_BOOL		= 1,
    ZEGR_SCDT_INT		= 2,
    ZEGR_SCDT_UINT		= 3,
	ZEGR_SCDT_FLOAT		= 4
};

ZE_EXPORT_ZEENGINE ZE_ENUM(ZEGRShaderSamplerType)
{
	ZEGR_SSRT_NONE					= 0,
	ZEGR_SSRT_UNSIGNED_NORMALIZED	= 1,
	ZEGR_SSRT_SIGNED_NORMALIZED		= 2,
	ZEGR_SSRT_SIGNED_INTEGER		= 3,
	ZEGR_SSRT_UNSIGNED_INTEGER		= 4,
	ZEGR_SSRT_FLOAT					= 5,
	ZEGR_SSRT_DOUBLE				= 6,
	ZEGR_SSRT_MIXED					= 7
};

class ZE_EXPORT_ZEENGINE ZEGRShaderInput : public ZEObject
{
	ZE_OBJECT
	public:
		ZESize							Hash;
		ZEString						Name;
		ZEGRShaderSystemSemantic		SystemSemantic;
		ZEString						Semantic;
		ZEUInt8							Index;
		ZEGRVertexElementType			ElementType;
		ZEShaderRegisterMask			Mask;
		ZEGRShaderRegisterType			RegisterType;
};

class ZE_EXPORT_ZEENGINE ZEGRShaderConstant : public ZEObject
{
	ZE_OBJECT
	public:
		ZESize							Hash;
		ZEString						Name;
		ZEGRShaderConstantType			Type;
		ZESize							Size;
		ZESize							Offset;
		ZEUInt							RowCount;
		ZEUInt							ColumnCount;
};

class ZE_EXPORT_ZEENGINE ZEGRShaderConstantBuffer : public ZEObject
{
	ZE_OBJECT
	public:
		ZESize							Hash;
		ZEString						Name;
		ZEString						Semantic;
		ZEUInt							Slot;
		ZESize							Size;
		ZEGRShaderConstantBufferType	Type;
		ZEArray<ZEGRShaderConstant>		Constants;
};

class ZE_EXPORT_ZEENGINE ZEGRShaderSampler : public ZEObject
{
	ZE_OBJECT
	public:
		ZESize							Hash;
		ZEString						Name;
		ZEString						Semantic;
		ZEUInt							Slot;
		ZEUInt							SampleCount;
		ZEGRShaderSamplerType			SamplerReturnType;
};

class ZE_EXPORT_ZEENGINE ZEGRShaderTexture : public ZEObject
{
	ZE_OBJECT
	public:
		ZESize							Hash;
		ZEString						Name;
		ZEString						Semantic;
		ZEUInt							Slot;
		ZEUInt							Count;
		ZEGRTextureType					Type;
};

class ZE_EXPORT_ZEENGINE ZEGRShaderMeta : public ZEObject
{
	ZE_OBJECT
	public:
		ZEArray<ZEGRShaderSampler>			Samplers;
		ZEArray<ZEGRShaderTexture>			Textures;
		ZEArray<ZEGRShaderConstantBuffer>	Buffers;
		ZEArray<ZEGRShaderInput>			Inputs;
		ZEGRShaderCompileOptions			CompileOptions;
};
