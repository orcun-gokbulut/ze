//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIMultiLineTextEdit.cpp
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

#include "ZEUIMultiLineTextEdit.h"
#include "ZEFontResourceBitmap.h"
#include "ZEUIRenderer.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZEUIMaterial.h"
#include "ZEFontResourceDynamic.h"

void ZEUIMultiLineTextEdit::SelectCursorPosition(ZEVector2 MousePosition)
{
	if (TextDisplayer.GetText().GetLength() == 0)
	{
		Cursor.Positions.LeftUp.x = TextDisplayer.GetPosition().x + 1;
		Cursor.Positions.LeftUp.y = TextDisplayer.GetPosition().y + 1;
		Cursor.Positions.RightDown.x = TextDisplayer.GetPosition().x + 2;
		Cursor.Positions.RightDown.y = TextDisplayer.GetPosition().y + 16;

		CursorTextPosition = 0;	
		return;
	}

	InitialMousePosition = MousePosition;
	LastMousePosition = MousePosition;
	SelectedTextItemsPositions.Clear();

	for (size_t I = 0; I < TextDisplayer.GetTextControlCharacters().GetCount(); I++)
		TextDisplayer.DisplayTextCharacters[I].IsSelected = false;

	IsAnyCharacterClicked = false;

	// CONTROL IF ANY CHARACTER PRESSED
	for (size_t I = 0; I < TextDisplayer.GetTextControlCharacters().GetCount(); I++)
	{
		ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.RightDown);
		
		if (ZERectangle::IntersectionTest(CharacterRectangle, MousePosition))
		{
			Cursor.Positions.LeftUp = CharacterRectangle.LeftUp;
			Cursor.Positions.RightDown.x = CharacterRectangle.LeftUp.x + 1;
			Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;

			CursorTextPosition = I;
			return;
		}
	}

	// IF NO CHARACTER PRESSED, CONTROL IF ANY LINE PRESSED
	bool IsAnyLineClicked = false;

	for (size_t I = 0; I < TextDisplayer.GetTextControlCharacters().GetCount(); I++)
	{
		if (TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.LeftUp.y <= MousePosition.y && TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.RightDown.y > MousePosition.y)
		{
			ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.RightDown);

			Cursor.Positions.LeftUp.x = CharacterRectangle.RightDown.x;
			Cursor.Positions.LeftUp.y = CharacterRectangle.LeftUp.y;
			Cursor.Positions.RightDown.x = CharacterRectangle.RightDown.x + 1;
			Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;

			CursorTextPosition = I + 1;
			IsAnyCharacterClicked = true;
		}
	}

	if(IsAnyCharacterClicked == true)
		return;

	// IF MOUSE PRESS IS OUTSIDE THE TEXT
	ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown);

	Cursor.Positions.LeftUp.x = CharacterRectangle.RightDown.x;
	Cursor.Positions.LeftUp.y = CharacterRectangle.LeftUp.y;
	Cursor.Positions.RightDown.x = CharacterRectangle.RightDown.x + 1;
	Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;

	CursorTextPosition = TextDisplayer.GetTextControlCharacters().GetCount();	
}

