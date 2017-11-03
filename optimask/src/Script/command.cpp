#include "command.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include <QTextCursor>
#include <QDebug>
#include <./Edit/CDrawWithMouseCommand.h>

Command::Command(QWidget * parent):QWidget(parent)
{
    init();
    createLayout();
    createConnection();
}

Command::~Command()
{
}

void Command::init()     //set the initial interface
{
    histBrow              = new QTextBrowser();
    sensLab               = new QLabel();
    queryStr              = QStringList() << "Add Rectangle" << "Add Box"  << "Add Line"  << "Add Polygon"  << "Add Ellipse"  << "Add Arc"  << "Add Path"<<"Add Circle"
                                          << "Draw Rectangle"<< "Draw Box" << "Draw Line" << "Draw Polygon"<< "Draw Ellipse"  << "Draw Arc" << "Draw Path" <<"Draw Circle"
                                          << "Redo" << "Undo";

    cmdTEdit              = new FuzzyQuery(queryStr);
    QPixmap icon;
    QPixmap runIcon;
    QPixmap openIcon;
    QPixmap saveIcon;
    QPixmap saveHistIcon;
    icon.load("../src/Icon/tip.png");
    runIcon.load("../src/Icon/run.png");
    openIcon.load("../src/Icon/fileopen.png");
    saveIcon.load("../src/Icon/filesave.png");
    saveHistIcon.load("../src/Icon/histsave.png");
    iconLab               = new QLabel();
    iconLab->setPixmap(icon);
    iconLab->resize(icon.width(),icon.height());
    runButton              = new QPushButton(this);
    openButton             = new QPushButton();
    saveButton             = new QPushButton();
    saveHistButton         = new QPushButton();
    openButton->setIcon(openIcon);
    saveButton->setIcon(saveIcon);
    runButton->setIcon(runIcon);
    saveHistButton->setIcon(saveHistIcon);
    openButton->setToolTip(tr("Open File"));
    saveButton->setToolTip(tr("Save Command"));
    saveHistButton->setToolTip(tr("Save History"));
    runButton->setToolTip(tr("Run"));

    //cmdTEdit->setMaximumHeight(100);
    histBrow->setMinimumHeight(150);
    cmdTEdit->setMaximumHeight(30);
    cmdTEdit->setMinimumHeight(30);   

    //QClipboard *board = QApplication::clipboard();  //Get the system clipboard pointer
    //QString str = board->text();                    //Set the clipboard contents

}

void Command::createLayout()
{
    QHBoxLayout *hLayout    = new QHBoxLayout();
    QVBoxLayout *btnLayout  = new QVBoxLayout();
    hLayout->addWidget(iconLab);
    hLayout->addWidget(cmdTEdit);
    btnLayout->addWidget(openButton);
    btnLayout->addWidget(saveButton);
    btnLayout->addWidget(saveHistButton);
    btnLayout->addWidget(runButton);
    //btnLayout->setMargin(0);
    //btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(0,0,0,80);

    QVBoxLayout *vLayout    = new QVBoxLayout();
    vLayout->addWidget(histBrow);
    vLayout->addLayout(hLayout);
    //vLayout->setStretch(1,10);
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(btnLayout);
    mainLayout->addLayout(vLayout);
    this->setLayout(mainLayout);
}

