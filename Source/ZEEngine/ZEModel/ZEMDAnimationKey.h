//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDAnimationKey.h
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

#include "ZETypes.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZETexture/ZEPixel.h"
#include "ZEExport.ZEEngine.h"

enum ZEMDAnimationKeyType
{
	ZEMD_AKT_RAW,
	ZEMD_AKT_QUANTIZED
};

enum ZEMDAnimationKeyChannel
{
	ZEMD_AKC_POSITION,
	ZEMD_AKC_ROTATION,
	ZEMD_AKC_SCALE
};

#pragma pack(push)
#pragma pack(1)

class ZE_EXPORT_ZEENGINE ZEMDAnimationKey
{
	private:
		struct 
		{
			unsigned					Index:10;
			unsigned					Type:2;
			unsigned					Interpolation:2;
			unsigned					Channel:2;
		};

	protected:
		void							SetType(ZEMDAnimationKeyType Type);

										ZEMDAnimationKey();

	public:
		ZEMDAnimationKeyType			GetType() const;
		ZESize							GetSize() const;

		void							SetIndex(ZEUInt Index);
		ZEUInt							GetIndex() const;

		ZEMDAnimationKeyChannel			GetChannel() const;
		void							SetChannel(ZEMDAnimationKeyChannel);

		void							SetTime(float NormalizedTime);
		float							GetTime() const;

};

class ZE_EXPORT_ZEENGINE ZEMDAnimationKeyRaw : public ZEMDAnimationKey
{
	private:
		float							Data[4];

	public:
		void							SetPosition(const ZEVector3& Position);
		const ZEVector3&				GetPosition() const;

		void							SetQuaternion(const ZEQuaternion& Rotation);
		const ZEQuaternion&				GetQuaternion() const;

		void							SetScale(const ZEVector3& Scale);
		const ZEVector3&				GetScale() const;

										ZEMDAnimationKeyRaw();
};

class ZE_EXPORT_ZEENGINE ZEMDAnimationKeyQuantized : public ZEMDAnimationKey
{
	private:
		ZEUInt16						Data[3];

	public:
		void							SetPosition(const ZEVector3& Position, const ZEVector3& Initial, const ZEVector3& Delta);
		ZEVector3						GetPosition(const ZEVector3& Initial, const ZEVector3& Delta) const;

		void							SetQuaternion(const ZEQuaternion& Rotation);
		ZEQuaternion					GetQuaternion() const;

		void							SetScale(const ZEVector3& Scale, const ZEVector3& Initial, const ZEVector3& Delta);
		ZEVector3						GetScale(const ZEVector3& Initial, const ZEVector3& Delta) const;

										ZEMDAnimationKeyQuantized();
};

#pragma pack(pop)
