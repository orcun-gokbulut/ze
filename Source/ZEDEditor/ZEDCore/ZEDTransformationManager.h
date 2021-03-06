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

#include "ZEDComponent.h"

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEDUserInterface/ZEDCommand.h"

class ZEDEditor;
class ZEDGizmo;
class ZEDObjectWrapper;
class ZEDObjectWrapper3D;
class ZEDSelectionManager;
class ZEDViewport;
class ZEDTransformationManager;
class ZEDSelectionManager;
class ZERNPreRenderParameters;
class ZEDObjectManager;
class ZEMTProperty;

enum ZEDTransformType
{
	ZED_TT_NONE,
	ZED_TT_TRANSLATE,
	ZED_TT_ROTATE,
	ZED_TT_SCALE,
	ZED_TT_AXIS_ROTATE,
	ZED_TT_SNAP,
};

enum ZEDTransformSpace
{
	ZED_TS_LOCAL,
	ZED_TS_PARENT,
	ZED_TS_WORLD,
	ZED_TS_VIEW
};

enum ZEDTransformPivot
{
	ZED_TP_OBJECT,
	ZED_TP_FOCUSED_OBJECT,
	ZED_TP_SELECTION_CENTER,
	ZED_TP_WORLD
};



class ZEDTransformationState
{
	public:
		ZEDObjectWrapper3D*						Wrapper;
		ZEVector3								Pivot;
		ZEVector3								OriginalPosition;
		ZEQuaternion							OriginalRotation;
		ZEVector3								OriginalScale;
		ZEDGizmo*								Gizmo;
		ZEVector3								Position;
		ZEQuaternion							Rotation;

												ZEDTransformationState();
												~ZEDTransformationState();
};

class ZEDTransformationManager : public ZEDComponent
{
	ZE_OBJECT
	friend class ZEDEditorCore;
	private:
		ZEDTransformType						TransformType;
		ZEDTransformSpace						TransformSpace;
		ZEDTransformPivot						TransformPivot;
		ZEDGizmo*								TransformGizmo;
		bool									TransformActive;
		ZEArray<ZEDTransformationState>			TransformStates;
		ZEDTransformationState*					TransformFocused;

		ZEVector3								PivotPosition;
		ZEVector3								PivotRotation;
		ZEVector3								PivotScale;

		ZEVector2								MouseStartPosition;

		void									UpdateGizmo(ZEDGizmo* Gizmo, const ZEVector3& WorldPosition, const ZEQuaternion& WorldRotation);
		void									UpdateGizmos();
		void									UpdateTransformStates();

		void									StartTransform(ZEDGizmo* TransformGizmo);
		void									EndTransform();
		void									ResetTransform();

		void									ApplyTranslation(const ZEVector3& Translation);
		void									ApplyRotation(const ZEQuaternion& Rotation);
		void									ApplyScale(const ZEVector3& Scale);

		ZEVector3								GetPosition(bool& Valid);
		ZEVector3								GetRotation(bool& Valid);
		ZEVector3								GetScale(bool& Valid);

		bool									InitializeInternal();
		bool									DeinitializeInternal();

		void									ObjectManager_OnObjectPropertyChanged(ZEDObjectManager* Manager, ZEDObjectWrapper* Object, ZEMTProperty* Property);
		void									SelectionManager_OnSelectionChanged(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& Selection);
		void									Viewport_OnKeyboardKeyPressed(ZEDViewport* Viewport, ZEDKeyboardKey Key);	
		void									Viewport_OnMouseButtonPressed(ZEDViewport* Viewport, ZEDMouseButton Button);
		void									Viewport_OnMouseButtonRelease(ZEDViewport* Viewport, ZEDMouseButton Button);
		void									Viewport_OnMouseMoved(ZEDViewport* Viewport, const ZEVector2& Position);
		void									Viewport_OnPreRender(ZEDViewport* Viewport, const ZERNPreRenderParameters& Parameters);

												ZEDTransformationManager();
		virtual									~ZEDTransformationManager();
	
	private: /* COMMANDS */
		ZEDCommand								SelectCommand;
		ZEDCommand								MoveCommand;
		ZEDCommand								RotateCommand;
		ZEDCommand								AxisRotateCommand;
		ZEDCommand								ScaleCommand;
		ZEDCommand								SnapCommand;
		ZEDCommand								TransformSpaceCommand;
		ZEDCommand								TransformPivotCommand;
		ZEDCommand								XCommand;
		ZEDCommand								YCommand;
		ZEDCommand								ZCommand;

		void									RegisterCommands();
		void									UpdateCommands();

		void									SelectCommand_OnAction(const ZEDCommand* Command);
		void									MoveCommand_OnAction(const ZEDCommand* Command);
		void									RotateCommand_OnAction(const ZEDCommand* Command);
		void									AxisRotateCommand_OnAction(const ZEDCommand* Command);
		void									ScaleCommand_OnAction(const ZEDCommand* Command);
		void									SnapCommand_OnAction(const ZEDCommand* Command);
		void									TransformSpaceCommand_OnAction(const ZEDCommand* Command);
		void									TransformPivotCommand_OnAction(const ZEDCommand* Command);
		void									XCommand_OnAction(const ZEDCommand* Command);
		void									YCommand_OnAction(const ZEDCommand* Command);
		void									ZCommand_OnAction(const ZEDCommand* Command);



	public:
		void									SetTransformType(ZEDTransformType Type);
		ZEDTransformType						GetTransformType();

		void									SetTransformSpace(ZEDTransformSpace Space);
		ZEDTransformSpace						GetTransformSpace();

		void									SetTransformPivot(ZEDTransformPivot Pivot);
		ZEDTransformPivot						GetTransformPivot();

		void									SetX(float Value);
		float									GetX(bool& Valid);

		void									SetY(float Value);
		float									GetY(bool& Valid);

		void									SetZ(float Value);
		float									GetZ(bool& Valid);

		ZE_EVENT(								OnTransformationStarted,(ZEDTransformationManager* Manager));
		ZE_EVENT(								OnTransforming,(ZEDTransformationManager* Manager));
		ZE_EVENT(								OnTransformationEnded,(ZEDTransformationManager* Manager));
		ZE_EVENT(								OnTransformationReset,(ZEDTransformationManager* Manager));
		ZE_EVENT(								OnTransformationStateChanged,(ZEDTransformationManager* Manager));

		static ZEDTransformationManager*		CreateInstance();
};