void Command::createConnection()
{
    connect(runButton,SIGNAL(clicked()), this, SLOT(slotExcuteCmd()));
    connect(openButton, SIGNAL(clicked()), this, SLOT(slotOpenFileDialog()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(slotSaveCmdEidt()));
    connect(saveHistButton, SIGNAL(clicked()), this, SLOT(slotSaveHistEdit()));

    connect(cmdTEdit,SIGNAL(textChanged()), this, SLOT(slotCommandResize()));
    connect(histBrow , SIGNAL(cursorPositionChanged()), this, SLOT(slotAutoScroll()));    //Set the QTextBrowser slider to automatically scroll
    connect(CommandManager::Instance(), SIGNAL(FinishCommand(CCommandBase*)), this, SLOT(slotShowCommand(CCommandBase*)));

    connect(CommandManager::Instance(), SIGNAL(undoandredoInf(QString&,CCommandBase *)), this, SLOT(slotUndoAndRedo(QString&,CCommandBase *)));

}

void Command::keyPressEvent(QKeyEvent  *event)      //Implement the  function of shortcut keys
{
    if(event->modifiers() == Qt::ControlModifier)   //Modifier key Ctrl press
    {
        switch (event->key())
        {
        case Qt::Key_Return:
            slotExcuteCmd();
            break;
        case Qt::Key_Enter:
            slotExcuteCmd();
            break;
        case Qt::Key_Up:
            preCommand();
            break;
        case Qt::Key_Down:
            nextCommand();
            break;

        }
    }
    else
    {
        switch (event->key())
        {
        case Qt::Key_Escape:
            cmdTEdit->clear();
            break;
        case Qt::Key_Return:
            break;
        case Qt::Key_Enter:
            slotExcuteCmd();
            break;
        case Qt::Key_Left:
            break;
        case Qt::Key_Right:
            break;
        }
     }
}

void Command::slotCommandResize()
{
    cmdStr = cmdTEdit->toPlainText();
    int j  = 0;
    for( int i = 0;!cmdStr.section("\n",i,-1).isEmpty();++i)
    {
        rowStr = cmdStr.section("\n",i,i);
        if(i<=6)
        {
            if(rowStr.length()<=33)
            {
                cmdTEdit->setMaximumHeight(25+15*j);
                cmdTEdit->setMinimumHeight(25+15*j);
            }
            else if(rowStr.length()>33&&rowStr.length()<66)
            {
                j++;
                cmdTEdit->setMaximumHeight(25+15*j);
                cmdTEdit->setMinimumHeight(25+15*j);
            }
            else if(rowStr.length()>=66&&rowStr.length()<99)
            {
                j = j + 2;
                cmdTEdit->setMaximumHeight(25+15*j);
                cmdTEdit->setMinimumHeight(25+15*j);
            }
            else if(rowStr.length()>=99&&rowStr.length()<132)
            {
                j = j + 3;
                cmdTEdit->setMaximumHeight(25+15*j);
                cmdTEdit->setMinimumHeight(25+15*j);
            }
            else
            {
                j = j + 4;
                cmdTEdit->setMaximumHeight(25+15*j);
                cmdTEdit->setMinimumHeight(25+15*j);
            }
        }
        else
        {
            cmdTEdit->setMaximumHeight(115);
            cmdTEdit->setMinimumHeight(115);
        }
        j++;

    }
}

void Command::slotAutoScroll()
{
    QTextCursor cursor =  histBrow->textCursor();
    cursor.movePosition(QTextCursor::End);
    histBrow->setTextCursor(cursor);
}

void Command::slotOpenFileDialog()
{
    QString fileName = QFileDialog :: getOpenFileName(this, NULL, NULL, "*.h *.cpp *.txt");   //Get the file name
    QFile file(fileName);      //Open the file and read the contents of the file

    // Open file successfully
    if (file.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&file);
        while (!textStream.atEnd())
        {
            QString str;
            str      = textStream.readAll();
//           str.replace("\n",";");
//           str.remove(";;");
            cmdTEdit->setText(str);

        }
    }
    else    //fail to open the file
    {
        QMessageBox ::information(NULL, NULL, "open file error");
        return;
    }

}

void Command::slotSaveCmdEidt()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Save File",QDir::currentPath(),"*.h *.cpp *.txt");
        if(fileName.isEmpty())
        {
            QMessageBox::information(this,"Error Message","Plesase Select a Text File");
            return;
        }
        QFile *file  = new QFile;
        file->setFileName(fileName);
        bool ok=file->open(QIODevice::WriteOnly);
        if(ok)
        {
            QString text = cmdTEdit->toPlainText();
            text.replace(QString("\n"), QString("\r\n"));
            QTextStream infile(file);
            infile << text << "\r\n";
            delete file;

        }
        else
        {
            QMessageBox::information(this,"Error Messaage","File Save Error"+file->errorString());
            return;
        }
}

