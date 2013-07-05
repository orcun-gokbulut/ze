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

#ifndef	__ZE_VERTEX_BUFFER_H__
#define __ZE_VERTEX_BUFFER_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEVertexLayout.h"
#include "ZEGraphicsDefinitions.h"

struct ZEVertexBufferElement
{
	char				Semantic[ZE_MAX_SHADER_VARIABLE_NAME];
	ZEUInt8				Index;
	ZEVertexElementType	Type;
	ZEUInt16			Offset;
	ZEVertexUsage		Usage;
	ZEUInt16			InstanceCount;

	static ZESize		GetHash(const char* Semantic, ZEUInt8 Index);
};

class ZEVertexBuffer
{
	friend class ZEVertexLayout;
	friend class ZEGraphicsDevice;
	friend class ZEGraphicsModule;

	protected:
		bool							Static;
		ZESize							BufferSize;
		ZESize							VertexSize;
		ZESize							VertexCount;

		ZESize							ElementCount;
		ZESize							Hashes[ZE_MAX_VERTEX_LAYOUT_ELEMENT];
		ZEVertexBufferElement			Elements[ZE_MAX_VERTEX_LAYOUT_ELEMENT];

										ZEVertexBuffer();
		virtual							~ZEVertexBuffer();

	public:
		bool							IsStatic() const;
		ZESize							GetBufferSize() const;
		ZESize							GetVertexSize() const;
		ZESize							GetVertexCount() const;

		const ZEVertexBufferElement*	GetElements() const;
		ZESize							GetElementCount() const;
		
		void							ClearElements();
		void							RegisterElements(const ZEVertexBufferElement* Elements, ZESize Count);

		virtual bool					Lock(void** Data) = 0;
		virtual bool					Unlock() = 0;

		virtual bool					CreateStatic(ZEUInt VertexCount, ZESize VertexSize, const void* VertexData) = 0;
		virtual bool					CreateDynamic(ZEUInt VertexCount, ZESize VertexSize, const void* VertexData = NULL) = 0;

		virtual void					Destroy();

		static ZEVertexBuffer*			CreateInstance();
};

#endif
