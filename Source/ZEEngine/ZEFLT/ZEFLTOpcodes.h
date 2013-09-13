//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFLTOpcodes.h
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
#ifndef __ZE_FLT_OPCODES_H__
#define __ZE_FLT_OPCODES_H__

enum ZEFLTOpcodes
{
    ZE_FLT_OPCODE_HEADER							= 1,
    ZE_FLT_OPCODE_GROUP								= 2,
    ZE_FLT_OPCODE_OLD_LOD							= 3,
    ZE_FLT_OPCODE_OBJECT							= 4,
    ZE_FLT_OPCODE_FACE								= 5,
    ZE_FLT_OPCODE_OLD_ABSOLUTE_VERTEX				= 7,
    ZE_FLT_OPCODE_OLD_SHADED_VERTEX					= 8,
    ZE_FLT_OPCODE_OLD_NORMAL_VERTEX					= 9,
    ZE_FLT_OPCODE_PUSH_LEVEL						= 10,
    ZE_FLT_OPCODE_POP_LEVEL							= 11,
    ZE_FLT_OPCODE_DOF								= 14,
    ZE_FLT_OPCODE_PUSH_SUBFACE						= 19,
    ZE_FLT_OPCODE_POP_SUBFACE						= 20,
    ZE_FLT_OPCODE_PUSH_EXTENSION					= 21,
    ZE_FLT_OPCODE_POP_EXTENSION						= 22,
    ZE_FLT_OPCODE_CONTINUATION						= 23,
    ZE_FLT_OPCODE_COMMENT							= 31,
    ZE_FLT_OPCODE_COLOR_PALETTE						= 32,
    ZE_FLT_OPCODE_LONG_ID							= 33,
    ZE_FLT_OPCODE_OLD_TRANSLATE						= 40,
    ZE_FLT_OPCODE_OLD_ROTATE_ABOUT_POINT			= 41,
    ZE_FLT_OPCODE_OLD_ROTATE_ABOUT_EDGE				= 42,
    ZE_FLT_OPCODE_OLD_SCALE							= 43,
    ZE_FLT_OPCODE_OLD_TRANSLATE2					= 44,
    ZE_FLT_OPCODE_OLD_NONUNIFORM_SCALE				= 45,
    ZE_FLT_OPCODE_OLD_ROTATE_ABOUT_POINT2			= 46,
    ZE_FLT_OPCODE_OLD_ROTATE_SCALE_TO_POINT			= 47,
    ZE_FLT_OPCODE_OLD_PUT_TRANSFORM					= 48,
    ZE_FLT_OPCODE_MATRIX							= 49,
    ZE_FLT_OPCODE_VECTOR							= 50,
    ZE_FLT_OPCODE_OLD_BOUNDING_BOX					= 51,
    ZE_FLT_OPCODE_MULTITEXTURE						= 52,
    ZE_FLT_OPCODE_UV_LIST							= 53,
    ZE_FLT_OPCODE_BINARY_SEPARATING_PLANE			= 55,
    ZE_FLT_OPCODE_REPLICATE							= 60,
    ZE_FLT_OPCODE_INSTANCE_REFERENCE				= 61,
    ZE_FLT_OPCODE_INSTANCE_DEFINITION				= 62,
    ZE_FLT_OPCODE_EXTERNAL_REFERENCE				= 63,
    ZE_FLT_OPCODE_TEXTURE_PALETTE					= 64,
    ZE_FLT_OPCODE_OLD_EYEPOINT_PALETTE				= 65,
    ZE_FLT_OPCODE_OLD_MATERIAL_PALETTE				= 66,
    ZE_FLT_OPCODE_VERTEX_PALETTE					= 67,
    ZE_FLT_OPCODE_VERTEX_C							= 68,
    ZE_FLT_OPCODE_VERTEX_CN							= 69,
    ZE_FLT_OPCODE_VERTEX_CNT						= 70,
    ZE_FLT_OPCODE_VERTEX_CT							= 71,
    ZE_FLT_OPCODE_VERTEX_LIST						= 72,
    ZE_FLT_OPCODE_LOD								= 73,
    ZE_FLT_OPCODE_BOUNDING_BOX						= 74,
    ZE_FLT_OPCODE_ROTATE_ABOUT_EDGE					= 76,
    ZE_FLT_OPCODE_OLD_SCALE2						= 77,
    ZE_FLT_OPCODE_TRANSLATE							= 78,
    ZE_FLT_OPCODE_SCALE								= 79,
    ZE_FLT_OPCODE_ROTATE_ABOUT_POINT				= 80,
    ZE_FLT_OPCODE_ROTATE_SCALE_TO_POINT				= 81,
    ZE_FLT_OPCODE_PUT								= 82,
    ZE_FLT_OPCODE_EYEPOINT_AND_TRACKPLANE_PALETTE	= 83,
    ZE_FLT_OPCODE_MESH								= 84,
    ZE_FLT_OPCODE_LOCAL_VERTEX_POOL					= 85,
    ZE_FLT_OPCODE_MESH_PRIMITIVE					= 86,
    ZE_FLT_OPCODE_ROAD_SEGMENT						= 87,
    ZE_FLT_OPCODE_ROAD_ZONE							= 88,
    ZE_FLT_OPCODE_MORPH_VERTEX_LIST					= 89,
    ZE_FLT_OPCODE_LINKAGE_PALETTE					= 90,
    ZE_FLT_OPCODE_SOUND								= 91,
    ZE_FLT_OPCODE_ROAD_PATH							= 92,
    ZE_FLT_OPCODE_SOUND_PALETTE						= 93,
    ZE_FLT_OPCODE_GENERAL_MATRIX					= 94,
    ZE_FLT_OPCODE_TEXT								= 95,
    ZE_FLT_OPCODE_SWITCH							= 96,
    ZE_FLT_OPCODE_LINE_STYLE_PALETTE				= 97,
    ZE_FLT_OPCODE_CLIP_REGION						= 98,
    ZE_FLT_OPCODE_EXTENSION							= 100,
    ZE_FLT_OPCODE_LIGHT_SOURCE						= 101,
    ZE_FLT_OPCODE_LIGHT_SOURCE_PALETTE				= 102,
    ZE_FLT_OPCODE_BOUNDING_SPHERE					= 105,
    ZE_FLT_OPCODE_BOUNDING_CYLINDER					= 106,
    ZE_FLT_OPCODE_BOUNDING_CONVEX_HULL				= 107,
    ZE_FLT_OPCODE_BOUNDING_VOLUME_CENTER			= 108,
    ZE_FLT_OPCODE_BOUNDING_VOLUME_ORIENTATION		= 109,
    ZE_FLT_OPCODE_LIGHT_POINT						= 111,
    ZE_FLT_OPCODE_TEXTURE_MAPPING_PALETTE			= 112,
    ZE_FLT_OPCODE_MATERIAL_PALETTE					= 113,
    ZE_FLT_OPCODE_NAME_TABLE						= 114,
    ZE_FLT_OPCODE_CAT								= 115,
    ZE_FLT_OPCODE_CAT_DATA							= 116,
    ZE_FLT_OPCODE_BOUNDING_HISTOGRAM                = 119,
    ZE_FLT_OPCODE_PUSH_ATTRIBUTE					= 122,
    ZE_FLT_OPCODE_POP_ATTRIBUTE						= 123,
    ZE_FLT_OPCODE_CURVE_NODE						= 126,
    ZE_FLT_OPCODE_ROAD_CONSTRUCTION					= 127,
    ZE_FLT_OPCODE_LIGHT_POINT_APPEARANCE_PALETTE	= 128,
    ZE_FLT_OPCODE_LIGHT_POINT_ANIMATION_PALETTE		= 129,
    ZE_FLT_OPCODE_INDEXED_LIGHT_POINT				= 130,
    ZE_FLT_OPCODE_LIGHT_POINT_SYSTEM				= 131,
    ZE_FLT_OPCODE_INDEXED_STRING					= 132,
    ZE_FLT_OPCODE_SHADER_PALETTE					= 133,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_HEADER			= 135,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_AMBIENT			= 136,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_DIFFUSE			= 137,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_SPECULAR		= 138,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_EMISSIVE		= 139,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_ALPHA			= 140,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_LIGHT_MAP		= 141,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_NORMAL_MAP		= 142,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_BUMP_MAP		= 143,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_SHADOW_MAP		= 145,
	ZE_FLT_OPCODE_EXTENDED_MATERIAL_REFLECTION_MAP	= 147
};

#endif




