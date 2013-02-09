//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageLighting.h
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
#ifndef __ZE_RENDER_STAGE_LIGHTING_H__
#define __ZE_RENDER_STAGE_LIGHTING_H__

#include "ZETypes.h"
#include "ZELight.h"
#include "ZEDS/ZEArray.h"
#include "ZERenderStage.h"

class ZEShader;
class ZETexture2D;
class ZEVertexBuffer;
class ZERenderTarget;
class ZERenderCommand;
class ZEConstantBuffer;
class ZERenderStageGeometry;

class ZEPointLight;
class ZEProjectiveLight;
class ZEDirectionalLight;
class ZEOmniProjectiveLight;

class ZERenderStageLighting : public ZERenderStage
{
	protected:
		ZELightType						LastLight;

		const ZERenderStageGeometry*	GBufferInput;
		ZESmartArray<ZELight*>*			LightList;
		ZEUInt32						StencilMask;
		ZEVertexBuffer*					VertexBuffer;

		struct
		{
			ZETexture2D*				LBuffer1;
			ZETexture2D*				LBuffer2;

		} Textures;

		struct
		{
			ZERenderTarget*				LBuffer1;
			ZERenderTarget*				LBuffer2;

		} RenderTargets;


		struct
		{
			ZEShader*					VertexShader;
			ZEShader*					PixelShader;
			ZEConstantBuffer*			Transformations;
			ZEConstantBuffer*			LightParameters;
			ZEConstantBuffer*			ShadowParameters;

		} PointLight;

		struct
		{
			ZEShader*					VertexShader;
			ZEShader*					PixelShader;
			ZEConstantBuffer*			Transformations;
			ZEConstantBuffer*			LightParameters;
			ZEConstantBuffer*			ShadowParameters;

		} DirectionalLight;

		struct
		{
			ZEShader*					VertexShader;
			ZEShader*					PixelShader;
			ZEConstantBuffer*			Transformations;
			ZEConstantBuffer*			LightParameters;
			ZEConstantBuffer*			ShadowParameters;

		} ProjectiveLight;

		struct
		{
			ZEShader*					VertexShader;
			ZEShader*					PixelShader;
			ZEConstantBuffer*			Transformations;
			ZEConstantBuffer*			LightParameters;
			ZEConstantBuffer*			ShadowParameters;

		} OmniProjectiveLight;

		void							UpdateShaders();
		void							DestroyShaders();

		void							UpdateBuffers();
		void							DestroyBuffers();

		void							ResetStageDefaults();
		void							CommitStageDefaults();

		void							RenderPointLight(const ZEPointLight* Light);
		void							RenderProjectiveLight(const ZEProjectiveLight* Light);
		void							RenderDirectionalLight(const ZEDirectionalLight* Light);
		void							RenderOmniProjectiveLight(const ZEOmniProjectiveLight* Light);

	public:
		ZEUInt32						GetStageFlags() const;

		const ZETexture2D*				GetLBuffer1() const;
		const ZETexture2D*				GetLBuffer2() const;

		void							SetGBufferInput(const ZERenderStageGeometry* Input);
		const ZERenderStageGeometry*	GetGBufferInput() const;

		void							SetLightList(ZESmartArray<ZELight*>* Lights);
		ZESmartArray<ZELight*>*			GetLightList() const;

		void							Setup();
		void							Process(ZERenderCommand* RenderCommand);
		
										ZERenderStageLighting();
		virtual							~ZERenderStageLighting();
};

#endif
