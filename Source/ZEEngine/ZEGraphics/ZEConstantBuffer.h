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

#define ZE_MAX_CONSTANT_BUFFER_INSTANCE		4096

class ZEConstantBuffer;
struct ZEShaderConstant;

class ZEConstantBufferLink
{
	friend class ZEConstantBuffer;

	protected:
		ZEConstantBuffer*			Buffer;
		ZESSize						BufferId;
		ZESize						Index;

									ZEConstantBufferLink(ZEConstantBuffer* ConstantBuffer, ZESSize ConstantBufferId, ZESize ConstantIndex);
									~ZEConstantBufferLink();

	public:
		const ZEShaderConstant*		GetShaderConstant() const;

		virtual bool				SetValue(const void* Source);
		virtual bool				GetValue(void* Destination) const;
		
};


struct ZEShaderBuffer;

class ZEConstantBuffer
{
	friend class ZEMaterial;
	friend class ZERenderCommand;
	friend class ZEGraphicsDevice;
	friend class ZEGraphicsModule;

	private:
		struct ZEConstantBufferState
		{
			ZEUInt8		Deleted	: 1;	// Is buffer deleted ?
			ZEUInt8		InUse	: 1;	// Is buffer set to renderer ?
			ZEUInt8		Changed	: 1;	// Is level buffer changed ?
			ZEUInt8		Locked	: 1;	// Is level buffer locked ?

		} State;

	protected:
		ZESSize							Id;
		void*							Data;
		ZESize							Size;
		const ZEShaderBuffer*			Info;

		bool							NeedUpdate() const;

		virtual bool					UpdateBuffer();

										ZEConstantBuffer();
		virtual							~ZEConstantBuffer();

	public:
		ZESize							GetSize() const;
		const ZEShaderBuffer*			GetInfo() const;

		virtual bool					Lock(void** ConstantData);
		virtual void					Unlock(bool NotChanged = false);

		virtual ZEConstantBufferLink	GetConstantLink(ZESize Index);
		virtual ZEConstantBufferLink	GetConstantLink(const char* Name);

		virtual bool					SetConstant(ZESize Index, const void* SourceData);
		virtual bool					SetConstant(const char* Name, const void* SourceData);

		virtual bool					GetConstant(ZESize Index, void* DestinationData) const;
		virtual bool					GetConstant(const char* Name, void* DestinationData) const;

		virtual bool					Create(const ZEShaderBuffer* BufferInfo);
		virtual bool					Create(ZESize BufferSize);

		virtual void					Destroy();

		static ZEConstantBuffer*		CreateInstance();
};

#endif
