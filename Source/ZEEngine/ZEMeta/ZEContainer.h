//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEContainer.h
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
#ifndef __ZE_META_CONTAINER_H__
#define __ZE_META_CONTAINER_H__

#include "ZEDS/ZEArray.h"

typedef ZEUInt ZEContainerAccess;
#define ZE_CA_NO_ACCESS		0
#define ZE_CA_ADD			1
#define ZE_CA_REMOVE		2
#define ZE_CA_GET			4

class ZEObject;
class ZEObjectProvider;
class ZEObjectDescription;

struct ZEContainerDescription
{
	const char*				Name;
	const char*				Description;
	const char*				GroupName;
	ZEContainerAccess		Access;
	bool					Visibility;

	ZEObjectDescription*	BaseClass;
	bool					AllowDerived;
};

enum ZEContainerMode
{
	ZE_CM_OWNER,
	ZE_CM_REFERANCE
};

class ZEContainer
{
	private:
		ZEContainerMode				Mode;
		ZEObjectDescription*		Type;
		bool						AllowDerivedTypes;

		ZEArray<ZEObject*>			Instances;

	public:
		void						SetContainerMode(ZEContainerMode Mode);
		ZEContainerMode				GetContainerMode();

		void						SetBaseClass(ZEObjectDescription* Type);
		ZEObjectDescription*		GetBaseClass();

		void						SetAllowDerivedClasses(bool Allow);
		bool						GetAllowDerivedClasses();

		const ZEArray<ZEObject*>&	GetInstances();
		bool						AddInstance(ZEObject* Instance);
		bool						RemoveInstance(ZEObject* Instance);

									ZEContainer();
									ZEContainer(ZEContainerMode Mode, ZEObjectDescription* Type, bool AllowDerived);
									~ZEContainer();
};

#endif
