//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewportSelectionController.cpp
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

#include "ZEDViewportSelectionController.h"

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEFrustum.h"
#include "ZEDObjectWrapper.h"
#include "ZEDEditor.h"
#include "ZEDSelectionManager.h"
#include "ZEDViewport.h"
#include "ZEDObjectManager.h"
#include "ZEDViewportEvent.h"
#include "ZEDUserInterface/ZEDCommandManager.h"
#include "ZEUI/ZEUIFrameControl.h"
#include "ZERenderer/ZERNScreenUtilities.h"
#include "ZERenderer/ZERNView.h"
#include "ZEError.h"
#include "ZEDObjectWrapper3D.h"

ZEDSelectionManager* ZEDViewportSelectionController::GetSelectionManager()
{
	ZEDEditor* Editor = GetEditor();
	if (Editor == NULL)
		return NULL;

	return Editor->GetSelectionManager();
}

void ZEDViewportSelectionController::CastVolume(ZEArray<ZEDObjectWrapper*>& List, const ZEFrustum& Frustum, ZEDObjectWrapper* Wrapper)
{
	const ZEArray<ZEDObjectWrapper*> ChildWrappers = Wrapper->GetChildWrappers();
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		zeDebugCheck(!ZEClass::IsDerivedFrom(ZEDObjectWrapper3D::Class(), ChildWrappers[I]->GetClass()), "ChildWrapper is not derived from ZEDObjectWrapper3D");

		ZEDObjectWrapper3D* ChildWrapper = static_cast<ZEDObjectWrapper3D*>(ChildWrappers[I]);

		CastVolume(List, Frustum, ChildWrapper);

		if (!ChildWrapper->GetSelectable())
			continue;

		if (!ChildWrapper->GetPickable())
			continue;

		if (ChildWrapper->GetFrozen())
			continue;

		ZEOBBox BoundingBox;
		ZEAABBox::GenerateOBoundingBox(BoundingBox, ChildWrapper->GetBoundingBox());
		ZEOBBox WorldBoundingBox;
		ZEOBBox::Transform(WorldBoundingBox, ChildWrapper->GetWorldTransform(), BoundingBox);

		if (GetSelectionMode() == ZE_SM_FULLY_COVERS)
		{
			if (Frustum.IntersectionTestExact(Frustum, WorldBoundingBox) != ZE_IR_COVERS)
				continue;
		}
		else
		{
			if (Frustum.IntersectionTestExact(Frustum, WorldBoundingBox) == ZE_IR_NONE)
				continue;
		}

		List.Add(ChildWrapper);
	}
}

void ZEDViewportSelectionController::ViewportKeyboardEvent(const ZEDViewportKeyboardEvent* Event)
{
	if (GetSelectionManager() == NULL)
		return;

	if (Event->GetKey() == ZED_VKK_ESCAPE && Event->GetType() == ZED_VIET_BUTTON_PRESSED)
	{
		GetSelectionManager()->ClearSelection();
		Event->Acquire();
	}
}