void Command::slotSaveHistEdit()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Save File",QDir::currentPath(),"*.h *.cpp *.txt");
        if(fileName.isEmpty())
        {
            QMessageBox::information(this,"Error Message","Plesase Select a Text File");
            return;
        }
        QFile *file  = new QFile;
        file->setFileName(fileName);
        bool ok = file->open(QIODevice::WriteOnly);
        if(ok)
        {
            QString text = histBrow->toPlainText();
            text.replace(QString("\n"), QString("\r\n"));
            QTextStream infile(file);
            infile << text << "\r\n";
            delete file;

        }
        else
        {
            QMessageBox::information(this,"Error Messaage","File Save Error"+file->errorString());
            return;
        }
}

void Command::slotExcuteCmd()
{
   judgeRequest();
   displayHistCmd();
}

void Command::slotShowCommand(CCommandBase * commandBase)
{
    switch(commandBase->GetType())
    {
    case GEO_TYPE_RECT:
    {
        CDrawRectWithMouseCommand  *rect = dynamic_cast<CDrawRectWithMouseCommand*>(commandBase);
        CGeoPt topleft = rect->GetRectData()->GetTopLeftPt();
        CGeoPt rightbottom = rect->GetRectData()->GetRightBottomPt();
        QString firstX,firstY,secondX,secondY;
        firstX  = QString::number(topleft.dx);
        firstY  = QString::number(topleft.dy);
        secondX = QString::number(rightbottom.dx);
        secondY = QString::number(rightbottom.dy);

        histStr +="Draw Rectangle:(" +firstX+ "," +firstY + ")(" + secondX + "," + secondY +")";
        histStr += "\n";
    }
        break;
    case GEO_TYPE_LINE:
    {
        CDrawLineWithMouseCommand *line = dynamic_cast<CDrawLineWithMouseCommand*>(commandBase);
        CGeoPt first = line->GetLineData()->GetFirstPt();
        CGeoPt second = line->GetLineData()->GetSecondPt();
        QString firstX,firstY,secondX,secondY;
        firstX  = QString::number(first.dx);
        firstY  = QString::number(first.dy);
        secondX = QString::number(second.dx);
        secondY = QString::number(second.dy);
        histStr +="Draw Line: (" +firstX+ "," +firstY + ") (" + secondX + "," + secondY +")";
        histStr += "\n";
    }
        break;
    case GEO_TYPE_LWPOLYLINE:
        {
        CDrawLWPolylineWithMouseCommand  *lwPolyline = dynamic_cast<CDrawLWPolylineWithMouseCommand*>(commandBase);
        if(lwPolyline->GetLWPolylineData()->IsClosed())
        {
            QString x,y,saveStr;
            saveStr = "";
            std::vector<CGeoPt> allPoint = lwPolyline->GetLWPolylineData()->GetPtList();
            for(std::vector<CGeoPt>::iterator itr= allPoint.begin(); itr != allPoint.end();++itr)
            {
                x = QString::number((*itr).dx);
                y = QString::number((*itr).dy);
                saveStr += "("+x+","+y+")";
            }

            histStr +="Draw Polygon: "+ saveStr;
            histStr += "\n";
        }
        else if(!lwPolyline->GetLWPolylineData()->IsClosed())
        {
            QString x,y,saveStr;
            saveStr = "";
            std::vector<CGeoPt> allPoint = lwPolyline->GetLWPolylineData()->GetPtList();
            for(std::vector<CGeoPt>::iterator itr= allPoint.begin(); itr != allPoint.end();++itr)
            {
                x = QString::number((*itr).dx);
                y = QString::number((*itr).dy);
                saveStr += "("+ x + "," + y + ") ";
            }

            histStr += "Draw Path: " + saveStr;
            histStr += "\n";
        }
        }
        break;
    case GEO_TYPE_ELLIPSE:
    {
        CDrawEllipseWithMouseCommand  *ellipse = dynamic_cast<CDrawEllipseWithMouseCommand*>(commandBase);
        CGeoPt center = ellipse->GetEllipseData()->GetCenterPt();
        double width = ellipse->GetEllipseData()->GetWidth();
        double height = ellipse->GetEllipseData()->GetHeight();
        QString centerX,centerY,axisX,axisY;
        centerX  = QString::number(center.dx);
        centerY  = QString::number(center.dy);
        axisX  = QString::number(width);
        axisY  = QString::number(height);
        if (axisX == axisY)
        {
            histStr +="Draw Circle： Center(" +centerX+ "," + centerY + ") Radius:" + axisX ;
            histStr += "\n";
        }
        else
        {
            histStr +="Draw Ellipse： Center(" +centerX+ "," + centerY + ") Axis(" + axisX + "," + axisY + ")" ;
            histStr += "\n";
        }
    }
        break;
    case GEO_TYPE_CIRCLE:
    {
        CDrawCircleWithMouseCommand *circle = dynamic_cast<CDrawCircleWithMouseCommand*>(commandBase);
        CGeoPt center = circle->GetCircleData()->GetCenterPt();
        double rad = circle->GetCircleData()->GetRadius();
        QString centerX,centerY,radius;
        centerX  = QString::number(center.dx);
        centerY  = QString::number(center.dy);
        radius   = QString::number(rad);
        histStr +="Draw Circle： Center(" +centerX+ "," + centerY + ") Radius:" + radius ;
        histStr += "\n";
    }
        break;
    case GEO_TYPE_ARC:
    {
        CDrawArcWithMouseCommand *arc = dynamic_cast<CDrawArcWithMouseCommand*>(commandBase);
        CGeoPt center = arc->GetArcData()->GetCenterPt();
        double rad = arc->GetArcData()->GetRadius();
        double angleS = arc->GetArcData()->GetAngleS();
        double angleE = arc->GetArcData()->GetAngleE();
        QString centerX,centerY,radius,startangle,endangle;
        centerX  = QString::number(center.dx);
        centerY  = QString::number(center.dy);
        radius  = QString::number(rad);
        startangle  = QString::number(angleS);
        endangle  = QString::number(angleE);
        histStr +="Draw Arc： Center(" +centerX+ "," + centerY + ") Radius:" + radius + " angleS:" + startangle +"° angleE:" + endangle + "°" ;
        histStr += "\n";
    }
        break;
    }
    displayHistCmd();
}