void ZEUIMultiLineTextEdit::MultiCharacterSelection(ZEVector2 MoveAmount)
{
	if (TextDisplayer.GetText().GetLength() == 0)
		return;

	LastMousePosition = LastMousePosition + MoveAmount; // HACK

	size_t FromCharacter = 0;
	size_t ToCharacter = 0;

	size_t HoveredCharacterPosition = 0;

	bool AnyCharacterIntersected = false;

	SelectedTextItemsPositions.Clear();		

	for (size_t I = 0; I < TextDisplayer.GetTextControlCharacters().GetCount(); I++)
		TextDisplayer.DisplayTextCharacters[I].IsSelected = false;

	// CONTROL IF ANY CHARACTER HOVERED
	for (size_t I = 0; I < TextDisplayer.GetTextControlCharacters().GetCount(); I++)
	{
		ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.RightDown);

		if (ZERectangle::IntersectionTest(CharacterRectangle, LastMousePosition))
		{
			HoveredCharacterPosition = I;
			AnyCharacterIntersected = true;

			if (CursorTextPosition >= HoveredCharacterPosition)
			{
				Cursor.Positions.LeftUp = CharacterRectangle.LeftUp;
				Cursor.Positions.RightDown.x = CharacterRectangle.LeftUp.x - 1;
				Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;				
			}
			else
			{
				Cursor.Positions.LeftUp = CharacterRectangle.LeftUp;
				Cursor.Positions.RightDown.x = CharacterRectangle.LeftUp.x + 1;
				Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;
			}
			break;
		}
	}

	// IF THERE IS NO INTERSECTION
	if (AnyCharacterIntersected == false)
	{
		bool AnyLineHovered = false;

		// IF NO CHARACTER HOVERED, CONTROL IF ANY LINE HOVERED
		for (size_t I = 0; I < TextDisplayer.GetTextControlCharacters().GetCount(); I++)
		{
			if (TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.LeftUp.y <= LastMousePosition.y && TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.RightDown.y >= LastMousePosition.y)
			{			
				ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[I + 1].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[I + 1].RenderableCharacter.Positions.RightDown);

				Cursor.Positions.LeftUp.x = CharacterRectangle.LeftUp.x;
				Cursor.Positions.LeftUp.y = CharacterRectangle.LeftUp.y;
				Cursor.Positions.RightDown.x = CharacterRectangle.LeftUp.x + 1;
				Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;

				HoveredCharacterPosition = I + 1;
				AnyLineHovered = true;
				//break;
			}
		}

		if (AnyLineHovered == false)
			return;

	}

	// ARRANGE 'FromCharacter' AND 'ToCharacter' ACCORDING TO 'CharacterTextPosition' AND 'HoveredCharacterPosition'
	if (CursorTextPosition >= HoveredCharacterPosition)
	{
		FromCharacter = HoveredCharacterPosition;
		ToCharacter = CursorTextPosition;
	}
	else
	{
		FromCharacter = CursorTextPosition;
		ToCharacter = HoveredCharacterPosition;
	}

	// SELECT CHARACTERS BETWEEN FROM CHARACTER AND TO CHARACTER
	for (size_t I = FromCharacter; I < ToCharacter; I++)
	{
		ZEUIRectangle SelectedCharacter = TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter;

// 		SelectedCharacter.Material = HightlightMaterial;
// 		SelectedCharacter.Color = ZEVector4(0.0f, 0.0f, 1.0f , 0.5f);

		//SelectedTextItemsRectangles.Add(SelectedCharacter);
		TextDisplayer.DisplayTextCharacters[I].IsSelected = true;
		SelectedTextItemsPositions.Add(I);
	}

	// ON SELECTION IF MOUSE EXCEEDS VISIBLE RECTANGLE SCROLL TEXT
	// MOVING UP
	if (LastMousePosition.y <= TextDisplayer.GetVisibleRectangle().LeftUp.y && TextDisplayer.GetRectangle().LeftUp.y < TextDisplayer.GetVisibleRectangle().LeftUp.y)
	{
		//TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + TextDisplayer.GetVisibleRectangle().LeftUp.y - SelectedTextItemsRectangles.GetFirstItem().Positions.LeftUp.y));
		if (FromCharacter > 0)
		{
			float MoveY = TextDisplayer.GetTextControlCharacters()[FromCharacter - 1].RenderableCharacter.Positions.RightDown.y - TextDisplayer.GetTextControlCharacters()[FromCharacter - 1].RenderableCharacter.Positions.LeftUp.y;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + MoveY));
		}
		else if (FromCharacter == 0)
		{
			float MoveY = TextDisplayer.GetVisibleRectangle().LeftUp.y - TextDisplayer.GetRectangle().LeftUp.y;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + MoveY));
		}

		// UPDATE SCROLL BARS
		//GetVerticalScrollbar()->SetContentArea(&TextDisplayer, GetVisibleContentArea());
	}

	// MOVING DOWN
	else if (LastMousePosition.y >= TextDisplayer.GetVisibleRectangle().RightDown.y && TextDisplayer.GetRectangle().RightDown.y > TextDisplayer.GetVisibleRectangle().RightDown.y)
	{
		//TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y - (SelectedTextItemsRectangles.GetLastItem().Positions.RightDown.y - TextDisplayer.GetVisibleRectangle().RightDown.y)));
		
		if (ToCharacter < TextDisplayer.GetText().GetLength() - 1)
		{
			float MoveY = TextDisplayer.GetTextControlCharacters()[ToCharacter].RenderableCharacter.Positions.RightDown.y - TextDisplayer.GetTextControlCharacters()[ToCharacter].RenderableCharacter.Positions.LeftUp.y;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y - MoveY));
		}
		else if (ToCharacter = TextDisplayer.GetText().GetLength() - 1)
		{
			float MoveY = TextDisplayer.GetRectangle().RightDown.y - TextDisplayer.GetVisibleRectangle().RightDown.y;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y - MoveY));
		}
		// UPDATE SCROLL BARS
		//GetVerticalScrollbar()->SetContentArea(&TextDisplayer, GetVisibleContentArea());
	}

	// MOVING RIGHT
	else if (LastMousePosition.x + 10.0f > GetVisibleContentArea()->GetRectangle().RightDown.x && TextDisplayer.GetRectangle().RightDown.x > GetVisibleContentArea()->GetRectangle().RightDown.x)
	{
		if (ToCharacter < TextDisplayer.GetText().GetLength() - 1)
		{
			float MoveX = TextDisplayer.GetTextControlCharacters()[ToCharacter].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[ToCharacter].RenderableCharacter.Positions.LeftUp.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - MoveX, TextDisplayer.GetPosition().y));
		}
		else if (ToCharacter = TextDisplayer.GetText().GetLength() - 1)
		{
			float MoveX = TextDisplayer.GetRectangle().RightDown.x - GetVisibleContentArea()->GetRectangle().RightDown.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - MoveX, TextDisplayer.GetPosition().y));
		}
		// UPDATE SCROLL BARS
		//GetHorizontalScrollbar()->SetContentArea(&TextDisplayer, GetVisibleContentArea());
	}

	// MOVING LEFT
	else if (LastMousePosition.x  - 10.0f < GetVisibleContentArea()->GetRectangle().LeftUp.x && TextDisplayer.GetRectangle().LeftUp.x < GetVisibleContentArea()->GetRectangle().LeftUp.x)
	{
		if (FromCharacter > 0)
		{
			float MoveX = TextDisplayer.GetTextControlCharacters()[FromCharacter - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[FromCharacter - 1].RenderableCharacter.Positions.LeftUp.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + MoveX, TextDisplayer.GetPosition().y));
		}
		else if (FromCharacter == 0)
		{
			float MoveX = GetVisibleContentArea()->GetRectangle().LeftUp.x - TextDisplayer.GetRectangle().LeftUp.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + MoveX, TextDisplayer.GetPosition().y));
		}
		// UPDATE SCROLL BARS
		//GetHorizontalScrollbar()->SetContentArea(&TextDisplayer, GetVisibleContentArea());
	}
		UpdateScrollbars();
}

