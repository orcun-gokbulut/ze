//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLCommon.h
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
#ifndef __ZEML_COMMON_H__
#define __ZEML_COMMON_H__

#define ZEML_ITEM_FILE_IDENTIFIER	'Z'
#define ZEML_MAX_NAME_SIZE			256

enum ZEMLItemType
{
	ZEML_IT_UNDEFINED		= 0,
	ZEML_IT_FLOAT			= 1,
	ZEML_IT_DOUBLE			= 2,
	ZEML_IT_INT8			= 3,
	ZEML_IT_INT16			= 4,
	ZEML_IT_INT32			= 5,
	ZEML_IT_INT64			= 6,
	ZEML_IT_UINT8			= 7,
	ZEML_IT_UINT16			= 8,
	ZEML_IT_UINT32			= 9,
	ZEML_IT_UINT64			= 10,
	ZEML_IT_BOOLEAN			= 11,
	ZEML_IT_STRING			= 12,
	ZEML_IT_QUATERNION		= 13,
	ZEML_IT_VECTOR2			= 14,
	ZEML_IT_VECTOR3			= 15,
	ZEML_IT_VECTOR4			= 16,
	ZEML_IT_MATRIX3X3		= 17,
	ZEML_IT_MATRIX4X4		= 18,
	ZEML_IT_INLINE_DATA		= 19,
	ZEML_IT_OFFSET_DATA		= 20,
	ZEML_IT_NODE			= 21
};

#endif
