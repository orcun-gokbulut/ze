//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMethod.h
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
#include "ZEDS/ZEType.h"

class ZEClass;
struct ZEMetaAttribute;

enum ZEMetaOperatorType
{
	ZE_MOT_UNDEFINED,
	ZE_MOT_PLUS,
	ZE_MOT_PLUSEQUAL,
	ZE_MOT_PLUSPLUS,
	ZE_MOT_MINUS,
	ZE_MOT_MINUSEQUAL,
	ZE_MOT_MINUSMINUS,
	ZE_MOT_STAR,
	ZE_MOT_STAREQUAL,
	ZE_MOT_SLASH,
	ZE_MOT_SLASHEQUAL,
	ZE_MOT_PERCENT,
	ZE_MOT_PERCENTEQUAL,
	ZE_MOT_AMP,
	ZE_MOT_AMPEQUAL,
	ZE_MOT_AMPAMP,
	ZE_MOT_PIPE,
	ZE_MOT_PIPEEQUAL,
	ZE_MOT_PIPEPIPE,
	ZE_MOT_CARET,
	ZE_MOT_CARETEQUAL,
	ZE_MOT_EQUAL,
	ZE_MOT_EQUALEQUAL,
	ZE_MOT_EXCLAIMEQUAL,
	ZE_MOT_LESS,
	ZE_MOT_LESSEQUAL,
	ZE_MOT_LESSLESS,
	ZE_MOT_LESSLESSEQUAL,
	ZE_MOT_GREATER,
	ZE_MOT_GREATEREQUAL,
	ZE_MOT_GREATERGREATER,
	ZE_MOT_GREATERGREATEREQUAL,
	ZE_MOT_CALL,
	ZE_MOT_SUBSCRIPT
};

struct ZEMethodParameter
{
	const char*			Name;
	ZEType				Type;
};

struct ZEMethod
{
	ZESize				Id;
	const char*			Name;
	ZEUInt32			Hash;

	ZEClass*			MemberOf;

	void*				MethodPtr;

	bool				IsEvent;
	bool				IsVirtual;
	bool				IsStatic;
	bool				IsOperator;

	ZEMetaOperatorType	OperatorType;
	ZEType				ReturnType;
	ZEMethodParameter*	Parameters;
	ZESize				ParameterCount;

	ZEMetaAttribute*	Attributes;
	ZESize				AttributeCount;
};
