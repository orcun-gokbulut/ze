//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDAnimationKey.cpp
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

#include "ZEMDAnimationKey.h"
#include "ZEError.h"
#include "ZEMath\ZEMath.h"

struct ZEQuaternionQuantized
{
	ZEInt16 a : 15;
	ZEInt16 b : 15;
	ZEInt16 c : 15;
	ZEUInt8 Largest : 2;
	ZEUInt8 Reserved : 1;
};


// ZEMDAnimationKey
//////////////////////////////////////////////////////////////////////////////////////

void ZEMDAnimationKey::SetChannel(ZEMDAnimationKeyChannel)
{
	this->Channel = Channel;
}

void ZEMDAnimationKey::SetType(ZEMDAnimationKeyType Type)
{
	this->Type = Type;
}

ZEMDAnimationKey::ZEMDAnimationKey()
{
	zeDebugCheck(sizeof(ZEMDAnimationKey) != sizeof(ZEUInt32), "Size problem");
}

ZEMDAnimationKeyType ZEMDAnimationKey::GetType() const
{
	return static_cast<ZEMDAnimationKeyType>(Type);
}

ZESize ZEMDAnimationKey::GetSize() const
{
	switch (Type)
	{
		case ZEMD_AKT_RAW:
			return sizeof(ZEMDAnimationKeyRaw);

		case ZEMD_AKT_QUANTIZED:
			return sizeof(ZEMDAnimationKeyQuantized);

		default:
			return 0;
	}
}

void ZEMDAnimationKey::SetIndex(ZEUInt Index)
{
	zeDebugCheck(Index >= 2048, "Index is too big");
	this->Index = Index;
}

ZEUInt ZEMDAnimationKey::GetIndex() const
{
	return Index;
}

ZEMDAnimationKeyChannel ZEMDAnimationKey::GetChannel() const
{
	return static_cast<ZEMDAnimationKeyChannel>(Channel);
}


// ZEMDAnimationKeyRaw
//////////////////////////////////////////////////////////////////////////////////////

void ZEMDAnimationKeyRaw::SetPosition(const ZEVector3& Position)
{
	SetChannel(ZEMD_AKC_POSITION);
	*reinterpret_cast<ZEVector3*>(Data) = Position;
}

const ZEVector3& ZEMDAnimationKeyRaw::GetPosition() const
{
	return *reinterpret_cast<const ZEVector3*>(Data);
}

void ZEMDAnimationKeyRaw::SetQuaternion(const ZEQuaternion& Rotation)
{
	SetChannel(ZEMD_AKC_ROTATION);
	*reinterpret_cast<ZEQuaternion*>(Data) = Rotation;
}

const ZEQuaternion& ZEMDAnimationKeyRaw::GetQuaternion() const
{
	return *reinterpret_cast<const ZEQuaternion*>(Data);
}

void ZEMDAnimationKeyRaw::SetScale(const ZEVector3& Scale)
{
	SetChannel(ZEMD_AKC_SCALE);
	*reinterpret_cast<ZEVector3*>(Data) = Scale;
}

const ZEVector3& ZEMDAnimationKeyRaw::GetScale() const
{
	return *reinterpret_cast<const ZEVector3*>(Data);
}

ZEMDAnimationKeyRaw::ZEMDAnimationKeyRaw()
{
	SetType(ZEMD_AKT_RAW);
}


// ZEMDAnimationKeyQuantized
//////////////////////////////////////////////////////////////////////////////////////

void ZEMDAnimationKeyQuantized::SetPosition(const ZEVector3& Position, const ZEVector3& Initial, const ZEVector3& Delta)
{
	ZEVector3 Quantized = (Position - Initial) / Delta;
	zeDebugCheck((ZEInt)Quantized.x < -0x7FFF || (ZEInt)Quantized.x > 0x7FFF, "Vector.x is out of quantization boundries");
	zeDebugCheck((ZEInt)Quantized.y < -0x7FFF || (ZEInt)Quantized.y > 0x7FFF, "Vector.y is out of quantization boundries");
	zeDebugCheck((ZEInt)Quantized.z < -0x7FFF || (ZEInt)Quantized.z > 0x7FFF, "Vector.z is out of quantization boundries");

	SetChannel(ZEMD_AKC_POSITION);
	Data[0] = (ZEInt)Quantized.x;
	Data[1] = (ZEInt)Quantized.x;
	Data[2] = (ZEInt)Quantized.x;
}