void Command::slotUndoAndRedo(QString& string,CCommandBase *ccommandbase)
{
    histStr = histStr + string + " ";
    slotShowCommand(ccommandbase);
}

void Command::judgeRequest()
{
    histBrow->clear();
    cmdStr      = cmdTEdit->toPlainText();
    QString saveNoteStr;
    //judge "/* */"
    saveNoteStr = cmdStr;
    if(cmdStr.contains("/*"))
    {
        for( int i = 1;!saveNoteStr.section("*",i,-1).isEmpty();i = i + 2) //-1 present the end
        {
            noteStr  = cmdStr.section("*",i,i);    //note string
            cmdStr   = cmdStr.remove(noteStr);
        }
    }

    //judge the "\n"
    for( int i = 0;!cmdStr.section("\n",i,-1).isEmpty();i++)
    {
        rowStr     = cmdStr.section("\n",i,i);
        rowStr     = rowStr.trimmed();
        para       = 1;
        judgeNote(rowStr,para);
        cmdStr = saveNoteStr;
    }
}

void Command::displayHistCmd()
{
    //histBrow->setTextBackgroundColor("yellow");
    if(cmdStr.contains("@"))
    {
        histBrow->setText(histStr);
        cmdTEdit->clear();
        cmdTEdit->setText(openStr);
    }
    else
    {
        histBrow->setText(histStr);
        cmdTEdit->clear();
    }
    for(int k = 0;!histStr.section("\n",k,-1).isEmpty();k++)
    {
        histLastStr  = histStr.section("\n",k,k);
        histLastStr  = histLastStr.trimmed();
        upLenght      = k;
        downLenght    = k;
    }
}
void Command::preCommand()
{
    if(upLenght >= 0)
    {
       sglStr = histStr.section("\n",upLenght,upLenght);
       startRequest(sglStr);
      // histBrow->setText(histStr);
       upLenght--;
       histStr = histBrow->toPlainText();
    }

}