void ZEUIMultiLineTextEdit::DeleteCharacter(int Position)
{	
	if (TextDisplayer.GetTextControlCharacters().GetCount() == 0)
	{
		Cursor.Positions.LeftUp.x = TextDisplayer.GetPosition().x;
		Cursor.Positions.RightDown.x = TextDisplayer.GetPosition().x + 1;
		return;
	}

	ZEString Manipulator = TextDisplayer.GetText();
	ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.RightDown);

	if (Position == 0)
	{
		Cursor.Positions.LeftUp.x = CharacterRectangle.LeftUp.x;
		Cursor.Positions.LeftUp.y = TextDisplayer.GetPosition().y;
		Cursor.Positions.RightDown.x = CharacterRectangle.LeftUp.x + 1;
		Cursor.Positions.RightDown.y = TextDisplayer.GetPosition().y + CharacterRectangle.GetHeight();
	}
	else
	{
		Cursor.Positions.LeftUp.x = TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.RightDown.x;
		Cursor.Positions.LeftUp.y = TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.y;
		Cursor.Positions.RightDown.x = TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.RightDown.x + 1;
		Cursor.Positions.RightDown.y = TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.RightDown.y;
	}

	// FOCUS TO CURSOR
	// IF TEXT WRAP NOT ENABLED
	if (!TextDisplayer.GetTextWrap())
	{
		if (TextDisplayer.GetPosition().x < GetVisibleContentArea()->GetRectangle().LeftUp.x)
		{
			// If LAST CHARACTER POSITION SMALLER THAN VISIBLE RECTANGLE
			if (TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.x < GetVisibleContentArea()->GetRectangle().RightDown.x)
			{
				float XMoveAmount = TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + XMoveAmount, TextDisplayer.GetPosition().y));
				//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - XMoveAmount);
				Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x + XMoveAmount, Cursor.Positions.GetPosition().y));
			}
			// IF CURSOR POSITION EXCEEDS THE LEFT BOUND (WILL SHOW TWO CHARACTERS THAT EXCEEDS THE BOUND)
			else if (Position - 2 >= 0 && TextDisplayer.GetTextControlCharacters()[Position - 2].RenderableCharacter.Positions.LeftUp.x < GetVisibleContentArea()->GetRectangle().LeftUp.x)
			{
				float XMoveAmount = GetVisibleContentArea()->GetRectangle().LeftUp.x - TextDisplayer.GetTextControlCharacters()[Position - 2].RenderableCharacter.Positions.LeftUp.x;
				float CharacterWidth = TextDisplayer.GetTextControlCharacters()[Position - 2].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[Position - 2].RenderableCharacter.Positions.LeftUp.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + XMoveAmount, TextDisplayer.GetPosition().y));
				//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - CharacterWidth);
				Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x + XMoveAmount, Cursor.Positions.GetPosition().y));
			}
			// IF THERE ARE NO TWO CHARACTERS THAN
			else if(TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.x < GetVisibleContentArea()->GetRectangle().LeftUp.x)
			{
				float XMoveAmount = GetVisibleContentArea()->GetRectangle().LeftUp.x - TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.x;
				float CharacterWidth = TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + XMoveAmount, TextDisplayer.GetPosition().y));
				//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - CharacterWidth);
				Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x + XMoveAmount, Cursor.Positions.GetPosition().y));
			}			

			// IF LAST CHARACTER EXCEEDS THAN THE LIMIT THAN SET WIDTH 
			else if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x > GetVisibleContentArea()->GetRectangle().RightDown.x)
			{
				float LastCharacterWidth = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.x;
				
				if (LastCharacterWidth < TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - GetVisibleContentArea()->GetRectangle().RightDown.x)
				{
					//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - LastCharacterWidth);
				}
				else
				{
					float LastCharacterWidth = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - GetVisibleContentArea()->GetRectangle().RightDown.x;
					//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - LastCharacterWidth);
				}
			}
		}
		
		// IF TEXTDISPLAYER EXCEEDS THE UP LIMIT
		if (TextDisplayer.GetPosition().y < GetVisibleContentArea()->GetRectangle().LeftUp.y)
		{
			// IF LAST CHARACTER IS ABOVE FROM VISIBLE RECTANGLE
			if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y < GetVisibleContentArea()->GetRectangle().RightDown.y)
			{
				float YMoveAmount = GetVisibleContentArea()->GetRectangle().RightDown.y - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y;

				if (YMoveAmount < GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetPosition().y)
				{
					TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + YMoveAmount));
					//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - YMoveAmount);
					Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x, Cursor.Positions.GetPosition().y + YMoveAmount));
				}
				else
				{
					YMoveAmount = GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetPosition().y;
					TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + YMoveAmount));
					//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - YMoveAmount);
					Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x, Cursor.Positions.GetPosition().y + YMoveAmount));
				}

			}
			// IF THE CHARACTER WHICH IS GOING TO BE ERASED EXCEEDS THE HEIGHT LIMIT
			else if (TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.y < GetVisibleContentArea()->GetRectangle().LeftUp.y)
			{
				float YMoveAmount = GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.y;

				if (YMoveAmount < GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetPosition().y)
				{
					TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + YMoveAmount));
					//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - YMoveAmount);
					Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x, Cursor.Positions.GetPosition().y + YMoveAmount));
				}
				else
				{
					YMoveAmount = GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetPosition().y;
					TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + YMoveAmount));
					//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - YMoveAmount);
					Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x, Cursor.Positions.GetPosition().y + YMoveAmount));
				}
			}
		}
		// IF LAST CHARACTER JUMPED TO PREVIOUS LINE WHEN LAST CHARACTER IS OUTSIDE OF VISIBLE CONTENT AREA LIMIT THAN SET HEIGHT 
		if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y > GetVisibleContentArea()->GetRectangle().RightDown.y)
		{
			if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 2].RenderableCharacter.Positions.RightDown.y < TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y)
			{
				float LastCharacterHeight = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.y;
				//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - LastCharacterHeight);
			}
		}

		// IF ENTER THAN MOVE TEXTDISPLAYER TO LAST CHARACTER POSITION
