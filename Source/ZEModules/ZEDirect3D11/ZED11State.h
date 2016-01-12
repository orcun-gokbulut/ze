//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11State.h
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
#include "ZEDS/ZEList.h"
#include "ZEGraphics/ZEGRBlendState.h"
#include "ZEGraphics/ZEGRSamplerState.h"
#include "ZEGraphics/ZEGRDepthStencilState.h"
#include "ZEGraphics/ZEGRRasterizerState.h"
#include "ZEGraphics/ZEGRVertexLayout.h"

struct IUnknown;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;

class ZED11StateBase : public ZEListItem, public ZEGRResource
{
	friend class ZED11RenderStateData;
	private:
		ZESize						ReferenceCount;

		void						AddRef();
		void						Release();

	protected:
									ZED11StateBase();
									~ZED11StateBase();

	public:
		virtual ZEGRResourceType	GetResourceType();
		ZESize						GetReferenceCount();
		virtual IUnknown*			GetInterfaceBase() = 0;
		virtual const ZEGRState&	GetStateBase() = 0;
};

template<typename _ZEGRState, typename _IInterface>
class ZED11State : public ZED11StateBase
{
	friend class ZED11StatePool;
	private:
		_IInterface*				Interface;
		_ZEGRState					State;

	public:
		virtual IUnknown*			GetInterfaceBase();
		virtual const ZEGRState&	GetStateBase();
		_IInterface*				GetInterface();
		const _ZEGRState&			GetState();

									ZED11State();
									~ZED11State();
};

typedef ZED11State<ZEGRBlendState, ID3D11BlendState> ZED11BlendState;
typedef ZED11State<ZEGRSamplerState, ID3D11SamplerState> ZED11SamplerState;
typedef ZED11State<ZEGRDepthStencilState, ID3D11DepthStencilState> ZED11DepthStencilState;
typedef ZED11State<ZEGRRasterizerState, ID3D11RasterizerState> ZED11RasterizerState;
typedef ZED11State<ZEGRVertexLayout,ID3D11InputLayout> ZED11VertexLayout;

template<typename _ZEGRState, typename _IInterface>
IUnknown* ZED11State<_ZEGRState, _IInterface>::GetInterfaceBase() 
{
	return Interface;
}

template<typename _ZEGRState, typename _IInterface>
const ZEGRState& ZED11State<_ZEGRState, _IInterface>::GetStateBase()
{
	return State;
}

template<typename _ZEGRState, typename _IInterface>
_IInterface* ZED11State<_ZEGRState, _IInterface>::GetInterface() 
{
	return Interface;
}

template<typename _ZEGRState, typename _IInterface>
const _ZEGRState& ZED11State<_ZEGRState, _IInterface>::GetState()
{
	return State;
}

template<typename _ZEGRState, typename _IInterface>
ZED11State<_ZEGRState, _IInterface>::ZED11State()
{
	Interface = NULL;
}

template<typename _ZEGRState, typename _IInterface>
ZED11State<_ZEGRState, _IInterface>::~ZED11State() 
{
	if (Interface != NULL) 
		Interface->Release();
}
