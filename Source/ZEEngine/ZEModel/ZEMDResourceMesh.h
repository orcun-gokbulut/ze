//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceMesh.h
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

#include "ZEMeta\ZEObject.h"

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEList2.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMDResourcePhysics.h"

class ZEMDResourceMesh : public ZEObject
{
	ZE_OBJECT
	friend class ZEMDResource;
	friend class ZEModelResourceMeshLOD;
	private:
		ZELink<ZEMDResourceMesh>						Link;

		ZEString										Name; 
		ZEAABBox										BoundingBox;
		ZEInt32											ParentMesh;
		ZEVector3										Position;
		ZEQuaternion									Rotation;
		ZEVector3										Scale;
		bool											Visible;
		bool											Skinned;
		ZEMDResourcePhysicalBody						PhysicalBody;
		ZEString										UserDefinedProperties;
		ZEList2<ZEModelResourceMeshLOD>					LODs;
		ZEArray<ZEVector3>								Geometry;

	public:
		void											SetParentMesh(ZEInt32 ParentMesh);
		ZEInt32											GetParentMesh() const;

		void											SetName(const ZEString& Name);
		const ZEString&									GetName() const;

		void											SetBoundingBox(const ZEAABBox& BoundingBox);
		const ZEAABBox&									GetBoundingBox() const;

		void											SetPosition(const ZEVector3& Position);
		const ZEVector3&								GetPosition() const;

		void											SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion&								GetRotation() const;

		void											SetScale(const ZEVector3& Scale);
		const ZEVector3&								GetScale() const;

		void											SetVisible(bool IsVisible);
		bool											GetVisible() const;

		void											SetSkinned(bool IsSkinned);
		bool											GetSkinned() const;

		void											SetUserDefinedProperties(const ZEString& UserDefinedProperties);
		const ZEString&									GetUserDefinedProperties() const;

		const ZEList2<ZEModelResourceMeshLOD>&			GetLODs();
		const ZEList2<const ZEModelResourceMeshLOD>&	GetLODs() const;
		void											AddLOD(ZEModelResourceMeshLOD* LOD);
		void											RemoveLOD(ZEModelResourceMeshLOD* LOD);

		const ZEArray<ZEVector3>&						GetGeometry() const;

		bool											Load(const ZEMLReaderNode& MeshNode);
		bool											Save(ZEMLWriterNode& MeshNode) const;

														ZEMDResourceMesh();
														~ZEMDResourceMesh();
};
