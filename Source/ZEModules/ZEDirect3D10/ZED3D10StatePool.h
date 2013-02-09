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

#define	ZE_D3D10_STATE_CACHE_CAPACITY	256

class ZEStatePoolEntry : public ZEListItem
{
	public:
		ZEUInt64 Hash;
		ZEUInt64 AccessCount;

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
		~ZEBlendStateEntry()
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
		~ZESamplerStateEntry()
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
		~ZERasterizerStateEntry()
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
		~ZEDepthStencilStateEntry()
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
		~ZEVertexLayoutEntry()
		{
			ZED3D_RELEASE(VertexLayout);
		}
};

class ZEShader;
class ZEVertexLayout;
class ZEBlendState;
class ZESamplerState;
class ZERasterizerState;
class ZEDepthStencilState;

struct ID3D10Blob;

class ZED3D10StatePool : public ZED3D10ComponentBase
{
	private:
		static ZEList<ZEBlendStateEntry>		BlendStatePool;
		static ZEList<ZEVertexLayoutEntry>		VertexLayoutPool;
		static ZEList<ZESamplerStateEntry>		SamplerStatePool;
		static ZEList<ZERasterizerStateEntry>	RasterizerStatePool;
		static ZEList<ZEDepthStencilStateEntry>	DepthStencilStatePool;

		static ID3D10BlendState*				CreateD3D10BlendState(const ZEBlendState* BlendState);
		static ID3D10SamplerState*				CreateD3D10SamplerState(const ZESamplerState* SamplerState);
		static ID3D10RasterizerState*			CreateD3D10RasterizerState(const ZERasterizerState* RasterizerState);
		static ID3D10DepthStencilState*			CreateD3D10DepthStencilState(const ZEDepthStencilState* DepthStencilState);
		static ID3D10InputLayout*				CreateD3D10VertexLayout(const ZEVertexLayout* VertexLayout, ID3D10Blob* ByteCode);

												ZED3D10StatePool();
		virtual									~ZED3D10StatePool();

	public:
		static void								ClearStates();
		static void								ClearStatistics();

		static ZEUInt							GetStateCount();
		static ZEUInt							GetBlendStateCount();
		static ZEUInt							GetVertexLayoutCount();
		static ZEUInt							GetSamplerStateCount();
		static ZEUInt							GetRasterizerStateCount();
		static ZEUInt							GetDepthStencilStateCount();
		
		static ID3D10BlendState*				GetBlendState(const ZEBlendState* BlendState);
		static ID3D10SamplerState*				GetSamplerState(const ZESamplerState* SamplerState);
		static ID3D10RasterizerState*			GetRasterizerState(const ZERasterizerState* RasterizerState);
		static ID3D10DepthStencilState*			GetDepthStencilState(const ZEDepthStencilState* DepthStencilState);
		static ID3D10InputLayout*				GetVertexLayout(const ZEVertexLayout* VertexLayout, const ZEShader* VertexShader);
};

#endif
