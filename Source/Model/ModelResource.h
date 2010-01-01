//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModelResource.h
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
#ifndef	__ZE_MODEL_RESOURCE_H__
#define __ZE_MODEL_RESOURCE_H__

#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/AABoundingBox.h"
#include "ZEDS/Array.h"
#include "Graphics/TextureResource.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/FixedMaterial.h"

#define ZE_MDLF_MAX_NAME_SIZE					128
#define ZE_MDLF_MAX_FILENAME_SIZE				256

class ZEModelResourceMeshLOD
{
	private:
		ZEStaticVertexBuffer*				SharedVertexBuffer;

	public:
		ZEINT32								LODLevel;
		ZEINT32								MaterialId;
		ZEArray<ZEModelVertex>				Vertices;
		ZEArray<ZESkinnedModelVertex>		SkinnedVertices;
		
		ZEStaticVertexBuffer*				GetSharedVertexBuffer() const;
		ZEStaticVertexBuffer*				CreatePrivateVertexBuffer() const;

											ZEModelResourceMeshLOD();
											~ZEModelResourceMeshLOD();
};

enum ZEPhysicalShapeType
{
	ZE_PST_BOX				= 0,
	ZE_PST_SPHERE			= 1,
	ZE_PST_CYLINDER			= 2,
	ZE_PST_CAPSULE			= 3,
	ZE_PST_MESH				= 4
};
enum ZEPhysicalBodyType
{
	ZE_PBT_NONE				= 0,
	ZE_PBT_RIGID			= 1,
	ZE_PBT_DEFORMABLE		= 2,
	ZE_PBT_CLOTH			= 3
};

struct ZEModelResourcePhysicalMaterial
{
	bool										Enabled;
	float										Restitution;
	float										DynamicFriction;
	float										StaticFriction;
};

struct ZEModelResourcePhysicalPolygon
{
	int					VertexIndexes[3];
};

struct ZEModelResourcePhysicalShape
{
	ZEVector3									Position;
	ZEQuaternion								Orientation;
	ZEDWORD										Type;
	float										Mass;
	ZEModelResourcePhysicalMaterial				Material;

	union
	{
		struct
		{
			float			Widht;
			float			Height;
			float			Lenght;
		} Box;

		struct
		{
			float			Radius;
		} Sphere;

		struct 
		{
			float			Radius;
			float			Height;
		} Cylinder;

		struct
		{
			float			Radius;
			float			Height;
		} Capsule;
	};

	struct
	{
		ZEArray<ZEModelResourcePhysicalPolygon>	Indices;
		ZEArray<ZEVector3>						Vertices;
	} Mesh;
};

struct ZEModelResourcePhysicalBody
{
	ZEPhysicalBodyType							Type;
	ZEArray<ZEModelResourcePhysicalShape>		Shapes;	
};

struct ZEModelResourceMesh
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE]; 
	ZEAABoundingBox								BoundingBox;
	ZEVector3									Position;
	ZEQuaternion								Orientation;
	bool										IsSkinned;
	ZEArray<ZEModelResourceMeshLOD>				LODs;
	ZEModelResourcePhysicalBody					PhysicalBody;
};

struct ZEModelResourceBone
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEAABoundingBox								BoundingBox;
	ZEINT32										ParentBone;
	ZEVector3									RelativePosition;
	ZEQuaternion								RelativeOrientation;
	ZEMatrix4x4									ForwardTransform;
	ZEVector3									AbsolutePosition;
	ZEQuaternion								AbsoluteOrientation;
	ZEMatrix4x4									InverseTransform;
	ZEModelResourcePhysicalBody					PhysicalBody;
};

struct ZEModelResourceAnimationKey
{
	ZEDWORD										ItemId;
	ZEVector3									Position;
	ZEQuaternion								Orientation;
};

struct ZEModelResourceAnimationFrame
{
	ZEArray<ZEModelResourceAnimationKey>		BoneKeys;
	ZEArray<ZEModelResourceAnimationKey>		MeshKeys;
};

struct ZEModelResourceAnimation
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEArray<ZEModelResourceAnimationFrame>		Frames;
};


class ZEModelResource : public ZEResource
{
	protected:
		virtual									~ZEModelResource();

	public:
		ZEArray<ZEModelResourceMesh>			Meshes;
		ZEArray<ZEModelResourceBone>			Bones;
		ZEArray<ZEModelResourceAnimation>		Animations;
		ZESmartArray<ZETexture2DResource*>		TextureResources;
		ZEArray<ZEMaterial*>					Materials;

		const char*								GetResourceType() const;
		
		static ZEModelResource*					LoadResource(const char* FileName);
		static const ZEModelResource*			LoadSharedResource(const char* FileName);
		static void								CacheResource(const char* FileName);

};

#endif

