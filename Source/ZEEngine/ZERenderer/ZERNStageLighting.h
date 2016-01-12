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
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEGraphics/ZEGRSamplerState.h"
#include "ZEGraphics/ZEGRHolder.h"

class ZEGRShader;
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

#define MAX_LIGHTS 511
#define TILE_SIZE_IN_PIXELS 32	//32x32

struct ZERNGPULight
{
	ZEVector3	PositionView;
	float		Range;
	ZEVector3	Color;
	float		Intensity;
	ZEVector3	Attenuation;
	float		Fov;
	ZEVector3	DirectionView;
	int			Type;	
};

struct ZERNTileInfo
{
	ZEUInt	LightIndices[MAX_LIGHTS];
	ZEUInt	LightCount;

	ZERNTileInfo():LightCount(0)
	{
		memset(&LightIndices[0], 0x00, sizeof(ZEUInt) * MAX_LIGHTS);
	}
};

struct ZERNLightConstants
{
	ZERNGPULight							Light;
	ZEMatrix4x4								ProjectionMatrix;
	ZEMatrix4x4								WorldMatrix;
	ZEMatrix4x4								RotationMatrix;
	bool									CastShadow;
	ZEUInt									SampleCount;
	float									SampleLengthOffset;
	ZEUInt									Reserved;
	bool									ShowCascades;
	ZEUInt									Reserved2[3];
	ZEVector4								OffsetVectors[8];
};

class ZERNStageLighting : public ZERNStage
{
	private:
		ZEFlags								DirtyFlags;

		ZEGRHolder<ZEGRShader>				TiledDeferredVertexShader;
		ZEGRHolder<ZEGRShader>				TiledDeferredPixelShader;
		ZEGRHolder<ZEGRShader>				DeferredVertexShader;
		ZEGRHolder<ZEGRShader>				DeferredPixelShader;

		ZEGRHolder<ZEGRRenderStateData>		DeferredRenderState;
		ZEGRHolder<ZEGRRenderStateData>		TiledDeferredRenderState;

		ZEGRHolder<ZEGRStructuredBuffer>	LightBuffer;
		ZEGRHolder<ZEGRStructuredBuffer>	TileInfoBuffer;
		ZEGRHolder<ZEGRConstantBuffer>		LightConstantBuffer;
		ZEGRHolder<ZEGRVertexBuffer>		LightVertexBuffer;

		ZEArray<ZELight*>					Lights;
		ZEArray<ZERNTileInfo>				TileInfos;

		ZEGRHolder<ZEGRTexture2D>			RandomVectorsTexture;
		ZEVector2							OffsetVectors[16];

		ZEGRRenderTarget*					OutputRenderTarget;

		ZEGRSamplerState					SamplerLinearBorder;
		ZEGRSamplerState					SamplerComparisonLinearBorder;
		ZEGRSamplerState					SamplerPointWrap;
		ZEGRSamplerState					SamplerPointBorder;

		ZEUInt								PrevWidth;
		ZEUInt								PrevHeight;

		bool								SetTiledDeferred;
		bool								ShowCascades;

		void								CreateRandomVectors();
		void								CreateOffsetVectors();
		void								CreateLightGeometries();
		void								CreateSamplers();

		bool								UpdateBuffers();
		bool								UpdateRenderState();
		bool								UpdateShaders();
		bool								Update();

		ZEVector4							ComputeClipRegion(const ZEVector3& lightPosView, float lightRadius, float CameraScaleX, float CameraScaleY, float CameraNear);
		void								UpdateClipRegion(float lc, float lz, float lightRadius, float cameraScale, float& clipMin, float& clipMax);
		void								UpdateClipRegionRoot(float nc, float lc, float lz, float lightRadius, float cameraScale, float& clipMin, float& clipMax);
		void								AssignLightsToTiles(ZERNRenderer* Renderer, const ZEArray<ZELight*>& Lights, float CameraScaleX, float CameraScaleY, float CameraNear);

		bool								SetupTiledDeferred(ZERNRenderer* Renderer, ZEGRContext* Context);
		bool								SetupDeferred(ZERNRenderer* Renderer, ZEGRContext* Context);

		void								DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZERNRenderer* Renderer, ZEGRContext* Context);
		void								DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context);
		void								DrawPointLight(ZELightPoint* PointLight, ZERNRenderer* Renderer, ZEGRContext* Context);
		void								DrawOmniProjectiveLight(ZELightOmniProjective* OmniProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context);

		virtual bool						InitializeSelf();
		virtual void						DeinitializeSelf();

	public:
		void								SetIsTiledDeferred(bool Value);
		bool								GetIsTiledDefferred() const;

		void								SetShowCascades(bool ShowCascades);
		bool								GetShowCascades() const;

											ZERNStageLighting();

		virtual ZEInt						GetId();
		virtual const ZEString&				GetName();

		virtual bool						Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands);
		virtual void						CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context);

};
