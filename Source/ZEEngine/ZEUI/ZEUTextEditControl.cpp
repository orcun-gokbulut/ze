//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUTextEditControl.cpp
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

#include "ZEUTextEditControl.h"
#include "ZEFontResourceBitmap.h"
#include "ZEUIRenderer.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZEUIMaterial.h"

void ZEUITextEditControl::SelectCursorPosition(ZEVector2 MousePosition)
{
	if (TextDisplayer.GetText().GetLength() == 0)
	{
		Cursor.Positions.LeftUp.x = TextDisplayer.GetScreenPosition().x + 1;
		Cursor.Positions.LeftUp.y = TextDisplayer.GetScreenPosition().y + 1;
		Cursor.Positions.RightDown.x = TextDisplayer.GetScreenPosition().x + 2;
		Cursor.Positions.RightDown.y = TextDisplayer.GetScreenPosition().y + 16;

		CursorTextPosition = 0;	
		return;
	}

	InitialMousePosition = MousePosition;
	LastMousePosition = MousePosition;
	SelectedTextItemsRectangles.Clear();
	SelectedTextItemsPositions.Clear();

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
	for (size_t I = 0; I < TextDisplayer.GetTextControlCharacters().GetCount(); I++)
	{
		if (TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.LeftUp.y < MousePosition.y && TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.RightDown.y > MousePosition.y)
		{
			if (I + 1 < TextDisplayer.GetTextControlCharacters().GetCount())
			{
				if (TextDisplayer.GetTextControlCharacters()[I + 1].RenderableCharacter.Positions.LeftUp.y > TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.LeftUp.y)
				{
					ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[I].RenderableCharacter.Positions.RightDown);

					Cursor.Positions.LeftUp.x = CharacterRectangle.RightDown.x;
					Cursor.Positions.LeftUp.y = CharacterRectangle.LeftUp.y;
					Cursor.Positions.RightDown.x = CharacterRectangle.RightDown.x + 1;
					Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;

					CursorTextPosition = I;
					return;
				}
			}
		}
	}

	// IF MOUSE PRESS IS OUTSIDE THE TEXT
	ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown);

	Cursor.Positions.LeftUp.x = CharacterRectangle.RightDown.x;
	Cursor.Positions.LeftUp.y = CharacterRectangle.LeftUp.y;
	Cursor.Positions.RightDown.x = CharacterRectangle.RightDown.x + 1;
	Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;

	CursorTextPosition = TextDisplayer.GetTextControlCharacters().GetCount();	

}

