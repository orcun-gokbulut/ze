//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRState.h
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

enum ZEGRComparisonFunction : ZEUInt8
{ 
	ZEGR_CF_NEVER			= 0,
	ZEGR_CF_LESS			= 1,
	ZEGR_CF_EQUAL			= 2,
	ZEGR_CF_LESS_EQUAL		= 3,
	ZEGR_CF_GREATER			= 4,
	ZEGR_CF_NOT_EQUAL		= 5,
	ZEGR_CF_GREATER_EQUAL	= 6,
	ZEGR_CF_ALWAYS			= 7 
};

enum class ZEGRStateType : ZEUInt8
{
	ZEGR_ST_VERTEX_LAYOUT	= 0,
	ZEGR_ST_RASTERIZER		= 1,
	ZEGR_ST_DEPTH_STENCIL	= 2,
	ZEGR_ST_BLEND			= 3
};

class ZEGRState
{
	private:
		mutable ZEUInt32		Hash;
		bool					Dirty;

	protected:
		void					MarkDirty();

								ZEGRState();
		virtual					~ZEGRState();

	public:
		virtual ZEGRStateType	GetStateType() const = 0;

		bool					IsDirty() const;

		ZEUInt32				GetHash() const;
		virtual const void*		GetData() const = 0;
		virtual ZESize			GetDataSize() const = 0;

		bool					Equals(const ZEGRState& State) const;

		virtual void			Update();

};
