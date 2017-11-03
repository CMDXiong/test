#ifndef CCOMMANDRECEVIERBASE
#define CCOMMANDRECEVIERBASE

#pragma once

#include "./Edit/factory.h"
class CCommandBase;

class CCommandReceiverBase
{
public:
    virtual ~CCommandReceiverBase() {}
    virtual bool Action(CCommandBase*) = 0;
    virtual bool UnAction(CCommandBase* ) = 0;

    static CCommandReceiverBase * CreateCommandReceiver(const std::string& strCommandReceiver)
    {
        return Factory<CCommandReceiverBase, const std::string>::instance()->CreateObject(strCommandReceiver);
    }
};

template <class DerivedCommandReceiver>
class RegisterCommandReceiverClass
{
public:
    static CCommandReceiverBase * Create()
    {
        return new DerivedCommandReceiver;
    }
    RegisterCommandReceiverClass(const std::string& strId)
    {
        Factory<CCommandReceiverBase, const std::string>::instance()->Register(strId, RegisterCommandReceiverClass::Create);
    }
};

#define CREATECOMMANDRECEIVER(CommandReceiver) CCommandReceiverBase::CreateCommandReceiver(ClassNameToString(CommandReceiver))


#endif // CCOMMANDRECEVIERBASE