void ZEUITextEditControl::MultiCharacterSelection(ZEVector2 MoveAmount)
{
	if (TextDisplayer.GetText().GetLength() == 0)
		return;

	LastMousePosition = LastMousePosition + MoveAmount; // HACK

	size_t FromCharacter = 0;
	size_t ToCharacter = 0;

	size_t HoveredCharacterPosition = 0;

	bool AnyCharacterIntersected = false;

	SelectedTextItemsRectangles.Clear();
	SelectedTextItemsPositions.Clear();		

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
		}
	}

	// IF THERE IS NO INTERSECTION
	if (AnyCharacterIntersected == false)
	{
		// IF MOUSE POSITION IS BIGGER THAN LAST CHARACTER RIGHT DOWN THEN SELECT ALL ELEMENTS BETWEEN CURSORTEXTPOSITION AND LAST CHARACTER
		if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x < LastMousePosition.x || TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y < LastMousePosition.y )
		{
			HoveredCharacterPosition = TextDisplayer.GetTextControlCharacters().GetCount();

			Cursor.Positions.LeftUp.x = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x;
			Cursor.Positions.LeftUp.y = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.y;
			Cursor.Positions.RightDown.x = Cursor.Positions.LeftUp.x + 1;
			Cursor.Positions.RightDown.y = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.y;	
		}
		else
		{
			Cursor.Positions.LeftUp.x = TextDisplayer.GetTextControlCharacters()[0].RenderableCharacter.Positions.LeftUp.x;
			Cursor.Positions.LeftUp.y = TextDisplayer.GetTextControlCharacters()[0].RenderableCharacter.Positions.LeftUp.y;
			Cursor.Positions.RightDown.x = Cursor.Positions.LeftUp.x + 1;
			Cursor.Positions.RightDown.y = TextDisplayer.GetTextControlCharacters()[0].RenderableCharacter.Positions.RightDown.y;	
		}
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

		SelectedCharacter.Material = HightlightMaterial;
		SelectedCharacter.Color = ZEVector4(0.0f, 0.0f, 1.0f , 0.5f);

		SelectedTextItemsRectangles.Add(SelectedCharacter);
		SelectedTextItemsPositions.Add(I);
	}

	if (SelectedTextItemsRectangles.GetSize() == 0)
		return;

	// ON SELECTION IF MOUSE EXCEEDS VISIBLE RECTANGLE SCROLL TEXT
	// MOVING RIGHT
	if (LastMousePosition.x + 10.0f > TextDisplayer.GetVisibleRectangle().RightDown.x && TextDisplayer.GetRectangle().RightDown.x > TextDisplayer.GetVisibleRectangle().RightDown.x)
	{
		if (ToCharacter < TextDisplayer.GetText().GetLength() - 1)
		{
			float MoveX = TextDisplayer.GetTextControlCharacters()[ToCharacter].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[ToCharacter].RenderableCharacter.Positions.LeftUp.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - MoveX, TextDisplayer.GetPosition().y));
		}
		else if (ToCharacter = TextDisplayer.GetText().GetLength() - 1)
		{
			float MoveX = TextDisplayer.GetRectangle().RightDown.x - TextDisplayer.GetVisibleRectangle().RightDown.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - MoveX, TextDisplayer.GetPosition().y));
		}
	}

	// MOVING LEFT
	else if (LastMousePosition.x - 10.0f < TextDisplayer.GetVisibleRectangle().LeftUp.x && TextDisplayer.GetRectangle().LeftUp.x < TextDisplayer.GetVisibleRectangle().LeftUp.x)
	{
		if (FromCharacter > 0)
		{
			float MoveX = TextDisplayer.GetTextControlCharacters()[FromCharacter - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[FromCharacter - 1].RenderableCharacter.Positions.LeftUp.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + MoveX, TextDisplayer.GetPosition().y));
		}
		else if (FromCharacter == 0)
		{
			float MoveX = TextDisplayer.GetVisibleRectangle().LeftUp.x - TextDisplayer.GetRectangle().LeftUp.x;
			TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + MoveX, TextDisplayer.GetPosition().y));
		}
	}
}

