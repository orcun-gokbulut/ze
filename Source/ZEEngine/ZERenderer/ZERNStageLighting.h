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
#include "ZEGraphics\ZEGRHolder.h"
#include "ZEMath\ZEVector.h"
#include "ZEDS\ZEArray.h"
#include "ZEMath\ZEMatrix.h"

class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRConstantBuffer;
class ZEGRStructuredBuffer;
class ZEGRVertexBuffer;
class ZELight;
class ZELightProjective;
class ZELightPoint;
class ZELightDirectional;
class ZELightOmniProjective;
class ZEGRRenderTarget;
class ZEGRTexture2D;

#define MAX_LIGHTS 511
#define TILE_SIZE_IN_PIXELS 16	//16x16

struct ZERNGPULight
{
	ZEVector3	PositionWorld;
	float		Range;
	ZEVector3	Color;
	float		Intensity;
	ZEVector3	Attenuation;
	float		Fov;
	ZEVector3	Direction;
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
	ZERNGPULight	Light;
	ZEMatrix4x4		ProjectionMatrix;
	ZEMatrix4x4		WorldMatrix;
	ZEMatrix3x3		RotationMatrix;
	float			Reserved[3];
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

		ZEGRRenderTarget*					OutputRenderTarget;

		ZEUInt								PrevWidth;
		ZEUInt								PrevHeight;

		bool								SetTiledDeferred;

	private:
		virtual bool						InitializeSelf();
		virtual void						DeinitializeSelf();

		void								CreateLights();
		void								CreateLightGeometries();

		bool								UpdateBuffers();
		bool								UpdateRenderState();
		bool								UpdateShaders();
		bool								Update();

		ZEVector4							ComputeClipRegion(const ZEVector3& lightPosView, float lightRadius, float CameraScaleX, float CameraScaleY, float CameraNear);
		void								UpdateClipRegion(float lc, float lz, float lightRadius, float cameraScale, float& clipMin, float& clipMax);
		void								UpdateClipRegionRoot(float nc, float lc, float lz, float lightRadius, float cameraScale, float& clipMin, float& clipMax);
		void								AssingLightsToTiles(ZERNRenderer* Renderer, const ZEArray<ZELight*>& Lights, float CameraScaleX, float CameraScaleY, float CameraNear);

		void								SetupGbufferResources(ZERNRenderer* Renderer, ZEGRContext* Context);
		bool								SetupTiledDeferred(ZERNRenderer* Renderer, ZEGRContext* Context);
		bool								SetupDeferred(ZERNRenderer* Renderer, ZEGRContext* Context);
		void								DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZERNRenderer* Renderer, ZEGRContext* Context);
		void								DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context);
		void								DrawPointLight(ZELightPoint* PointLight, ZERNRenderer* Renderer, ZEGRContext* Context);
		void								DrawOmniProjectiveLight(ZELightOmniProjective* OmniProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context);

	public:
		virtual ZEInt						GetId();
		virtual const ZEString&				GetName();

		void								SetOutput(ZEGRRenderTarget* Output);
		ZEGRRenderTarget*					GetOutput() const;

		void								SetIsTiledDeferred(bool Value);
		bool								GetIsTiledDefferred() const;

		virtual bool						Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands);
		virtual void						CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context);

};
