//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - RenderOrder.h
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

#include "ZEDS/Array.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/AABoundingBox.h"

// ZERenderOrderFlags
typedef ZEDWORD ZERenderOrderFlags;
#define	ZE_ROF_NONE									0
#define	ZE_ROF_TRANSPARENT							1
#define	ZE_ROF_IMPOSTER								2
#define	ZE_ROF_ENABLE_Z_CULLING						4
#define	ZE_ROF_ENABLE_NO_Z_WRITE					8
#define	ZE_ROF_ENABLE_WORLD_TRANSFORM				16
#define ZE_ROF_ENABLE_VIEW_TRANSFORM				32
#define ZE_ROF_ENABLE_PROJECTION_TRANSFORM			64
#define	ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM		(ZE_ROF_ENABLE_VIEW_TRANSFORM | ZE_ROF_ENABLE_PROJECTION_TRANSFORM) 
#define ZE_ROF_INSTANCED							128
#define ZE_ROF_SKINNED								256
#define ZE_ROF_INDEXED								512


enum ZEROPrimitiveType
{
	ZE_ROPT_POINT,
	ZE_ROPT_LINE,
	ZE_ROPT_TRIANGLE,
	ZE_ROPT_TRIANGLESTRIPT
};

class ZELight;
class ZEMaterial;
class ZEVertexDeclaration;
class ZEVertexBuffer;

class ZERenderOrder
{
	public:
		int								Priority;
		ZERenderOrderFlags				Flags;
		ZEROPrimitiveType				PrimitiveType;
		ZEVertexDeclaration*			VertexDeclaration;
		size_t							VertexBufferOffset;
		size_t							PrimitiveCount;

		const ZEMaterial*				Material;

		void*							IndexBuffer;
		ZEVertexBuffer*					VertexBuffer;

		ZEMatrix4x4						WorldMatrix;

		ZEArray<ZERenderOrder*>			Instances;
		ZEArray<ZEMatrix4x4>			BoneTransforms;

		ZEArray<ZELight*>				Lights;		

		void							SetZero();
};
#endif