void Command::nextCommand()
{
    if(upLenght + 1 <downLenght)
    {
       sglStr = histStr.section("\n",upLenght+2,upLenght+2);
       upLenght++;
       startRequest(sglStr);
       histStr = histBrow->toPlainText();
       //histBrow->setText(histStr);
    }
}

void Command::judgeNote(QString& ,int)
{
    //judge "//"
    if(rowStr.contains("//"))
    {
        noteStr      = rowStr.section("//",1,1);    //note string
        rowStr      = rowStr.section("//",0,0);
        semiRequest(rowStr,para);
    }
    //judge "%"
    else if(rowStr.contains("%"))
    {
        noteStr      = rowStr.section("%",1,1);    //note string
        rowStr      = rowStr.section("%",0,0);
        semiRequest(rowStr,para);
    }
    //judge "#"
    else if(rowStr.contains("#"))
    {
        noteStr      = rowStr.section("#",1,1);    //note string
        rowStr      = rowStr.section("#",0,0);
        semiRequest(rowStr,para);
    }
    //judge "!"
    else if(rowStr.contains("!"))
    {
        noteStr      = rowStr.section("!",1,1);    //note string
        rowStr      = rowStr.section("!",0,0);
        semiRequest(rowStr,para);
    }
    else
    {
        semiRequest(rowStr,para);
    }
}

//judge the ";"
void Command::semiRequest(QString& ,int)
{
    QString preStr,lastStr;
    if(rowStr.contains(";"))
    {
        semiStr    =  rowStr;
        for( int j = 0;!semiStr.section(";",j,j).isEmpty();j++)
        {
            rowStr = semiStr.section(";",j,j);
            rowStr =  rowStr.trimmed();
            if(rowStr.startsWith("rotate",Qt::CaseInsensitive))
            {
                if(j == 0&&para == 0&&rowStr.startsWith("rotate"))
                {
                    judgeRotate(rowStr,histLastStr);
                }
                else
                {
                    preStr      = rowSaveStr;
                    histLastStr = preStr;
                    judgeRotate(rowStr,histLastStr);
                }
            }
            startRequest(rowStr);
        }
    }

    //exit no  ";"
    else
    {
        if(rowStr.startsWith("rotate",Qt::CaseInsensitive))
        {
            if(para == 0&&rowStr.startsWith("rotate"))
            {
                judgeRotate(rowStr,histLastStr);
            }
            else
            {
                preStr      = rowSaveStr;
                histLastStr = preStr;
                judgeRotate(rowStr,histLastStr);
            }
        }
        startRequest(rowStr);
    }
}

void Command::judgeRotate(QString& ,QString&)
{
    if(histLastStr.contains("rotate",Qt::CaseInsensitive))
    {
        QRegExp seq("\\D+");
        QString curAngleStr;
        int preAnglePara,staAnglePara;
        preAngleStr   = histLastStr.section(seq,5,5);
        histLastStr   = histLastStr.section(seq,0,4);
        preAnglePara  = preAngleStr.toDouble();
        staAngleStr   = rowStr.section(seq,1,1);
        staAnglePara  = staAngleStr.toDouble();
        curAnglePara  = staAnglePara + preAnglePara;
        curAngleStr   = QString::number(curAnglePara,10);
        rotateStr     = rowStr.replace(staAngleStr,curAngleStr);
        histLastStr   = histLastStr.append(rotateStr);
        cmdStr.replace(rowStr,histLastStr);
        rowStr        = histLastStr;
    }
    else
    {
        histLastStr   = histLastStr.append(rowStr);
        rowStr        = histLastStr;
    }
}