void ZEUITextEditControl::DeleteCharacter(int Position)
{
	// IF THERE IS NO TEXT OR THERE IS NO CHARACTER BEFORE CURSOR WHEN NO SELECTION
	if (TextDisplayer.GetText().GetLength() == 0 || (CursorTextPosition == 0 && SelectedTextItemsPositions.GetCount() == 0))
		return;

	ZEString Manipulator = TextDisplayer.GetText();
	ZERectangle CharacterRectangle = ZERectangle(TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.LeftUp, TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.RightDown);

	Cursor.Positions.LeftUp.x = CharacterRectangle.LeftUp.x;
	Cursor.Positions.LeftUp.y = CharacterRectangle.LeftUp.y;
	Cursor.Positions.RightDown.x = CharacterRectangle.LeftUp.x + 1;
	Cursor.Positions.RightDown.y = CharacterRectangle.RightDown.y;

	// FOCUS TO CURSOR
	// IF TEXT WRAP NOT ENABLED
	if (!TextDisplayer.GetTextWrap())
	{
		if (TextDisplayer.GetPosition().x < TextDisplayer.GetVisibleRectangle().LeftUp.x)
		{
			// If LAST CHARACTER POSITION SMALLER THAN VISIBLE RECTANGLE
			if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.x < TextDisplayer.GetVisibleRectangle().RightDown.x)
			{
				float XMoveAmount = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + XMoveAmount, TextDisplayer.GetPosition().y));
				//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - XMoveAmount);
				Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x + XMoveAmount, Cursor.Positions.GetPosition().y));
			}
			// IF CURSOR POSITION EXCEEDS THE LEFT BOUND (WILL SHOW TWO CHARACTERS THAT EXCEEDS THE BOUND)
			else if (Position - 2 >= 0 && TextDisplayer.GetTextControlCharacters()[Position - 2].RenderableCharacter.Positions.LeftUp.x < TextDisplayer.GetVisibleRectangle().LeftUp.x)
			{
				float XMoveAmount = TextDisplayer.GetVisibleRectangle().LeftUp.x - TextDisplayer.GetTextControlCharacters()[Position - 2].RenderableCharacter.Positions.LeftUp.x;
				float CharacterWidth = TextDisplayer.GetTextControlCharacters()[Position - 2].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[Position - 2].RenderableCharacter.Positions.LeftUp.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + XMoveAmount, TextDisplayer.GetPosition().y));
				//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - CharacterWidth);
				Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x + XMoveAmount, Cursor.Positions.GetPosition().y));
			}
			// IF THERE ARE NO TWO CHARACTERS THAN
			else if(TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.LeftUp.x < TextDisplayer.GetVisibleRectangle().LeftUp.x)
			{
				float XMoveAmount = TextDisplayer.GetVisibleRectangle().LeftUp.x - TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.LeftUp.x;
				float CharacterWidth = TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[Position].RenderableCharacter.Positions.LeftUp.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x + XMoveAmount, TextDisplayer.GetPosition().y));
				//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - CharacterWidth);
				Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x + XMoveAmount, Cursor.Positions.GetPosition().y));
			}			

			// IF LAST CHARACTER EXCEEDS THAN THE LIMIT THAN SET WIDTH 
			else if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x > TextDisplayer.GetVisibleRectangle().RightDown.x)
			{
				float LastCharacterWidth = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.x;

				if (LastCharacterWidth < TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetVisibleRectangle().RightDown.x)
				{
					//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - LastCharacterWidth);
				}
				else
				{
					float LastCharacterWidth = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetVisibleRectangle().RightDown.x;
					//TextDisplayer.SetWidth(TextDisplayer.GetWidth() - LastCharacterWidth);
				}
			}
		}
	}

// 	Manipulator.Remove(Position);
// 	TextDisplayer.SetText(Manipulator);

	CursorTextPosition--;
}

void ZEUITextEditControl::BackspaceKeyPressed()
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

		SelectedTextItemsPositions.Clear();
		SelectedTextItemsRectangles.Clear();
	}

	// IF THERE IS NO SELECTION
	else if (SelectedTextItemsPositions.GetCount() == 0)
	{
		DeleteCharacter(CursorTextPosition - 1);
		TextDisplayer.DeleteCharacter(CursorTextPosition);
	}
}

