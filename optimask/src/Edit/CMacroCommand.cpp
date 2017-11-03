#include <algorithm>
#include "./Edit/CMacroCommand.h"
#include "./Edit/cutil.h"

//RegisterCommandClass<CMacroCommand> RegisterCommandClass(ClassNameToString(CMacroCommand));

CMacroCommand::CMacroCommand()
{
}

CMacroCommand::~CMacroCommand()
{
    ContainerDeleter<std::vector<CCommandBase *> >(m_vecCommands);
}

bool CMacroCommand::Execute()
{
    for (unsigned int i = 0; i < m_vecCommands.size(); i++)
    {
        CCommandBase * pCommand = m_vecCommands[i];
        if (!pCommand->Execute())
        {
            return false;
        }
    }
    return true;
}

bool CMacroCommand::Unexecute()
{
    for (unsigned int i = m_vecCommands.size(); i > 0; i--)
    {
        CCommandBase * pCommand = m_vecCommands[i-1];
        if (!pCommand->UnExecute())
        {
            return false;
        }
    }
    return true;
}

void CMacroCommand::AddCommand(CCommandBase * pCommand)
{
    if (pCommand)
    {
        m_vecCommands.push_back(pCommand);
    }
}

void CMacroCommand::DeleteCommand(CCommandBase * pCommand)
{
    if (pCommand)
    {
        m_vecCommands.erase(std::remove(m_vecCommands.begin(), m_vecCommands.end(), pCommand));
    }
}


