//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVertexBuffer.h
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
#ifndef	__ZE_VERTEX_BUFFER_H__
#define __ZE_VERTEX_BUFFER_H__

#include "ZEDS/ZEArray.h"

class ZEVertexBuffer
{
	public:
		virtual bool						IsStatic() = 0;
		virtual size_t						GetBufferSize() = 0;

											ZEVertexBuffer();
		virtual								~ZEVertexBuffer();
};

class ZEStaticVertexBuffer : public ZEVertexBuffer
{
	protected:
											ZEStaticVertexBuffer();
		virtual								~ZEStaticVertexBuffer();

	public:
		virtual bool						IsStatic();

		virtual bool						Create(unsigned int BufferSize) = 0;
		virtual void*						Lock() = 0;
		virtual void						Unlock() = 0;
		virtual void						Release() = 0;

		virtual void						Destroy();

		static ZEStaticVertexBuffer*		CreateInstance();
};

class ZEDynamicVertexBuffer : public ZEVertexBuffer
{
	public:
		virtual bool						IsStatic();

		virtual unsigned int				GetBufferSize() = 0;
		virtual void*						GetVertexBuffer() = 0;
};

template<typename _VertexType, typename _Allocator = ZESmartAllocator<_VertexType> >
class ZEArrayVertexBuffer : public ZEDynamicVertexBuffer, public ZEArray<_VertexType, _Allocator>
{
	public:
		virtual unsigned int GetBufferSize()
		{
			return GetCount() * sizeof(_VertexType);
		}

		virtual void* GetVertexBuffer()
		{
			return GetCArray();
		}
};

#endif




