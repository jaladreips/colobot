// editvalue.cpp

#define STRICT
#define D3D_OVERLOADS

#include <windows.h>
#include <stdio.h>
#include <d3d.h>

#include "struct.h"
#include "D3DEngine.h"
#include "math3d.h"
#include "event.h"
#include "misc.h"
#include "iman.h"
#include "edit.h"
#include "button.h"
#include "editvalue.h"




// Constructeur de l'objet.

CEditValue::CEditValue(CInstanceManager* iMan) : CControl(iMan)
{
	CControl::CControl(iMan);

	m_edit       = 0;
	m_buttonUp   = 0;
	m_buttonDown = 0;

	m_type = EVT_100;  // %
	m_stepValue = 0.1f;  // 10%
	m_minValue = 0.0f;  // 0%
	m_maxValue = 1.0f;  // 100%
}

// Destructeur de l'objet.

CEditValue::~CEditValue()
{
	delete m_edit;
	delete m_buttonUp;
	delete m_buttonDown;

	CControl::~CControl();
}


// Cr�e un nouveau bouton.

BOOL CEditValue::Create(FPOINT pos, FPOINT dim, int icon, EventMsg eventMsg)
{
	CEdit*		pe;
	CButton*	pc;

	if ( eventMsg == EVENT_NULL )  eventMsg = GetUniqueEventMsg();
	CControl::Create(pos, dim, icon, eventMsg);

	GlintDelete();

	m_edit = new CEdit(m_iMan);
	pe = (CEdit*)m_edit;
	pe->Create(pos, dim, 0, EVENT_NULL);
	pe->SetMaxChar(4);

	m_buttonUp = new CButton(m_iMan);
	pc = (CButton*)m_buttonUp;
	pc->Create(pos, dim, 49, EVENT_NULL);  // ^
	pc->SetRepeat(TRUE);

	m_buttonDown = new CButton(m_iMan);
	pc = (CButton*)m_buttonDown;
	pc->Create(pos, dim, 50, EVENT_NULL);  // v
	pc->SetRepeat(TRUE);

	MoveAdjust();
	return TRUE;
}


void CEditValue::SetPos(FPOINT pos)
{
	CControl::SetPos(pos);
	MoveAdjust();
}

void CEditValue::SetDim(FPOINT dim)
{
	CControl::SetDim(dim);
	MoveAdjust();
}

void CEditValue::MoveAdjust()
{
	FPOINT		pos, dim;

	if ( m_edit != 0 )
	{
		pos.x = m_pos.x;
		pos.y = m_pos.y;
		dim.x = m_dim.x-m_dim.y*0.6f;
		dim.y = m_dim.y;
		m_edit->SetPos(pos);
		m_edit->SetDim(dim);
	}

	if ( m_buttonUp != 0 )
	{
		pos.x = m_pos.x+m_dim.x-m_dim.y*0.6f;
		pos.y = m_pos.y+m_dim.y*0.5f;
		dim.x = m_dim.y*0.6f;
		dim.y = m_dim.y*0.5f;
		m_buttonUp->SetPos(pos);
		m_buttonUp->SetDim(dim);
	}

	if ( m_buttonDown != 0 )
	{
		pos.x = m_pos.x+m_dim.x-m_dim.y*0.6f;
		pos.y = m_pos.y;
		dim.x = m_dim.y*0.6f;
		dim.y = m_dim.y*0.5f;
		m_buttonDown->SetPos(pos);
		m_buttonDown->SetDim(dim);
	}
}


// Gestion d'un �v�nement.