void Command::startRequest(QString& rowStr)
{
    rowSaveStr = rowStr;
    if(rowStr.startsWith("ad",Qt::CaseInsensitive)|rowStr.startsWith("dra",Qt::CaseInsensitive))
    {
       judgeItem(rowStr);
    }
    if(rowStr.contains("do",Qt::CaseInsensitive))
    {
       if(rowStr.startsWith("re",Qt::CaseInsensitive))
       {
           histStr  = histStr +"Redo";
           emit redoInf();
       }
       else if(rowStr.startsWith("un",Qt::CaseInsensitive))
       {
           histStr  = histStr + "Undo";
           emit undoInf();
       }
    }

    //open file
    if(rowStr.startsWith("@"))
    {
        cmdOpenFile();
    }

}

void Command::judgeItem(QString& rowStr)
{
    judgePara(rowStr);
    if(rowStr.contains("rec",Qt::CaseInsensitive)|rowStr.contains("b",Qt::CaseInsensitive))
    {
        judgeRect();
        emit  drawRectInf(begin,end);
    }

    else if(rowStr.contains("line",Qt::CaseInsensitive))
    {
        judgeRect();
        emit drawLineInf(begin,end);
    }

    else if(rowStr.contains("poly",Qt::CaseInsensitive)|rowStr.contains("path",Qt::CaseInsensitive))
    {
        //b_poAndPa = true;
        if(rowStr.contains("poly",Qt::CaseInsensitive))
        {
           b_poAndPa = true;
        }
        else
        {
           b_poAndPa = false;
        }
        judgePolygon(rowStr);
    }
    else if(rowStr.contains("ell",Qt::CaseInsensitive))
    {
        judgeRect();
        emit drawEllipseInf(begin,w,h);
    }
    else if(rowStr.contains("circ",Qt::CaseInsensitive))
    {
        judgeRect();
        emit drawEllipseInf(begin,w,w);
    }
    else if(rowStr.contains("arc",Qt::CaseInsensitive))
    {
        judgeArc(rowStr);
    }
}

void Command::judgePara(QString& rowStr)
{
    QRegExp          sep1("\\d+");
    QRegExp          sep2("\\D+");
    QRegExp          sep3("\\W+");
    QString          letterStr,startX,startY,width,height,textStr;
    QStringList      paralist;
    letterStr        = rowStr.section(sep1,0,0);

    int j = 0;
    for(paraNum = 1; j<4 ; j++)
    {
        startX   = rowStr.section(sep2,paraNum,paraNum);
        startY   = rowStr.section(sep2,paraNum+1,paraNum+1);
        textStr  = rowStr.section(sep2,paraNum,paraNum+1);
        textStr.remove(sep1);
        if(textStr ==".")
        {
          startX = rowStr.section(sep2,paraNum,paraNum+1);
          paraNum+=2;
        }
        else
        {
            ++paraNum;
        }
        paralist<<startX;

    }
    startX     = paralist[0];
    startY     = paralist[1];
    width      = paralist[2];
    height     = paralist[3];

//    if(letterStr.contains("-"))
//    {
//        startX       = "-" + startX;
//    }
//    if(rowStr.section(sep3,1,2).contains("-"))
//    {
//        startY       = "-" + startY;
//    }
//    if(rowStr.section(sep3,2,3).contains("-"))
//    {
//        width        = "-" + width;
//    }
//    if(rowStr.section(sep3,3,4).contains("-"))
//    {
//        height       = "-" + height;
//    }

    x                = startX.toDouble();
    y                = startY.toDouble();
    w                = width.toDouble();
    h                = height.toDouble();
    //qDebug()<<x<<" "<<y<<" "<<w<<" "<<h;

}

void Command::judgeRect()
{
    begin.dx = x;
    begin.dy = y;
    end.dx   = w;
    end.dy   = h;
}