void ZEDViewportSelectionController::ViewportMouseEvent(const ZEDViewportMouseEvent* Event)
{
	if (GetEditor() == NULL)
		return;

	if (GetSelectionManager() == NULL)
		return;

	if (GetEditor()->GetObjectManager()->GetRootWrapper() == NULL)
		return;

	if (Event->GetButton() == ZED_VMB_LEFT && Event->GetType() == ZED_VIET_BUTTON_PRESSED)
	{
		MultiSelection = true;
		MultiSelectionStartPosition = Event->GetPosition();

		// Single Selection	
		ZERayCastParameters Parameters;
		Parameters.Ray = ZERNScreenUtilities::ScreenToWorld(Event->GetViewport()->GetView(), Event->GetPosition());
		Parameters.Components |= ZE_RCRE_MULTI_COLLISIONS;

		ZERayCastReport Report;
		Report.SetParameters(&Parameters);

		zeDebugCheck(!ZEClass::IsDerivedFrom(ZEDObjectWrapper3D::Class(), GetEditor()->GetObjectManager()->GetRootWrapper()->GetClass()), "RootWrapper is not derived from ZEDObjectWrapper3D");

		ZEDObjectWrapper3D* RootWrapper = static_cast<ZEDObjectWrapper3D*>(GetEditor()->GetObjectManager()->GetRootWrapper());
		RootWrapper->RayCast(Report, Parameters);	

		const ZEArray<ZERayCastCollision>& Collisions = Report.GetCollisions();
		bool HasSelection = false;
		for (ZESize I = 0; I < Collisions.GetCount(); I++)
		{
			ZEDObjectWrapper3D* Wrapper = ZEClass::Cast<ZEDObjectWrapper3D>(Collisions[I].Object);
			
			if (Wrapper == NULL)
				continue;

			if (!Wrapper->GetSelectable())
				continue;
			
			if (!Wrapper->GetPickable())
				continue;

			if (Wrapper->GetFrozen())
				continue;

			HasSelection = true;

			if ((Event->GetModifiers() & ZED_VKM_CTRL) != 0 || (Event->GetModifiers() & ZED_VKM_SHIFT) != 0)
			{
				GetSelectionManager()->SelectObject(Wrapper);
			}
			else if (Event->GetModifiers() & ZED_VKM_ALT)
			{
				GetSelectionManager()->DeselectObject(Wrapper);
			}
			else
			{
				if (GetSelectionManager()->GetSelection().Exists(Wrapper))
				{
					GetSelectionManager()->FocusObject(Wrapper);
				}
				else
				{
					GetSelectionManager()->ClearSelection();
					GetSelectionManager()->SelectObject(Wrapper);
				}
			}

			break;
		}
	
		if (!HasSelection)
			GetSelectionManager()->ClearSelection();

		Event->Acquire();
	}
	else if (Event->GetType() == ZED_VIET_MOUSE_MOVED)
	{
		if (MultiSelection)
		{
			ZEVector2 Size;
			Size.x = ZEMath::Abs(MultiSelectionStartPosition.x - Event->GetPosition().x);
			Size.y = ZEMath::Abs(MultiSelectionStartPosition.y - Event->GetPosition().y);

			if (Size.Min() != 0)
			{
				ZEVector2 Position;
				Position.x = MultiSelectionStartPosition.x < Event->GetPosition().x ? MultiSelectionStartPosition.x : Event->GetPosition().x;
				Position.y = MultiSelectionStartPosition.y < Event->GetPosition().y ? MultiSelectionStartPosition.y : Event->GetPosition().y;

				MultiSelectionBox->SetPosition(Position);
				MultiSelectionBox->SetSize(Size);
				MultiSelectionBox->SetVisiblity(true);
			}
		}
		else
		{
			MultiSelectionBox->SetVisiblity(false);
		}
	}
	else if (Event->GetButton() == ZED_VMB_LEFT && Event->GetType() == ZED_VIET_BUTTON_RELEASED)
	{
		if (MultiSelection)
		{
			ZEVector2 Size;
			Size.x = ZEMath::Abs(MultiSelectionStartPosition.x - Event->GetPosition().x);
			Size.y = ZEMath::Abs(MultiSelectionStartPosition.y - Event->GetPosition().y);

			ZEArray<ZEDObjectWrapper*> List;
			if (Size.Min() != 0)
			{
				ZEVector2 Position;
				Position.x = MultiSelectionStartPosition.x < Event->GetPosition().x ? MultiSelectionStartPosition.x : Event->GetPosition().x;
				Position.y = MultiSelectionStartPosition.y < Event->GetPosition().y ? MultiSelectionStartPosition.y : Event->GetPosition().y;

				const ZERNView& View = Event->GetViewport()->GetView();
				ZERay LeftBottomRay =  ZERNScreenUtilities::ScreenToWorld(View, Position + ZEVector2(0.0f, Size.y));
				ZERay RightBottomRay = ZERNScreenUtilities::ScreenToWorld(View, Position + ZEVector2(Size.x, Size.y));
				ZERay LeftTopRay =	ZERNScreenUtilities::ScreenToWorld(View, Position);
				ZERay RightTopRay = ZERNScreenUtilities::ScreenToWorld(View, Position + ZEVector2(Size.x, 0.0f));

				ZEFrustum Frustum;
				ZEPlane::Create(Frustum.LeftPlane,	LeftBottomRay.GetPointOn(1.0f),	 LeftBottomRay.p,  LeftTopRay.GetPointOn(1.0f));
				ZEPlane::Create(Frustum.RightPlane,	RightTopRay.GetPointOn(1.0f),    RightBottomRay.p, RightBottomRay.GetPointOn(1.0f));
				ZEPlane::Create(Frustum.BottomPlane,	RightBottomRay.GetPointOn(1.0f), LeftBottomRay.p,  LeftBottomRay.GetPointOn(1.0f));
				ZEPlane::Create(Frustum.TopPlane,	LeftTopRay.GetPointOn(1.0f),	 LeftTopRay.p,	   RightTopRay.GetPointOn(1.0f));

				Frustum.NearPlane.p = View.Position + View.Direction * View.NearZ;
				Frustum.NearPlane.n = View.Direction;
				Frustum.FarPlane.p = View.Position + View.Direction * View.FarZ;
				Frustum.FarPlane.n = -View.Direction;

				ZEDObjectWrapper* RootWrapper = GetEditor()->GetObjectManager()->GetRootWrapper();
				CastVolume(List, Frustum, RootWrapper);

				if ((Event->GetModifiers() & ZED_VKM_CTRL) != 0 || (Event->GetModifiers() & ZED_VKM_SHIFT) != 0)
					GetSelectionManager()->SelectObjects(List);
				else if (Event->GetModifiers() & ZED_VKM_ALT)
					GetSelectionManager()->DeselectObjects(List);
				else
					GetSelectionManager()->SetSelection(List);
			}
		}

		MultiSelection = false;
		MultiSelectionBox->SetVisiblity(false);
	}
	else if (Event->GetButton() == ZED_VMB_RIGHT && Event->GetType() == ZED_VIET_BUTTON_PRESSED)
	{
		MultiSelection = false;
		MultiSelectionBox->SetVisiblity(false);
	}
}