BOOL CEditValue::EventProcess(const Event &event)
{
	float	value;
	BOOL	bShift;

	CControl::EventProcess(event);

	if ( (m_state & STATE_VISIBLE) == 0 )  return TRUE;
	if ( (m_state & STATE_ENABLE) == 0 )  return TRUE;

	bShift = (event.keyState&KS_SHIFT);

	if ( m_edit != 0 )
	{
		if ( m_edit->RetFocus()           &&
			 event.event == EVENT_KEYDOWN &&
			 event.param == VK_RETURN     )
		{
			value = RetValue();
			if ( value > m_maxValue )  value = m_maxValue;
			if ( value < m_minValue )  value = m_minValue;
			SetValue(value, TRUE);
			HiliteValue(event);
		}
		if ( !m_edit->EventProcess(event) )  return FALSE;

		if ( event.event == m_edit->RetEventMsg() )
		{
			Event		newEvent;
			m_event->MakeEvent(newEvent, m_eventMsg);
			m_event->AddEvent(newEvent);
		}
	}

	if ( m_buttonUp != 0 )
	{
		if ( event.event == m_buttonUp->RetEventMsg() )
		{
			value = RetValue()+m_stepValue;
			if ( value > m_maxValue )  value = m_maxValue;
			SetValue(value, TRUE);
			HiliteValue(event);
		}
		if ( !m_buttonUp->EventProcess(event) )  return FALSE;
	}

	if ( m_buttonDown != 0 )
	{
		if ( event.event == m_buttonDown->RetEventMsg() )
		{
			value = RetValue()-m_stepValue;
			if ( value < m_minValue )  value = m_minValue;
			SetValue(value, TRUE);
			HiliteValue(event);
		}
		if ( !m_buttonDown->EventProcess(event) )  return FALSE;
	}

	if ( event.event == EVENT_KEYDOWN &&
		 event.param == VK_WHEELUP    &&
		 Detect(event.pos)            )
	{
		value = RetValue()+m_stepValue;
		if ( value > m_maxValue )  value = m_maxValue;
		SetValue(value, TRUE);
		HiliteValue(event);
	}
	if ( event.event == EVENT_KEYDOWN &&
		 event.param == VK_WHEELDOWN  &&
		 Detect(event.pos)            )
	{
		value = RetValue()-m_stepValue;
		if ( value < m_minValue )  value = m_minValue;
		SetValue(value, TRUE);
		HiliteValue(event);
	}

	if ( m_bFocus &&
		 (m_state & STATE_VISIBLE) &&
		 (m_state & STATE_ENABLE)  &&
		 event.event == EVENT_KEYDOWN &&
		 (event.param == VK_UP ||
		  event.param == VK_JUP) )
	{
		value = RetValue()+m_stepValue;
		if ( value > m_maxValue )  value = m_maxValue;
		SetValue(value, TRUE);
		HiliteValue(event);
	}
	if ( m_bFocus &&
		 (m_state & STATE_VISIBLE) &&
		 (m_state & STATE_ENABLE)  &&
		 event.event == EVENT_KEYDOWN &&
		 (event.param == VK_DOWN ||
		  event.param == VK_JDOWN) )
	{
		value = RetValue()-m_stepValue;
		if ( value < m_minValue )  value = m_minValue;
		SetValue(value, TRUE);
		HiliteValue(event);
	}

	return TRUE;
}


// Met en �vidence la valeur �dit�e.

void CEditValue::HiliteValue(const Event &event)
{
	int		pos;

	if ( m_edit == 0 )  return;

	pos = m_edit->RetTextLength();
	if ( m_type == EVT_100 && pos > 0 )
	{
		pos --;  // ne s�lectionne pas le "%"
	}

	m_edit->SetCursor(pos, 0);
	m_edit->SetFocus(TRUE);

	Event newEvent = event;
	newEvent.event = EVENT_FOCUS;
	newEvent.param = m_edit->RetEventMsg();
	m_event->AddEvent(newEvent);  // d�focus les autres objets
}


// Dessine le bouton.

void CEditValue::Draw()
{
	if ( (m_state & STATE_VISIBLE) == 0 )  return;

	if ( m_state & STATE_SHADOW )
	{
		DrawShadow(m_pos, m_dim);
	}
	if ( m_bFocus )
	{
		DrawFocus(m_pos, m_dim);
	}

	if ( m_edit != 0 )
	{
		m_edit->Draw();
	}
	if ( m_buttonUp != 0 )
	{
		m_buttonUp->SetState(STATE_SIMPLY);
		m_buttonUp->Draw();
	}
	if ( m_buttonDown != 0 )
	{
		m_buttonDown->SetState(STATE_SIMPLY);
		m_buttonDown->Draw();
	}
}


// Choix du type de la valeur.

void CEditValue::SetType(EditValueType type)
{
	m_type = type;
}

EditValueType CEditValue::RetType()
{
	return m_type;
}


// Modifie la valeur.

void CEditValue::SetValue(float value, BOOL bSendMessage)
{
	char	text[100];

	if ( m_edit == 0 )  return;

	text[0] = 0;

	if ( m_type == EVT_INT )
	{
		sprintf(text, "%d", (int)value);
	}

	if ( m_type == EVT_FLOAT )
	{
		sprintf(text, "%.2f", value);
	}

	if ( m_type == EVT_100 )
	{
		sprintf(text, "%d%%", (int)(value*100.0f));
	}

	m_edit->SetText(text);

	if ( bSendMessage )
	{
		Event		newEvent;
		m_event->MakeEvent(newEvent, m_eventMsg);
		m_event->AddEvent(newEvent);
	}
}

// Retourne la valeur �dit�e.

float CEditValue::RetValue()
{
	char	text[100];
	float	value;

	if ( m_edit == 0 )  0.0f;

	m_edit->GetText(text, 100);
	sscanf(text, "%f", &value);

	if ( m_type == EVT_100 )
	{
		value = (value+0.5f)/100.0f;
		if ( value < 0.01f )  value = 0.0f;  // moins que 1% ?
	}

	return value;
}


// Gestion du pas pour les boutons.

void CEditValue::SetStepValue(float value)
{
	m_stepValue = value;
}

float CEditValue::RetStepValue()
{
	return m_stepValue;
}


// Gestion de la valeur minimale.

void CEditValue::SetMinValue(float value)
{
	m_minValue = value;
}

float CEditValue::RetMinValue()
{
	return m_minValue;
}


// Gestion de la valeur maximale.

void CEditValue::SetMaxValue(float value)
{
	m_maxValue = value;
}

float CEditValue::RetMaxValue()
{
	return m_maxValue;
}
