//#include "./Edit/ccommand.h"
//#include "./Edit/ccommandrecevierbase.h"

//// 注册此类
////RegisterCommandClass<CCommand> RegisterCommandClass(ClassNameToString(CCommand));

//CCommand::CCommand(void):
//m_pReceiver(NULL),
//m_bAutoDeleteReceiver(true)
//{
//}

//CCommand::~CCommand(void)
//{
//    if (m_bAutoDeleteReceiver && m_pReceiver)
//    {
//        delete m_pReceiver;
//        m_pReceiver = NULL;
//    }
//}

//bool CCommand::Execute()
//{
//    if (m_pReceiver)
//    {
//        return m_pReceiver->Action(false);
//    }
//    return false;
//}

//bool CCommand::Unexecute()
//{
//    if (m_pReceiver)
//    {
//        return m_pReceiver->Action(true);
//    }

//    return false;
//}

//void CCommand::SetReceiver(CCommandReceiverBase * pReceiver, bool bAutoDelete/* = true*/)
//{
//    m_pReceiver = pReceiver;
//    m_bAutoDeleteReceiver = bAutoDelete;
//}

