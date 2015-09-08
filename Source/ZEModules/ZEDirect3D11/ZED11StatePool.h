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
#include "ZED11State.h"
#include "ZETypes.h"
#include "ZEDS/ZEList.h"

class ZEGRShader;
class ZEGRRenderState;
class ZEGRRenderStateData;

class ZED11StatePool : public ZED11ComponentBase
{
	friend class ZED11Module;
	protected:
		ZEList<ZED11StateBase>				BlendStatePool;
		ZEList<ZED11StateBase>				VertexLayoutPool;
		ZEList<ZED11StateBase>				SamplerStatePool;
		ZEList<ZED11StateBase>				RasterizerStatePool;
		ZEList<ZED11StateBase>				DepthStencilStatePool;
		
		ID3D11BlendState*					CreateBlendState(const ZEGRBlendState& BlendState);
		ID3D11SamplerState*					CreateSamplerState(const ZEGRSamplerState& SamplerState);
		ID3D11RasterizerState*				CreateRasterizerState(const ZEGRRasterizerState& RasterizerState);
		ID3D11DepthStencilState*			CreateDepthStencilState(const ZEGRDepthStencilState& DepthStencilState);
		ID3D11InputLayout*					CreateVertexLayout(const ZEGRVertexLayout& VertexLayout, ZEGRShader* Shader);

		ZED11StateBase*						FindPoolEntry(ZEList<ZED11StateBase>& Pool, const ZEGRState& State);

											ZED11StatePool();
		virtual								~ZED11StatePool();

	public:
		virtual void						ClearStates();

		ZED11BlendState*					GetBlendState(const ZEGRBlendState& BlendState);
		ZED11SamplerState*					GetSamplerState(const ZEGRSamplerState& SamplerState);
		ZED11RasterizerState*				GetRasterizerState(const ZEGRRasterizerState& RasterizerState);
		ZED11DepthStencilState*				GetDepthStencilState(const ZEGRDepthStencilState& DepthStencilState);
		ZED11VertexLayout*					GetVertexLayout(const ZEGRVertexLayout& VertexLayout, ZEGRShader* VertexShader);
};
