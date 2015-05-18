//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelection.h
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
#ifndef __ZED_SELECTION_H__
#define __ZED_SELECTION_H__

#include "ZEFoundation/ZEMath/ZEAABBox.h"
#include "ZEFoundation/ZEMath/ZEQuaternion.h"

class ZEObject;
class ZEDrawParameters;

struct ZEDSelectionElement
{
	ZEObject* Object;
	ZEMatrix4x4 OffsetToPivot;
};

class ZEDSelection
{
	friend class ZEDSelectionManager;

	private:
		ZEAABBox BoundingBox;
		ZEVector3 PivotPosition;
		ZEQuaternion PivotRotation;
		ZEVector3 PivotScale;
		bool Lock;

		ZEArray<ZEDSelectionElement> Elements;

		void CalculateBoundingBox();

	public:
		ZEArray<ZEDSelectionElement>& GetElements();
		ZEObject* GetElement(ZESize Index);
		void AddElement(const ZEObject* Object);
		void RemoveElement(const ZEObject* Object);

		const ZEMatrix4x4& GetTransform();

		void SetPosition(const ZEVector3& Position);
		const ZEVector3& GetPosition();

		void SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion& GetRotation();

		void SetScale(const ZEVector3& Scale);
		const ZEVector3& GetScale();

		bool IsLocked();
		void Tick(float Time);
		void Draw(ZEDrawParameters* Parameters);

		ZEDSelection();
		virtual ~ZEDSelection();
};

#endif
