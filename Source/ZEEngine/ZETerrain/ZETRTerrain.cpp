//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETRTerrain.cpp
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

#include "ZETRTerrain.h"
#include "ZETRLayer.h"

#include "ZEGame\ZEDrawParameters.h"
#include "ZERenderer\ZECamera.h"
#include "ZERenderer\ZESimpleMaterial.h"
#include "ZERenderer\ZETerrainMaterial.h"
#include "ZEError.h"

bool ZETRTerrain::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	Drawer.SetTerrain(this);
	if (!Drawer.Initialize())
	{
		zeError("Can not initialize.");
		return false;
	}

	if (ElevationLayer != NULL)
	{
		if (!ElevationLayer->Initialize())
		{
			zeError("Cannot initialize terrain elevation layer. Layer Name: \"%s\".", ElevationLayer->GetName().ToCString());
			return false;
		}
	}

	if (ColorLayer != NULL)
	{
		if (!ColorLayer->Initialize())
		{
			zeError("Cannot initialize terrain color layer. Layer Name: \"%s\".", ColorLayer->GetName().ToCString());
			return false;
		}
	}
	
	for (ZESize I = 0; I < ExtraLayers.GetCount(); I++)
	{
		if (!ExtraLayers[I]->Initialize())
		{
			zeError("Cannot initialize terrain layer. Layer Name: \"%s\".", ExtraLayers[I]->GetName().ToCString());
			return false;
		}
	}

	return true;
}

bool ZETRTerrain::DeinitializeSelf()
{
	Drawer.Deinitialize();
	return false;
}

ZETRTerrain::ZETRTerrain()
{
	BlockSize = 1024;
	ElevationLayer = NULL;
	ColorLayer = NULL;
}

ZETRTerrain::~ZETRTerrain()
{
	Deinitialize();
	for (ZESize I = 0; I < ExtraLayers.GetCount(); I++)
		delete ExtraLayers[I];
}

ZEDrawFlags ZETRTerrain::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

ZETRDrawer& ZETRTerrain::GetDrawer()
{
	return Drawer;
}

const ZEArray<ZETRLayer*>& ZETRTerrain::GetExtraLayers()
{
	return ExtraLayers;
}

bool ZETRTerrain::AddExtraLayer(ZETRLayer* Layer)
{
	if (Layer == NULL)
		return false;

	if (ExtraLayers.Exists(Layer))
	{
		zeError("Layer is already registered.");
		return false;
	}

	ExtraLayers.Add(Layer);
	Layer->Terrain = this;

	if (IsInitialized())
	{
		if (!Layer->Initialize())
		{
			zeError("Cannot initialize terrain layer. Layer Name: \"%s\".", Layer->GetName().ToCString());
			return false;
		}
	}

	return true;
}

void ZETRTerrain::RemoveExtraLayer(ZETRLayer* Layer)
{
	if (Layer == NULL)
		return;

	if (!ExtraLayers.Exists(Layer))
	{
		zeError("Cannot remove layer that is not registered.");
		return;
	}

	if (IsInitialized())
		Layer->Deinitialize();

	Layer->Terrain = NULL;

	ExtraLayers.RemoveValue(Layer);
}

bool ZETRTerrain::SetElevationLayer(ZETRLayer* Layer)
{
	if (ElevationLayer == Layer)
		return true;

	if (ElevationLayer != NULL)
		ElevationLayer->Deinitialize();

	ElevationLayer = Layer;
	ElevationLayer->Terrain = this;

	if (IsInitialized())
		ElevationLayer->Initialize();

	return true;
}

ZETRLayer* ZETRTerrain::GetElevationLayer()
{
	return ElevationLayer;
}

bool ZETRTerrain::SetColorLayer(ZETRLayer* Layer)
{
	if (ColorLayer == Layer)
		return true;

	if (ColorLayer != NULL)
		ColorLayer->Deinitialize();

	ColorLayer = Layer;
	ColorLayer->Terrain = this;

	if (IsInitialized())
		ColorLayer->Initialize();

	return true;
}
ZETRLayer* ZETRTerrain::GetColorLayer()
{
	return ColorLayer;
}

void ZETRTerrain::SetBlockSize(ZESize Size)
{
	BlockSize = Size;
}

ZESize ZETRTerrain::GetBlockSize()
{
	return BlockSize;
}

void ZETRTerrain::SetPrimitiveSize(ZEUInt Size)
{
	Drawer.SetPrimitiveSize(Size);
}

ZEUInt ZETRTerrain::GetPrimitiveSize()
{
	return Drawer.GetPrimitiveSize();
}

void ZETRTerrain::Draw(ZEDrawParameters* DrawParameters)
{
	static ZEVector3 ViewPosition;

	if (GetEnabled())
	{
		ViewPosition = DrawParameters->View->Camera->GetWorldPosition();

		if (ElevationLayer != NULL && ElevationLayer->GetEnabled())
		{
			ElevationLayer->SetViewPosition(ViewPosition);
			ElevationLayer->Process();
		}

		if (ColorLayer != NULL && ColorLayer->GetEnabled())
		{
			ColorLayer->SetViewPosition(ViewPosition);
			ColorLayer->Process();
		}

		for (ZESize I = 0; I < ExtraLayers.GetCount(); I++)
		{
			ZETRLayer* CurrentLayer = ExtraLayers[I];

			if (!CurrentLayer->GetEnabled())
				continue;

			CurrentLayer->SetViewPosition(ViewPosition);
			CurrentLayer->Process();
		}
	}

	Drawer.SetViewPosition(ViewPosition);
	Drawer.Draw(DrawParameters);
}

ZETRTerrain* ZETRTerrain::CreateInstance()
{
	return new ZETRTerrain();
}
