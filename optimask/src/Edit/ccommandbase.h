#ifndef CCOMMANDBASE_H
#define CCOMMANDBASE_H

#include "./Edit/factory.h"
#include "./base/Geo.h"
class QPaintDevice;

class CCommandBase
{
public:
    virtual ~CCommandBase(){}
    virtual bool Execute(CCommandBase*) = 0;
    virtual bool UnExecute(CCommandBase*) = 0;
    virtual void Draw(QPaintDevice * device) = 0;
    virtual void UnDraw(QPaintDevice * device) = 0;
    virtual int GetType() = 0;

    // 使用对象工厂（Object Factory）设计模式来实现CCommandBase子类的创建，
    // 目的是解除这些子类与框架的耦合
//    static CCommandBase * CreateCommand(const std::string& strCommand)
//        {
//            return Factory<CCommandBase, const std::string>::instance()->CreateObject(strCommand);
//        }
};

template <class DerivedCommand>
class RegisterCommandClass
{
public:
    static CCommandBase * Create()
    {
        return new DerivedCommand;
    }
    RegisterCommandClass(const std::string& strId)
    {
        Factory<CCommandBase, const std::string>::instance()->Register(strId, RegisterCommandClass::Create);
    }
};

#define CREATECOMMAND(Command) BaseCommand::CreateCommand(ClassNameToString(Command))

#endif // CCOMMANDBASE_H
