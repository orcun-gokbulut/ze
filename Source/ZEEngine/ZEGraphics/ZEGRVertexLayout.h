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

#include "ZEGRState.h"
#include "ZEGRDefinitions.h"

#include "ZETypes.h"
#include "ZECommon.h"
#include "ZEDS/ZEArray.h"

class ZEGRShaderMeta;

ZE_ENUM(ZEGRVertexElementType)
{
	ZEGR_VET_NONE				= 0,

		ZEGR_VET_FLOAT				= 1,
		ZEGR_VET_FLOAT_2			= 2,
		ZEGR_VET_FLOAT_3			= 3,
		ZEGR_VET_FLOAT_4			= 4,

		ZEGR_VET_INT8				= 5,
		ZEGR_VET_INT8_NORM			= 6,
		ZEGR_VET_INT8_2				= 7,
		ZEGR_VET_INT8_2_NORM		= 8,
		ZEGR_VET_INT8_4				= 9,
		ZEGR_VET_INT8_4_NORM		= 10,

		ZEGR_VET_UINT8				= 11,
		ZEGR_VET_UINT8_NORM			= 12,
		ZEGR_VET_UINT8_2			= 13,
		ZEGR_VET_UINT8_2_NORM		= 14,
		ZEGR_VET_UINT8_4			= 15,
		ZEGR_VET_UINT8_4_NORM		= 16,

		ZEGR_VET_INT16				= 17,
		ZEGR_VET_INT16_NORM			= 18,
		ZEGR_VET_INT16_2			= 19,
		ZEGR_VET_INT16_2_NORM		= 20,
		ZEGR_VET_INT16_4			= 21,
		ZEGR_VET_INT16_4_NORM		= 22,

		ZEGR_VET_UINT16				= 23,
		ZEGR_VET_UINT16_NORM		= 24,
		ZEGR_VET_UINT16_2			= 25,
		ZEGR_VET_UINT16_2_NORM		= 26,
		ZEGR_VET_UINT16_4			= 27,
		ZEGR_VET_UINT16_4_NORM		= 28,

		ZEGR_VET_INT32				= 29,
		ZEGR_VET_INT32_NORM			= 30,
		ZEGR_VET_INT32_2			= 31,
		ZEGR_VET_INT32_2_NORM		= 32,
		ZEGR_VET_INT32_3			= 33,
		ZEGR_VET_INT32_3_NORM		= 34,
		ZEGR_VET_INT32_4			= 35,
		ZEGR_VET_INT32_4_NORM		= 36,

		ZEGR_VET_UINT32				= 37,
		ZEGR_VET_UINT32_2			= 38,
		ZEGR_VET_UINT32_3			= 39,
		ZEGR_VET_UINT32_4			= 40,
};

ZE_ENUM(ZEGRVertexUsage)
{
	ZEGR_VU_PER_VERTEX			= 0,
	ZEGR_VU_PER_INSTANCE		= 1
};

ZE_ENUM(ZEGRVertexElementSemantic)
{
	ZEGR_VES_UNKNOWN			= 0,
	ZEGR_VES_POSITION			= 1,
	ZEGR_VES_NORMAL				= 2,
	ZEGR_VES_BINORMAL			= 3,
	ZEGR_VES_TANGENT			= 4,
	ZEGR_VES_TEXCOORD			= 5,
	ZEGR_VES_COLOR				= 6,
	ZEGR_VES_BLEND_INDICES		= 7,
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
	ZE_OBJECT
	private:
		struct ZEVertexLayoutData
		{
			ZEUInt8							ElementCount;
			ZEGRVertexElement				Elements[ZEGR_MAX_VERTEX_LAYOUT_ELEMENT];		
		} StateData;

	public:
		virtual ZEGRStateType				GetStateType() const;
		virtual const void*					GetData() const;
		virtual ZESize						GetDataSize() const;

		bool								SetElements(const ZEGRVertexElement* VertexElements, ZEUInt ElementCount);
		
		ZEUInt								GetElementCount() const;
		const ZEGRVertexElement*			GetElements() const;
		
		void								SetToDefault();

		static ZEGRVertexLayout*			Generate(ZEGRShaderMeta* Shader);

											ZEGRVertexLayout();
};
