//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureSampler.h
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

/*#include "ZETypes.h"
enum ZETextureSamplerAddresingMode
{
	ZE_TSAM_BORDER,
	ZE_TSAM_CLAMP,
	ZE_TSAM_WRAP,
	ZE_TSAM_MIRROR
};

enum ZETextureSamplerFilteringMode
{
	ZE_TSFM_POINT,
	ZE_TSFM_LINEAR
};

class ZETextureSampler
{
	private:
		void*							Data;
		ZESize							Width;
		ZESize							Height;
		ZEUInt							MipmapCount;
		ZETextureSamplerAddresingMode	VerticalAddressingMode;
		ZETextureSamplerAddresingMode	HorizontalAddressingMode;
		ZETextureSamplerFilteringMode	FilteringMode;

	public:
		void							SetData(void* Data);
		void*							GetData();

		void							SetWidth(ZESize Width);
		ZESize							GetWidth();

		void							SetHeight(ZESize Height);
		ZESize							GetHeight();

		void							SetMipmapCount(ZEUInt MipmapCount);
		ZESize							GetMipmapCount();

		void							SetPixelSize(ZESize PixelSize);
		ZESize							GetPixelSize();

		void							SetVerticalAddresingMode(ZETextureSamplerAddresingMode AddresingMode);
		ZETextureSamplerAddresingMode	GetVerticalAddresingMode();

		void							SetHorizontalAddresingMode(ZETextureSamplerAddresingMode AddresingMode);
		ZETextureSamplerAddresingMode	GetHorizontalAddresingMode();

		void							SetFilteringMode(ZETextureSamplerFilteringMode FilteringMode);
		ZETextureSamplerFilteringMode	GetFilteringMode();

		template<typename ZEPixelType>
		ZEPixelType*					SampleRaw(ZEInt x, ZEInt y);
		template<typename ZEPixelType>
		ZEPixelType						SampleLinear(float x, float y);
		template<typename ZEPixelType>
		ZEPixelType						SamplePoint(float x, float y);

		template<typename ZEPixelType>
		ZEPixelType*					Sample(float x, float y);
};

struct ZEUInt32_4
{
	ZEUInt32 Components[4];
	
	typedef ZEUInt32_4 ImmediateType;

	void Add(ZEUInt32_4& Output, const ZEUInt8_4& A, const ZEUInt8_4& B)
	{
		Output.Components[0] = A.Components[0] + B.Components[0];
		Output.Components[1] = A.Components[1] + B.Components[1];
		Output.Components[2] = A.Components[2] + B.Components[2];
		Output.Components[3] = A.Components[3] + B.Components[3];
	}

	void Scale(ZEUInt32_4& Output, const ZEUInt8_4& A, float Factor)
	{
		Output.Components[0] = A.Components[0] * Factor;
		Output.Components[1] = A.Components[1] * Factor;
		Output.Components[2] = A.Components[2] * Factor;
		Output.Components[3] = A.Components[3] * Factor;
	}
};

struct ZEUInt8_4
{
	ZEUInt8 Components[4];
	
	typedef ZEUInt32_4 ImmediateType;

	void Add(ZEUInt32_4& Output, const ZEUInt8_4& A, const ZEUInt8_4& B)
	{
		Output.Components[0] = A.Components[0] + B.Components[0];
		Output.Components[1] = A.Components[1] + B.Components[1];
		Output.Components[2] = A.Components[2] + B.Components[2];
		Output.Components[3] = A.Components[3] + B.Components[3];
	}

	void Scale(ZEUInt32_4& Output, const ZEUInt8_4& A, float Factor)
	{
		Output.Components[0] = A.Components[0] * Factor;
		Output.Components[1] = A.Components[1] * Factor;
		Output.Components[2] = A.Components[2] * Factor;
		Output.Components[3] = A.Components[3] * Factor;
	}
};

template<typename ZEPixelType>
ZEPixelType ZETextureSampler::SampleLinear(float x, float y)
{
	u = x - 0.5f;
	v = y - 0.5f;
	ZEInt x = (int)(u);
	ZEInt y = (int)(v);
	float HorizontalBlend = u - x;
	float VerticalBlend = v - y;

	ZEPixelType::Immediate Sample00 = *(ZEPixelType*)SampleRaw(x,		y);
	ZEPixelType::Immediate Sample10 = *(ZEPixelType*)SampleRaw(x + 1,	y);
	ZEPixelType::Immediate Sample01 = *(ZEPixelType*)SampleRaw(x,		y + 1);
	ZEPixelType::Immediate Sample11 = *(ZEPixelType*)SampleRaw(x + 1,	y + 1);

	ZEPixelType::::Immediate Vertical, Horizontal;
	ZE
	return (Sample00 * (1.0f - HorizontalBlend) + Sample10 * HorizontalBlend) * (1.0f - VerticalBlend) + 
		(Sample00 * (1.0f - HorizontalBlend) + Sample10  * HorizontalBlend) * VerticalBlend;
}

template<typename ZEPixelType>
ZEPixelType ZETextureSampler::SamplePoint(float x, float y)
{
	u = x - 0.5f;
	v = y - 0.5f;
	return (ZEPixelType*)SampleRaw(u, v);
}

template<typename ZEPixelType>
ZEPixelType* ZETextureSampler::SampleRaw(ZEInt x, ZEInt y)
{
	switch(VerticalAddressingMode)
	{
		case ZE_TSAM_BORDER:
			break;

		case ZE_TSAM_CLAMP:
			if (x < 0)
				x = 0;
			break;

		case ZE_TSAM_WRAP:
			x = x % Width;
			break;
	}

	switch(HorizontalAddressingMode)
	{
		case ZE_TSAM_CLAMP:
			if (y < 0)
				y = 0;
			break;

		case ZE_TSAM_WRAP:
			y = y % Width;
			break;
	}

	return (ZEPixelType*)((ZEUInt8*)Data + Width * PixelSize * y + x * PixelSize);
}*/
