//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTransformationManager.h
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

#include "ZEInitializable.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"

enum ZEDTransformType
{
	ZED_TT_NONE,
	ZED_TT_TRANSLATE,
	ZED_TT_ROTATE,
	ZED_TT_SCALE
};

enum ZEDTransformSpace
{
	ZED_TS_LOCAL,
	ZED_TS_WORLD,
	ZED_TS_PARENT,
	ZED_TS_VIEW
};

enum ZEDTransformPivot
{
	ZED_TP_OBJECT,
	ZED_TP_FIRST_OBJECT,
	ZED_TP_LAST_OBJECT,
	ZED_TP_CENTER,
	ZED_TP_WORLD
};

class ZEDModule;
class ZEDObjectWrapper;
class ZEDGizmo;
class ZEDSelectionEvent;
class ZEDViewportKeyboardEvent;
class ZEDViewportMouseEvent;
class ZEDViewportChangedEvent;
class ZERNRenderer;

class ZEDTransformationState
{
	public:
		ZEDObjectWrapper*					Wrapper;
		ZEVector3							Pivot;
		ZEVector3							OriginalPosition;
		ZEQuaternion						OriginalRotation;
		ZEVector3							OriginalScale;
		ZEDGizmo*							Gizmo;

											ZEDTransformationState();
											~ZEDTransformationState();
};

class ZEDTransformationManager : public ZEInitializable
{
	friend class ZEDCore;
	private:
		ZEDModule*							Module;
		ZEDTransformType					TransformType;
		ZEDTransformSpace					TransformSpace;
		ZEDTransformPivot					TransformPivot;
		ZEDGizmo*							TransformGizmo;
		bool								TransformActive;
		ZEArray<ZEDTransformationState>		TransformStates;


		ZEVector2							MouseStartPosition;

		void								UpdateGizmos();
		void								UpdateTransformStates();

		bool								InitializeSelf();
		void								DeinitializeSelf();

		void								StartTransform(ZEDGizmo* TransformGizmo);
		void								ResetTransform();
		void								ApplyTranslation(const ZEVector3& Translation);
		void								ApplyRotation(const ZEQuaternion& Rotation);
		void								ApplyScale(const ZEVector3& Scale);
		void								EndTransform();

											ZEDTransformationManager();

	public:
		ZEDModule*							GetModule();

		void								SetTransformType(ZEDTransformType Type);
		ZEDTransformType					GetTransformType();

		void								SetTransformSpace(ZEDTransformSpace Space);
		ZEDTransformSpace					GetTransformSpace();

		void								SetTransformPivot(ZEDTransformPivot Pivot);
		ZEDTransformPivot					GetTransformPivot();

		// Events
		virtual void						SelectionEvent(const ZEDSelectionEvent& Event);
		virtual void						ViewportChangedEvent(const ZEDViewportChangedEvent& Event);
		virtual bool						ViewportKeyboardEvent(const ZEDViewportKeyboardEvent& Event);
		virtual bool						ViewportMouseEvent(const ZEDViewportMouseEvent& Event);
		virtual void						PreRender(ZERNRenderer* Renderer);

		void								Destroy();

		static ZEDTransformationManager*	GetInstance();
};
