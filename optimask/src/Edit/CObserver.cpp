#include "CObserver.h"

#include "./Edit/CObserver.h"
#include "Subject.h"

CObserver::CObserver()
{
}

CObserver::~CObserver()
{
  ClearSubjects();
}

void CObserver::ClearSubjects()
{
  E2S::iterator E2SI = m_E2Subjects.begin();
  while(E2SI != m_E2Subjects.end())
  {
    Subject* pSubject = (E2SI->second );
    pSubject->DetachObserver(this);
    E2SI = m_E2Subjects.begin();
  }
  m_E2Subjects.clear();

}

void CObserver::NotifySubject(unsigned int Event, const std::stringstream& Data)
{
  ESPairI subjects = m_E2Subjects.equal_range(Event);
  ESI esi = subjects.first;
  while(esi != subjects.second)
  {
    Subject* pSubject = esi->second;
    pSubject->Notify(this,Event,Data);
    ++esi;
  }

}



void CObserver::RemoveSubject(Subject* pSubject)
{
  ESI esi = m_E2Subjects.begin();
  for(; esi != m_E2Subjects.end();)
  {
    if(esi->second == pSubject)
    {
      m_E2Subjects.erase(esi);
    }
    else
    {
      esi ++;
    }
  }

}

void CObserver::Observe(Subject* pSubject, unsigned int Event)
{
  m_E2Subjects.insert(ESPair(Event, pSubject));
  pSubject->AttachObserver(this, Event);
  //m_bOnUpdating = false;
}

void CObserver::UnObserve(Subject* pSubject, unsigned int Event)
{
  pSubject->DetachObserver(this, Event);
}

