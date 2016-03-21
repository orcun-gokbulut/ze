//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageLighting.h
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
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEList2.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEPointer/ZESharedPointer.h"
#include "ZEGraphics/ZEGRViewport.h"

class ZEGRShader;
class ZEGRSampler;
class ZEGRRenderStateData;
class ZEGRConstantBuffer;
class ZEGRStructuredBuffer;
class ZEGRVertexBuffer;
class ZEGRRenderTarget;
class ZEGRTexture2D;
class ZELight;
class ZELightProjective;
class ZELightPoint;
class ZELightDirectional;
class ZELightOmniProjective;

#define MAX_LIGHT			255
#define TILE_SIZE_IN_PIXELS	16

enum ZERNRenderModel
{
	ZERN_RM_DEFERRED				= 0,
	ZERN_RM_TILED_DEFERRED			= 1,
	ZERN_RM_COMPUTE_TILED_DEFERRED	= 2
};

class ZERNStageLighting : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags								DirtyFlags;

		ZEHolder<ZEGRShader>				ScreenCoverVertexShader;
		ZEHolder<ZEGRShader>				DeferredVertexShader;
		ZEHolder<ZEGRShader>				DeferredPixelShader;
		ZEHolder<ZEGRShader>				TiledDeferredPixelShader;
		ZEHolder<ZEGRShader>				TiledDeferredComputeShader;
		ZEHolder<ZEGRShader>				AccumulatePixelShader;

		ZEHolder<ZEGRRenderStateData>		DeferredRenderState;
		ZEHolder<ZEGRRenderStateData>		TiledDeferredRenderState;
		ZEHolder<ZEGRRenderStateData>		TiledDeferredComputeRenderState;
		ZEHolder<ZEGRRenderStateData>		AccumulateRenderState;

		ZEHolder<ZEGRConstantBuffer>		DeferredLightConstantBuffer;
		ZEHolder<ZEGRVertexBuffer>			DeferredLightVertexBuffer;
		ZEHolder<ZEGRStructuredBuffer>		TiledDeferredTileStructuredBuffer;
		ZEHolder<ZEGRConstantBuffer>		TiledDeferredLightConstantBuffer;

		ZEHolder<ZEGRTexture2D>				TiledDeferredComputeOutputTexture;

		ZEGRViewport						Viewport;

		struct LightStruct
		{
			ZEVector3						PositionView;
			float							Range;
			ZEVector3						Color;
			float							Fov;
			ZEVector3						Attenuation;
			bool							CastShadow;
			ZEVector3						DirectionView;
			int								Type;
		};

		struct TileStruct
		{
			ZEUInt							LightIndices[MAX_LIGHT];
			ZEUInt							LightCount;
		};

		struct DeferredLightConstantsStruct
		{
			LightStruct						Light;
			ZEMatrix4x4						ProjectionMatrix;
			ZEMatrix4x4						WorldMatrix;
			ZEMatrix4x4						RotationMatrix;
			ZEBool32						CastShadow;
			ZEUInt							SampleCount;
			float							SampleLength;
			ZEBool32						ShowCascades;
			float							ShadowDepthBias;
			ZEVector3						Reserved;
		};

		struct TiledDeferredLightConstantsStruct
		{
			LightStruct						Lights[MAX_LIGHT];
			ZEUInt							LightCount;
			ZEVector3						Reserved;
		} TiledDeferredLightConstants;

		ZEArray<TileStruct>					Tiles;
		ZEList2<ZELight>					DeferredLightList;
		ZEList2<ZELight>					TiledDeferredLightList;

		ZEHolder<ZEGRTexture2D>				RandomVectorsTexture;

		const ZEGRRenderTarget*				OutputRenderTarget;

		ZEHolder<ZEGRSampler>				SamplerLinearBorder;
		ZEHolder<ZEGRSampler>				SamplerComparisonLinearPointClamp;
		ZEHolder<ZEGRSampler>				SamplerPointWrap;
		ZEHolder<ZEGRSampler>				SamplerPointBorder;

		ZEUInt								Width;
		ZEUInt								Height;

		ZERNRenderModel						RenderModel;
		bool								ShowCascades;

		void								CreateRandomVectors();
		void								CreateLightGeometries();
		void								CreateSamplers();

		bool								UpdateRenderState();
		bool								UpdateShaders();
		bool								Update();

		ZEVector4							ComputeClipRegion(const ZEVector3& lightPosView, float lightRadius, float CameraScaleX, float CameraScaleY, float CameraNear);
		void								UpdateClipRegion(float lc, float lz, float lightRadius, float cameraScale, float& clipMin, float& clipMax);
		void								UpdateClipRegionRoot(float nc, float lc, float lz, float lightRadius, float cameraScale, float& clipMin, float& clipMax);
		void								AssignLightsToTiles(const ZEList2<ZELight>& Lights, float CameraScaleX, float CameraScaleY, float CameraNear);

		bool								SetupDeferred( ZEGRContext* Context);
		bool								SetupTiledDeferred(ZEGRContext* Context);
		bool								SetupComputeTiledDeferred(ZEGRContext* Context);

		void								DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZEGRContext* Context);
		void								DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZEGRContext* Context);
		void								DrawPointLight(ZELightPoint* PointLight, ZEGRContext* Context);
		void								DrawOmniProjectiveLight(ZELightOmniProjective* OmniProjectiveLight, ZEGRContext* Context);

		virtual bool						InitializeSelf();
		virtual void						DeinitializeSelf();

	public:
		virtual ZEInt						GetId() const;
		virtual const ZEString&				GetName() const;

		void								SetRenderModel(ZERNRenderModel RenderModel);
		ZERNRenderModel						GetRenderModel() const;

		void								SetShowCascades(bool ShowCascades);
		bool								GetShowCascades() const;

		virtual bool						Setup(ZEGRContext* Context);
		virtual void						CleanUp(ZEGRContext* Context);

											ZERNStageLighting();

};
