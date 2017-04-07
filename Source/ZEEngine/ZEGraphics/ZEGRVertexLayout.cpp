//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRVertexLayout.cpp
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

#include "ZEGRVertexLayout.h"

#include "ZEError.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEFile/ZEFile.h"
#include "ZEGRShader.h"
#include "ZEGRBuffer.h"
#include "ZEGRGraphicsModule.h"
#include "ZEGRShaderMeta.h"

ZEGRStateType ZEGRVertexLayout::GetStateType() const
{
	return ZEGRStateType::ZEGR_ST_VERTEX_LAYOUT;
}

const void* ZEGRVertexLayout::GetData() const
{
	return &StateData;
}

ZESize ZEGRVertexLayout::GetDataSize() const
{
	return sizeof(StateData);
}

bool ZEGRVertexLayout::SetElements(const ZEGRVertexElement* VertexElements, ZEUInt ElementCount)
{
	zeCheckError(VertexElements == NULL && ElementCount != 0, false, "VertexElements is NULL but ElementCount is not zero.");
	zeCheckError(ElementCount >= ZEGR_MAX_VERTEX_LAYOUT_ELEMENT, false, "ElementCount is too much.");

	SetToDefault();

	StateData.ElementCount = ElementCount;
	for (ZESize I = 0; I < ElementCount; ++I)
	{
		StateData.Elements[I].Semantic = VertexElements[I].Semantic;
		StateData.Elements[I].Index = VertexElements[I].Index;
		StateData.Elements[I].Type = VertexElements[I].Type;
		StateData.Elements[I].Stream = VertexElements[I].Stream;
		StateData.Elements[I].Offset = VertexElements[I].Offset;
		StateData.Elements[I].Usage = VertexElements[I].Usage;
		StateData.Elements[I].InstanceCount = VertexElements[I].InstanceCount;
	}

	MarkDirty();

	return true;
}

const ZEGRVertexElement* ZEGRVertexLayout::GetElements() const
{
	return StateData.Elements;
}

ZEUInt ZEGRVertexLayout::GetElementCount() const
{
	return StateData.ElementCount;
}

ZEGRVertexLayout* ZEGRVertexLayout::Generate(ZEGRShaderMeta* MetaTable)
{
	return NULL;
	/*zeDebugCheck(MetaTable == NULL, "NUll pointer.");
	zeDebugCheck(MetaTable->CompileOptions.Type != ZEGR_ST_VERTEX, "Wrong shader type");

	ZESize ShaderInputCount = MetaTable->Inputs.GetCount();
	if (ShaderInputCount == 0)
		return NULL;

	Output.SetToDefault();

	ZESize HashSearchList[ZEGR_MAX_VERTEX_LAYOUT_ELEMENT] = {0};
	
	ZEArray<ZEGRVertexElement> FinalLayout;
	FinalLayout.SetCount(ShaderInputCount);

	// Generate search list of hashes
	for (ZESize I = 0; I < ShaderInputCount; ++I)
		HashSearchList[I] = MetaTable->Inputs[I].Hash;

	// Iterate all buffers in list only once
	ZESize InputN = 0;
	ZESize BufferN = 0;
	ZESize ElementN = 0;
	for (; InputN < ShaderInputCount && BufferN != ZEGR_MAX_VERTEX_BUFFER_SLOT; )
	{
		for (; BufferN < ZEGR_MAX_VERTEX_BUFFER_SLOT; ++BufferN)
		{
			const ZEVertexBuffer* Buffer = BufferArr[BufferN];

			if (Buffer == NULL)
				continue;

			ZESize BufferElementCount = Buffer->GetElementCount();
			const ZEVertexBufferElement* BufferElements = Buffer->GetElements();

			for (; ElementN < BufferElementCount; ++ElementN)
			{
				if (HashSearchList[InputN] != Buffer->Hashes[ElementN])
					continue;
			
				const ZEVertexBufferElement* BufferElement = &BufferElements[ElementN];

				zeDebugCheck(BufferElement->Type != MetaTable->Inputs[InputN].ElementType,
								"Shader input type and registerd vertex buffer element type does not match!");
				
				// Match found
				// Generate layout for the buffer element
				strcpy(FinalLayout[InputN].Semantic, BufferElement->Semantic);
				FinalLayout[InputN].Index = BufferElement->Index;
				FinalLayout[InputN].Type = BufferElement->Type;
				FinalLayout[InputN].Stream = (ZEUInt8)BufferN;
				FinalLayout[InputN].Offset = BufferElement->Offset;
				FinalLayout[InputN].Usage = BufferElement->Usage;
				FinalLayout[InputN].InstanceCount = BufferElement->InstanceCount;

				InputN++;
			}
		}
	}

	if (InputN != ShaderInputCount)
		return false;

	Output.SetElements(FinalLayout.GetConstCArray(), (ZEUInt)ShaderInputCount);
	return true;*/
}


void ZEGRVertexLayout::SetToDefault()
{
	memset(&StateData, 0, sizeof(ZEVertexLayoutData));
}


ZEGRVertexLayout::ZEGRVertexLayout()
{
	SetToDefault();
}
