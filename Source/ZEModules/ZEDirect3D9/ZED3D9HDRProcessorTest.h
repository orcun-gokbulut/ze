//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9HDRProcessorTest.h
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
#ifndef __ZE_D3D9_HDR_PROCESSOR_H__
#define __ZE_D3D9_HDR_PROCESSOR_H__

#include "ZETypes.h"
#include "ZED3D9ComponentBase.h"
#include "ZEMeta/ZEObject.h"

#include <d3d9.h>

class ZETexture2D;
class ZED3D9Texture2D;
class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9ViewPort;

struct ZEHDRScreenAlignedQuad
{
	float Position[3];
};

ZE_META_OBJECT_DESCRIPTION(ZED3D9HDRProcessorTest);

class ZED3D9HDRProcessorTest : public ZED3D9ComponentBase, public ZEObject
{
	ZE_META_OBJECT(ZED3D9HDRProcessorTest);

	private:
		static ZEHDRScreenAlignedQuad	Vertices[4];

		ZED3D9Texture2D*				Input;
		ZED3D9ViewPort*					Output;
		ZESize							ScreenWidth, ScreenHeight;

		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;

		ZED3D9Texture2D*				Blurred[6];
		ZED3D9Texture2D*				Mipmaps[15];
		ZEUInt							MipCount;
		
		struct
		{
			ZED3D9VertexShader*			VertexShader;
			ZED3D9PixelShader*			MeasureLuminanceStart;
			ZED3D9PixelShader*			MeasureLuminanceDownSample3x;
			ZED3D9PixelShader*			MeasureLuminanceEnd;
			ZED3D9PixelShader*			BrightPass; 
			ZED3D9PixelShader*			DownSample2x;
			ZED3D9PixelShader*			VerticalBloom;
			ZED3D9PixelShader*			HorizontalBloom;
			ZED3D9PixelShader*			ToneMap;

		} Shaders;

		void							UpdateRenderTargets();
		void							ReleaseRenderTargets();

		void							ApplyBloomToneMap();
		void							GenerateBloomTexture();
		void							CreateMipMapsMeasureLuminance();
		
		struct 
		{
			float						Key;
			float						BrightPassTreshold;
			float						BloomFactor;
			float						BloomStandardDeviation;
			
			float						MaxLuminanceChange;
			float						MaxLuminanceChangePerSecond;
			float						Reserved1;
			float						Reserved2;
		} Parameters;

		struct
		{
			ZEInt						BloomSampleCount;
			ZEInt						BloomPassCount;
			ZEInt						Reserved0;
			ZEInt						Reserved1;
		} IntParameters;

	public:
		void							Initialize();
		void							Deinitialize();

		void							SetKey(float Key);
		float							GetKey() const;

		void							SetBrightPassTreshold(float Treshold);
		float							GetBrightPassTreshold() const;

		void							SetMaxLuminanceChangePerSecond(float LuminanceChange);
		float							GetMaxLuminanceChangePerSecond() const;

		void							SetBloomFactor(float Factor);
		float							GetBloomFactor() const;

		void							SetBloomStandardDeviation(float Deviation);
		float							GetBloomStandardDeviation() const;

		void							SetBloomSampleCount(ZEUInt Count);
		ZEUInt							GetBloomSampleCount() const;

		void							SetBloomPassCount(ZEUInt Count);
		ZEUInt							GetBloomPassCount() const;

		void							SetInput(ZED3D9Texture2D* Input);
		ZED3D9Texture2D*				GetInput();

		void							SetOutput(ZED3D9ViewPort* Output);
		ZED3D9ViewPort*					GetOutput();

		void							Process(float ElapsedTime);

										ZED3D9HDRProcessorTest();
										~ZED3D9HDRProcessorTest();
};

/*
ZE_POST_PROCESSOR_START(Meta)
	<zinek>
		<meta> 
			<class name="ZED3D9HDRProcessorTest">
				<noinstance>true</noinstance>
				<description>ZED3D9HDRProcessorTest</description>
				<property name="Key" type="float" autogetset="yes" description="..."/>
				<property name="BrightPassTreshold" type="float" autogetset="yes" description="..."/>
				<property name="BloomFactor" type="float" autogetset="yes" description="..."/>
				<property name="BloomStandardDeviation" type="float" autogetset="yes" description="..."/>
				<property name="BloomSampleCount" type="integer32" autogetset="yes" description="..."/>
				<property name="BloomPassCount" type="integer32" autogetset="yes" description="..."/>
				<property name="MaxLuminanceChangePerSecond" type="float" autogetset="yes" description="..."/>
				
			</class>
		</meta>
	</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif
