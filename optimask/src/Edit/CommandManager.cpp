
#include "./Edit/CommandManager.h"
#include "./Edit/CCommandBase.h"

CommandManager::UndoRedoStateInspector::UndoRedoStateInspector(CommandManager * pCommandManager):
m_pCommandManager(pCommandManager),
m_bUndoable(pCommandManager->CanUndo()),
m_bRedoable(pCommandManager->CanRedo())
{
}

CommandManager::UndoRedoStateInspector::~UndoRedoStateInspector()
{
    if (m_bUndoable != m_pCommandManager->CanUndo() || m_bRedoable != m_pCommandManager->CanRedo())
    {
        emit m_pCommandManager->UndoRedoState(m_pCommandManager->CanUndo(), m_pCommandManager->CanRedo());
    }
}

CommandManager::CommandManager()
{
}

CommandManager::~CommandManager()
{
    ClearAllCommands();
}

CommandManager * CommandManager::Instance()
{
    return Singleton<CommandManager>::Instance();
}

bool CommandManager::CallCommand(CCommandBase * pCommand)
{
     UndoRedoStateInspector si(this);

    if (pCommand)
    {
        if (pCommand->Execute(pCommand))
        {
            PushUndoCommand(pCommand);
            DeleteRedoCommands();
            //emit UndoRedoState(CanUndo(), CanRedo());
            emit FinishCommand(pCommand);
            return true;

        }
        else
        {
            delete pCommand;
        }
    }
    return false;
}

void CommandManager::ClearAllCommands()
{
    UndoRedoStateInspector si(this);
    DeleteUndoCommands();
    DeleteRedoCommands();
    //emit UndoRedoState(CanUndo(), CanRedo());
}

void CommandManager::Undo()
{
    UndoRedoStateInspector si(this);

    CCommandBase * pCommand = PopUndoCommand();
    if (pCommand)
    {
        if (pCommand->UnExecute(pCommand))
        {
            PushRedoCommand(pCommand);
            //emit UndoRedoState(CanUndo(), CanRedo());
            QString undoStr = "Undo";
            emit undoandredoInf(undoStr,pCommand);
        }
        else
        {
            delete pCommand;
        }
    }
}

void CommandManager::Redo()
{
    UndoRedoStateInspector si(this);

    CCommandBase * pCommand = PopRedoCommand();
    if (pCommand)
    {
        if (pCommand->Execute(pCommand))
        {
            PushUndoCommand(pCommand);
            //emit UndoRedoState(CanUndo(), CanRedo());
            QString redoStr = "Redo";
            emit undoandredoInf(redoStr,pCommand);
        }
        else
        {
            delete pCommand;
        }
    }
}

bool CommandManager::CanUndo() const
{
    return !m_stackUndo.empty();
}

bool CommandManager::CanRedo() const
{
    return !m_stackRedo.empty();
}

void CommandManager::PushUndoCommand(CCommandBase * pCommand)
{
    if (pCommand)
    {
        m_stackUndo.push(pCommand);
    }
}

CCommandBase * CommandManager::PopUndoCommand()
{
    CCommandBase * pCommand = NULL;
    if (!m_stackUndo.empty())
    {
        pCommand = m_stackUndo.top();
        m_stackUndo.pop();
    }
    return pCommand;
}

void CommandManager::PushRedoCommand(CCommandBase * pCommand)
{
    if (pCommand)
    {
        m_stackRedo.push(pCommand);
    }
}

CCommandBase * CommandManager::PopRedoCommand()
{
    CCommandBase * pCommand = NULL;
    if (!m_stackRedo.empty())
    {
        pCommand = m_stackRedo.top();
        m_stackRedo.pop();
    }
    return pCommand;
}

void CommandManager::DeleteUndoCommands()
{
    while (!m_stackUndo.empty())
    {
        delete m_stackUndo.top();
        m_stackUndo.pop();
    }
}

void CommandManager::DeleteRedoCommands()
{
    while (!m_stackRedo.empty())
    {
        delete m_stackRedo.top();
        m_stackRedo.pop();
    }
}