// 		if (TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.RightDown.y < TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.RightDown.y)
// 		{
			if (TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.RightDown.x > GetVisibleContentArea()->GetRectangle().RightDown.x)
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - (TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.RightDown.x - GetVisibleContentArea()->GetRectangle().RightDown.x), TextDisplayer.GetPosition().y));
/*		}*/
	}
	// IF TEXT WRAP ENABLED
	else if (TextDisplayer.GetTextWrap())
	{
		if (TextDisplayer.GetPosition().y < GetVisibleContentArea()->GetRectangle().LeftUp.y)
		{
			// IF LAST CHARACTER IS ABOVE FROM VISIBLE RECTANGLE
			if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y < GetVisibleContentArea()->GetRectangle().RightDown.y)
			{
				float YMoveAmount = GetVisibleContentArea()->GetRectangle().RightDown.y - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y;

				if (YMoveAmount < GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetPosition().y)
				{
					TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + YMoveAmount));
					//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - YMoveAmount);
					Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x, Cursor.Positions.GetPosition().y + YMoveAmount));
				}
				else
				{
					YMoveAmount = GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetPosition().y;
					TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + YMoveAmount));
					//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - YMoveAmount);
					Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x, Cursor.Positions.GetPosition().y + YMoveAmount));
				}

			}
			// IF THE CHARACTER WHICH IS GOING TO BE ERASED EXCEEDS THE HEIGHT LIMIT
			else if (TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.y < GetVisibleContentArea()->GetRectangle().LeftUp.y)
			{
				float YMoveAmount = GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetTextControlCharacters()[Position - 1].RenderableCharacter.Positions.LeftUp.y;

				if (YMoveAmount < GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetPosition().y)
				{
					TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + YMoveAmount));
					//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - YMoveAmount);
					Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x, Cursor.Positions.GetPosition().y + YMoveAmount));
				}
				else
				{
					YMoveAmount = GetVisibleContentArea()->GetRectangle().LeftUp.y - TextDisplayer.GetPosition().y;
					TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y + YMoveAmount));
					//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - YMoveAmount);
					Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x, Cursor.Positions.GetPosition().y + YMoveAmount));
				}
			}
		}
		// IF LAST CHARACTER JUMPED TO PREVIOUS LINE WHEN LAST CHARACTER IS OUTSIDE OF VISIBLE CONTENT AREA LIMIT THAN SET HEIGHT 
		if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 2].RenderableCharacter.Positions.RightDown.y > GetVisibleContentArea()->GetRectangle().RightDown.y)
		{
			if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 3].RenderableCharacter.Positions.RightDown.y < TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 2].RenderableCharacter.Positions.RightDown.y)
			{
				float LastCharacterHeight = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 2].RenderableCharacter.Positions.RightDown.y - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 2].RenderableCharacter.Positions.LeftUp.y;
				//TextDisplayer.SetHeight(TextDisplayer.GetHeight() - LastCharacterHeight);
			}
		}
	}

// 	Manipulator.Remove(Position);
// 	TextDisplayer.SetText(Manipulator);

}

