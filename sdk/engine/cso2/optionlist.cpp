#include "optionlist.h"

BaseOptionList::BaseOptionList()
{
	m_Unknown00 = 0;
	m_Unknown01 = 0;
}

ModOptionList::ModOptionList()
{
	m_Unknown02 = 0;
	m_Unknown03 = 0;
	m_Unknown04 = 0;
	m_Unknown05 = 0;
	m_Unknown06 = 255;
}

AutoMatchOptionList::AutoMatchOptionList()
{
	m_Unknown03 = 0;
}
