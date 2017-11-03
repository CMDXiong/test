#ifndef SUBJECT_H
#define SUBJECT_H

#include <sstream>
#include <map>

class CObserver;

// Implement the Subject class for the CObserver pattern;
class Subject
{
  friend class CObserver;
public:
  typedef std::multimap<unsigned int, CObserver*> E2O;
  typedef std::pair<unsigned int, CObserver*> EOPair;
  typedef E2O::iterator EOI;
  typedef std::pair<EOI, EOI> EOPairI;

  // Notify all observers in this subject, pObserverFrom is the one who generated the event;
  void Notify(CObserver* pObserverFrom, unsigned int Event, const std::stringstream& Data);

protected:
  Subject();
  virtual ~Subject();
  Subject(const Subject& rhs);
  Subject& operator=(const Subject& rhs);

  // clear out all the subject/CObserver relationship;
  void ClearObservers();
  //Get CObserver count
  int GetObserverCount() ;
private:
  // Attach/Detach the pObserver to this subject;
  void AttachObserver(CObserver* pObserver, unsigned int Event);
  void DetachObserver(CObserver* pObserver, unsigned int Event);
  // detatch all EO pair that matches the pObbserver;
  void DetachObserver(CObserver* pObserver);
  // determine if the pObserver has any event registered with this subject;
  bool IsObserverRegistered(CObserver* pObserver);

private:
  //ACE_Thread_Mutex m_Mutex;
  // The Event to CObserver container;
  E2O m_Observers;
};
#endif // SUBJECT_H
