#pragma once

#include "../../xrUI/Widgets/UIWindow.h"
#include "../../xrUI/xrUIXmlParser.h"

class CUIStatic;
class CUITextWnd;
struct GAME_NEWS_DATA;

class CUINewsItemWnd :public CUIWindow
{
	typedef	CUIWindow		inherited;

	CUITextWnd*				m_UIDate;
	CUITextWnd*				m_UICaption;
	CUITextWnd*				m_UIText;
	CUIStatic*				m_UIImage;

public:
					CUINewsItemWnd		();
	virtual			~CUINewsItemWnd		();
			void	Init				(CUIXml& uiXml, LPCSTR start_from);
			void	Setup				(GAME_NEWS_DATA& news_data);
	virtual	void	Update				(){};
};