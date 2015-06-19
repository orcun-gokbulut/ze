//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRCounter.h
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

#include "ZETypes.h"
#include "ZEThread/ZELock.h"

#define ZEGR_COUNTER_RESOURCE_INCREASE(Pointer, Type, SuperType) \
	{ \
		ZEGRCounter& Counter = ZEGRGraphicsModule::GetInstance()->GetCounter(); \
		Counter.UpdateLock.Lock(); \
		Counter.ObjectCount.Object++; \
		Counter.ObjectCount.Type++; \
		Counter.ObjectCount.SuperType++; \
		Counter.MemoryUsage.Resource += Pointer->GetSize(); \
		Counter.MemoryUsage.Type += Pointer->GetSize(); \
		Counter.MemoryUsage.SuperType += Pointer->GetSize(); \
		Counter.UpdateLock.Unlock(); \
	}
		
#define ZEGR_COUNTER_RESOURCE_DECREASE(Pointer, Type, SuperType) \
	{ \
		ZEGRCounter& Counter = ZEGRGraphicsModule::GetInstance()->GetCounter(); \
		Counter.UpdateLock.Lock(); \
		Counter.ObjectCount.Object--; \
		Counter.ObjectCount.Type--; \
		Counter.ObjectCount.SuperType--; \
		Counter.MemoryUsage.Resource -= Pointer->GetSize(); \
		Counter.MemoryUsage.Type -= Pointer->GetSize(); \
		Counter.MemoryUsage.SuperType -= Pointer->GetSize(); \
		Counter.UpdateLock.Unlock(); \
	}

struct ZEGRDrawStatistics
{
	ZESize		FrameCount;
	ZESize		DrawCall;
	ZESize		InstancedDrawCall;
	ZESize		DrawedPrimtivies;
	ZESize		StateChanges;
};

struct ZEGRMemoryUsage
{
	ZESize		Resource;

	ZESize		Geometry;
	ZESize		IndexBuffer;
	ZESize		VertexBuffer;

	ZESize		Pipeline;
	ZESize		Shader;
	ZESize		RenderState;
	ZESize		ConstantBuffer;	

	ZESize		Texture;
	ZESize		Texture3D;
	ZESize		Texture2D;
	ZESize		TextureCube;
	ZESize		RenderTarget;
	ZESize		DepthStencilBuffer;
};

struct ZEGRObjectsCount
{
	ZESize		Object;

	ZESize		Geometry;
	ZESize		IndexBuffer;
	ZESize		VertexBuffer;

	ZESize		Pipeline;
	ZESize		Shader;
	ZESize		RenderState;
	ZESize		ConstantBuffer;	

	ZESize		Texture;
	ZESize		Texture3D;
	ZESize		Texture2D;
	ZESize		TextureCube;
	ZESize		RenderTarget;
	ZESize		DepthStencilBuffer;
};

class ZEGRCounter
{
	ZE_DISALLOW_COPY(ZEGRCounter)

	friend class ZEGRGraphicsModule;
	friend class ZEGRVertexBuffer;
	friend class ZEGRIndexBuffer;
	friend class ZEGRConstantBuffer;
	friend class ZEGRShader;
	friend class ZEGRRenderTarget;
	friend class ZEGRRenderState;
	friend class ZEGRDepthStencilBuffer;
	friend class ZEGRTexture2D;
	friend class ZEGRTexture3D;
	friend class ZEGRTextureCube;
	
	private:
		ZELock						UpdateLock;

		ZEGRDrawStatistics			DrawStatistics;
		ZEGRMemoryUsage				MemoryUsage;
		ZEGRObjectsCount			ObjectCount;

		void						Clean();
		void						Reset();

									ZEGRCounter();

	public:
		const ZEGRDrawStatistics&	GetDrawStatistics();
		const ZEGRMemoryUsage&		GetMemoryUsage();
		const ZEGRObjectsCount&		GetObjectCount();
};
