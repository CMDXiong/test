#ifndef COBSERVER_H
#define COBSERVER_H

#include <sstream>
#include <map>

class Subject;

class CObserver
{
    friend class Subject;
public:
    virtual bool Update(Subject* pSubject, CObserver* pObserverFrom, unsigned int Event, const std::stringstream& Data) = 0;

      bool HasSubject() const {return !m_E2Subjects.empty();}

      // CObserver/unobserve the Event from pSubject;
      void Observe(Subject* pSubject, unsigned int Event);
      void UnObserve(Subject* pSubject, unsigned int Event);

    protected:

        CObserver();
        virtual ~CObserver();
      CObserver(const CObserver& rhs);
      CObserver& operator=(const CObserver& rhs);

      // call subject to notify other observers;
      void NotifySubject(unsigned int Event, const std::stringstream& Data);

      // clear out all the subject/CObserver relationship;
      void ClearSubjects();

    private:
      // remove the pSubject for this CObserver;
      void RemoveSubject(Subject* pSubject);
      // refer to the subject object;
      typedef std::multimap<unsigned int, Subject*> E2S;
      typedef std::pair<unsigned int, Subject*> ESPair;
      typedef E2S::iterator ESI;
      typedef std::pair<ESI, ESI> ESPairI;

      E2S m_E2Subjects;
};

#endif // COBSERVER_H