void ZEUIMultiLineTextEdit::BackspaceKeyPressed()
{
	// IF THERE IS NO TEXT OR THERE IS NO CHARACTER BEFORE CURSOR WHEN NO SELECTION
	if (TextDisplayer.GetText().GetLength() == 0 || (CursorTextPosition == 0 && SelectedTextItemsPositions.GetCount() == 0))
		return;

	// IF THERE IS A SELECTION
	if (SelectedTextItemsPositions.GetCount() != 0)
	{
		for (int I = SelectedTextItemsPositions.GetCount() - 1; I >= 0; I--)
		{
			TextDisplayer.DeleteCharacter(SelectedTextItemsPositions[I]);
			DeleteCharacter(SelectedTextItemsPositions[I]);
		}

//		TextDisplayer.DeleteCharacters(SelectedTextItemsPositions);
		CursorTextPosition = SelectedTextItemsPositions.GetFirstItem();

		SelectedTextItemsPositions.Clear();
	}

	// IF THERE IS NO SELECTION
	else if (SelectedTextItemsPositions.GetCount() == 0)
	{
		CursorTextPosition--;
		TextDisplayer.DeleteCharacter(CursorTextPosition);
		DeleteCharacter(CursorTextPosition);
	}

	UpdateScrollbars();
}
void ZEUIMultiLineTextEdit::AddCharacter(char Key)
{
	ZEString Manipulator = TextDisplayer.GetText();

	// INSERT NEW CHARACTER TO CURSOR POSITION
// 	Manipulator.Insert(CursorTextPosition, &Key);
// 	TextDisplayer.SetText(Manipulator);

	TextDisplayer.AddCharacter(CursorTextPosition, Key);

	ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown);

	Cursor.Positions.LeftUp.x = CharacterRectangle.RightDown.x;
	Cursor.Positions.LeftUp.y = CharacterRectangle.LeftUp.y;
	Cursor.Positions.RightDown.x = CharacterRectangle.RightDown.x + 1;
	Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;

	// FOCUS TO CURSOR
	// IF TEXT WRAP NOT ENABLED
	if (!TextDisplayer.GetTextWrap())
	{
		// IF WIDTH EXCEEDED
		if (TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x > TextDisplayer.GetVisibleRectangle().RightDown.x)
		{
			float XMoveAmount = TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x - GetVisibleContentArea()->GetRectangle().RightDown.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - XMoveAmount, TextDisplayer.GetPosition().y));
			//TextDisplayer.SetWidth(TextDisplayer.GetWidth() + XMoveAmount);

		}
		// IF LAST CHARACTER EXCEEDS THE LIMIT THAN SET WIDTH 
		else if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x > TextDisplayer.GetVisibleRectangle().RightDown.x)
		{
			float LastCharacterWidth = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.x;
			//TextDisplayer.SetWidth(TextDisplayer.GetWidth() + LastCharacterWidth);
		}

		// IF HEIGHT EXCEEDED (ENTER KEY BEHAVIOUR)
		if (TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.y > GetVisibleContentArea()->GetRectangle().RightDown.y)
		{
			float YMoveAmount = TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.y - GetVisibleContentArea()->GetRectangle().RightDown.y;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y - YMoveAmount));
			//TextDisplayer.SetHeight(TextDisplayer.GetHeight() + YMoveAmount);
		}
		// IF LAST CHARACTER JUMPED TO NEXT LINE WHEN LAST CHARACTER IS OUTSIDE OF VISIBLE CONTENT AREA LIMIT THAN SET HEIGHT 
		else if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y > GetVisibleContentArea()->GetRectangle().RightDown.y)
		{
			if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y > TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 2].RenderableCharacter.Positions.RightDown.y)
			{
				float LastCharacterHeight = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.y;
				//TextDisplayer.SetHeight(TextDisplayer.GetHeight() + LastCharacterHeight);
				TextDisplayer.SetPosition(TextDisplayer.GetPosition());
			}
		}
		// IF ENTER KEY PRESSED THAN MOVE TEXTDISPLAYER TO START POSITION (TYPEWRITER BEHAVIOUR)
		if (TextDisplayer.GetTextControlCharacters()[CursorTextPosition - 1].RenderableCharacter.Positions.RightDown.y < TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.y)
		{
			TextDisplayer.SetPosition(ZEVector2(0.0f, TextDisplayer.GetPosition().y));
		}

	}
	// IF TEXT WRAP ENABLED
	else if (TextDisplayer.GetTextWrap())
	{
		// IF HEIGHT EXCEEDED
		if (TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.y > GetVisibleContentArea()->GetRectangle().RightDown.y)
		{
			float YMoveAmount = TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.y - GetVisibleContentArea()->GetRectangle().RightDown.y;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y - YMoveAmount));
			//TextDisplayer.SetHeight(TextDisplayer.GetHeight() + YMoveAmount);
		}
		// IF LAST CHARACTER JUMPED TO NEXT LINE WHEN LAST CHARACTER IS OUTSIDE OF VISIBLE CONTENT AREA LIMIT THAN SET HEIGHT 
		else if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y > GetVisibleContentArea()->GetRectangle().RightDown.y)
		{
			if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y > TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 2].RenderableCharacter.Positions.RightDown.y)
			{
				float LastCharacterHeight = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.y;
				//TextDisplayer.SetHeight(TextDisplayer.GetHeight() + LastCharacterHeight);
				TextDisplayer.SetPosition(TextDisplayer.GetPosition());
			}
		}
	}

	CursorTextPosition++;
	SelectedTextItemsPositions.Clear();

}
void ZEUIMultiLineTextEdit::CharacterKeyPressed(char Key)
{
	// IF THERE IS A SELECTION
	if (SelectedTextItemsPositions.GetCount() != 0)
	{
		for (int I = SelectedTextItemsPositions.GetCount() - 1; I >= 0; I--)
		{
			DeleteCharacter(SelectedTextItemsPositions[I]);
			TextDisplayer.DeleteCharacter(SelectedTextItemsPositions[I]);
		}

		//TextDisplayer.DeleteCharacters(SelectedTextItemsPositions);
		CursorTextPosition = SelectedTextItemsPositions.GetFirstItem();

		AddCharacter(Key);

		SelectedTextItemsPositions.Clear();
	}

	// IF THERE IS NO SELECTION
	else if (SelectedTextItemsPositions.GetCount() == 0)
		AddCharacter(Key);

	UpdateScrollbars();
}

void ZEUIMultiLineTextEdit::TextNavigation(int X, int Y)
{
	if (CursorTextPosition + X >= TextDisplayer.GetText().GetLength() || CursorTextPosition + X <= 0)
		return;

}

void ZEUIMultiLineTextEdit::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{	
	if (!TextDisplayer.GetFocused()) // HACKK!!!
		return;

	if (Button == ZE_UI_MOUSE_BUTTON_LEFT)
	{
		SelectCursorPosition(MousePosition);
	}

	ZEUIScrollArea::MouseButtonPressed(Button, MousePosition);
}

void ZEUIMultiLineTextEdit::MouseMoveEvent(ZEUIMouseKey Button, const ZEVector2& MoveAmount)
{
	if (!TextDisplayer.GetFocused()) // HACKK!!!
		return;

	if (Button == ZE_UI_MOUSE_BUTTON_LEFT)
	{		
		MultiCharacterSelection(MoveAmount);
	}
	ZEUIScrollArea::MouseMoveEvent(Button, MoveAmount);
}