void Command::judgePolygon(QString& rowStr)
{
    QRegExp          sep2("\\D+");
    QRegExp          sep3("\\W+");
    std::vector<CGeoPt> pointlist;
    CGeoPt  point;
    pointlist.clear();
    point.dx = x;
    point.dy = y;
    pointlist.push_back(point);

    for(int i = 3 ; !rowStr.section(sep2,i,i).isEmpty() ; i = i + 2 )
    {
        QString     startX,startY;
        startX      = rowStr.section(sep2,i,i);
        startY      = rowStr.section(sep2,i+1,i+1);
        if(rowStr.section(sep3,i-1,i).contains("-"))
        {
          startX    = "-" + startX;
        }
        if(rowStr.section(sep3,i,i+1).contains("-"))
        {
          startY    = "-" + startY;
        }
        x           = startX.toDouble();
        y           = startY.toDouble();
        point.dx = x;
        point.dy = y;
        pointlist.push_back(point);
    }
    emit            drawPolygonInf(pointlist,b_poAndPa);
}

void Command::judgeArc(QString& rowStr)
{
    QRegExp          sep1("\\d+");
    QRegExp          sep2("\\D+");
    QRegExp          sep3("\\W+");
    QString          startAngle,startAngle1,textStr;
    startAngle   = rowStr.section(sep2,paraNum,paraNum);
    startAngle1   = rowStr.section(sep2,paraNum+1,paraNum+1);
    textStr  = rowStr.section(sep2,paraNum,paraNum+1);
    textStr.remove(sep1);
    if(textStr ==".")
    {
      startAngle = rowStr.section(sep2,paraNum,paraNum+1);
    }

//    QRegExp        sep2("\\D+");
//    QRegExp        sep3("\\W+");

//    startAngle     = rowStr.section(sep2,5,5);
//    if(rowStr.section(sep3,4,5).contains("-"))
//    {
//        startAngle = "-" +startAngle;
//    }
    CGeoPt  center;
    double rad,angleS,angleE;
    center.dx = x;
    center.dy = y;
    rad = w;
    angleS = h;
    angleE = startAngle.toDouble();;
    emit  drawArcInf(center,rad,angleS,angleE);
}

void Command::cmdOpenFile()
{
    QString filename,strSave1,contSave;
    filename                        = rowStr.remove("@");
    filename                        = filename.trimmed();
    filename                        = filename + ".txt";
    QFile file(filename);
    // Open file successfully
    if (file.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&file);
        while (!textStream.atEnd())
        {
            openSave                 = textStream.readAll();

            if(openSave.contains("@"))
            {
                QString rowOStr,contStr;

                for( int i = 0;!openSave.section("\n",i,i).isEmpty();i++)
                {
                    rowOStr           = openSave.section("\n",i,i);
                    rowOStr           = openSave.trimmed();

                    if(rowOStr.contains("@"))
                    {
                        QString openfile,fileStr;
                        openfile       = rowOStr.remove("@");
                        openfile       = openfile.trimmed();
                        openfile       = openfile + ".txt";
                        QFile file(openfile);
                        if (file.open(QIODevice ::ReadOnly | QIODevice ::Text))
                        {
                            QTextStream textStream(&file);
                            while (!textStream.atEnd())
                            {
                                fileStr = textStream.readAll();
                            }
                        }
                        contStr         = fileStr;
                    }
                    else
                    {
                        contStr         = rowOStr;
                    }
                    contStr             += "\n";
                    contSave            += contStr;
                }
            }
            else
            {
                contSave                = openSave;
            }
        }
    }
    else    //fail to open the file
    {
        QMessageBox ::information(NULL, NULL, "you have open the wrong file");
        return;
    }
    strSave1                            =  "Open File";
    strSave1                            =  strSave1 + ": " + filename ;
//    strSave1                            += "\n";
    histStr                             += strSave1;
    contSave                            += "\n";
    openStr                             += contSave;
}

