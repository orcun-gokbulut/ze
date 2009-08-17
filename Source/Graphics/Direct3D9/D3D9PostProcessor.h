//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9PostProcessor.h
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
#ifndef __ZE_D3D9POSTPROCESSOR_H__
#define __ZE_D3D9POSTPROCESSOR_H__

#include "Graphics/PostProcessor.h"
#include "Graphics/Texture.h"
#include "D3D9ComponentBase.h"
#include <d3d9.h>

class ZED3D9PostProcessor : public ZEPostProcessor, public ZED3D9ComponentBase
{
	private:
		LPDIRECT3DTEXTURE9						Source;
		LPDIRECT3DTEXTURE9						Destination;
		LPDIRECT3DTEXTURE9						InternalSource;
		LPDIRECT3DTEXTURE9						InternalDestination;

		static LPDIRECT3DVERTEXSHADER9			GeneralVS;
		static LPDIRECT3DPIXELSHADER9			ColorTransformPS;
		static LPDIRECT3DPIXELSHADER9			InversePS;
		static LPDIRECT3DPIXELSHADER9			GrayscalePS;
		static LPDIRECT3DPIXELSHADER9			DownSample4xPS;
		static LPDIRECT3DPIXELSHADER9			UpSample4xPS;
		static LPDIRECT3DPIXELSHADER9			HBlurPS;
		static LPDIRECT3DPIXELSHADER9			VBlurPS;
		static LPDIRECT3DPIXELSHADER9			SharpenPS;
		static LPDIRECT3DPIXELSHADER9			HEdgePS;
		static LPDIRECT3DPIXELSHADER9			VEdgePS;
		static LPDIRECT3DPIXELSHADER9			DirectPS;

		static LPDIRECT3DVERTEXDECLARATION9		ScreenQuadVD;
		static LPDIRECT3DVERTEXBUFFER9			ScreenQuadVB;

		bool									ManageSourceDestination(ZEPostProcessorSource Source_, ZEPostProcessorDestination Destination_, float Scale);
		void									DrawPostEffect(LPDIRECT3DPIXELSHADER9 PixelShader);

public:
		static bool								BaseInitialize();
		static void								BaseDeinitialize();

		virtual void							SetInput(ZETexture2D* Texture);
		virtual void							SetOutput(ZETexture2D* Texture);

		virtual void							DeviceLost();
		virtual bool							DeviceRestored();

		virtual void							DirectOutput(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);

		virtual void							ApplyUpSample4x(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);
		virtual void							ApplyDownSample4x(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);

		virtual void							ApplyColorTransform(ZEPostProcessorSource Source, ZEMatrix3x3 Matrix, ZEPostProcessorDestination Destination);
		virtual void							ApplyInverse(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);
		virtual void							ApplyGrayscale(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);

		virtual void							ApplyBlurH(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);
		virtual void							ApplyBlurV(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);

		virtual void							ApplySharpen(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);

		virtual void							ApplyEdgeDetectionH(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);
		virtual void							ApplyEdgeDetectionV(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination);

												ZED3D9PostProcessor();
												~ZED3D9PostProcessor();
};

#endif
