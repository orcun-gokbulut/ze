//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectWrapper.cpp
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

#include "ZEDObjectWrapper.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEMath.h"

ZEDObjectWrapper::ZEDObjectWrapper()
{
	Id = 0;
/*	CustomWidget = NULL;*/
	PopupMenu = NULL;

	Object = NULL;
	ParentWrapper = NULL;
}

ZEDObjectWrapper::~ZEDObjectWrapper()
{
	if (Object != NULL)
	{
		delete Object;
		Object = NULL;
	}
}

void ZEDObjectWrapper::SetObjectId(ZESize Id)
{
	this->Id = Id;
}

ZESize ZEDObjectWrapper::GetObjectId()
{
	return Id;
}

void ZEDObjectWrapper::SetObjectName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEDObjectWrapper::GetObjectName()
{
	return Name;
}

// void ZEDObjectWrapper::SetCustomWidget(QWidget* Widget)
// {
// 	CustomWidget = Widget;
// }
// 
// QWidget* ZEDObjectWrapper::GetCustomWidget()
// {
// 	return CustomWidget;
// }

void ZEDObjectWrapper::SetPopupMenu(QMenu* Menu)
{
	PopupMenu = Menu;
}

QMenu* ZEDObjectWrapper::GetPopupMenu()
{
	return PopupMenu;
}

bool ZEDObjectWrapper::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	float TMin = 0.0f;

	if (!ZEOBBox::IntersectionTest(GetObjectBoundingBox(), Parameters.Ray, TMin))
		return false;

	ZEVector3 IntersectionPoint = Parameters.Ray.GetPointOn(TMin);
	float DistanceSquare = ZEVector3::DistanceSquare(Parameters.Ray.p, IntersectionPoint);
	if (Report.Distance * Report.Distance > DistanceSquare && DistanceSquare < Parameters.MaximumDistance * Parameters.MaximumDistance)
	{
		Report.Position = IntersectionPoint;
		Report.Distance = ZEMath::Sqrt(DistanceSquare);
		Report.Object = this;
		Report.SubComponent = NULL;
		Report.PoligonIndex = 0;
		Report.Normal = ZEVector3::Zero;
		Report.Binormal = ZEVector3::Zero;

		return true;
	}

	return false;
}

void ZEDObjectWrapper::SetObject(ZEObject* Object)
{
	if (this->Object != NULL)
		return;

	this->Object = Object;
}

ZEObject* ZEDObjectWrapper::GetObject()
{
	return Object;
}

void ZEDObjectWrapper::SetObjectIcon(const ZEString& Icon)
{
	this->Icon = Icon;
}

const ZEString& ZEDObjectWrapper::GetObjectIcon()
{
	return Icon;
}

void ZEDObjectWrapper::SetParentWrapper(ZEDObjectWrapper* Wrapper)
{
	ParentWrapper = Wrapper;
}

ZEDObjectWrapper* ZEDObjectWrapper::GetParentWrapper()
{
	return ParentWrapper;
}

ZEArray<ZEDObjectWrapper*>& ZEDObjectWrapper::GetChildWrappers()
{
	return ChildWrapper;
}

void ZEDObjectWrapper::AddChildWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Wrapper == NULL)
		return;

	if (ChildWrapper.Exists(Wrapper))
		return;

	if (!ZEClass::IsDerivedFrom(Object->GetClass(), Wrapper->GetObject()->GetClass()))
		return;

	ChildWrapper.Add(Wrapper);
	Wrapper->SetParentWrapper(this);
}

void ZEDObjectWrapper::RemoveChildWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Wrapper == NULL)
		return;

	if (!ChildWrapper.Exists(Wrapper))
		return;

	ChildWrapper.RemoveValue(Wrapper);
	Wrapper->SetParentWrapper(NULL);
}

void ZEDObjectWrapper::Draw(ZEDrawParameters* Parameters)
{
	//Icon
}

void ZEDObjectWrapper::Tick(float ElapsedTime)
{

}

void ZEDObjectWrapper::Destroy()
{
	delete this;
}
