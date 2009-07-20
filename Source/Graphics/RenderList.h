//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - RenderList.h
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
#ifndef	__ZE_RENDERLIST_H__
#define __ZE_RENDERLIST_H__

#include "ZEDS/ZEDS.h"
#include "ZEMath/ZEMath.h"
#include "TextureResource.h"
#include "Material.h"
#include "VertexBuffer.h"

#define	ZE_RLF_ZCULLING						2
#define	ZE_RLF_NOZWRITE						4
#define	ZE_RLF_WORLD_TRANSFORM				8
#define	ZE_RLF_VIEW_TRANSFORM				16
#define ZE_RLF_PROJECTION_TRANSFROM			32
#define ZE_RLF_INSTANCED					64
#define ZE_RLF_SKINNED						128
#define ZE_RLF_INDEXED						256

#define ZE_DO_RIGID							0
#define ZE_DO_TRANSPARENT					10
#define ZE_DO_IMPOSTER						20
#define ZE_DO_GUI							40

enum ZERLPrimitiveType
{
	ZE_RLPT_POINT,
	ZE_RLPT_LINE,
	ZE_RLPT_TRIANGLE,
	ZE_RLPT_TRIANGLESTRIPT,
	ZE_RLPT_TRIANGLEFAN,
};

enum ZERLLightType
{
	ZE_RLLT_POINT,
	ZE_RLLT_DIRECTIONAL,
	ZE_RLLT_PROJECTIVE,
	ZE_RLLT_OMNIPROJECTIVE
};

struct ZERLLight
{	
		ZERLLightType						Type;
		ZEVector3							Color;
		ZEVector3							Position;
		ZEVector3							Direction;
		ZEVector3							Attenuation;
		float								Intensity;
		float								Range;
		ZEMatrix4x4							LightViewProjMatrix;
		ZEMatrix4x4							LightRotationMatrix;
		union
		{
			const ZETexture*				ProjectionMap;
			const ZECubeTexture*			CubeProjectionMap;
		};
		union
		{
			const ZETexture*				ShadowMap;
			const ZECubeTexture*			CubeShadowMap;
		};

		void								SetZero();
};

class ZERenderList
{
	public:
		ZEDWORD								DrawOrder;
		ZEDWORD								Flags;

		void*								IndexBuffer;
		ZEVertexBuffer*						VertexBuffer;
		ZEVertexType						VertexType;
		unsigned int						VertexBufferOffset;

		ZERLPrimitiveType					PrimitiveType;
		unsigned int						PrimitiveCount;

		ZEMatrix4x4							WorldMatrix;
		ZEArray<ZEMatrix4x4>				BoneTransforms;

		const ZEShader*						Shader;
		ZEChunkArray<const ZERLLight*, 10>	Lights;		

		void								SetZero();
};
#endif
