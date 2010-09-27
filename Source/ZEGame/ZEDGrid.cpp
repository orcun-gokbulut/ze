//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDGrid.cpp
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

#include "ZEDGrid.h"
#include "ZEScene.h"
#include "ZEGraphics/ZESimpleMaterial.h"

void ZEDGrid::GenerateGrid()
{
	Grid->Canvas.Clean();
	this->Grid->PrimitiveType = ZE_ROPT_LINE;

	int DrawStartPoint = -GridSize/2;

	for (int I = 0; I <= GridSize/GridSpacing; I++)
	{
		Grid->Canvas.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));

		if( I % MajorLineSpacing == 0)
			Grid->Canvas.SetColor(ZEVector4(0.0f, 0.3f, 1.0f, 1.0f));

		if( I == GridSize / GridSpacing / 2)
			Grid->Canvas.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));

		Grid->Canvas.AddLine(ZEVector3(DrawStartPoint, 0, DrawStartPoint + GridSpacing * I), ZEVector3(-DrawStartPoint, 0, DrawStartPoint + GridSpacing * I));
		Grid->Canvas.AddLine(ZEVector3(DrawStartPoint + GridSpacing * I, 0, -DrawStartPoint), ZEVector3(DrawStartPoint + GridSpacing * I, 0, DrawStartPoint));
	}

	Grid->UpdateCanvas();
}

int ZEDGrid::GetGridSpacing() const
{
	return GridSpacing;
}

void ZEDGrid::SetGridSpacing(int Spacing)
{
	this->GridSpacing = Spacing;
	GenerateGrid();
}

int ZEDGrid::GetMajorLineSpacing() const
{
	return MajorLineSpacing;
}

void ZEDGrid::SetMajorLineSpacing(int MajorLineSpacing)
{
	this->MajorLineSpacing = MajorLineSpacing;
	GenerateGrid();
}

int ZEDGrid::GetGridSize() const
{
	return GridSize;
}

void ZEDGrid::SetGridSize(int GridSize)
{
	this->GridSize = GridSize;
	GenerateGrid();
}

bool ZEDGrid::GetVisible()
{
	return Grid->GetVisible();
}

void ZEDGrid::SetVisible(bool Visibility)
{
	Grid->SetVisible(Visibility);
}

ZEDGrid::ZEDGrid(ZEScene* Scene, int Spacing, int MajorLineSpacing, int GridSize)
{
	this->GridSpacing = Spacing;
	this->MajorLineSpacing = MajorLineSpacing;
	this->GridSize = GridSize;
	this->Scene = Scene;
	this->Grid = new ZECanvasBrush();
	this->Grid->PrimitiveType = ZE_ROPT_LINE;
	Scene->AddEntity(this->Grid);
	Grid->SetName(".:SELECTION__ITEM:.");
	ZESimpleMaterial* Material = ZESimpleMaterial::CreateInstance();
	Grid->Canvas.SetColor(ZEVector4(1, 1, 1, 1));
	Grid->Material = Material;
	Material->UpdateMaterial();
	Grid->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	GenerateGrid();
}