ZEVector3 ZEMDAnimationKeyQuantized::GetPosition(const ZEVector3& Initial, const ZEVector3& Delta) const
{
	return Initial + ZEVector3((float)Data[0], (float)Data[1], (float)Data[2]) * Delta;
}

void ZEMDAnimationKeyQuantized::SetQuaternion(const ZEQuaternion& Rotation)
{
	zeDebugCheck(!Rotation.IsValid(), "Input is not valid");

	ZEQuaternion RotationNormalized = Rotation.Normalize();

	SetChannel(ZEMD_AKC_ROTATION);

	ZEQuaternionQuantized& RotationQuantized = *reinterpret_cast<ZEQuaternionQuantized*>(Data);
	if (RotationNormalized.x > RotationNormalized.y)
	{
		if (RotationNormalized.x > RotationNormalized.z)
		{
			if (RotationNormalized.x > RotationNormalized.w)
			{
				RotationQuantized.Largest = 0;
				RotationQuantized.a = (ZEInt16)(RotationNormalized.y * 0x03FFF);
				RotationQuantized.b = (ZEInt16)(RotationNormalized.z * 0x03FFF);
				RotationQuantized.c = (ZEInt16)(RotationNormalized.w * 0x03FFF);
			}
			else
			{
				RotationQuantized.Largest = 3;
				RotationQuantized.a = (ZEInt16)(RotationNormalized.x * 0x03FFF);
				RotationQuantized.b = (ZEInt16)(RotationNormalized.y * 0x03FFF);
				RotationQuantized.c = (ZEInt16)(RotationNormalized.z * 0x03FFF);
			}
		}
		else
		{
			if (RotationNormalized.z > RotationNormalized.w)
			{
				RotationQuantized.Largest = 2;
				RotationQuantized.a = (ZEInt16)(RotationNormalized.x / 0x03FFF);
				RotationQuantized.b = (ZEInt16)(RotationNormalized.y / 0x03FFF);
				RotationQuantized.c = (ZEInt16)(RotationNormalized.w / 0x03FFF);
			}
			else
			{
				RotationQuantized.Largest = 3;
				RotationQuantized.a = (ZEInt16)(RotationNormalized.x / 0x03FFF);
				RotationQuantized.b = (ZEInt16)(RotationNormalized.y / 0x03FFF);
				RotationQuantized.c = (ZEInt16)(RotationNormalized.z / 0x03FFF);
			}
		}
	}
	else
	{
		if (RotationNormalized.y > RotationNormalized.z)
		{
			if (RotationNormalized.y > RotationNormalized.w)
			{
				RotationQuantized.Largest = 1;
				RotationQuantized.a = (ZEInt16)(RotationNormalized.x / 0x03FFF);
				RotationQuantized.b = (ZEInt16)(RotationNormalized.z / 0x03FFF);
				RotationQuantized.c = (ZEInt16)(RotationNormalized.w / 0x03FFF);
			}
			else
			{
				RotationQuantized.Largest = 3;
				RotationQuantized.a = (ZEInt16)(RotationNormalized.x / 0x03FFF);
				RotationQuantized.b = (ZEInt16)(RotationNormalized.y / 0x03FFF);
				RotationQuantized.c = (ZEInt16)(RotationNormalized.z / 0x03FFF);
			}
		}
		else
		{
			if (RotationNormalized.z > RotationNormalized.w)
			{
				RotationQuantized.Largest = 2;
				RotationQuantized.a = (ZEInt16)(RotationNormalized.x / 0x03FFF);
				RotationQuantized.b = (ZEInt16)(RotationNormalized.y / 0x03FFF);
				RotationQuantized.c = (ZEInt16)(RotationNormalized.w / 0x03FFF);
			}
			else
			{
				RotationQuantized.Largest = 3;
				RotationQuantized.a = (ZEInt16)(RotationNormalized.x / 0x03FFF);
				RotationQuantized.b = (ZEInt16)(RotationNormalized.y / 0x03FFF);
				RotationQuantized.c = (ZEInt16)(RotationNormalized.z / 0x03FFF);
			}
		}
	}
}

