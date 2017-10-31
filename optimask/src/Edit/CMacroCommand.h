#ifndef CMACROCOMMAND_H
#define CMACROCOMMAND_H

#include <vector>
#include "./Edit/ccommandbase.h"


class CMacroCommand : public CCommandBase
{
public:
    CMacroCommand();
    ~CMacroCommand();

    virtual bool Execute();
    virtual bool Unexecute();

    void AddCommand(CCommandBase * pCommand);
    void DeleteCommand(CCommandBase * pCommand);

private:
    CMacroCommand(const CMacroCommand& rhs);
    CMacroCommand& operator=(const CMacroCommand& rhs);

private:
    std::vector<CCommandBase *> m_vecCommands;
};
#endif // CMACROCOMMAND_H
