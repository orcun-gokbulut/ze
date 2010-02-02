//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GUIRenderer.cpp
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

#include "GUIRenderer.h"
#include "Core/Core.h"

void ZEGUIRenderer::DrawImage(const ZEGUIImage* Image)
{
	for (int I = 0; I < Images.GetCount(); I++)
	{
		ZEGUIRenderList* GUIRenderList = &Images[I];

		if (GUIRenderList->Texture == Image->Texture)
		{
			ZEArray<ZEGUIVertex>* Vertices = &GUIRenderList->Vertices;
			size_t VerticesCount = Vertices->GetCount();
			Vertices->Resize(VerticesCount + 6);

			(*Vertices)[VerticesCount].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
			(*Vertices)[VerticesCount].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
			(*Vertices)[VerticesCount + 1].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_LEFTUP);
			(*Vertices)[VerticesCount + 1].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_LEFTUP);
			(*Vertices)[VerticesCount + 2].Position	  = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_RIGHTUP);
			(*Vertices)[VerticesCount + 2].Texcoord	  = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_RIGHTUP);
			(*Vertices)[VerticesCount + 3].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_RIGHTDOWN);
			(*Vertices)[VerticesCount + 3].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_RIGHTDOWN);
			(*Vertices)[VerticesCount + 4].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
			(*Vertices)[VerticesCount + 4].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
			(*Vertices)[VerticesCount + 5].Position	  = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_RIGHTUP);
			(*Vertices)[VerticesCount + 5].Texcoord	  = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_RIGHTUP);

			return;
		}
	}

	ZEGUIRenderList* NewGUIRenderList = Images.Add();
	ZEArray<ZEGUIVertex>* Vertices = &NewGUIRenderList->Vertices;
	size_t VerticesCount = Vertices->GetCount();
	Vertices->Resize(VerticesCount + 6);

	(*Vertices)[VerticesCount].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
	(*Vertices)[VerticesCount].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
	(*Vertices)[VerticesCount + 1].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_LEFTUP);
	(*Vertices)[VerticesCount + 1].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_LEFTUP);
	(*Vertices)[VerticesCount + 2].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_RIGHTUP);
	(*Vertices)[VerticesCount + 2].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_RIGHTUP);
	(*Vertices)[VerticesCount + 3].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_RIGHTDOWN);
	(*Vertices)[VerticesCount + 3].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_RIGHTDOWN);
	(*Vertices)[VerticesCount + 4].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
	(*Vertices)[VerticesCount + 4].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
	(*Vertices)[VerticesCount + 5].Position = Image->PositionRectangle.GetCorner(ZERECTANGLECORNER_RIGHTUP);
	(*Vertices)[VerticesCount + 5].Texcoord = Image->TexcoordRectangle.GetCorner(ZERECTANGLECORNER_RIGHTUP);
	NewGUIRenderList->Texture = Image->Texture;
}

void ZEGUIRenderer::Clear()
{
	for (size_t I = 0; I < Images.GetCount(); I++)
		Images[I].Vertices.Clear();
}

void ZEGUIRenderer::Render()
{
	ZERenderList RenderList;
	ZEMatrix4x4::CreateIdentity(RenderList.WorldMatrix);
//	ZEMatrix4x4::CreateViewPortTransform(RenderList.ViewFrustum.ViewProjMatrix, 0.0f, zeGraphics->GetScreenWidth(), zeGraphics->GetScreenHeight(), 0.0f, 0.0f, 1.0f);
	ZEFixedMaterial* Material = new ZEFixedMaterial();
	RenderList.Material = Material;
	Material->SetZero();
	Material->SetShaderComponents(ZESHADER_DIFFUSEMAP);
	Material->AmbientColor = ZEVector3(1.0f, 1.0f, 1.0f);

	for (size_t I = 0; I < Images.GetCount(); I++)
	{
		//RenderList.SetVertexBuffer(Images[I].Vertices.GetCArray(), Images[I].Vertices.GetCount(), ZE_VT_GUIVERTEX);
//		Material->DiffuseMap = Images[I].Texture;
		//zeGraphics->DrawRenderList(&RenderList);
	}
}
