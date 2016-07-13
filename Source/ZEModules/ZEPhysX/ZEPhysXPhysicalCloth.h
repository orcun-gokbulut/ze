//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalCloth.h
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
#ifndef	__ZE_PHYSX_PHYSICAL_CLOTH_H__
#define __ZE_PHYSX_PHYSICAL_CLOTH_H__

#include "ZEModules/ZEPhysX/ZEPhysXPhysicalWorld.h"
#include "ZEModules/ZEPhysX/ZEPhysXStream.h"

#include "ZEPhysics/ZEPhysicalCloth.h"

#include <NxCooking.h>

#include <cloth/NxCloth.h>
#include <cloth/NxClothDesc.h>
#include <cloth/NxClothMesh.h>
#include <cloth/NxClothMeshDesc.h>

class ZEPhysXPhysicalCloth : public ZEPhysicalCloth
{
	friend class ZEPhysXModule;
	private:
		ZEUInt32								ParticleCount;

		ZEArray<ZEVector3>						ClothVertices;
		ZEArray<ZEUInt32>						ClothIndices;

		ZEArray<ZEClothVertexAttachment>		Attachments;

		ZEPhysXPhysicalWorld*					PhysicalWorld;

		NxCloth*								Cloth;
		NxClothDesc								ClothDesc;

		NxClothMesh*							ClothMesh;
		NxClothMeshDesc							ClothMeshDesc;

		NxMeshData								MeshData;

		virtual void							ReCreate();

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

												ZEPhysXPhysicalCloth();
		virtual									~ZEPhysXPhysicalCloth();

	public:
		virtual void							SetEnabled(bool IsEnabled = true);
		virtual bool							GetEnabled() const;

		virtual void							SetPhysicalWorld(ZEPhysicalWorld* World);
		virtual ZEPhysicalWorld*				GetPhysicalWorld();

		virtual void							SetPosition(const ZEVector3& Position);
		virtual ZEVector3						GetPosition() const;

		virtual void							SetRotation(const ZEQuaternion& Rotation);
		virtual ZEQuaternion					GetRotation() const;

		virtual void							SetBendingMode(bool IsEnabled);
		virtual bool							GetBendingMode() const;

		virtual void							SetBendingStiffness(float BendingStiffness);
		virtual float							GetBendingStiffness() const;

		virtual void							SetStretchingStiffness(float StretchingStiffness);
		virtual float							GetStretchingStiffness() const;

		virtual void							SetDampingMode(bool IsEnabled);
		virtual bool							GetDampingMode() const;

		virtual void							SetDampingCoefficient(float DampingCoefficient);
		virtual float							GetDampingCoefficient() const;

		virtual void							SetFriction(float Friction);
		virtual float							GetFriction() const;

		virtual void							SetPressureMode(bool IsEnabled);
		virtual bool							GetPressureMode() const;

		virtual void							SetPressure(float Pressure);
		virtual float							GetPressure() const;

		virtual void							SetTearableMode(bool IsEnabled);
		virtual bool							GetTearableMode() const;

		virtual void							SetTearFactor(float TearFactor);
		virtual float							GetTearFactor() const;

		virtual void							SetAttachmentTearFactor(float AttachmentTearFactor);
		virtual float							GetAttachmentTearFactor() const;

		virtual void							SetThickness(float Thickness);
		virtual float							GetThickness() const;

		virtual void							SetSolverIterations(ZEUInt SolverIterationCount);
		virtual ZEUInt							GetSolverIterations() const;

		virtual ZEArray<ZEVector3>&				GetVertices();
		virtual ZEArray<ZEUInt32>&				GetIndices();

		virtual void							AttachVertex(ZEUInt VertexId, const ZEPhysicalShape* Shape, const ZEVector3& LocalPosition);
		virtual void							AttachVertex(const ZEUInt VertexId, const ZEVector3& WorldPosition);
		virtual void							DetachVertex(const ZEUInt VertexId);

		virtual const ZEClothVertexAttachment*	GetVertexAttachment(ZEUInt VertexId) const;

		virtual bool							TearVertex(const ZEUInt VertexId, const ZEVector3& Normal);
};

#endif
