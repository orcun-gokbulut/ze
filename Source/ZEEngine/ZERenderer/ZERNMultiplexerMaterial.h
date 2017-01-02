//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNMultiplexerMaterial.h
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


#include "ZERNMaterial.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEPointer/ZEHolder.h"

struct ZERNMultiplexerSubMaterial
{
	ZEString											Name;
	ZEHolder<const ZERNMaterial>						Material;
};

class ZERNMultiplexerMaterial : public ZERNMaterial
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERNMultiplexerMaterial)
	friend class ZERSTemplates;
	private:
		ZEArray<ZERNMultiplexerSubMaterial>				Materials;
		ZEString										DefaultMaterialName;
		ZEString										SelectedMaterialName;
		const ZERNMaterial*								SelectedMaterial;

		void											UpdateSelection();

		virtual ZETaskResult							LoadInternal() override;
		virtual ZETaskResult							UnloadInternal() override;

														ZERNMultiplexerMaterial();
		virtual											~ZERNMultiplexerMaterial() override;

	public:
		virtual ZEUInt									GetStageMask() const override;

		const ZEArray<ZERNMultiplexerSubMaterial>&		GetMaterials() const;
		const ZERNMultiplexerSubMaterial*				GetMaterial(const ZEString& Name) const;
		bool											AddMaterial(const ZEString& Name, ZERNMaterial* Material);
		bool											RemoveMaterial(const ZEString& Name);

		void											SetDefaultMaterial(const ZEString& Name);
		const ZEString&									GetDefaultMaterial() const;

		void											SetSelectedMaterial(const ZEString& Name);
		ZEHolder<const ZERNMaterial>					GetSelectedMaterial() const;

		virtual bool									PreRender(ZERNCommand& Command) const override;
		virtual bool									SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced = false) const override;
		virtual void									CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced = false) const override;

		virtual bool									Update() override;

		bool											Serialize(ZEMLWriterNode* Node);
		bool											Unserialize(ZEMLReaderNode* Node);

		static ZERNMultiplexerMaterial*					CreateInstance();
		static ZEHolder<ZERNMultiplexerMaterial>		CreateResource();
		static ZEHolder<ZERNMultiplexerMaterial>		LoadResource(const ZEString& FileName);
		static ZEHolder<const ZERNMultiplexerMaterial>	LoadResourceShared(const ZEString& FileName);
};
