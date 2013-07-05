//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVertexLayout.h
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
#ifndef	__ZE_DEVICE_STATE_VERTEX_LAYOUT_H__
#define __ZE_DEVICE_STATE_VERTEX_LAYOUT_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEGraphicsDefinitions.h"

enum ZEVertexElementType
{
	ZE_VET_NONE			= 0,
	ZE_VET_INT			= 1,
	ZE_VET_INT2			= 2,
	ZE_VET_INT3			= 3,
	ZE_VET_INT4			= 4,
	ZE_VET_UINT			= 5,
	ZE_VET_UINT2		= 6,
	ZE_VET_UINT3		= 7,
	ZE_VET_UINT4		= 8,
	ZE_VET_FLOAT		= 9,
	ZE_VET_FLOAT2		= 10,
	ZE_VET_FLOAT3		= 11,
	ZE_VET_FLOAT4		= 12
};

enum ZEVertexUsage
{
	ZE_VU_PER_VERTEX	= 0,
	ZE_VU_PER_INSTANCE	= 1
};

struct ZEVertexElement
{
	char					Semantic[ZE_MAX_SHADER_VARIABLE_NAME];
	ZEUInt8					Index;
	ZEVertexElementType		Type;
	ZEUInt8					Stream;
	ZEUInt16				Offset;
	ZEVertexUsage			Usage;
	ZEUInt16				InstanceCount;
};

class ZEShader;
class ZEVertexBuffer;

class ZEVertexLayout
{
	protected:
		ZESize					Hash;
		bool					Dirty;

		struct ZEVertexLayoutData
		{
			ZEUInt				ElementCount;
			ZEVertexElement		VertexElements[ZE_MAX_VERTEX_LAYOUT_ELEMENT];
		
		} StateData;

	public:
		ZESize					GetHash();
		bool					IsEmpty() const;
		
		ZEUInt					GetElementCount() const;
		const ZEVertexElement*	GetLayout() const;

		void					SetLayout(const ZEVertexElement* VertexElements, ZEUInt ElementCount);
		
		void					SetToDefault();

		const ZEVertexLayout&	operator=(const ZEVertexLayout& State);
		bool					operator==(const ZEVertexLayout& State);
		bool					operator!=(const ZEVertexLayout& State);

		static bool				GenerateLayout(ZEVertexLayout& Output, ZEVertexBuffer* BufferArr[ZE_MAX_VERTEX_BUFFER_SLOT], const ZEShader* VertexShader);

								ZEVertexLayout();
								~ZEVertexLayout();
};

#endif