void ZEUITextEditControl::AddCharacter(char Key)
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
			// IF THERE ARE AT LEAST 6 CHARACTERS AFTER CURSOR POSITION THEN SHOW THAT CHARACTERS TOO
			if (CursorTextPosition + 6 <= TextDisplayer.GetTextControlCharacters().GetCount() - 1)
			{
				float XMoveAmount = TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetVisibleRectangle().RightDown.x + (TextDisplayer.GetTextControlCharacters()[CursorTextPosition + 6].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x);
				float CharacterWidth = TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.LeftUp.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - XMoveAmount, TextDisplayer.GetPosition().y));
				TextDisplayer.SetWidth(TextDisplayer.GetWidth() + CharacterWidth);
				Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x - XMoveAmount, Cursor.Positions.GetPosition().y));
			}
			// IF ANY CHARACTER AFTER CURSOR POSITION
			else if (CursorTextPosition + 1 < TextDisplayer.GetTextControlCharacters().GetCount() - 1)
			{
				float XMoveAmount = TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetVisibleRectangle().RightDown.x + (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x);
				float CharacterWidth = TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.LeftUp.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - XMoveAmount, TextDisplayer.GetPosition().y));
				TextDisplayer.SetWidth(TextDisplayer.GetWidth() + CharacterWidth);
				Cursor.Positions.SetPosition(ZEVector2(Cursor.Positions.GetPosition().x - XMoveAmount, Cursor.Positions.GetPosition().y));
			}
			// IF NEWLY INSERTED CHARACTER IS THE LAST CHARACTER
			else
			{
				float XMoveAmount = TextDisplayer.GetTextControlCharacters()[CursorTextPosition].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetVisibleRectangle().RightDown.x;
				TextDisplayer.SetPosition(ZEVector2(TextDisplayer.GetPosition().x - XMoveAmount, TextDisplayer.GetPosition().y));
				//TextDisplayer.SetWidth(TextDisplayer.GetWidth() + XMoveAmount);
			}
		}
		// IF LAST CHARACTER EXCEEDS THE LIMIT THAN SET WIDTH 
		else if (TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x > TextDisplayer.GetVisibleRectangle().RightDown.x)
		{
			float LastCharacterWidth = TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.RightDown.x - TextDisplayer.GetTextControlCharacters()[TextDisplayer.GetTextControlCharacters().GetCount() - 1].RenderableCharacter.Positions.LeftUp.x;
			TextDisplayer.SetWidth(TextDisplayer.GetWidth() + LastCharacterWidth);
		}
	}

	CursorTextPosition++;
	SelectedTextItemsRectangles.Clear();
	SelectedTextItemsPositions.Clear();

}

void ZEUITextEditControl::CharacterKeyPressed(char Key)
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
		SelectedTextItemsRectangles.Clear();
	}

	// IF THERE IS NO SELECTION
	else if (SelectedTextItemsPositions.GetCount() == 0)
		AddCharacter(Key);
}

void ZEUITextEditControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{	
	if (Button == ZE_UI_MOUSE_BUTTON_LEFT)
	{
		SelectCursorPosition(MousePosition);
	}

	ZEUIControl::MouseButtonPressed(Button, MousePosition);
}

void ZEUITextEditControl::MouseMoveEvent(ZEUIMouseKey Button, const ZEVector2& MoveAmount)
{
	if (Button == ZE_UI_MOUSE_BUTTON_LEFT)
	{		
		MultiCharacterSelection(MoveAmount);
	}
	ZEUIControl::MouseMoveEvent(Button, MoveAmount);
}

void ZEUITextEditControl::KeyPressed(ZEUIInputKey Key)
{
	ZEUIControl::KeyPressed(Key);
	if (ReadOnly == true || Key == ZE_UI_IKB_RETURN)
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

		break;
	case ZE_UI_IKB_RIGHT:

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
}	

void ZEUITextEditControl::Tick(float ElapsedTime)
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

	ZEUIFrameControl::Tick(ElapsedTime);
}

void ZEUITextEditControl::Draw(ZEUIRenderer* Renderer)
{
	ZEUIFrameControl::Draw(Renderer);

	ZEUIRectangle Output;
	Output.ZOrder = GetZOrder() + 1;

	if (CursorVisible && TextDisplayer.GetFocused())
		if (!ZEUIRectangle::Clip(Output, Cursor, TextDisplayer.GetVisibleRectangle()))
			Renderer->AddRectangle(Output);


	if (SelectedTextItemsRectangles.GetSize() != 0)
		for (size_t I = 0; I < SelectedTextItemsRectangles.GetSize(); I++)
			if (!ZEUIRectangle::Clip(Output, SelectedTextItemsRectangles[I], TextDisplayer.GetVisibleRectangle()))
				{
					Output.ZOrder = GetZOrder() + 2;
					Renderer->AddRectangle(Output);
				}
}

