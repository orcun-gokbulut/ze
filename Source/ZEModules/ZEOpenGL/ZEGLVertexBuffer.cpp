//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGLVertexBuffer.cpp
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

#include "ZEGLVertexBuffer.h"

void ZEGLStaticVertexBuffer::PrepareForDrawCall()
{
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	ZEVertexElement E;
	for(int i = 0; i < VertexElements.GetCount(); i++)
	{
		E = VertexElements[i];
		GLuint VertexDataIndex;
		GLuint DataAmount;
		GLenum DataType;
		switch(E.Semantic)
		{
			case ZE_VES_POSITION : 
				VertexDataIndex = ZE_GISO_POSITION;
				glEnableVertexAttribArray(ZE_GISO_POSITION);
				break;
			case ZE_VES_NORMAL :
				VertexDataIndex = ZE_GISO_NORMAL;
				glEnableVertexAttribArray(ZE_GISO_NORMAL);
				break;
			case ZE_VES_BINORMAL : 
				VertexDataIndex = ZE_GISO_BINORMAL;
				glEnableVertexAttribArray(ZE_GISO_BINORMAL);
				break;
			case ZE_VES_TANGENT : 
				VertexDataIndex = ZE_GISO_TANGENT;
				glEnableVertexAttribArray(ZE_GISO_TANGENT);
				break;
			case ZE_VES_TEXTCOORD : 
				VertexDataIndex = ZE_GISO_TEXCOORD0 + E.Index;
				glEnableVertexAttribArray(ZE_GISO_TEXCOORD0 + E.Index);
				break;
			case ZE_VES_COLOR : 
				VertexDataIndex = ZE_GISO_COLOR0 + E.Index;
				glEnableVertexAttribArray(ZE_GISO_COLOR0 + E.Index);
				break;
			case ZE_VES_BLENDINDEX : 
				VertexDataIndex = ZE_GISO_BLEND_INDICIES;
				glEnableVertexAttribArray(ZE_GISO_BLEND_INDICIES);
				break;
			case ZE_VES_BLENDWEIGHT : 
				VertexDataIndex = ZE_GISO_BLENDWEIGHT;
				glEnableVertexAttribArray(ZE_GISO_BLENDWEIGHT);
				break;
		}
		// Type Conversion
		switch (E.Type)
		{
			case ZE_VET_FLOAT:
				DataAmount = 1;
				DataType = GL_FLOAT;
				break;
			case ZE_VET_FLOAT2:
				DataAmount = 2;
				DataType = GL_FLOAT;
				break;
			case ZE_VET_FLOAT3:
				DataAmount = 3;
				DataType = GL_FLOAT;
				break;
			case ZE_VET_FLOAT4:
				DataAmount = 4;
				DataType = GL_FLOAT;
				break;
			case ZE_VET_SHORT2:
				DataAmount = 2;
				DataType = GL_SHORT;
				break;
			case ZE_VET_SHORT4:
				DataAmount = 4;
				DataType = GL_SHORT;
				break;
			case ZE_VET_BYTE4:
				DataAmount = 4;
				DataType = GL_BYTE;
				break;
			case ZE_VET_COLOR:
				DataAmount = 4;
				DataType = GL_UNSIGNED_BYTE;
				break;
		}
		glVertexAttribPointer(VertexDataIndex, DataAmount, DataType, false, GetVertexSize(), (void*)&E.Offset);
	}
	// Now Server is Draw Call ready.

}

void ZEGLStaticVertexBuffer::RestoreAttribArrays()
{
	ZEVertexElement E;
	for(ZEUInt i = 0; i < VertexElements.GetCount(); i++)
	{
		E = VertexElements[i];
		switch(E.Semantic)
		{
			case ZE_VES_POSITION : 
				glDisableVertexAttribArray(ZE_GISO_POSITION);
				break;
			case ZE_VES_NORMAL :
				glDisableVertexAttribArray(ZE_GISO_NORMAL);
				break;
			case ZE_VES_BINORMAL : 
				glDisableVertexAttribArray(ZE_GISO_BINORMAL);
				break;
			case ZE_VES_TANGENT : 
				glDisableVertexAttribArray(ZE_GISO_TANGENT);
				break;
			case ZE_VES_TEXTCOORD : 
				glDisableVertexAttribArray(ZE_GISO_TEXCOORD0 + E.Index);
				break;
			case ZE_VES_COLOR : 
				glDisableVertexAttribArray(ZE_GISO_COLOR0 + E.Index);
				break;
			case ZE_VES_BLENDINDEX : 
				glDisableVertexAttribArray(ZE_GISO_BLEND_INDICIES);
				break;
			case ZE_VES_BLENDWEIGHT : 
				glDisableVertexAttribArray(ZE_GISO_BLENDWEIGHT);
				break;
		}
	}
	// Now Unnecessary Input Assembly will not happen in next draw call

}
