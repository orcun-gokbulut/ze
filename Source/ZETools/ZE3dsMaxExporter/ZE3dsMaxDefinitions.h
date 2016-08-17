//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxDefinitions.h
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
#include "ZEPacking.h"
#include "ZETypes.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEDS/ZEArray.h"

//MODEL DEFINITIONS

enum ZEPhysicalBodyType
{
	ZE_PBT_NONE				= 0,
	ZE_PBT_RIGID			= 1,
	ZE_PBT_DEFORMABLE		= 2,
	ZE_PBT_CLOTH			= 3
};

enum ZEPhysicalShapeType
{
	ZE_PBST_BOX				= 0,
	ZE_PBST_SPHERE			= 1,
	ZE_PBST_CAPSULE			= 2,
	ZE_PBST_CYLINDER		= 3,
	ZE_PBST_CONVEX			= 4
};

enum ZEModelHelperParentType
{
	ZE_MHPT_MODEL			= 0,
	ZE_MHPT_MESH			= 1,
	ZE_MHPT_BONE			= 2
};

ZEPackStruct(
struct ZEModelFileSkinnedVertex
{
	ZEVector3									Position;
	ZEVector3									Normal;
	ZEVector3									Tangent;
	ZEVector3									Binormal;
	ZEVector2									Texcoord;
	ZEUInt8										BoneIndices[4];
	float										BoneWeights[4];
});

ZEPackStruct(
struct ZEModelFileVertex
{
	ZEVector3									Position;
	ZEVector3									Normal;
	ZEVector3									Tangent;
	ZEVector3									Binormal;
	ZEVector2									Texcoord;
});

ZEPackStruct(
struct ZEModelFileAnimationKey
{
	ZEUInt32									ItemId;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
});

//INTERIOR DEFINITIONS

enum ZEInteriorHelperOwnerType
{
	ZE_IHOT_INTERIOR		= 0,
	ZE_IHOT_ROOM			= 1
};

ZEPackStruct(
struct ZEInteriorFilePhysicalMeshPolygon
{
	ZEUInt32								Indices[3];
});

ZEPackStruct(
struct ZEInteriorFilePhysicalMesh
{
	ZEArray<ZEVector3>						Vertices;
	ZEArray<ZEInteriorFilePhysicalMeshPolygon>	Polygons;
});

ZEPackStruct(
struct ZEInteriorFileVertex
{
	ZEVector3								Position;
	ZEVector3								Normal;
	ZEVector3								Tangent;
	ZEVector3								Binormal;
	ZEVector2								Texcoord;
});

ZEPackStruct(
struct ZEInteriorFilePolygon
{	
	ZEUInt32								Material;
	ZEInteriorFileVertex					Vertices[3];
});
