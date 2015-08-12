//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelectionManager.h
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

#include "ZEFoundation/ZEDS/ZEArray.h"
#include "ZEFoundation/ZEMath/ZERay.h"
#include "ZEFoundation/ZEMath/ZEMatrix.h"

class ZEDObjectWrapper;
class ZEObject;
class ZEClass;
class ZEViewVolume;

enum ZEDSelectionPivotMode
{
	ZED_SCM_ENTITY_PIVOT,
	ZED_SCM_SELECTION_CENTER,
	ZED_SCM_SPACE_CENTER
};

class ZEDSelectionManager
{
	friend class ZEDCore;

	private:
		ZEArray<ZEDObjectWrapper*> Selection;

		ZEDSelectionPivotMode PivotMode;
		ZEMatrix4x4 SelectionPivot;

		ZEClass* Filter;

		void CalculateSelectionPivot();
		

		ZEDSelectionManager();

	public:
		const ZEArray<ZEDObjectWrapper*>& GetSelectedObjects();

		void SelectObject(ZEDObjectWrapper* Object);
		void SelectObject(const ZERay& Ray);
		void SelectObject(ZEViewVolume* ViewVolume);
		void SelectObject(const ZEVector2& ScreenPoint1, const ZEVector2& ScreenPoint2);
		void SelectObject(const ZEString& Name);
		void SelectObject(ZESize Id);

		void DeselectObject(ZEDObjectWrapper* Object);
		void DeselectObject(const ZERay& Ray);
		void DeselectObject(ZEViewVolume* ViewVolume);
		void DeselectObject(const ZEVector2& ScreenPoint1, const ZEVector2& ScreenPoint2);
		void DeselectObject(const ZEString& Name);
		void DeselectObject(ZESize Id);

		void ClearSelection();

		void SetSelectionFilter(ZEClass* Class);
		ZEClass* GetSelectionFilter();
		bool FilterSelection(ZEObject* Object, void* Class);

		void SetSelectionPivotMode(ZEDSelectionPivotMode Mode);
		ZEDSelectionPivotMode GetSelectionPivotMode();
		const ZEMatrix4x4& GetSelectionPivot();
		void UpdateSelectionGizmo();

		void Destroy();

		static ZEDSelectionManager* GetInstance();
};
