//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDAnimationData.h
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

#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEExport.ZEEngine.h"

class ZEMDAnimationKey;
class ZEMLReaderNode;
class ZEMLWriterNode;

class ZE_EXPORT_ZEENGINE ZEMDAnimationDataMapping
{
	private:
		ZEString									Name;

		ZEVector3									PositionQuantizationInitial;
		ZEVector3									PositionQuantizationDelta;
		ZEVector3									ScaleQuantizationInitial;
		ZEVector3									ScaleQuantizationDelta;

	public:
		void										SetName(const ZEString& Name);
		const ZEString&								GetName() const;

		void										SetPositionInitial(const ZEVector3& Position);
		const ZEVector3&							GetPositionQuantizationInitial() const;

		void										SetPositionDelta(const ZEVector3& Delta);
		const ZEVector3&							SetPositionDelta() const;

		void										SetScaleInitial(const ZEVector3& Scale);
		const ZEVector3&							GetScaleQuantizationInitial() const;

		void										SetScaleDelta(const ZEVector3& Delta);
		const ZEVector3&							SetScaleDelta() const;

};

class ZEMDAnimationData
{
	private:
		ZEString									Name;
		float										FramesPerSecond;
		float										Length;

		ZEArray<ZEMDAnimationDataMapping>			Mappings;
		ZEArray<ZEBYTE>								Keys;
		ZESize										KeyCount;
		float										FrameCount;

		bool										UpdateCounters();

	public:
		void										SetName(const ZEString& Name);
		const ZEString&								GetName() const;

		void										SetFramesPerSecond(float FramesPerSecond);
		float										GetFramesPerSecond();

		void										SetFrameCount(float FrameCount);
		float										GetFrameCount();

		const ZEArray<ZEMDAnimationDataMapping>&	GetMappings() const;
		void										AddMapping(const ZEMDAnimationDataMapping& Mapping);
		void										RemoveMapping(ZESize Index);

		void										SetKeysData(const void* Data, ZESize Size);
		const void*									GetKeysData();
		ZESize										GetKeysDataSize();

		ZESize										GetKeyCount() const;
		ZEMDAnimationKey*							GetKey(ZESize Index);
		const ZEMDAnimationKey*						GetKey(ZESize Index) const;
		void										AddKey(const ZEMDAnimationKey* Key);
		void										RemoveKey(ZESize Index);

		void										SortKeys();
		void										QuantizeKeys();
		void										CompressKeys();

		bool										Unserialize(ZEMLReaderNode* Reader);
		bool										Serialize(ZEMLWriterNode* AnimationDataNode);
};
