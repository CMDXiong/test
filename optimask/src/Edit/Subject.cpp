#include "Subject.h"

#include "./Edit/Subject.h"
#include "./Edit/CObserver.h"

Subject::Subject()
{
}

Subject::~Subject()
{
  ClearObservers();
}

void Subject::ClearObservers()
{
  E2O::iterator OI = m_Observers.begin();
  while(OI != m_Observers.end())
  {
    OI->second->RemoveSubject(this);
    m_Observers.erase(OI);
    OI++;
  }
}

void Subject::AttachObserver(CObserver* pObserver, unsigned int Event)
{
    EOPairI observers = m_Observers.equal_range(Event);
    EOI eoi = observers.first;
    bool bAlreadyExist = false;
    while(eoi != observers.second)
    {
        CObserver* pO = eoi->second;
        if(pO == pObserver)
        {
            bAlreadyExist = true;
            break;
        }
        ++eoi;
    }

    if (!bAlreadyExist)
    {
        m_Observers.insert(EOPair(Event, pObserver));
    }
}

void Subject::DetachObserver(CObserver* pObserver, unsigned int Event)
{
  EOPairI observers = m_Observers.equal_range(Event);
  EOI eoi = observers.first;
  while(eoi != observers.second)
  {
    CObserver* pO = eoi->second;
    if(pO == pObserver)
    {
      m_Observers.erase(eoi);
      break;
    }
    ++eoi;
  }
  // unregister the CObserver from this subject if there is no longer any pObserver registered with this subject;
  if(!IsObserverRegistered(pObserver))
  {
    pObserver->RemoveSubject(this);
  }

}

void Subject::DetachObserver(CObserver* pObserver)
{
  E2O::iterator OI = m_Observers.begin();
  while(OI != m_Observers.end())
  {
    if(OI->second == pObserver)
    {
      m_Observers.erase(OI);
      OI = m_Observers.begin();
      continue;
    }
    ++OI;
  }
  // unregister the CObserver from this subject as well;
  pObserver->RemoveSubject(this);
}

void Subject::Notify(CObserver* pObserverFrom, unsigned int Event, const std::stringstream& Data)
{
    EOPairI observers = m_Observers.equal_range(Event);
    EOI eoi = observers.first;

/*
    while(eoi != observers.second)
    {
        CObserver* pO = eoi->second;
        ++eoi;
        if(pO != pObserverFrom)
        {
            pO->Update(this, pObserverFrom, Event, Data);
        }
    }
*/

    if (eoi != observers.second)
    {
        EOI eoiLast = observers.second;
        --eoiLast;

        while (true)
        {
            if (eoi == eoiLast)
            {
                CObserver* pO = eoi->second;
                if(pO != pObserverFrom)
                {
                    pO->Update(this, pObserverFrom, Event, Data);
                }
                break;
            }
            else
            {
                CObserver* pO = eoi->second;
                eoi++;
                if(pO != pObserverFrom)
                {
                    pO->Update(this, pObserverFrom, Event, Data);
                }
            }
        }
    }
}

bool Subject::IsObserverRegistered(CObserver* pObserver)
{
  E2O::iterator OI = m_Observers.begin();
  while(OI != m_Observers.end())
  {
    if(OI->second == pObserver)
    {
      return true;
    }
    ++OI;
  }
  return false;
}

int Subject::GetObserverCount()
{
    return m_Observers.size();
}

