//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProperty.h
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
#ifndef __ZE_PROPERTY_H__
#define __ZE_PROPERTY_H__

#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEVariant.h"
#include "ZEDefinitions.h"

enum ZEPropertySemantic
{
	ZE_PS_NONE,
	ZE_PS_POSITION,
	ZE_PS_ROTATION,
	ZE_PS_DIRECTION,
	ZE_PS_SCALE,
	ZE_PS_FILENAME,
	ZE_PS_COLOR,
};

enum ZEPropertyAccess
{
	ZE_PA_NOACCESS		= 0,
	ZE_PA_READONLY		= 1,
	ZE_PA_WRITEONLY		= 2,
	ZE_PA_READWRITE		= 3,
};

struct ZEPropertyEnumuratorItem
{
	const char*									Name;
	int											Value;
};

struct ZEPropertyEnumurator
{
	const char*									Name;
	ZEPropertyEnumuratorItem*					Items;
	size_t										ItemCount;
};



struct ZEPropertyDescription
{
	const char*									Name;
	ZEVariantType								Type;
	ZEPropertyAccess							Access;
	bool										Visibility;
	bool										Animatable;
	const char*									Description;
	const char*									GroupName;
	ZEPropertySemantic							Semantic;
	void*										SemanticProperties;
	ZEPropertyEnumurator*						Enumurators;
};

struct ZEMethodDescription
{
	const char*									Name;
	size_t										ParameterCount;
	const ZEVariantType* const					Parameters;
	ZEVariantType								ReturnType;
};

#endif



