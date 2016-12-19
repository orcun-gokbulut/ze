//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetModelColumn.cpp
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

#include "ZEDAssetModelColumn.h"

void ZEDAssetModelColumn::SetType(ZEDAssetModelColumnType Type)
{
	this->Type = Type;
}

ZEDAssetModelColumnType ZEDAssetModelColumn::GetType() const
{
	return Type;
}

void ZEDAssetModelColumn::SetHeaderText(const ZEString& Text)
{
	this->HeaderText = Text;
}

const ZEString& ZEDAssetModelColumn::GetHeaderText() const
{
	return HeaderText;
}

void ZEDAssetModelColumn::SetAlignmnent(Qt::Alignment Alignment)
{
	this->Alignment = Alignment;
}

Qt::Alignment ZEDAssetModelColumn::GetAlignmnent() const
{
	return Alignment;
}

void ZEDAssetModelColumn::SetSelectable(bool Selectable)
{
	this->Selectable = Selectable;
}

bool ZEDAssetModelColumn::GetSelectable() const
{
	return Selectable;
}

void ZEDAssetModelColumn::SetCustomPropertyName(const ZEString& Name)
{
	this->CustomPropertyName = Name;
}

const ZEString& ZEDAssetModelColumn::GetCustomPropertyName() const
{
	return CustomPropertyName;
}

ZEDAssetModelColumn::ZEDAssetModelColumn()
{
	Type = ZED_AMHT_NONE;
	Alignment = Qt::AlignLeft | Qt::AlignVCenter;
	Selectable = false;
}
