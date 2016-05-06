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

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEMatrix.h"

#include "ZEDViewportInput.h"

enum ZEDSelectionShape
{
	ZED_SS_NONE,
	ZED_SS_RECTANGLE,
	ZED_SS_CIRCLE,
	ZED_SS_BRUSH
};

enum ZEDSelectionMode
{
	ZE_SM_NONE,
	ZE_SM_FULLY_INSIDE,
	ZE_SM_PARTIALY_INSIDE
};

class ZEObject;
class ZEClass;
class ZEViewVolume;
class ZEDModule;
class ZEDObjectWrapper;

enum ZEDSelectionEventType
{
	ZED_SET_NONE,
	ZED_SET_SELECTED,
	ZED_SET_DESELECTED
};

class ZEDSelectionEvent
{
	friend class ZEDSelectionManager;
	private:
		ZEDSelectionEventType				Type;
		ZEArray<ZEDObjectWrapper*>*			Selection;
		ZEArray<ZEDObjectWrapper*>*			OldSelection;
		ZEArray<ZEDObjectWrapper*>*			SelectedObjects;
		ZEArray<ZEDObjectWrapper*>*			UnselectedObjects;

											ZEDSelectionEvent();

	public:
		ZEDSelectionEventType				GetType() const;
		const ZEArray<ZEDObjectWrapper*>&	GetSelection() const;
		const ZEArray<ZEDObjectWrapper*>&	GetOldSelection() const;
		const ZEArray<ZEDObjectWrapper*>&	GetSelectedObjects() const;
		const ZEArray<ZEDObjectWrapper*>&	GetUnselectedObjects() const;
};

class ZEDSelectionManager
{
	friend class ZEDCore;
	private:
		ZEDModule*							Module;
		ZEArray<ZEDObjectWrapper*>			Selection;
		ZEDSelectionMode					SelectionMode;
		ZEDSelectionShape					SelectionShape;
		ZEClass*							Filter;
		ZEVector2							SelectionStartPosition;

		bool								FilterSelection(ZEObject* Object, void* Class);

											ZEDSelectionManager();
											~ZEDSelectionManager();

	public:
		ZEDModule*							GetModule();

		void								SetSelectionMode(ZEDSelectionMode Mode);
		ZEDSelectionMode					GetSelectionMode();

		void								SetSelectionShape(ZEDSelectionShape Shape);
		ZEDSelectionShape					GetSelectionShape();

		void								SetSelectionFilter(ZEClass* Class);
		ZEClass*							GetSelectionFilter();
		
		const ZEArray<ZEDObjectWrapper*>&	GetSelectedObjects();
		void								SelectObject(ZEDObjectWrapper* Object);
		void								SelectObjects(const ZEArray<ZEDObjectWrapper*>& Objects);
		void								DeselectObject(ZEDObjectWrapper* Object);
		void								DeselectObjects(const ZEArray<ZEDObjectWrapper*>& Object);
		void								ClearSelection();

		bool								KeyboardEvent(const ZEDViewportKeyboardEvent& Event);
		bool								MouseEvent(const ZEDViewportMouseEvent& Event);

		void								Destroy();

		static ZEDSelectionManager*			GetInstance();
};
