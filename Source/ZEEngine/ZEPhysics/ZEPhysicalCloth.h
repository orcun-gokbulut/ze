//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysicalCloth.h
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
#ifndef	__ZE_PHYSICAL_CLOTH_H__
#define __ZE_PHYSICAL_CLOTH_H__

#include "ZEFoundation/ZETypes.h"

#include "ZEPhysics/ZEPhysicsModule.h"
#include "ZEPhysics/ZEPhysicalShapes.h"
#include "ZEPhysics/ZEPhysicalRigidBody.h"
#include "ZEGraphics/ZEVertexTypes.h"
#include "ZEDS/ZEFlags.h"

enum ZEClothVertexAttachmentStatus
{
	ZE_PCVA_NONE	= 0,
	ZE_PCVA_GLOBAL	= 1,
	ZE_PCVA_SHAPE	= 2,
};

struct ZEClothVertexAttachment 
{
	ZEUInt32						VertexId;
	ZEClothVertexAttachmentStatus	AttachmentStatus;
	ZEPhysicalShape*				AttachmentShape;
	ZEVector3						AttachmentPosition;
};

class ZEPhysicalCloth
{
	protected:
												ZEPhysicalCloth();
		virtual									~ZEPhysicalCloth();

	public:
		virtual void							SetEnabled(bool IsEnabled = true) = 0;
		virtual bool							GetEnabled() const = 0;

		virtual void							SetPhysicalWorld(ZEPhysicalWorld* World) = 0;
		virtual ZEPhysicalWorld*				GetPhysicalWorld() = 0;

		virtual void							SetPosition(const ZEVector3& Position) = 0;
		virtual ZEVector3						GetPosition() const = 0;

		virtual void							SetRotation(const ZEQuaternion& Rotation) = 0;
		virtual ZEQuaternion					GetRotation() const = 0;

		virtual void							SetBendingMode(bool IsEnabled) = 0;
		virtual bool							GetBendingMode() const = 0;

		virtual void							SetBendingStiffness(float BendingStiffness) = 0;
		virtual float							GetBendingStiffness() const = 0;

		virtual void							SetStretchingStiffness(float StretchingStiffness) = 0;
		virtual float							GetStretchingStiffness() const = 0;

		virtual void							SetDampingMode(bool IsEnabled) = 0;
		virtual bool							GetDampingMode() const = 0;

		virtual void							SetDampingCoefficient(float DampingCoefficient) = 0;
		virtual float							GetDampingCoefficient() const = 0;

		virtual void							SetFriction(float Friction) = 0;
		virtual float							GetFriction() const = 0;

		virtual void							SetPressureMode(bool IsEnabled) = 0;
		virtual bool							GetPressureMode() const = 0;

		virtual void							SetPressure(float Pressure) = 0;
		virtual float							GetPressure() const = 0;

		virtual void							SetTearableMode(bool IsEnabled) = 0;
		virtual bool							GetTearableMode() const = 0;

		virtual void							SetTearFactor(float TearFactor) = 0;
		virtual float							GetTearFactor() const = 0;

		virtual void							SetAttachmentTearFactor(float AttachmentTearFactor) = 0;
		virtual float							GetAttachmentTearFactor() const = 0;

		virtual void							SetThickness(float Thickness) = 0;
		virtual float							GetThickness() const = 0;

		virtual void							SetSolverIterations(ZEUInt SolverIterationCount) = 0;
		virtual ZEUInt							GetSolverIterations() const = 0;

		virtual ZEArray<ZEVector3>&				GetVertices() = 0;
		virtual ZEArray<ZEUInt32>&				GetIndices() = 0;

		virtual void							AttachVertex(const ZEUInt VertexId, const ZEVector3& WorldPosition) = 0;
		virtual void							AttachVertex(ZEUInt VertexId, const ZEPhysicalShape* Shape, const ZEVector3& LocalPosition) = 0;
		virtual void							DetachVertex(const ZEUInt VertexId) = 0;

		virtual ZEClothVertexAttachment			GetVertexAttachment(ZEUInt VertexId) const = 0;

		virtual bool							TearVertex(const ZEUInt VertexId, const ZEVector3& Normal) = 0;

		virtual bool							Initialize() = 0;
		virtual void							Deinitialize() = 0;

		virtual void							ReCreate() = 0;

		virtual void							Destroy();

		static ZEPhysicalCloth*					CreateInstance();
};
#endif
