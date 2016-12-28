//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetItemDelegate.cpp
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

#include "ZEDAssetItemDelegate.h"

#include "ZEMeta/ZEObject.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEDCore/ZEDAsset.h"
#include "ZEDCore/ZEDAssetType.h"
#include "ZEDCore/ZEDAssetDirectory.h"

#include <QPainter>
#include <QIcon>

QRect ZEDAssetItemDelegate::GetClientRect(const QStyleOptionViewItem& Option) const
{
	QRect Output = Option.rect;

	int HorizontalMargins = ThumbnailMargins.left() + ThumbnailMargins.right();
	if (Output.width() > ThumbnailSize.width() + HorizontalMargins)
		Output.setWidth(ThumbnailSize.width() + HorizontalMargins);

	int VerticalMargins = ThumbnailMargins.bottom() + ThumbnailMargins.top();
	if (Output.height() > ThumbnailSize.height() + VerticalMargins)
		Output.setHeight(ThumbnailSize.height() + VerticalMargins);

	return Output.marginsRemoved(ThumbnailMargins);
}

void ZEDAssetItemDelegate::paintBorder(QPainter* Painter, const QRect& ClientRect, const QStyleOptionViewItem& Option, const QModelIndex& Index) const
{
	ZEDAsset* Asset = ZEClass::Cast<ZEDAsset>(static_cast<ZEObject*>(Index.internalPointer()));
	bool Selected = (Option.state & QStyle::State_Selected) != 0;
	int SelectionMargin = (Selected ? 1 : 0);
	
	QRect BorderRect = GetClientRect(Option).adjusted(SelectionMargin, SelectionMargin, -SelectionMargin, -SelectionMargin);
	QPen BorderPen;
	const ZEVector3& Color = Asset->GetType()->GetColor();
	BorderPen.setColor(QColor(Color.x * 255, Color.y * 255, Color.z * 255));
	BorderPen.setWidth(Selected ? 2 : 1);
	Painter->setPen(BorderPen);
	Painter->drawRect(BorderRect);
}

void ZEDAssetItemDelegate::paintThumbnail(QPainter* Painter, const QRect& ClientRect, const QStyleOptionViewItem& Option, const QModelIndex& Index) const
{
	ZEDAsset* Asset = ZEClass::Cast<ZEDAsset>(static_cast<ZEObject*>(Index.internalPointer()));
	const QImage* ThumbnailImage = Asset->GetThumbnailImage();
	if (ThumbnailImage == NULL)
	{
		Painter->setFont(Option.font);
		Painter->setBrush(Qt::black);
		ZEString IconPath = Asset->GetType()->GetIconPath(ZEPathInfo(Asset->GetName()).GetExtension());
		ZEString IconRealPath = ZEPathInfo(IconPath).GetRealPath().Path;
		QIcon Icon = QIcon(IconRealPath.ToCString());
		Icon.paint(Painter, ClientRect, Qt::AlignCenter | Qt::AlignVCenter);
		Painter->drawText(ClientRect.adjusted(0, 0, 0, -18), "Thumbnail is not Available", QTextOption(Qt::AlignCenter | Qt::AlignBottom));
		return;
	}
		
	QRect Rect = ClientRect;
	if (ThumbnailImage->width() >= Rect.width() || ThumbnailImage->height() >= Rect.height())
	{
		float AspectRatio = (float)ThumbnailImage->width() / (float)ThumbnailImage->height();

		if (AspectRatio > 1.0f)
		{
			int NewHeight = Rect.height() / AspectRatio;
			Rect.setY(Rect.y() + (Rect.height() - NewHeight) / 2.0f);
			Rect.setHeight(NewHeight);
		}
		else
		{
			int NewWidth = Rect.width() * AspectRatio;
			Rect.setX(Rect.x() + (Rect.width() - NewWidth) / 2.0f);
			Rect.setWidth(NewWidth);
		}
	}
	else
	{
		Rect.setX(Rect.x() + (Rect.width() - ThumbnailImage->width()) / 2.0f);
		Rect.setY(Rect.y() + (Rect.height() - ThumbnailImage->height()) / 2.0f);
		Rect.setWidth(ThumbnailImage->width());
		Rect.setHeight(ThumbnailImage->height());
	}

	Painter->drawImage(Rect, *ThumbnailImage);
}

void ZEDAssetItemDelegate::paintHeader(QPainter* Painter, const QRect& ClientRect, const QStyleOptionViewItem& Option, const QModelIndex& Index) const
{
	ZEDAsset* Asset = ZEClass::Cast<ZEDAsset>(static_cast<ZEObject*>(Index.internalPointer()));
	bool Selected = (Option.state & QStyle::State_Selected) != 0;

	QFont Font = Option.font;
	Font.setBold(Selected);
	QFontMetrics FontMetrics(Font);
	int FontHeight = FontMetrics.height() * 1.20;

	QRect TitleRect = ClientRect;
	TitleRect.setHeight(FontHeight);

	const ZEVector3& Color = Asset->GetType()->GetColor();
	Painter->setPen(Qt::NoPen);
	Painter->setBrush(QColor(Color.x * 255, Color.y * 255, Color.z * 255));
	Painter->drawRect(TitleRect);

	QBrush TitleBrush;
	ZEVector3 ColorDarker = Color * 0.5;
	TitleBrush.setColor(QColor(ColorDarker.x * 255, ColorDarker.y * 255, ColorDarker.z * 255));
	TitleBrush.setStyle(Qt::BrushStyle::Dense4Pattern);
	Painter->setBrush(TitleBrush);
	Painter->drawRect(TitleRect);
	Painter->setBrush(Qt::NoBrush);

	QTextOption TextOption;
	TextOption.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	TextOption.setWrapMode(QTextOption::NoWrap);

	Painter->setFont(Font);
	Painter->setPen(QPen(Qt::white));
	Painter->drawText(TitleRect, Asset->GetType()->GetName(), TextOption);
}

