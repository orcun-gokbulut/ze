//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9TextureResizer.h
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
#ifndef __ZE_D3D9_TEXTURE_DOWNSAMPLER_H__
#define __ZE_D3D9_TEXTURE_DOWNSAMPLER_H__

#include "ZED3D9ComponentBase.h"

class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9Texture2D;
class ZETexture2D;
class ZED3D9ViewPort;
class ZEFrameRenderer;
class ZED3D9FrameRenderer;
class ZETexture2DResource;


enum ZED3D9TextureResizeFilter
{
	ZE_D3D9_RF_BOX			= 0,
	ZE_D3D9_RF_TRIANGLE		= 1,
	ZE_D3D9_RF_CUBIC		= 2,
	ZE_D3D9_RF_GAUSSIAN		= 3,
	ZE_D3D9_RF_QUADRATIC	= 4,
	ZE_D3D9_RF_KAISER		= 5,
	ZE_D3D9_RF_MITCHELL		= 6,
};

enum ZED3D9FittingPowerof2Mode
{
	ZE_D3D9_FPO2_AUTO			= 0,
	ZE_D3D9_FPO2_NEXTPO2		= 1,
	ZE_D3D9_FPO2_PREVIOUSPO2	= 2,
};


class ZED3D9TextureResizer : public ZED3D9ComponentBase
{
	struct TextureInfo
	{
		void*			Buffer;
		unsigned int	Pitch;
		unsigned int	Width;
		unsigned int	Height;
	};

	private:

		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;
		ZED3D9VertexShader*				VertexShader;
		ZED3D9PixelShader*				PixelShaderHorizontal;
		ZED3D9PixelShader*				PixelShaderVertical;

		ZED3D9TextureResizeFilter		ResizeFilter;
		ZED3D9FittingPowerof2Mode		AutoFitMode;

		TextureInfo						DestInfo;
		TextureInfo						SrcInfo;

		unsigned int					NextPowerOfTwo(unsigned int Value);
		unsigned int					PreviousPowerOfTwo(unsigned int Value);
		bool							IsPowerOfTwo(unsigned int Value);
		unsigned int					GetPowerOfTwo(unsigned int Value);
		bool							Resize(unsigned int DestWidth, unsigned int DestHeight);

	public:

		void							SetResizeFilter(ZED3D9TextureResizeFilter ResizeFilter);
		ZED3D9TextureResizeFilter		GetResizeFilter();

		void							SetAutoFitMode(ZED3D9FittingPowerof2Mode AutoFitMode);
		ZED3D9FittingPowerof2Mode		GetAutoFitMode();

		void							OnDeviceLost();
		void							OnDeviceRestored();

		void							Initialize(void* DestData, unsigned int DestPitch, unsigned int DestWidth, unsigned int DestHegiht,
													void* SrcData, unsigned int SrcPitch, unsigned int SrcWidth, unsigned int SrcHeight);
		void							Deinitialize();

		bool							Process();

										ZED3D9TextureResizer();
										~ZED3D9TextureResizer();
};

#endif	/* __ZE_D3D9_TEXTURE_DOWNSAMPLER_H__ */
