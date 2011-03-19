//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9HDRProcessor.h
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

#include "ZED3D9ComponentBase.h"
#include <d3d9.h>
#include "ZEMeta/ZEClass.h"

class ZETexture2D;
class ZED3D9Texture2D;
class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9ViewPort;

ZE_META_CLASS_DESCRIPTION(ZED3D9HDRProcessor);

class ZED3D9HDRProcessor : public ZED3D9ComponentBase, public ZEClass
{
	ZE_META_CLASS(ZED3D9HDRProcessor);

	private:
		ZED3D9Texture2D*				Input;
		ZED3D9ViewPort*					Output;
		size_t							ScreenWidth, ScreenHeight;

		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;

		struct 
		{
			ZED3D9Texture2D*			Luminance5;
			ZED3D9Texture2D*			Luminance4;
			ZED3D9Texture2D*			Luminance3;
			ZED3D9Texture2D*			Luminance2;
			ZED3D9Texture2D*			Luminance1;
			ZED3D9Texture2D*			Luminance;
			ZED3D9Texture2D*			OldLuminance;
			ZED3D9Texture2D*			DownSampled2xA;
			ZED3D9Texture2D*			DownSampled2xB;
			ZED3D9Texture2D*			DownSampled4xA;
			ZED3D9Texture2D*			DownSampled4xB;
			ZED3D9Texture2D*			DownSampled8xA;
			ZED3D9Texture2D*			DownSampled8xB;
			ZED3D9Texture2D*			DownSampled16xA;
			ZED3D9Texture2D*			DownSampled16xB;
		} Textures;

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

		void							CreateRenderTargets();
		void							ReleaseRenderTargets();

		void							MeasureLuminance(ZED3D9Texture2D* Input, ZED3D9Texture2D* OldLuminance, ZED3D9ViewPort* Output);
		void							BrightPass(ZED3D9Texture2D* Input, ZED3D9ViewPort* Output);
		void							BlurPass(ZED3D9Texture2D* Input, ZED3D9Texture2D* Temp, ZED3D9ViewPort* Output);
		void							DownSample2x(ZED3D9Texture2D* Input, ZED3D9ViewPort* Output);
		void							ToneMap(ZED3D9Texture2D* Input, ZED3D9ViewPort* Output);
		
		struct 
		{
			float Key;
			float BrightPassTreshold;
			float BloomFactor;
			float BloomStandardDeviation;
			
			float MaxLuminanceChange;
			float MaxLuminanceChangePerSecond;
			float Reserved1;
			float Reserved2;
		} Parameters;

		struct
		{
			int BloomSampleCount;
			int	BloomPassCount;
			int Reserved0;
			int Reserved1;
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

		void							SetBloomSampleCount(unsigned int Count);
		unsigned int					GetBloomSampleCount() const;

		void							SetBloomPassCount(unsigned int Count);
		unsigned int					GetBloomPassCount() const;

		void							SetInput(ZED3D9Texture2D* Input);
		ZED3D9Texture2D*				GetInput();

		void							SetOutput(ZED3D9ViewPort* Output);
		ZED3D9ViewPort*					GetOutput();

		void							Process(float ElapsedTime);

										ZED3D9HDRProcessor();
										~ZED3D9HDRProcessor();
};

/*
ZE_POST_PROCESSOR_START(Meta)
	<zinek>
		<meta> 
			<class name="ZED3D9HDRProcessor">
				<noinstance>true</noinstance>
				<description>ZED3D9HDRProcessor</description>
				<property name="Key" type="float" autogetset="yes" description="..."/>
				<property name="BrightPassTreshold" type="float" autogetset="yes" description="..."/>
				<property name="BloomFactor" type="float" autogetset="yes" description="..."/>
				<property name="BloomStandardDeviation" type="float" autogetset="yes" description="..."/>
				<property name="BloomSampleCount" type="integer" autogetset="yes" description="..."/>
				<property name="BloomPassCount" type="integer" autogetset="yes" description="..."/>
				<property name="MaxLuminanceChangePerSecond" type="float" autogetset="yes" description="..."/>
				
			</class>
		</meta>
	</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif
