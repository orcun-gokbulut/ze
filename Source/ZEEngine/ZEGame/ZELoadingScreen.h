//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELoadingScreen.h
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
#include "ZEEntity.h"
#include "ZEUI/ZEUILabel.h"
#include "ZEExport.ZEEngine.h"

class ZERNMaterial;
class ZEGRBuffer;
class ZEUIFontTrueType;
class ZEUIManager;

class ZE_EXPORT_ZEENGINE ZELoadingScreen : public ZEEntity
{
	ZE_OBJECT
	private:
		ZEUIManager*						Manager;

		ZEArray<ZEUILabel*>					ConsoleLines;
		ZEUILabel*							LoadingLabel;

		ZEUIFrameControl*					LoadingIndicatorFrame;
		ZEHolder<const ZEUIFontTrueType>	Font;

		ZEUInt								LoadingPercentage;
		ZEUInt								LastLoadingPercentage;
		ZESize								LastOutputBufferCount;

		virtual ZEEntityResult				LoadInternal();
		virtual ZEEntityResult				UnloadInternal();

											ZELoadingScreen();
		virtual								~ZELoadingScreen();

	public:
		void								SetManager(ZEUIManager* Manager);
		ZEUIManager*						GetManager();

		void								SetLoadingStageCount(ZEUInt Count);
		ZEUInt								GetLoadingStageCount() const;

		void								SetLoadingStageIndex(ZEUInt Index);
		ZEUInt								GetLoadingStageIndex() const;

		void								ShowStartupScreen();
		void								HideStartupScreen();

		void								ShowLoadingScreen();
		void								HideLoadingScreen();

		virtual void						Tick(float ElapsedTime);
		virtual bool						PreRender(const ZERNPreRenderParameters* Parameters);

		static ZELoadingScreen*				CreateInstance();
};
