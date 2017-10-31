/* ========================================================================== */
/* CommandManager.h -- 命令管理器(Undo/Redo)头文件
 *
 * REFERENCE:
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001) */
/* HISTORY: 201701(v1.00) Initial Version;
 * ========================================================================== */

#pragma once
#include <QObject>
#include <stack>
#include "./Edit/CCommandManagerBase.h"
#include "./Edit/Subject.h"
#include "./Edit/Singleton.h"

#include "./base/Geo.h"

// #define EVENT_UNDOREDOSTATECHANGED  1

class CCommandBase;

class CommandManager : public QObject, public CCommandManagerBase
{
    Q_OBJECT

    class UndoRedoStateInspector
    {
        friend class CommandManager;

    private:
        UndoRedoStateInspector(CommandManager * pCommandManager);
        ~UndoRedoStateInspector();

    private:
        CommandManager * m_pCommandManager;
        bool m_bUndoable;
        bool m_bRedoable;
    };

    friend class Singleton<CommandManager>;
signals:
    void UndoRedoState(bool undo, bool redo);

private:
    CommandManager();
	~CommandManager();
    CommandManager(const CommandManager& rhs);
    CommandManager& operator=(const CommandManager& rhs);

public:
    static CommandManager * Instance();

    bool CallCommand(CCommandBase * pCommand);
	void ClearAllCommands();
    void Undo();
    void Redo();
    bool CanUndo() const;
    bool CanRedo() const;
signals:
    void FinishCommand(CCommandBase*);
    void undoandredoInf(QString&,CCommandBase *);

private:
    void PushUndoCommand(CCommandBase * pCommand);
    CCommandBase * PopUndoCommand();
    void PushRedoCommand(CCommandBase * pCommand);
    CCommandBase * PopRedoCommand();
    void DeleteUndoCommands();
    void DeleteRedoCommands();

private:
    std::stack<CCommandBase *> m_stackUndo;
    std::stack<CCommandBase *> m_stackRedo;
};

#define CALLCOMMAND(Command) CommandManager::Instance()->CallCommand(Command)
#define UNDO CommandManager::Instance()->Undo()
#define REDO CommandManager::Instance()->Redo()
#define CLEARALLCOMMANDS CommandManager::Instance()->ClearAllCommands();
#define CANUNDO CommandManager::Instance()->CanUndo()
#define CANREDO CommandManager::Instance()->CanRedo()
