//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRVertexLayout.h
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

#include "ZEGRResource.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEGRDefinitions.h"
#include "ZEGRState.h"

struct ZEGRShaderMeta;

enum ZEGRVertexElementType
{
	ZEGR_VET_NONE				= 0,
	ZEGR_VET_INT				= 1,
	ZEGR_VET_INT2				= 2,
	ZEGR_VET_INT3				= 3,
	ZEGR_VET_INT4				= 4,
	ZEGR_VET_UINT				= 5,
	ZEGR_VET_UINT2				= 6,
	ZEGR_VET_UINT3				= 7,
	ZEGR_VET_UINT4				= 8,
	ZEGR_VET_FLOAT				= 9,
	ZEGR_VET_FLOAT2				= 10,
	ZEGR_VET_FLOAT3				= 11,
	ZEGR_VET_FLOAT4				= 12
};

enum ZEGRVertexUsage
{
	ZEGR_VU_PER_VERTEX			= 0,
	ZEGR_VU_PER_INSTANCE		= 1
};

enum ZEGRVertexElementSemantic
{
	ZEGR_VES_UNKNOWN			= 0,
	ZEGR_VES_POSITION			= 1,
	ZEGR_VES_NORMAL				= 2,
	ZEGR_VES_BINORMAL			= 3,
	ZEGR_VES_TANGENT			= 4,
	ZEGR_VES_TEXCOORD			= 5,
	ZEGR_VES_COLOR				= 6,
	ZEGR_VES_BLEND_INDEXES		= 7,
	ZEGR_VES_BLEND_WEIGHTS		= 8,
	ZEGR_VES_CUSTOM				= 9
};


struct ZEGRVertexElement
{
	ZEGRVertexElementSemantic		Semantic;
	ZEUInt8							Index;
	ZEGRVertexElementType			Type;
	ZEUInt8							Stream;
	ZEUInt16						Offset;
	ZEGRVertexUsage					Usage;
	ZEUInt16						InstanceCount;
};

class ZEGRVertexLayout : public ZEGRState
{
	private:
		struct ZEVertexLayoutData
		{
			ZEUInt8					ElementCount;
			ZEGRVertexElement		Elements[ZEGR_MAX_VERTEX_LAYOUT_ELEMENT];		
		} StateData;

	public:
		virtual const void*			GetData() const;
		virtual ZESize				GetDataSize() const;

		bool						SetElements(const ZEGRVertexElement* VertexElements, ZEUInt ElementCount);
		
		ZEUInt						GetElementCount() const;
		const ZEGRVertexElement*	GetElements() const;
		
		void						SetToDefault();

		static ZEGRVertexLayout*	Generate(ZEGRShaderMeta* Shader);

									ZEGRVertexLayout();
									~ZEGRVertexLayout();
};