void ZEUITextEditControl::AppendText(const ZEString& Value)
{
	ZEString Text = TextDisplayer.GetText();
	Text.Append(Value);

	TextDisplayer.SetText(Text);
}

void ZEUITextEditControl::SetText(const ZEString& Value)
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

const ZEString& ZEUITextEditControl::GetText()
{
	return TextDisplayer.GetText();
}

void ZEUITextEditControl::SetTextColor(const ZEVector4& Color)
{
	TextDisplayer.SetTextColor(Color);
}

const ZEVector4& ZEUITextEditControl::GetTextColor()
{
	return TextDisplayer.GetTextColor();
}

void ZEUITextEditControl::SetFont(ZEFontResource* FontResource)
{
	TextDisplayer.SetFont(FontResource);
}

ZEFontResource* ZEUITextEditControl::GetFont()
{
	return TextDisplayer.GetFont();
}

void ZEUITextEditControl::SetFontSize(const ZEVector2& FontSize)
{
	TextDisplayer.SetFontSize(FontSize);
}

const ZEVector2& ZEUITextEditControl::GetFontSize()
{
	return TextDisplayer.GetFontSize();
}

void ZEUITextEditControl::SetReadOnly(bool ReadOnly)
{
	this->ReadOnly = ReadOnly;
}

bool ZEUITextEditControl::GetReadOnly()
{
	return ReadOnly;
}

void ZEUITextEditControl::SetMaterial(ZEMaterial* Material)
{
	
}

ZEMaterial* ZEUITextEditControl::GetMaterial() const
{
	return NULL;
}

void ZEUITextEditControl::SetSize(const ZEVector2& Size)
{
	SetWidth(Size.x);
	SetHeight(Size.y);
}

void ZEUITextEditControl::SetWidth(float Width)
{
	ZEUIFrameControl::SetWidth(Width);
	TextDisplayer.SetWidth(Width);
}

void ZEUITextEditControl::SetHeight(float Height)
{
	ZEUIFrameControl::SetHeight(Height);
	TextDisplayer.SetHeight(Height);
}

ZEUITextEditControl::ZEUITextEditControl()
{
	Cursor.Material = ZEUIMaterial::CreateInstance();
	Cursor.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);

	HightlightMaterial = ZEUIMaterial::CreateInstance();

	SetWidth(400);
	SetHeight(24);
	SetReadOnly(false);

//	TextDisplayer.SetSize(ZEVector2(400, 25));
	TextDisplayer.SetBackgroundColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	TextDisplayer.SetTextColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
	TextDisplayer.SetText("");
	TextDisplayer.SetTextWrap(false);
	TextDisplayer.SetFontSize(ZEVector2::One);
	TextDisplayer.SetFont(ZEFontResourceBitmap::LoadResource("Courier New.zeFont"));

	SetPosition(ZEVector2::Zero);
	TextDisplayer.SetPosition(ZEVector2(0,1));

	AddChildControl(&TextDisplayer);

	TextDisplayer.SetFocusable(true);

	TextDisplayer.SetKeyPressedEvent(BindDelegate(this, &ZEUITextEditControl::KeyPressed));
	TextDisplayer.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUITextEditControl::MouseButtonPressed));
	TextDisplayer.SetMouseMovedEvent(BindDelegate(this, &ZEUITextEditControl::MouseMoveEvent));

	CursorBlinkTime = 0.5f;
	TotalTime = 0;
	CursorTextPosition = 0;
	CharacterScenePosition = ZEVector2(0,0);

	InitialMousePosition = ZEVector2(0,0);
	LastMousePosition = ZEVector2(0,0);

	CursorVisible = false;
	IsAnyCharacterClicked = false;

	Frame.Color = ZEVector4(0.2f, 0.2f, 0.2f, 1.0f);
}
