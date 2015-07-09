//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRResource.h
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

#include "ZEGRDefinitions.h"
#include "ZEDS/ZEString.h"
#include "ZECommon.h"

enum ZEGRResourceType
{
	ZEGR_RT_NONE					= 0,
	ZEGR_RT_VERTEX_BUFFER			= 1,
	ZEGR_RT_INDEX_BUFFER			= 2,
	ZEGR_RT_CONSTANT_BUFFER			= 3,
	ZEGR_RT_TEXTURE					= 4,
	ZEGR_RT_RENDER_TARGET			= 5,
	ZEGR_RT_DEPTH_STENCIL_BUFFER	= 6,
	ZEGR_RT_SHADER					= 7,
	ZEGR_RT_RENDER_STATE			= 8
};

class ZEGRResource
{
	ZE_DISALLOW_COPY(ZEGRResource)

	template <typename Type> friend class ZEGRHolder;
	friend class ZEGRGraphicsModule;

	private:
		ZESSize							ReferenceCount;
		ZESize							Size;

	protected:
		void							SetSize(ZESize Size);

		#ifdef ZE_DEBUG_ENABLE
		ZEString						Name;
		#endif

		void							AddRef();
		virtual void					Destroy();

										ZEGRResource();
		virtual 						~ZEGRResource();

	public:
		virtual ZEGRResourceType		GetResourceType() = 0;
		ZESize							GetSize() const;
		ZESize							GetReferenceCount() const;

		void							SetName(const char* Name);
		const char*						GetName() const;

		void							Release();
};
