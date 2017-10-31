#ifndef COMMAND_H
#define COMMAND_H

#include <QWidget>
#include <QKeyEvent>
#include <QTextBrowser>
#include <QLabel>
#include <QString>
#include <QPushButton>
//#include <QClipboard>
#include <QFileDialog>
#include <QIODevice>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>

#include "fuzzyquery.h"
#include "./base/Geo.h"
#include "./Edit/ccommandbase.h"
#include "./Edit/CommandManager.h"

class Command:public QWidget
{
    Q_OBJECT

public:
    Command(QWidget * parent = 0);
    ~Command();
    void  init();        //set the initial interface
    void  createLayout();
    void  createConnection();
    void  keyPressEvent(QKeyEvent  *event);       //Implement the  function of shortcut keys
    void  judgeRequest();
    void  displayHistCmd();
    void  preCommand();
    void  nextCommand();
    void  judgeNote(QString&,int);
    void  semiRequest(QString&,int );
    void  judgeRotate(QString& ,QString& );
    void  startRequest(QString&  );

    void  judgeItem(QString& );
    void  judgePara(QString& );
    void  judgeRect();
    void  judgePolygon(QString&);
    void  judgeArc(QString&);
    void  cmdOpenFile();

private slots:
    void  slotCommandResize();
    void  slotAutoScroll();
    void  slotOpenFileDialog();
    void  slotSaveCmdEidt();
    void  slotSaveHistEdit();
    void  slotExcuteCmd();
    void  slotShowCommand(CCommandBase*);

    void  slotUndoAndRedo(QString &,CCommandBase *);

signals:
    void  drawRectInf(CGeoPt ,CGeoPt);
    void  drawLineInf(CGeoPt ,CGeoPt);
    void  drawPolygonInf(std::vector<CGeoPt> ,bool);
    void  drawEllipseInf(CGeoPt ,double ,double );
    void  drawArcInf(CGeoPt,double,double,double);
    void  redoInf();
    void  undoInf();

private:
    FuzzyQuery *    cmdTEdit;
    QStringList     queryStr;

    QTextBrowser *  histBrow;
    QLabel *        sensLab;
    QLabel *        iconLab; 
    QPushButton *   runButton;
    QPushButton *   openButton;
    QPushButton *   saveButton;
    QPushButton *   saveHistButton;
    QString         cmdStr,histStr,openStr,openSave,noteStr,semiStr;
    QString         rowStr,cmdSaveStr,rowSaveStr,histLastStr,staAngleStr,preAngleStr,rotateStr;
    QString         displayStr,preSaveStr,nextSaveStr,sglStr;
    int             curAnglePara, upLenght,downLenght;
    int             para;
    int             paraNum;
    double          x,y,w,h;
    bool            b_poAndPa;
    CGeoPt          begin,end;
};

#endif // COMMAND_H
