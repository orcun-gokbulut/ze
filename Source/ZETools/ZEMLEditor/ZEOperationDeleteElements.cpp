//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOperationDeleteElements.cpp
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

#include "ZEOperationDeleteElements.h"
#include "ZEML/ZEMLNode.h"
#include "ZETypes.h"

#include <QtGui/QTreeWidgetItem>


bool ZEOperationDeleteElements::Apply()
{
	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		ZEOperationDeletedElement& CurrentElement = Elements[I];
		CurrentElement.Index = CurrentElement.Parent->GetElements().FindIndex(CurrentElement.Element);
		
		CurrentElement.Parent->RemoveElement(CurrentElement.Element);
		((QTreeWidgetItem*)CurrentElement.Parent->GetUserData())->removeChild((QTreeWidgetItem*)CurrentElement.Element->GetUserData());
	}
	return true;
}

bool ZEOperationDeleteElements::Revert()
{
	for (ZESSize I = Elements.GetCount() - 1; I >= 0; I--)
	{
		ZEOperationDeletedElement& CurrentElement = Elements[I];

		CurrentElement.Parent->InsertElement(CurrentElement.Index, CurrentElement.Element);
		((QTreeWidgetItem*)CurrentElement.Parent->GetUserData())->addChild((QTreeWidgetItem*)CurrentElement.Element->GetUserData());
	}
	return true;
}

void ZEOperationDeleteElements::AddDeletedElement(ZEMLElement* Element)
{
	ZEOperationDeletedElement DeleteElement;
	DeleteElement.Parent = Element->GetParent();
	DeleteElement.Element = Element;
	Elements.Add(DeleteElement);
}

ZEOperationDeleteElements::ZEOperationDeleteElements()
{

}

ZEOperationDeleteElements::~ZEOperationDeleteElements()
{
	if (GetStatus() == ZED_OS_NOT_DONE)
	{
		for (ZESize I = 0; I < Elements.GetCount(); I++)
		{
			delete (QTreeWidgetItem*)(Elements[I].Element->GetUserData());
			delete Elements[I].Element;
		}
	}
}
