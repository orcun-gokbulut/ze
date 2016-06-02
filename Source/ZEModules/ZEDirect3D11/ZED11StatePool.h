//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11StatePool.h
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

#include "ZED11ComponentBase.h"

#include "ZETypes.h"
#include "ZEDS/ZEList2.h"
#include "ZED11State.h"

class ZEGRShader;
class ZEGRRenderState;
class ZEGRRenderStateData;

class ZED11StatePool : public ZED11ComponentBase
{
	friend class ZED11Module;
	template<typename _ZEGRState, typename _IInterface> friend class ZED11State;
	protected:
		mutable ZEList2<ZED11StateBase>			BlendStatePool;
		mutable ZEList2<ZED11StateBase>			VertexLayoutPool;
		mutable ZEList2<ZED11StateBase>			RasterizerStatePool;
		mutable ZEList2<ZED11StateBase>			DepthStencilStatePool;

		ID3D11BlendState*						CreateBlendState(const ZEGRBlendState& BlendState) const;
		ID3D11RasterizerState*					CreateRasterizerState(const ZEGRRasterizerState& RasterizerState) const;
		ID3D11DepthStencilState*				CreateDepthStencilState(const ZEGRDepthStencilState& DepthStencilState) const;
		ID3D11InputLayout*						CreateVertexLayout(const ZEGRVertexLayout& VertexLayout, const ZEGRShader* Shader) const;

		ZED11StateBase*							FindPoolEntry(ZEList2<ZED11StateBase>& Pool, const ZEGRState& State) const;

												ZED11StatePool();
		virtual									~ZED11StatePool();

	public:
		ZEHolder<const ZED11BlendState>			GetBlendState(const ZEGRBlendState& BlendState) const;
		ZEHolder<const ZED11RasterizerState>	GetRasterizerState(const ZEGRRasterizerState& RasterizerState) const;
		ZEHolder<const ZED11DepthStencilState>	GetDepthStencilState(const ZEGRDepthStencilState& DepthStencilState) const;
		ZEHolder<const ZED11VertexLayout>		GetVertexLayout(const ZEGRVertexLayout& VertexLayout, const ZEGRShader* VertexShader) const;
};
