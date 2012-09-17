//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLNode.h
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
#ifndef	__ZEML_NODE_H__
#define __ZEML_NODE_H__

#include "ZEMLItem.h"
#include "ZEDS\ZEList.h"
#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEString.h"
#include "ZEDS\ZEVariant.h"
#include "ZEEndian.h"

class ZEFile;
class ZEMLProperty;
class ZEMLDataProperty;

class ZEMLNode : public ZEMLItem
{
	private:

		ZEList<ZEMLNode>	SubNodes;
		ZEArray<ZEMLItem*>	Properties;
		ZEUInt64			NodeSize;

	protected:

		virtual void		WriteToFile(ZEFile* File);
		virtual void		ReadFromFile(ZEFile* File, bool DeferredDataReading);

	public:

		virtual ZEUInt64	GetTotalSize();

		void				AddProperty(ZEMLProperty* Property);
		ZEMLProperty*		AddProperty(const ZEString& Name, const ZEVariant& Value);
		ZEMLProperty*		AddProperty(const ZEString& Name);
		ZEMLProperty*		AddProperty();
		void				AddProperty(const ZEString& Name, float Value);
		void				AddProperty(const ZEString& Name, double Value);
		void				AddProperty(const ZEString& Name, ZEInt8 Value);
		void				AddProperty(const ZEString& Name, ZEInt16 Value);
		void				AddProperty(const ZEString& Name, ZEInt32 Value);
		void				AddProperty(const ZEString& Name, ZEInt64 Value);
		void				AddProperty(const ZEString& Name, ZEUInt8 Value);
		void				AddProperty(const ZEString& Name, ZEUInt16 Value);
		void				AddProperty(const ZEString& Name, ZEUInt32 Value);
		void				AddProperty(const ZEString& Name, ZEUInt64 Value);
		void				AddProperty(const ZEString& Name, bool Value);
		void				AddProperty(const ZEString& Name, const ZEString& Value);
		void				AddProperty(const ZEString& Name, const char* Value);
		void				AddProperty(const ZEString& Name, const ZEQuaternion& Value);
		void				AddProperty(const ZEString& Name, const ZEVector2& Value);
		void				AddProperty(const ZEString& Name, const ZEVector3& Value);
		void				AddProperty(const ZEString& Name, const ZEVector4& Value);
		void				AddProperty(const ZEString& Name, const ZEMatrix3x3& Value);
		void				AddProperty(const ZEString& Name, const ZEMatrix4x4& Value);
		bool				RemoveProperty(ZEMLProperty* Property);

		void				AddDataProperty(ZEMLDataProperty* Property);
		ZEMLDataProperty*	AddDataProperty(const ZEString& Name ,void* Data, ZEUInt64 DataSize, bool Cache = true);
		ZEMLDataProperty*	AddDataProperty(const ZEString& Name);
		ZEMLDataProperty*	AddDataProperty();
		bool				RemoveDataProperty(ZEMLDataProperty* Property);

		void				AddSubNode(ZEMLNode* Node);
		ZEMLNode*			AddSubNode(const ZEString& Name);
		void				InsertSubNode(ZEMLNode* Node, ZESize Index);
		ZEMLNode*			InsertSubNode(const ZEString& Name, ZESize Index);
		ZEMLNode*			AddSubNode();
		bool				RemoveSubNode(ZEMLNode* SubNode);

		const ZEList<ZEMLNode>&		GetSubNodes() const;
		const ZEArray<ZEMLNode*>	GetSubNodes(const ZEString& NodeName);
		const ZEArray<ZEMLItem*>&	GetProperties() const;
		const ZEArray<ZEMLItem*>	GetProperties(const ZEString& PropertyName);

		const ZEMLNode*				GetParent();

		void						Write(ZEFile* File);
		void						Read(ZEFile* File, bool DeferredDataReading = false);

									ZEMLNode();
									ZEMLNode(const ZEString& Name);

									~ZEMLNode();
};

#endif
