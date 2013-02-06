//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxUtils.h
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
#ifndef __ZE3DS_MAX_UTILS_H__
#define __ZE3DS_MAX_UTILS_H__

#include "ZETypes.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEML/ZEMLNode.h"
#include "ZEML/ZEMLProperty.h"
#include "ZEPacking.h"

#include <Max.h>
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <IGame/IGame.h>
#include <IGame/IGameModifier.h>

#define ZE_EXFL_MAX_NAME_SIZE			128
#define ZE_EXFL_MAX_FILENAME_SIZE		256

#define ZE_MTMP_SKINTRANSFORM					1
#define ZE_MTMP_DIFFUSEMAP						2
#define ZE_MTMP_NORMALMAP						4
#define ZE_MTMP_SPECULARMAP						8
#define ZE_MTMP_EMISSIVEMAP						16
#define ZE_MTMP_OPACITYMAP						32
#define ZE_MTMP_DETAILDIFFUSEMAP				64
#define ZE_MTMP_DETAILNORMALMAP					128
#define ZE_MTMP_ENVIRONMENTMAP					256
#define ZE_MTMP_REFRACTION						512
#define ZE_MTMP_LIGHTMAP						1024
#define ZE_MTMP_DISTORTIONMAP					2048

enum ZEPropType //Do not change order
{
	ZE_UNKNOWN_PROP			= 0,
	ZE_FLOAT_PROP			= 1,
	ZE_VECTOR3_PROP			= 2,
	ZE_INT_PROP				= 3,
	ZE_STRING_PROP			= 4,
	ZE_VECTOR4_PROP			= 5,
	ZE_BOOL_PROP			= 6
};

class ZE3dsMaxUtils
{

	public:
		static ZEVector2		MaxtoZE(const Point2& Point);
		static ZEVector3		MaxtoZE(const Point3& Point);
		static ZEQuaternion		MaxtoZE(const Quat& Quaternion);

		static Point2			ZEtoMax(const ZEVector2& Vector);
		inline Point3			ZEtoMax(const ZEVector3& Vector);
		inline Quat				ZEtoMax(const ZEQuaternion& Quaternion);

		static bool				GetProperty(IExportEntity* Object, const char* Property, INode*& Value);
		static bool				GetProperty(IExportEntity* Object, const char* Property, IGameScene* Scene, IGameNode*& Value);
		static bool				GetProperty(IExportEntity* Object, ZEPropType Type, const char* Property, bool& Value);
		static bool				GetProperty(IExportEntity* Object, ZEPropType Type, const char* Property, const char*& Value);
		static bool				GetProperty(IExportEntity* Object, ZEPropType Type, const char* Property, ZEInt& Value);
		static bool				GetProperty(IExportEntity* Object, ZEPropType Type, const char* Property, ZEUInt& Value);
		static bool				GetProperty(IExportEntity* Object, ZEPropType Type, const char* Property, float& Value);
		static bool				GetProperty(IExportEntity* Object, ZEPropType Type, const char* Property, ZEMLProperty& Value);

		static void				CalculateLocalBoundingBox(ZEAABBox& BoundingBox, IGameMesh* Mesh);
};

#endif
