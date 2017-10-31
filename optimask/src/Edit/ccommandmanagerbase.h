#ifndef CCOMMANDMANAGERBASE
#define CCOMMANDMANAGERBASE

#pragma once

class CCommandBase;

class CCommandManagerBase
{
public:
    virtual ~CCommandManagerBase() {}
    virtual bool CallCommand(CCommandBase * pCommand) = 0;
    virtual void ClearAllCommands() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    virtual bool CanUndo() const = 0;
    virtual bool CanRedo() const = 0;
};

#endif // CCOMMANDMANAGERBASE