void ZEUIMultiLineTextEdit::KeyPressed(ZEUIInputKey Key)
{
	if (ReadOnly == true)
		return;
	
	switch(Key)
	{
	case ZE_UI_IKB_ESCAPE:

		break;
	case ZE_UI_IKB_1:
		CharacterKeyPressed('1');
		break;
	case ZE_UI_IKB_2:
		CharacterKeyPressed('2');
		break;
	case ZE_UI_IKB_3:
		CharacterKeyPressed('3');
		break;
	case ZE_UI_IKB_4:
		CharacterKeyPressed('4');
		break;
	case ZE_UI_IKB_5:
		CharacterKeyPressed('5');
		break;
	case ZE_UI_IKB_6:
		CharacterKeyPressed('6');
		break;
	case ZE_UI_IKB_7:
		CharacterKeyPressed('7');
		break;
	case ZE_UI_IKB_8:
		CharacterKeyPressed('8');
		break;
	case ZE_UI_IKB_9:
		CharacterKeyPressed('9');
		break;
	case ZE_UI_IKB_0:
		CharacterKeyPressed('0');
		break;
	case ZE_UI_IKB_MINUS:

		break;
	case ZE_UI_IKB_EQUALS:

		break;
	case ZE_UI_IKB_TAB:
		for (size_t I = 0; I < 4; I++)
			CharacterKeyPressed(' ');
		break;
	case ZE_UI_IKB_Q:
		CharacterKeyPressed('q');
		break;
	case ZE_UI_IKB_W:
		CharacterKeyPressed('w');
		break;
	case ZE_UI_IKB_E:
		CharacterKeyPressed('e');
		break;
	case ZE_UI_IKB_R:
		CharacterKeyPressed('r');
		break;
	case ZE_UI_IKB_T:
		CharacterKeyPressed('t');
		break;
	case ZE_UI_IKB_Y:
		CharacterKeyPressed('y');
		break;
	case ZE_UI_IKB_U:
		CharacterKeyPressed('u');
		break;
	case ZE_UI_IKB_I:
		CharacterKeyPressed('i');
		break;
	case ZE_UI_IKB_O:
		CharacterKeyPressed('o');
		break;
	case ZE_UI_IKB_P:
		CharacterKeyPressed('p');
		break;
	case ZE_UI_IKB_LBRACKET:

		break;
	case ZE_UI_IKB_RBRACKET:

		break;
	case ZE_UI_IKB_RETURN:
		CharacterKeyPressed('\n');
		break;
	case ZE_UI_IKB_LCONTROL:

		break;
	case ZE_UI_IKB_A:
		CharacterKeyPressed('a');
		break;
	case ZE_UI_IKB_S:
		CharacterKeyPressed('s');
		break;
	case ZE_UI_IKB_D:
		CharacterKeyPressed('d');
		break;
	case ZE_UI_IKB_F:
		CharacterKeyPressed('f');
		break;
	case ZE_UI_IKB_G:
		CharacterKeyPressed('g');
		break;
	case ZE_UI_IKB_H:
		CharacterKeyPressed('h');
		break;
	case ZE_UI_IKB_J:
		CharacterKeyPressed('j');
		break;
	case ZE_UI_IKB_K:
		CharacterKeyPressed('k');
		break;
	case ZE_UI_IKB_L:
		CharacterKeyPressed('l');
		break;
	case ZE_UI_IKB_SEMICOLON:

		break;
	case ZE_UI_IKB_APOSTROPHE:

		break;
	case ZE_UI_IKB_LSHIFT:

		break;
	case ZE_UI_IKB_BACKSLASH:

		break;
	case ZE_UI_IKB_Z:
		CharacterKeyPressed('z');	
		break;
	case ZE_UI_IKB_X:
		CharacterKeyPressed('x');
		break;
	case ZE_UI_IKB_C:
		CharacterKeyPressed('c');
		break;
	case ZE_UI_IKB_V:
		CharacterKeyPressed('v');
		break;
	case ZE_UI_IKB_B:
		CharacterKeyPressed('b');
		break;
	case ZE_UI_IKB_N:
		CharacterKeyPressed('n');
		break;
	case ZE_UI_IKB_M:
		CharacterKeyPressed('m');
		break;
	case ZE_UI_IKB_COMMA:

		break;
	case ZE_UI_IKB_PERIOD:
		CharacterKeyPressed('.');
		break;
	case ZE_UI_IKB_SLASH:

		break;
	case ZE_UI_IKB_RSHIFT:

		break;
	case ZE_UI_IKB_MULTIPLY:

		break;
	case ZE_UI_IKB_SPACE:
		CharacterKeyPressed(' ');
		break;
	case ZE_UI_IKB_CAPITAL:

		break;
	case ZE_UI_IKB_F1:

		break;
	case ZE_UI_IKB_F2:

		break;
	case ZE_UI_IKB_F3:

		break;
	case ZE_UI_IKB_F4:

		break;
	case ZE_UI_IKB_F5:

		break;
	case ZE_UI_IKB_F6:

		break;
	case ZE_UI_IKB_F7:

		break;
	case ZE_UI_IKB_F8:

		break;
	case ZE_UI_IKB_F9:

		break;
	case ZE_UI_IKB_F10:

		break;
	case ZE_UI_IKB_NUMLOCK:

		break;
	case ZE_UI_IKB_SCROLL:

		break;
	case ZE_UI_IKB_NUMPAD7:

		break;
	case ZE_UI_IKB_NUMPAD8:

		break;
	case ZE_UI_IKB_NUMPAD9:

		break;
	case ZE_UI_IKB_SUBTRACT:

		break;
	case ZE_UI_IKB_NUMPAD4:

		break;
	case ZE_UI_IKB_NUMPAD5:

		break;
	case ZE_UI_IKB_NUMPAD6:

		break;
	case ZE_UI_IKB_ADD:

		break;
	case ZE_UI_IKB_NUMPAD1:

		break;
	case ZE_UI_IKB_NUMPAD2:

		break;
	case ZE_UI_IKB_NUMPAD3:

		break;
	case ZE_UI_IKB_NUMPAD0:

		break;
	case ZE_UI_IKB_DECIMAL:

		break;
	case ZE_UI_IKB_OEM_102:

		break;
	case ZE_UI_IKB_F11:

		break;
	case ZE_UI_IKB_F12:

		break;
	case ZE_UI_IKB_F13:

		break;
	case ZE_UI_IKB_F14:

		break;
	case ZE_UI_IKB_F15:

		break;
	case ZE_UI_IKB_KANA:

		break;
	case ZE_UI_IKB_ABNT_C1:

		break;
	case ZE_UI_IKB_CONVERT:

		break;
	case ZE_UI_IKB_NOCONVERT:

		break;
	case ZE_UI_IKB_YEN:

		break;
	case ZE_UI_IKB_ABNT_C2:

		break;
	case ZE_UI_IKB_NUMPADEQUALS:

		break;
	case ZE_UI_IKB_PREVTRACK:

		break;
	case ZE_UI_IKB_AT:

		break;
	case ZE_UI_IKB_COLON:

		break;
	case ZE_UI_IKB_UNDERLINE:

		break;
	case ZE_UI_IKB_KANJI:

		break;
	case ZE_UI_IKB_STOP:

		break;
	case ZE_UI_IKB_AX:

		break;
	case ZE_UI_IKB_UNLABELED:

		break;
	case ZE_UI_IKB_NEXTTRACK:

		break;
	case ZE_UI_IKB_NUMPADENTER:

		break;
	case ZE_UI_IKB_RCONTROL:

		break;
	case ZE_UI_IKB_MUTE:

		break;
	case ZE_UI_IKB_CALCULATOR:

		break;
	case ZE_UI_IKB_NUMPADCOMMA:

		break;
	case ZE_UI_IKB_DIVIDE:

		break;
	case ZE_UI_IKB_SYSRQ:

		break;
	case ZE_UI_IKB_RMENU:

		break;
	case ZE_UI_IKB_PAUSE:

		break;
	case ZE_UI_IKB_HOME:

		break;
	case ZE_UI_IKB_UP:

		break;
	case ZE_UI_IKB_PRIOR:

		break;
	case ZE_UI_IKB_LEFT:
		TextNavigation(-1, 0);
		break;
	case ZE_UI_IKB_RIGHT:
		TextNavigation(+1, 0);
		break;
	case ZE_UI_IKB_END:

		break;
	case ZE_UI_IKB_DOWN:

		break;
	case ZE_UI_IKB_NEXT:

		break;
	case ZE_UI_IKB_INSERT:

		break;
	case ZE_UI_IKB_DELETE:

		break;
	case ZE_UI_IKB_BACKSPACE:
		BackspaceKeyPressed();
		break;
	}

// 		if(Key != '\b')
// 			CharacterKeyPressed(Key);
// 
// 		else if (Key == '\b')
// 			BackspaceKeyPressed();
}	

