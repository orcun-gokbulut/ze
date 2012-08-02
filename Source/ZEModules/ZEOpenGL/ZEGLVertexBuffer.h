//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGLVertexBuffer.h
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

#ifndef __ZE_GL_VERTEX_BUFFER_H__
#define __ZE_GL_VERTEX_BUFFER_H__


#include "ZETypes.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGL.h"

// 1 to 1 CG Order
enum ZEGLInputSemanticOrder
{
	ZE_GISO_POSITION			= 0,
	ZE_GISO_BLENDWEIGHT		= 1,
	ZE_GISO_NORMAL			= 2,
	ZE_GISO_COLOR0			= 3,	ZE_GISO_DIFFUSE		= 3,
	ZE_GISO_COLOR1			= 4,	ZE_GISO_SPECULAR	= 4,
	// Not Supported		= 5
	// Not Supported		= 6
	ZE_GISO_BLEND_INDICIES	= 7,
	ZE_GISO_TEXCOORD0		= 8,
	ZE_GISO_TEXCOORD1		= 9,
	ZE_GISO_TEXCOORD2		= 10,
	ZE_GISO_TEXCOORD3		= 11,
	ZE_GISO_TEXCOORD4		= 12,
	ZE_GISO_TEXCOORD5		= 13,
	ZE_GISO_TEXCOORD6		= 14,	ZE_GISO_TANGENT		= 14,
	ZE_GISO_TEXCOORD7		= 15,	ZE_GISO_BINORMAL	= 15
};


class ZEGLStaticVertexBuffer : public ZEVertexBuffer
{
private:

protected:

public:
	GLVertexBuffer			VertexBuffer;

	void					PrepareForDrawCall();
	void					RestoreAttribArrays();

};
#endif
