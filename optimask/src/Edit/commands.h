//#ifndef COMMANDS_H
//#define COMMANDS_H

//#include <QUndoCommand>
//#include "./Draw/graphicsview.h"

//class AddShapeCommand : public QUndoCommand
//{
////public:
////    AddShapeCommand(COptimaskView *doc, const ItemShape &shape, QUndoCommand *parent = 0);
////    void undo() Q_DECL_OVERRIDE;
////    void redo() Q_DECL_OVERRIDE;

////private:
////    COptimaskView *m_doc;
////    ItemShape m_shape;
////    QString m_shapeName;
//};

//class RemoveShapeCommand : public QUndoCommand
//{
////public:
////    RemoveShapeCommand(COptimaskView *doc, const QString &shapeName, QUndoCommand *parent = 0);
////    void undo() Q_DECL_OVERRIDE;
////    void redo() Q_DECL_OVERRIDE;

////private:
////    COptimaskView *m_doc;
////    ItemShape m_shape;
////    QString m_shapeName;
//};

////class SetShapeColorCommand : public QUndoCommand
////{
////public:
////    SetShapeColorCommand(COptimaskView *doc, const QString &shapeName, const QColor &color,
////                            QUndoCommand *parent = 0);

////    void undo() Q_DECL_OVERRIDE;
////    void redo() Q_DECL_OVERRIDE;

////    bool mergeWith(const QUndoCommand *command) Q_DECL_OVERRIDE;
////    int id() const Q_DECL_OVERRIDE;

////private:
////    COptimaskView *m_doc;
////    QString m_shapeName;
////    QColor m_oldColor;
////    QColor m_newColor;
////};

//class SetShapeRectCommand : public QUndoCommand
//{
//public:
//    SetShapeRectCommand(COptimaskView *doc, const QString &shapeName, const QRect &rect,
//                            QUndoCommand *parent = 0);

//    void undo() Q_DECL_OVERRIDE;
//    void redo() Q_DECL_OVERRIDE;

//    bool mergeWith(const QUndoCommand *command) Q_DECL_OVERRIDE;
//    int id() const Q_DECL_OVERRIDE;

//private:
//    COptimaskView *m_doc;
//    QString m_shapeName;
//    QRect m_oldRect;
//    QRect m_newRect;
//};


//#endif // COMMANDS_H