bool ZEDViewportSelectionController::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	MultiSelectionBox = new ZEUIFrameControl();
	MultiSelectionBox->SetBackgroundColor(ZEVector4(0.17f, 0.5f, 1.0f, 0.5f));
	MultiSelectionBox->SetVisiblity(false);
	MultiSelectionBox->SetZOrder(1000);
	GetEditor()->GetUIManager()->AddControl(MultiSelectionBox);

	RegisterCommands();

	return true;
}

bool ZEDViewportSelectionController::DeinitializeInternal()
{
	GetEditor()->GetUIManager()->RemoveControl(MultiSelectionBox);
	MultiSelectionBox->Destroy();

	return ZEInitializable::DeinitializeInternal();
}

ZEDViewportSelectionController::ZEDViewportSelectionController()
{
	MultiSelection = false;
}

ZEDViewportSelectionController::~ZEDViewportSelectionController()
{
	Deinitialize();
}

void ZEDViewportSelectionController::RegisterCommands()
{
	SelectionModeCommand.SetCategory("Selection");
	SelectionModeCommand.SetName("ZEDViewportSelectionController::SelectionModeCommand");
	SelectionModeCommand.SetText("Intersects");
	SelectionModeCommand.SetType(ZED_CT_TOGGLE);
	SelectionModeCommand.OnAction += ZEDCommandDelegate::Create<ZEDViewportSelectionController, &ZEDViewportSelectionController::SelectionModeCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SelectionModeCommand);

	SelectionShapeCommand.SetCategory("Selection");
	SelectionShapeCommand.SetName("ZEDViewportSelectionController::SelectionShapeCommand");
	SelectionShapeCommand.SetText("Shape");
	SelectionShapeCommand.SetType(ZED_CT_LIST);
	ZEArray<ZEString> Items;
	Items.Add("Rectangle");
	Items.Add("Circle");
	Items.Add("Brush");
	SelectionShapeCommand.SetListItems(Items);
	SelectionShapeCommand.SetValueIndex(0);
	SelectionShapeCommand.OnAction += ZEDCommandDelegate::Create<ZEDViewportSelectionController, &ZEDViewportSelectionController::SelectionShapeCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SelectionShapeCommand);
}

void ZEDViewportSelectionController::UpdateCommands()
{
	SelectionShapeCommand.SetValueIndex(GetSelectionMode());
	SelectionShapeCommand.SetValueIndex(GetSelectionShape());
}

void ZEDViewportSelectionController::SelectionModeCommand_OnAction(const ZEDCommand* Command)
{
	if (Command->GetValueChecked())
	{
		SetSelectionMode(ZE_SM_FULLY_COVERS);
		SelectionModeCommand.SetText("Covers");
	}
	else
	{
		SetSelectionMode(ZE_SM_INTERSECTS);
		SelectionModeCommand.SetText("Intersects");
	}
}

void ZEDViewportSelectionController::SelectionShapeCommand_OnAction(const ZEDCommand* Command)
{
	SetSelectionShape((ZEDSelectionShape)Command->GetValueIndex());
}

void ZEDViewportSelectionController::SetSelectionMode(ZEDSelectionMode Mode)
{
	if (SelectionMode == Mode)
		return;

	SelectionMode = Mode;
	
	UpdateCommands();
}

ZEDSelectionMode ZEDViewportSelectionController::GetSelectionMode() const
{
	return SelectionMode;
}

void ZEDViewportSelectionController::SetSelectionShape(ZEDSelectionShape Shape)
{
	if (SelectionShape == Shape)
		return;

	SelectionShape = Shape;

	UpdateCommands();
}

ZEDSelectionShape ZEDViewportSelectionController::GetSelectionShape() const
{
	return SelectionShape;
}

ZEDViewportSelectionController* ZEDViewportSelectionController::CreateInstance()
{
	return new ZEDViewportSelectionController();
}