ZEQuaternion ZEMDAnimationKeyQuantized::GetQuaternion() const
{
	const ZEQuaternionQuantized& RotationQuantized = *reinterpret_cast<const ZEQuaternionQuantized*>(Data);

	ZEQuaternion Rotation;
	switch (RotationQuantized.Largest)
	{
		case 0:
			Rotation.y = (float)RotationQuantized.a / (float)0x03FFF;
			Rotation.z = (float)RotationQuantized.b / (float)0x03FFF;
			Rotation.w = (float)RotationQuantized.c / (float)0x03FFF;
			Rotation.x = ZEMath::Sqrt(1.0f - Rotation.y * Rotation.y - Rotation.z * Rotation.z - Rotation.w * Rotation.w);
			break;

		case 1:
			Rotation.x = (float)RotationQuantized.a / (float)0x03FFF;
			Rotation.z = (float)RotationQuantized.b / (float)0x03FFF;
			Rotation.w = (float)RotationQuantized.c / (float)0x03FFF;
			Rotation.y = ZEMath::Sqrt(1.0f - Rotation.x * Rotation.x - Rotation.z * Rotation.z - Rotation.w * Rotation.w);
			break;

		case 2:
			Rotation.x = (float)RotationQuantized.a / (float)0x03FFF;
			Rotation.y = (float)RotationQuantized.b / (float)0x03FFF;
			Rotation.w = (float)RotationQuantized.c / (float)0x03FFF;
			Rotation.z = ZEMath::Sqrt(1.0f - Rotation.x * Rotation.x - Rotation.y * Rotation.y - Rotation.w * Rotation.w);
			break;

		case 3:
			Rotation.x = (float)RotationQuantized.a / (float)0x03FFF;
			Rotation.y = (float)RotationQuantized.b / (float)0x03FFF;
			Rotation.z = (float)RotationQuantized.c / (float)0x03FFF;
			Rotation.w = ZEMath::Sqrt(1.0f - Rotation.x * Rotation.x - Rotation.y * Rotation.y - Rotation.z * Rotation.z);
			break;
	}

	zeDebugCheck(!Rotation.IsValid(), "Output is not valid");
	zeDebugCheck(!Rotation.IsNormalized(), "Output is not normalized");

	return Rotation;
}

void ZEMDAnimationKeyQuantized::SetScale(const ZEVector3& Scale, const ZEVector3& Initial, const ZEVector3& Delta)
{
	ZEVector3 Quantized = (Scale - Initial) / Delta;

	zeDebugCheck((ZEInt)Quantized.x < -0x7FFF || (ZEInt)Quantized.x > 0x7FFF, "Vector.x is out of quantization boundries");
	zeDebugCheck((ZEInt)Quantized.y < -0x7FFF || (ZEInt)Quantized.y > 0x7FFF, "Vector.y is out of quantization boundries");
	zeDebugCheck((ZEInt)Quantized.z < -0x7FFF || (ZEInt)Quantized.z > 0x7FFF, "Vector.z is out of quantization boundries");

	SetChannel(ZEMD_AKC_SCALE);
	Data[0] = (ZEInt)Quantized.x;
	Data[1] = (ZEInt)Quantized.x;
	Data[2] = (ZEInt)Quantized.x;
}

ZEVector3 ZEMDAnimationKeyQuantized::GetScale(const ZEVector3& Initial, const ZEVector3& Delta) const
{
	return Initial + ZEVector3((float)Data[0], (float)Data[1], (float)Data[2]) * Delta;
}

ZEMDAnimationKeyQuantized::ZEMDAnimationKeyQuantized()
{
	SetType(ZEMD_AKT_QUANTIZED);
}
