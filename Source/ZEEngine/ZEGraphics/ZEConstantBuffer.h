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

#ifndef __ZE_SHADER_CONSTANT_BUFFER_H__
#define __ZE_SHADER_CONSTANT_BUFFER_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEGraphicsResource.h"
#include "ZEGraphicsDefinitions.h"

struct ZEShaderBuffer;

class ZEConstantBuffer : public ZEGraphicsResource
{
	friend class ZEGraphicsDevice;
	friend class ZEGraphicsModule;

	protected:
		static ZESize				TotalSize;
		static ZEUInt16				TotalCount;

		ZEShadowCopy				ShadowCopy;

		ZESize						Size;
		const ZEShaderBuffer*		Info;

		virtual bool				UpdateWith(ZEUInt ShadowIndex);

									ZEConstantBuffer();
		virtual						~ZEConstantBuffer();

	public:
		ZESize						GetSize() const;
		const ZEShaderBuffer*		GetInfo() const;

		ZEGraphicsResourceType		GetResourceType() const;

		void						SetZero();
		void						SetData(void* ConstantData);

		bool						SetConstant(ZEUInt Index, const void* SourceData);
		bool						SetConstant(const char* Name, const void* SourceData);

		bool						GetConstant(ZEUInt Index, void* DestinationData) const;
		bool						GetConstant(const char* Name, void* DestinationData) const;

		void						Unlock();
		bool						Lock(void** Data);
		
		virtual bool				Create(const ZEShaderBuffer* BufferInfo);
		virtual bool				Create(ZESize BufferSize);

		static ZEConstantBuffer*	CreateInstance();
};

#endif
