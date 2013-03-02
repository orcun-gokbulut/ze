//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10StatePool.h
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
#ifndef __ZE_D3D10_STATE_POOL_H__
#define __ZE_D3D10_STATE_POOL_H__

#include "ZETypes.h"
#include "ZEDS/ZEList.h"
#include "ZEDS/ZEArray.h"
#include "ZED3D10ComponentBase.h"
#include "ZED3D10GraphicsModule.h"
#include "ZEGraphics/ZEStatePool.h"

class ZEStatePoolEntry : public ZEListItem
{
	public:
		ZESize Hash;
		ZESize AccessCount;

		ZEStatePoolEntry()
		{
			Hash = 0;
			AccessCount = 0;
		}
		ZEStatePoolEntry(ZEUInt64 Hash, ZEUInt64 AccessCount)
		{
			this->Hash = Hash;
			this->AccessCount = AccessCount;
		}
		virtual ~ZEStatePoolEntry()
		{

		}
};

struct ID3D10InputLayout;
struct ID3D10BlendState;
struct ID3D10SamplerState;
struct ID3D10RasterizerState;
struct ID3D10DepthStencilState;

class ZEBlendStateEntry : public ZEStatePoolEntry
{
	public:
		ID3D10BlendState* BlendState;

		ZEBlendStateEntry()
		{
			BlendState = NULL;
		}
		ZEBlendStateEntry(ID3D10BlendState* BlendState, ZEUInt64 Hash, ZEUInt64 AccessCount) : ZEStatePoolEntry(Hash, AccessCount)
		{
			this->BlendState = BlendState;
		}
		virtual ~ZEBlendStateEntry()
		{
			ZED3D_RELEASE(BlendState);
		}
};

class ZESamplerStateEntry : public ZEStatePoolEntry
{
	public:
		ID3D10SamplerState*	SamplerState;

		ZESamplerStateEntry()
		{
			SamplerState = NULL;
		}
		ZESamplerStateEntry(ID3D10SamplerState* SamplerState, ZEUInt64 Hash, ZEUInt64 AccessCount) : ZEStatePoolEntry(Hash, AccessCount)
		{
			this->SamplerState = SamplerState;
		}
		virtual ~ZESamplerStateEntry()
		{
			ZED3D_RELEASE(SamplerState);
		}
};

class ZERasterizerStateEntry : public ZEStatePoolEntry
{
	public:
		ID3D10RasterizerState* RasterizerState;

		ZERasterizerStateEntry()
		{
			RasterizerState = NULL;
		}
		ZERasterizerStateEntry(ID3D10RasterizerState* RasterizerState, ZEUInt64 Hash, ZEUInt64 AccessCount) : ZEStatePoolEntry(Hash, AccessCount)
		{
			this->RasterizerState = RasterizerState;
		}
		virtual ~ZERasterizerStateEntry()
		{
			ZED3D_RELEASE(RasterizerState);
		}
};

class ZEDepthStencilStateEntry : public ZEStatePoolEntry
{
	public:
		ID3D10DepthStencilState* DepthStencilState;

		ZEDepthStencilStateEntry()
		{
			DepthStencilState = NULL;
		}
		ZEDepthStencilStateEntry(ID3D10DepthStencilState* DepthStencilState, ZEUInt64 Hash, ZEUInt64 AccessCount) : ZEStatePoolEntry(Hash, AccessCount)
		{
			this->DepthStencilState = DepthStencilState;
		}
		virtual ~ZEDepthStencilStateEntry()
		{
			ZED3D_RELEASE(DepthStencilState);
		}
};

class ZEVertexLayoutEntry : public ZEStatePoolEntry
{
	public:
		ID3D10InputLayout* VertexLayout;

		ZEVertexLayoutEntry()
		{
			VertexLayout = NULL;
		}
		ZEVertexLayoutEntry(ID3D10InputLayout* VertexLayout, ZEUInt64 Hash, ZEUInt64 AccessCount) : ZEStatePoolEntry(Hash, AccessCount)
		{
			this->VertexLayout = VertexLayout;
		}
		virtual ~ZEVertexLayoutEntry()
		{
			ZED3D_RELEASE(VertexLayout);
		}
};

class ZEShader;
class ZEBlendState;
class ZEVertexLayout;
class ZESamplerState;
class ZERasterizerState;
class ZEDepthStencilState;

struct ID3D10Blob;

class ZED3D10StatePool : public ZEStatePool, public ZED3D10ComponentBase
{
	friend class ZED3D10GraphicsModule;

	protected:
		ZEList<ZEStatePoolEntry>	BlendStatePool;
		ZEList<ZEStatePoolEntry>	VertexLayoutPool;
		ZEList<ZEStatePoolEntry>	SamplerStatePool;
		ZEList<ZEStatePoolEntry>	RasterizerStatePool;
		ZEList<ZEStatePoolEntry>	DepthStencilStatePool;

		ID3D10BlendState*			CreateD3D10State(const ZEBlendState* BlendState);
		ID3D10SamplerState*			CreateD3D10State(const ZESamplerState* SamplerState);
		ID3D10RasterizerState*		CreateD3D10State(const ZERasterizerState* RasterizerState);
		ID3D10DepthStencilState*	CreateD3D10State(const ZEDepthStencilState* DepthStencilState);
		ID3D10InputLayout*			CreateD3D10State(const ZEVertexLayout* VertexLayout, ID3D10Blob* ByteCode);

		ZEStatePoolEntry*			FindPoolEntry(ZEList<ZEStatePoolEntry>& Pool, ZESize Hash);

									ZED3D10StatePool();
		virtual						~ZED3D10StatePool();

	public:
		virtual void				ClearStates();

		virtual void*				GetBlendState(ZESize Hash);
		virtual void*				GetSamplerState(ZESize Hash);
		virtual void*				GetVertexLayout(ZESize Hash);
		virtual void*				GetRasterizerState(ZESize Hash);
		virtual void*				GetDepthStencilState(ZESize Hash);

		virtual void*				CreateState(const ZEBlendState* BlendState);
		virtual void*				CreateState(const ZESamplerState* SamplerState);
		virtual void*				CreateState(const ZERasterizerState* RasterizerState);
		virtual void*				CreateState(const ZEDepthStencilState* DepthStencilState);
		virtual void*				CreateState(const ZEVertexLayout* VertexLayout, const ZEShader* VertexShader);
};

#endif
