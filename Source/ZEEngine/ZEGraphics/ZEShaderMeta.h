//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEShaderMeta.h
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
#ifndef __ZE_SHADER_META_INFO_H__
#define __ZE_SHADER_META_INFO_H__

#include "ZEDS/ZEFlags.h"
#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZETexture.h"
#include "ZEVertexLayout.h"
#include "ZEGraphicsDefinitions.h"

enum ZEShaderRegisterType
{
	ZE_SRT_FLOAT_32			= 0,
	ZE_SRT_UNSIGNED_INT_32	= 1,
	ZE_SRT_SIGNED_INT_32	= 2,
	ZE_SRT_NONE				= 3,
};

enum ZEShaderSystemValueType
{
	ZE_SSVT_NONE						= 0,
	ZE_SSVT_POSITION					= 1,
	ZE_SSVT_CLIP_DISTANCE				= 2,
	ZE_SSVT_CULL_DISTANCE				= 3,
	ZE_SSVT_RENDER_TARGET_ARRAY_INDEX	= 4,
	ZE_SSVT_VIEWPORT_ARRAY_INDEX		= 5,
	ZE_SSVT_VERTEX_ID					= 6,
	ZE_SSVT_PRIMITIVE_ID				= 7,
	ZE_SSVT_INSTANCE_ID					= 8,
	ZE_SSVT_IS_FRONT_FACE				= 9,
	ZE_SSVT_SAMPLE_INDEX				= 10,
	ZE_SSVT_TARGET						= 11,
	ZE_SSVT_DEPTH						= 12,
	ZE_SSVT_COVERAGE					= 13
};

typedef ZEComponentMask	ZEShaderRegisterMask;

struct ZEShaderInput
{
	ZESize						Hash;
	char						Semantic[ZE_MAX_SHADER_VARIABLE_NAME];
	ZEUInt8						Index;
	ZEVertexElementType			ElementType;
	ZEShaderSystemValueType		SystemValue;
	ZEShaderRegisterMask		UsedRegisters;
	ZEShaderRegisterType		RegisterType;

	static ZESize				GetHash(const char* Semantic, ZEUInt8 Index);

								ZEShaderInput();
								~ZEShaderInput();
};

enum ZEShaderConstantBufferType
{
	ZE_SCBT_C_BUFFER	= 0,
	ZE_SCBT_T_BUFFER	= 1
};

enum ZEShaderConstantType
{
	ZE_SCDT_VOID		= 0,
    ZE_SCDT_BOOL		= 1,
    ZE_SCDT_INT			= 2,
    ZE_SCDT_UINT		= 3,
	ZE_SCDT_FLOAT		= 4
};

struct ZEShaderConstant
{
	ZESize					Hash;
	ZEString				Name;
	ZEShaderConstantType	Type;
	ZESize					Size;
	ZESize					Offset;
	ZEUInt					RowCount;
	ZEUInt					ColumnCount;
};

struct ZEShaderBuffer
{
	ZESize							Hash;
	ZEString						Name;
	ZEUInt							Slot;
	ZESize							Size;
	ZEShaderConstantBufferType		Type;
	ZEArray<ZEShaderConstant>		Constants;
};

enum ZEShaderSamplerReturnType
{
	ZE_SSRT_UNSIGNED_NORMALIZED		= 1,
	ZE_SSRT_SIGNED_NORMALIZED		= 2,
	ZE_SSRT_SIGNED_INTEGER			= 3,
	ZE_SSRT_UNSIGNED_INTEGER		= 4,
	ZE_SSRT_FLOAT					= 5,
	ZE_SSRT_MIXED					= 6
};

struct ZEShaderSampler
{
	ZESize						Hash;
	ZEString					Name;
	ZEUInt						Slot;
	ZEUInt						SampleCount;
	ZEShaderSamplerReturnType	SamplerReturnType;
};

struct ZEShaderTexture
{
	ZESize				Hash;
	ZEString			Name;
	ZEUInt				Slot;
	ZEUInt				Count;
	ZETextureType		Type;
};

#endif
