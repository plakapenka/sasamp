#include "main.h"

#include "game/game.h"
#include "keyboardhistory.h"
#include "keyboard.h"

CKeyBoardHistory::CKeyBoardHistory()
{
	Log("Initalizing KeyBoardHistory..");
	m_iCounter = 0;
}

CKeyBoardHistory::~CKeyBoardHistory()
{
}


void CKeyBoardHistory::AddStringToHistory(const std::string& msg)
{
	if (msg.size() == 0) return;
	m_Buffer.insert(m_Buffer.begin(), msg);
	while (m_Buffer.size() >= 20)
	{
		m_Buffer.pop_back();
	}
}

void CKeyBoardHistory::PageUp()
{
	if (!m_Buffer.size())
	{
		m_iCounter = 0;
		return;
	}
	m_iCounter++;
	if (m_iCounter >= m_Buffer.size() + 1)
	{
		m_iCounter = m_Buffer.size();
		return;
	}
	CKeyBoard::Flush();
	AddTextToBuffer(m_Buffer[m_iCounter - 1]);
}

void CKeyBoardHistory::PageDown()
{
	m_iCounter--;
	if (m_iCounter <= 0)
	{
		m_iCounter = 0;
		CKeyBoard::Flush();
		return;
	}
	CKeyBoard::Flush();
	AddTextToBuffer(m_Buffer[m_iCounter - 1]);
}

void CKeyBoardHistory::ResetPointer()
{
	m_iCounter = 0;
}

void CKeyBoardHistory::AddTextToBuffer(const std::string& msg)
{
	for (int i = 0; i < msg.size(); i++)
		CKeyBoard::AddCharToInput((char)msg[i]);
}