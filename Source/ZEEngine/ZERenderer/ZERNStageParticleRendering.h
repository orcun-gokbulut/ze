//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageParticleRendering.h
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

#include "ZERNStage.h"

#include "ZEDS/ZEFlags.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZERNShading.h"

class ZEGRTexture;
class ZEGRBuffer;
class ZERNMaterial;
class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRComputeRenderStateData;

class ZERNStageParticleRendering : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags									DirtyFlags;
		ZEGRViewport							Viewport;

		ZEHolder<ZEGRShader>					ScreenCoverVertexShader;
		ZEHolder<ZEGRShader>					TiledPixelShader;
		ZEHolder<ZEGRShader>					TiledCullingComputeShader;

		ZEHolder<ZEGRRenderStateData>			TiledRenderStateData;
		ZEHolder<ZEGRComputeRenderStateData>	TiledCullingRenderStateData;

		ZEHolder<ZEGRBuffer>					EmittersConstantBuffer;
		ZEHolder<ZEGRBuffer>					ParticlePoolBuffer;
		ZEHolder<ZEGRBuffer>					TileParticleStructuredBuffer;

		ZEHolder<const ZEGRTexture>				AccumulationTexture;
		ZEHolder<const ZEGRTexture>				DepthTexture;
		ZEHolder<const ZEGRBuffer>				FogConstantBuffer;
		ZEHolder<const ZEGRBuffer>				TileLightStructuredBuffer;
		ZEHolder<const ZEGRBuffer>				LightsConstantBuffer;

		struct MaterialInstanced
		{
			ZEHolder<const ZERNMaterial>		Material;
			ZEUInt								InstanceCount;
			ZEUInt								FirstConstant;
			ZEUInt								ConstantCount;
		};

		ZESmartArray<MaterialInstanced>			Instances;
		ZESmartArray<MaterialInstanced>			TiledInstances;
		ZEChunkArray<ZEBYTE, 256>				Emitters;
		ZEChunkArray<ZEBYTE, 65536>				ParticlePool;

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

		virtual void							CreateOutput(const ZEString& Name);

		bool									UpdateShaders();
		bool									UpdateRenderStates();
		bool									UpdateBuffers();
		bool									Update();

	public:
		virtual ZEInt							GetId() const;
		virtual const ZEString&					GetName() const;

		virtual void							Resized(ZEUInt Width, ZEUInt Height);

		virtual bool							Setup(ZEGRContext* Context);
		virtual void							CleanUp(ZEGRContext* Context);

												ZERNStageParticleRendering();
		virtual 								~ZERNStageParticleRendering();

		static ZEGRRenderState					GetRenderState();
};
