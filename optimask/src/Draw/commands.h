#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "./Draw/document.h"

class AddShapeCommand : public QUndoCommand
{
public:
    AddShapeCommand(Document *doc, const ItemShape &shape, QUndoCommand *parent = 0);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    Document *m_doc;
    ItemShape m_shape;
    QString m_shapeName;
};

class RemoveShapeCommand : public QUndoCommand
{
public:
    RemoveShapeCommand(Document *doc, const QString &shapeName, QUndoCommand *parent = 0);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    Document *m_doc;
    ItemShape m_shape;
    QString m_shapeName;
};

//class SetShapeColorCommand : public QUndoCommand
//{
//public:
//    SetShapeColorCommand(Document *doc, const QString &shapeName, const QColor &color,
//                            QUndoCommand *parent = 0);

//    void undo() Q_DECL_OVERRIDE;
//    void redo() Q_DECL_OVERRIDE;

//    bool mergeWith(const QUndoCommand *command) Q_DECL_OVERRIDE;
//    int id() const Q_DECL_OVERRIDE;

//private:
//    Document *m_doc;
//    QString m_shapeName;
//    QColor m_oldColor;
//    QColor m_newColor;
//};

//class SetShapeRectCommand : public QUndoCommand
//{
//public:
//    SetShapeRectCommand(Document *doc, const QString &shapeName, const QRect &rect,
//                            QUndoCommand *parent = 0);

//    void undo() Q_DECL_OVERRIDE;
//    void redo() Q_DECL_OVERRIDE;

//    bool mergeWith(const QUndoCommand *command) Q_DECL_OVERRIDE;
//    int id() const Q_DECL_OVERRIDE;

//private:
//    Document *m_doc;
//    QString m_shapeName;
//    QRect m_oldRect;
//    QRect m_newRect;
//};


#endif // COMMANDS_H