void ZEDAssetItemDelegate::paintFooter(QPainter* Painter, const QRect& ClientRect, const QStyleOptionViewItem& Option, const QModelIndex& Index) const
{
	ZEDAsset* Asset = ZEClass::Cast<ZEDAsset>(static_cast<ZEObject*>(Index.internalPointer()));
	bool Selected = (Option.state & QStyle::State_Selected) != 0;

	QFont Font = Option.font;
	QFontMetrics FontMetrics(Font);
	int FontHeight = FontMetrics.height() * 1.20;

	QRect FooterRect = ClientRect;
	FooterRect.setY(ClientRect.y() + (ThumbnailSize.height() - FontHeight));
	FooterRect.setHeight(FontHeight);

	const ZEVector3& Color = Asset->GetType()->GetColor();
	Painter->setPen(Qt::NoPen);
	Painter->setBrush(QColor(Color.x * 255, Color.y * 255, Color.z * 255));
	Painter->drawRect(FooterRect);

	QBrush FooterBrush;
	ZEVector3 ColorDarker =  Color * 0.5;
	FooterBrush.setColor(QColor(ColorDarker.x * 255, ColorDarker.y * 255, ColorDarker.z * 255));
	FooterBrush.setStyle(Qt::BrushStyle::Dense4Pattern);
	Painter->setBrush(FooterBrush);
	Painter->drawRect(FooterRect);
	Painter->setBrush(Qt::NoBrush);

	QTextOption TextOption;
	TextOption.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	TextOption.setWrapMode(QTextOption::NoWrap);

	Painter->setFont(Font);
	Painter->setPen(QPen(Qt::white));
	Painter->drawText(FooterRect, Asset->GetName().ToCString(), TextOption);
}

void ZEDAssetItemDelegate::SetThumbnailSize(const QSize& Size)
{
	ThumbnailSize = Size;
}

const QSize& ZEDAssetItemDelegate::GetThumbnailSize() const
{
	return ThumbnailSize;
}

void ZEDAssetItemDelegate::SetThumbnailMargin(const QMargins& Margins)
{
	ThumbnailMargins = Margins;
}

const QMargins& ZEDAssetItemDelegate::GetThumbnailMargin() const
{
	return ThumbnailMargins;
}

void ZEDAssetItemDelegate::SetThumbnailColumn(ZEUInt Column)
{
	ThumbnailColumn = Column;
}

ZEUInt ZEDAssetItemDelegate::GetThumbnailColumn() const
{
	return ThumbnailColumn;
}

void ZEDAssetItemDelegate::paint(QPainter* Painter, const QStyleOptionViewItem& Option, const QModelIndex& Index) const
{
	ZEDAsset* Asset = ZEClass::Cast<ZEDAsset>(static_cast<ZEObject*>(Index.internalPointer()));
	if (Index.column() == ThumbnailColumn && Asset != NULL && Asset->GetType()->GetCapabilities().GetFlags(ZED_ATC_THUMBNAIL))
	{
		QRect ClientRect = GetClientRect(Option);
		paintThumbnail(Painter, ClientRect, Option, Index);
		paintHeader(Painter, ClientRect, Option, Index);
		paintFooter(Painter, ClientRect, Option, Index);
		paintBorder(Painter, ClientRect, Option, Index);
	}
	else
	{
		QStyledItemDelegate::paint(Painter, Option, Index);
	}
}

QSize ZEDAssetItemDelegate::sizeHint(const QStyleOptionViewItem& Option, const QModelIndex& Index) const
{
	ZEDAsset* Asset = ZEClass::Cast<ZEDAsset>(static_cast<ZEObject*>(Index.internalPointer()));

	if (Index.column() == ThumbnailColumn && Asset != NULL && Asset->GetType()->GetCapabilities().GetFlags(ZED_ATC_THUMBNAIL))
	{
		return QSize(
			ThumbnailSize.width() + ThumbnailMargins.left() + ThumbnailMargins.right(), 
			ThumbnailSize.height()  + ThumbnailMargins.top() + ThumbnailMargins.bottom());
	}
	else
	{
		return QStyledItemDelegate::sizeHint(Option, Index);
	}
}

ZEDAssetItemDelegate::ZEDAssetItemDelegate()
{
	ThumbnailMargins = QMargins(5, 5, 5, 5);
	ThumbnailSize = QSize(128, 128);
	ThumbnailColumn = 0;
}