void ZEUIMultiLineTextEdit::KeyReleased(ZEUIInputKey Key)
{
	switch (Key)
	{
		case ZE_UI_IKB_LSHIFT:

		break;
	}
}

void ZEUIMultiLineTextEdit::Tick(float ElapsedTime)
{
	TotalTime = TotalTime + ElapsedTime;

	if (TotalTime > CursorBlinkTime)
	{
		TotalTime = 0;
		if (CursorVisible == true)
			CursorVisible = false;

		else if (CursorVisible == false)
			CursorVisible= true;
	}

	ZEUIScrollArea::Tick(ElapsedTime);
}

void ZEUIMultiLineTextEdit::Draw(ZEUIRenderer* Renderer)
{
	ZEUIScrollArea::Draw(Renderer);

	ZEUIRectangle Output;

	if (CursorVisible && TextDisplayer.GetFocused())
		if (!ZEUIRectangle::Clip(Output, Cursor, GetVisibleContentArea()->GetRectangle()))
			Renderer->AddRectangle(Output);


// 	if (SelectedTextItemsRectangles.GetSize() != 0)
// 		for (size_t I = 0; I < SelectedTextItemsRectangles.GetSize(); I++)
// 			if (!ZEUIRectangle::Clip(Output, SelectedTextItemsRectangles[I], GetVisibleContentArea()->GetRectangle()))
// 				Renderer->AddRectangle(Output);

}


void ZEUIMultiLineTextEdit::AppendText(const ZEString& Value)
{
	if (TextDisplayer.GetText().GetLength() == 0)
		TextDisplayer.AddText(0, Value);
	
	else
		TextDisplayer.AddText(TextDisplayer.GetText().GetLength(), Value);

	if (TextDisplayer.GetRectangle().RightDown.y > GetVisibleContentArea()->GetRectangle().RightDown.y)
	{
		float HeightDifferance = TextDisplayer.GetRectangle().RightDown.y - GetVisibleContentArea()->GetRectangle().RightDown.y;
		TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x, TextDisplayer.GetPosition().y - HeightDifferance));
	}

