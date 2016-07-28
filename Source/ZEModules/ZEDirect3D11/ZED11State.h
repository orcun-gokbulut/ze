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

#include "ZEPointer/ZEReferenceCounted.h"

#include "ZETypes.h"
#include "ZEDS/ZEList2.h"
#include "ZEGraphics/ZEGRBlendState.h"
#include "ZEGraphics/ZEGRVertexLayout.h"
#include "ZEGraphics/ZEGRRasterizerState.h"
#include "ZEGraphics/ZEGRDepthStencilState.h"

struct IUnknown;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;

class ZED11StateBase : public ZEReferenceCounted
{
	protected:
									ZED11StateBase();
		virtual						~ZED11StateBase();

	public:
		virtual IUnknown*			GetInterfaceBase() const = 0;
		virtual const ZEGRState&	GetStateBase() const = 0;
};

template<typename _ZEGRState, typename _IInterface>
class ZED11State : public ZED11StateBase
{
	friend class ZED11StatePool;
	private:
		ZELink<ZED11StateBase>*		Link;
		const ZED11StatePool*		Owner;

		_IInterface*				Interface;
		_ZEGRState					State;

	public:
		virtual IUnknown*			GetInterfaceBase() const;
		virtual const ZEGRState&	GetStateBase() const;
		_IInterface*				GetInterface() const;
		const _ZEGRState&			GetState() const;

									ZED11State();
		virtual						~ZED11State();
};

typedef ZED11State<ZEGRBlendState, ID3D11BlendState> ZED11BlendState;
typedef ZED11State<ZEGRDepthStencilState, ID3D11DepthStencilState> ZED11DepthStencilState;
typedef ZED11State<ZEGRRasterizerState, ID3D11RasterizerState> ZED11RasterizerState;
typedef ZED11State<ZEGRVertexLayout,ID3D11InputLayout> ZED11VertexLayout;

template<typename _ZEGRState, typename _IInterface>
IUnknown* ZED11State<_ZEGRState, _IInterface>::GetInterfaceBase() const
{
	return Interface;
}

template<typename _ZEGRState, typename _IInterface>
const ZEGRState& ZED11State<_ZEGRState, _IInterface>::GetStateBase() const
{
	return State;
}

template<typename _ZEGRState, typename _IInterface>
_IInterface* ZED11State<_ZEGRState, _IInterface>::GetInterface() const
{
	return Interface;
}

template<typename _ZEGRState, typename _IInterface>
const _ZEGRState& ZED11State<_ZEGRState, _IInterface>::GetState() const
{
	return State;
}

template<typename _ZEGRState, typename _IInterface>
ZED11State<_ZEGRState, _IInterface>::ZED11State()
{
	Interface = NULL;
	Link = new ZELink<ZED11StateBase>(this);
}

template<typename _ZEGRState, typename _IInterface>
ZED11State<_ZEGRState, _IInterface>::~ZED11State() 
{
	if (Link->GetInUse())
	{
		ZEGRStateType StateType = this->GetState().GetStateType();

		switch (StateType)
		{
			case ZEGRStateType::ZEGR_ST_VERTEX_LAYOUT:
				Owner->VertexLayoutPool.Remove(Link);
				break;

			case ZEGRStateType::ZEGR_ST_RASTERIZER:
				Owner->RasterizerStatePool.Remove(Link);
				break;

			case ZEGRStateType::ZEGR_ST_DEPTH_STENCIL:
				Owner->DepthStencilStatePool.Remove(Link);
				break;

			case ZEGRStateType::ZEGR_ST_BLEND:
				Owner->BlendStatePool.Remove(Link);
				break;

			default:
				break;
		}
	}

	delete Link;
	Owner = NULL;
	ZEGR_RELEASE(Interface);
}
