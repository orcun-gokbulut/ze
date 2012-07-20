//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEStencilZState.h
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

#ifndef __ZE_STENCIL_Z_STATE_H__
#define __ZE_STENCIL_Z_STATE_H__

enum ZEStencilOperation 
{ 
	ZE_SO_CURRENT		= 0,
	ZE_SO_KEEP			= 1,
	ZE_SO_ZERO			= 2,
	ZE_SO_REPLACE		= 3,
	ZE_SO_INCR_SAT		= 4,
	ZE_SO_DECR_SAT		= 5,
	ZE_SO_INVERT		= 6,
	ZE_SO_INCR			= 7,
	ZE_SO_DECR			= 8 
};

enum ZEComparisonFunction 
{ 
	ZE_CF_CURRENT			= 0,
	ZE_CF_NEVER				= 1,
	ZE_CF_LESS				= 2,
	ZE_CF_EQUAL				= 3,
	ZE_CF_LESS_EQUAL		= 4,
	ZE_CF_GREATER			= 5,
	ZE_CF_NOT_EQUAL			= 6,
	ZE_CF_GREATER_EQUAL		= 7,
	ZE_CF_ALWAYS			= 8 
};

class ZEStencilZState
{
protected:
	bool						ZTestEnable;
	bool						StencilTestEnable;
	bool						ZWriteEnable;
	unsigned char				StencilWriteMask;			// ZE TYPINA GECCEK
	ZEComparisonFunction		ZFunction;

	ZEStencilOperation			FrontStencilFailOperation;
	ZEStencilOperation			FrontZFailOperation;
	ZEStencilOperation			FrontStencilPassOperation;
	ZEComparisonFunction		FrontStencilFunction;

	ZEStencilOperation			BackStencilFailOperation;
	ZEStencilOperation			BackZFailOperation;
	ZEStencilOperation			BackStencilPassOperation;
	ZEComparisonFunction		BackStencilFunction;

	bool						Changed;

public:
	void						SetZTestEnable(bool Enable);
	bool						GetZTestEnable() const;
	void						SetStencilTestEnable(bool Enable);
	bool						GetStencilTestEnable() const;
	void						SetZWriteEnable(bool Enable);
	bool						GetZWriteEnable() const;
	void						SetStencilWriteMask(unsigned char Mask);
	unsigned char				GetStencilWriteMask() const;
	void						SetZFunction(ZEComparisonFunction Function);
	ZEComparisonFunction		GetZFunction() const;
	void						SetFrontStencilFail(ZEStencilOperation Operation);
	ZEStencilOperation			GetFrontStencilFail() const;
	void						SetFrontZFail(ZEStencilOperation Operation);
	ZEStencilOperation			GetFrontZFail() const;
	void						SetFrontStencilPass(ZEStencilOperation Operation);
	ZEStencilOperation			GetFrontStencilPass() const;
	void						SetFrontStencilFunction(ZEComparisonFunction Function);
	ZEComparisonFunction		GetFrontStencilFunction() const;
	void						SetBackStencilFail(ZEStencilOperation Operation);
	ZEStencilOperation			GetBackStencilFail() const;
	void						SetBackZFail(ZEStencilOperation Operation);
	ZEStencilOperation			GetBackZFail() const;
	void						SetBackStencilPass(ZEStencilOperation Operation);
	ZEStencilOperation			GetBackStencilPass() const;
	void						SetBackStencilFunction(ZEComparisonFunction Function);		
	ZEComparisonFunction		GetBackStencilFunction() const;
	void						SetChanged(bool Change);
	bool						GetChanged() const;

	const ZEStencilZState&		operator=(const ZEStencilZState& State);

								ZEStencilZState();
	virtual						~ZEStencilZState();
};
#endif
