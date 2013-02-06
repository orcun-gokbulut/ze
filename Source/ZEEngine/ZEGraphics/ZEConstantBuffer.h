//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEConstantBuffer.h
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
#ifndef __ZE_SHADER_CONSTANT_DATA_H__
#define __ZE_SHADER_CONSTANT_DATA_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"

struct ZEShaderBufferInfo;

class ZEConstantBuffer
{
	friend class ZEGraphicsDevice;
	friend class ZEGraphicsModule;

	// Should be public for only internal usage
	public:
		bool						NeedUpdate;
		
		void*						Data;
		ZESize						Size;
		const ZEShaderBufferInfo*	Info;

		virtual void				UpdateData() = 0;

									ZEConstantBuffer();
		virtual						~ZEConstantBuffer();

	public:
		ZESize						GetSize() const;
		const ZEShaderBufferInfo*	GetInfo() const;

		virtual void				Unlock() = 0;
		virtual bool				Lock(void** ConstantData) = 0;
		
		virtual bool				SetConstant(ZESize Index, const void* SourceData) = 0;
		virtual bool				GetConstant(ZESize Index, void* DestinationData) const = 0;
		
		virtual bool				SetConstant(const ZEString& Name, const void* SourceData) = 0;
		virtual bool				GetConstant(const ZEString& Name, void* DestinationData) const = 0;

		virtual void				Destroy();
		virtual bool				Create(ZESize BufferSize) = 0;
		virtual bool				Create(const ZEShaderBufferInfo* BufferInfo) = 0;
		
		static ZEConstantBuffer*	CreateInstance();
};

#endif