// 	if (TextDisplayer.GetScreenRectangle().LeftUp.x < GetVisibleContentArea()->GetScreenRectangle().LeftUp.x)
// 	{
// 		float WidthDifference =  GetVisibleContentArea()->GetScreenRectangle().LeftUp.x - TextDisplayer.GetScreenRectangle().LeftUp.x;
// 		TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + WidthDifference, TextDisplayer.GetPosition().y));
// 	}

	UpdateScrollbars();
}

void ZEUIMultiLineTextEdit::SetText(const ZEString& Value)
{
	TextDisplayer.SetText(Value);

	// UPDATE CURSOR POSITION
	if (TextDisplayer.GetTextControlCharacters().GetCount() != 0)
	{
		ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown);

		Cursor.Positions.LeftUp.x = CharacterRectangle.RightDown.x;
		Cursor.Positions.LeftUp.y = CharacterRectangle.LeftUp.y;
		Cursor.Positions.RightDown.x = CharacterRectangle.RightDown.x + 1;
		Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;
	}

	else
	{
		TextDisplayer.SetPosition(GetRectangle().GetPosition());
		Cursor.Positions.LeftUp.x = TextDisplayer.GetPosition().x + 1;
		Cursor.Positions.LeftUp.y = TextDisplayer.GetPosition().y + 1;
		Cursor.Positions.RightDown.x = TextDisplayer.GetPosition().x + 2;
		Cursor.Positions.RightDown.y = TextDisplayer.GetPosition().y + 16;
	}

	CursorTextPosition = TextDisplayer.GetTextControlCharacters().GetCount();	
}

const ZEString& ZEUIMultiLineTextEdit::GetText()
{
	return TextDisplayer.GetText();
}

void ZEUIMultiLineTextEdit::SetTextColor(const ZEVector4& Color)
{
	TextDisplayer.SetTextColor(Color);
}

const ZEVector4& ZEUIMultiLineTextEdit::GetTextColor()
{
	return TextDisplayer.GetTextColor();
}

void ZEUIMultiLineTextEdit::SetTextWrap(bool Wrap)
{
	TextDisplayer.SetTextWrap(Wrap);
}

bool ZEUIMultiLineTextEdit::GetTextWrap()
{
	return TextDisplayer.GetTextWrap();
}

void ZEUIMultiLineTextEdit::SetFont(ZEFontResource* FontResource)
{
	TextDisplayer.SetFont(FontResource);
}

ZEFontResource* ZEUIMultiLineTextEdit::GetFont()
{
	return TextDisplayer.GetFont();
}

void ZEUIMultiLineTextEdit::SetFontSize(const ZEVector2& FontSize)
{
	TextDisplayer.SetFontSize(FontSize);
}

const ZEVector2& ZEUIMultiLineTextEdit::GetFontSize()
{
	return TextDisplayer.GetFontSize();
}

void ZEUIMultiLineTextEdit::SetReadOnly(bool ReadOnly)
{
	this->ReadOnly = ReadOnly;
}


bool ZEUIMultiLineTextEdit::GetReadOnly()
{
	return ReadOnly;
}

void ZEUIMultiLineTextEdit::SetMaterial(ZEMaterial* Material)
{

}

ZEMaterial* ZEUIMultiLineTextEdit::GetMaterial() const
{
	return NULL;
}

void ZEUIMultiLineTextEdit::SetWidth(float Width)
{
	ZEUIScrollArea::SetWidth(Width);
	TextDisplayer.SetWidth(GetVisibleContentArea()->GetWidth());
	UpdateScrollbars();
}

void ZEUIMultiLineTextEdit::SetHeight(float Height)
{
	ZEUIScrollArea::SetHeight(Height);
	TextDisplayer.SetHeight(GetVisibleContentArea()->GetHeight());
	UpdateScrollbars();
}

ZEUIMultiLineTextEdit::ZEUIMultiLineTextEdit()
{
	Cursor.Material = ZEUIMaterial::CreateInstance();
	Cursor.Color = ZEVector4(1.0f, 1.0f, 1.0f , 1.0f);

	SetWidth(200);
	SetHeight(200);
	SetPosition(ZEVector2::Zero);
	SetReadOnly(false);

	TextDisplayer.SetPosition(ZEVector2::Zero);
	TextDisplayer.SetBackgroundColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	TextDisplayer.SetTextColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
	SetTextWrap(true);
	TextDisplayer.SetFontSize(ZEVector2::One);
	TextDisplayer.SetFont(ZEFontResourceDynamic::LoadResource("Arial.ttf", 12));
	TextDisplayer.SetText("");

	TextDisplayer.SetPosition(ZEVector2(0,0));

	AddChildControl(&TextDisplayer);

	TextDisplayer.SetFocusable(true);
	SetFocusable(false);

	TextDisplayer.SetKeyPressedEvent(BindDelegate(this, &ZEUIMultiLineTextEdit::KeyPressed));
	TextDisplayer.SetKeyReleasedEvent(BindDelegate(this, &ZEUIMultiLineTextEdit::KeyReleased));
	TextDisplayer.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIMultiLineTextEdit::MouseButtonPressed));
	TextDisplayer.SetMouseMovedEvent(BindDelegate(this, &ZEUIMultiLineTextEdit::MouseMoveEvent));

	CursorBlinkTime = 0.5f;
	TotalTime = 0;
	CursorTextPosition = 0;
	CharacterScenePosition = ZEVector2(0,0);

	InitialMousePosition = ZEVector2(0,0);
	LastMousePosition = ZEVector2(0,0);

	CursorVisible = false;
	IsAnyCharacterClicked = false;
}
