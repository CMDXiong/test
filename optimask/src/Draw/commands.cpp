#include "commands.h"
#include <iostream>

static const int setShapeRectCommandId = 1;
static const int setShapeColorCommandId = 2;

/******************************************************************************
** AddShapeCommand
*/

AddShapeCommand::AddShapeCommand(Document *doc, const ItemShape &shape, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_doc = doc;
    m_shape = shape;
}

void AddShapeCommand::undo()
{
    std::cout<<"panxiong: this is undo"<<endl;
    m_doc->deleteShape(m_shapeName);
}

void AddShapeCommand::redo()
{
    std::cout<<"panxiong: this is redo"<<endl;
    // A shape only gets a name when it is inserted into a document
    m_shapeName = m_doc->addShape(m_shape);
    setText(QObject::tr("Add %1").arg(m_shapeName));
}

/******************************************************************************
** RemoveShapeCommand
*/

RemoveShapeCommand::RemoveShapeCommand(Document *doc, const QString &shapeName,
                                        QUndoCommand *parent)
    : QUndoCommand(parent)
{
    setText(QObject::tr("Remove %1").arg(shapeName));
    m_doc = doc;
    m_shape = doc->shape(shapeName);
    m_shapeName = shapeName;
}

void RemoveShapeCommand::undo()
{
    m_shapeName = m_doc->addShape(m_shape);
}

void RemoveShapeCommand::redo()
{
    m_doc->deleteShape(m_shapeName);
}

/******************************************************************************
** SetShapeColorCommand
*/
/*
SetShapeColorCommand::SetShapeColorCommand(Document *doc, const QString &shapeName,
                                            const QColor &color, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    setText(QObject::tr("Set %1's color").arg(shapeName));

    m_doc = doc;
    m_shapeName = shapeName;
    m_oldColor = doc->shape(shapeName).color();
    m_newColor = color;
}

void SetShapeColorCommand::undo()
{
    m_doc->setShapeColor(m_shapeName, m_oldColor);
}

void SetShapeColorCommand::redo()
{
    m_doc->setShapeColor(m_shapeName, m_newColor);
}

bool SetShapeColorCommand::mergeWith(const QUndoCommand *command)
{
    if (command->id() != setShapeColorCommandId)
        return false;

    const SetShapeColorCommand *other = static_cast<const SetShapeColorCommand*>(command);
    if (m_shapeName != other->m_shapeName)
        return false;

    m_newColor = other->m_newColor;
    return true;
}

int SetShapeColorCommand::id() const
{
    return setShapeColorCommandId;
}

/******************************************************************************
** SetShapeRectCommand
*/
/*
SetShapeRectCommand::SetShapeRectCommand(Document *doc, const QString &shapeName,
                                            const QRect &rect, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    setText(QObject::tr("Change %1's geometry").arg(shapeName));

    m_doc = doc;
    m_shapeName = shapeName;
    m_oldRect = doc->shape(shapeName).rect();
    m_newRect = rect;
}

void SetShapeRectCommand::undo()
{
    m_doc->setShapeRect(m_shapeName, m_oldRect);
}

void SetShapeRectCommand::redo()
{
    m_doc->setShapeRect(m_shapeName, m_newRect);
}

bool SetShapeRectCommand::mergeWith(const QUndoCommand *command)
{
    if (command->id() != setShapeRectCommandId)
        return false;

    const SetShapeRectCommand *other = static_cast<const SetShapeRectCommand*>(command);
    if (m_shapeName != other->m_shapeName)
        return false;

    m_newRect = other->m_newRect;
    return true;
}

int SetShapeRectCommand::id() const
{
    return setShapeRectCommandId;
}
*/
