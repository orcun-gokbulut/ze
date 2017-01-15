//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFilterCommon.cpp
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

#include "ZERNFilterCommon.h"

const ZEMatrix4x4 ZERNFilterColorMatrix::Normal = ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
													  0.0f, 1.0f, 0.0f, 0.0f,
													  0.0f, 0.0f, 1.0f, 0.0f,
													  0.0f, 0.0f, 0.0f, 1.0f);


const ZEMatrix4x4 ZERNFilterColorMatrix::Inverse = ZEMatrix4x4(-1.0f,  0.0f,  0.0f, 1.0f,
													   0.0f, -1.0f,  0.0f, 1.0f,
													   0.0f,  0.0f, -1.0f, 1.0f,
													   0.0f,  0.0f,  0.0f, 1.0f);

const ZEMatrix4x4 ZERNFilterColorMatrix::NightVision = ZEMatrix4x4(0.0f, 0.000f, 0.0f, 0.0f,
														   0.0f, 1.500f, 0.0f, 0.0f,
														   0.0f, 0.000f, 0.0f, 0.0f,
														   0.0f, 0.000f, 0.0f, 1.0f);

const ZEMatrix4x4 ZERNFilterColorMatrix::Sepia = ZEMatrix4x4(0.393f, 0.769f, 0.198f, 0.0f,
													 0.349f, 0.686f, 0.168f, 0.0f,
													 0.272f, 0.534f, 0.131f, 0.0f,
													 0.000f, 0.000f, 0.000f, 1.0f);

const ZEMatrix4x4 ZERNFilterColorMatrix::GrayScale = ZEMatrix4x4(0.212671f, 0.715160f, 0.072169f, 0.0f,
														  0.212671f, 0.715160f, 0.072169f, 0.0f,
														  0.212671f, 0.715160f, 0.072169f, 0.0f,
														  0.0f, 0.0f, 0.0f, 1.0f);

const ZEMatrix4x4 ZERNFilterColorMatrix::Red = ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
												   0.0f, 0.0f, 0.0f, 0.0f,
												   0.0f, 0.0f, 0.0f, 0.0f,
												   0.0f, 0.0f, 0.0f, 1.0f);

const ZEMatrix4x4 ZERNFilterColorMatrix::Green = ZEMatrix4x4(0.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 1.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 1.0f);

const ZEMatrix4x4 ZERNFilterColorMatrix::Blue = ZEMatrix4x4(0.0f, 0.0f, 0.0f, 0.0f,
													0.0f, 0.0f, 0.0f, 0.0f,
													0.0f, 0.0f, 1.0f, 0.0f,
													0.0f, 0.0f, 0.0f, 1.0f);

const ZEMatrix4x4 ZERNFilterColorMatrix::Black = ZEMatrix4x4(0.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 1.0f);

const ZEMatrix4x4 ZERNFilterColorMatrix::White = ZEMatrix4x4(1.0f, 0.0f, 0.0f, 1.0f,
													 0.0f, 1.0f, 0.0f, 1.0f,
													 0.0f, 0.0f, 1.0f, 1.0f,
													 0.0f, 0.0f, 0.0f, 1.0f);
