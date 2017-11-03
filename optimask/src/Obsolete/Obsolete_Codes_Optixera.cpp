//This file saves archived codes from Optixera, which may be obsolete, but just kept for reference.

//Obsolete or not?? Need or not?
void Optimask::createLayerDockwidget()
{
    QDockWidget *layerdock=new QDockWidget(tr("Color"),this);
    layerdock->setFeatures(QDockWidget::DockWidgetMovable);
    layerdock->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea,layerdock);

////---------------获取层信息并显示------------------------------------

    QToolBox *toolBox = new QToolBox(this);
  //-----------------Color----------------
    QColorDialog *dialog1=new QColorDialog (Qt::red,this);
    dialog1->setOption(QColorDialog::ShowAlphaChannel);
    toolBox->addItem(dialog1, "color");
  //--------------Framecolor--------------
    QColorDialog *dialog2=new QColorDialog (Qt::red,this);
    dialog2->setOption(QColorDialog::ShowAlphaChannel);
    toolBox->addItem(dialog2, "Framecolor");
  //-----------------stipple----------------
    QFrame *frame1=new QFrame(this);

    QLabel *label100=new QLabel();
    label100->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label100-> setBackgroundRole (QPalette::Window);
    QLabel *label101=new QLabel();
    label101->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label101-> setBackgroundRole (QPalette::Window);
    QLabel *label102=new QLabel();
    label102->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label102-> setBackgroundRole (QPalette::Window);
    QLabel *label103=new QLabel();
    label103->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label103-> setBackgroundRole (QPalette::Window);
    QHBoxLayout *a = new QHBoxLayout;
    a->addWidget(label100);
    a->addWidget(label101);
    a->addWidget(label102);
    a->addWidget(label103);

    QLabel *label110=new QLabel();
    label110->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label110-> setBackgroundRole (QPalette::Window);
    QLabel *label111=new QLabel();
    label111->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label111-> setBackgroundRole (QPalette::Window);
    QLabel *label112=new QLabel();
    label112->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label112-> setBackgroundRole (QPalette::Window);
    QLabel *label113=new QLabel();
    label113->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label113-> setBackgroundRole (QPalette::Window);
    QHBoxLayout *b = new QHBoxLayout;
    b->addWidget(label110);
    b->addWidget(label111);
    b->addWidget(label112);
    b->addWidget(label113);

    QLabel *label120=new QLabel();
    label120->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label120-> setBackgroundRole (QPalette::Window);
    QLabel *label121=new QLabel();
    label121->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label121-> setBackgroundRole (QPalette::Window);
    QLabel *label122=new QLabel();
    label122->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label122-> setBackgroundRole (QPalette::Window);
    QLabel *label123=new QLabel();
    label123->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label123-> setBackgroundRole (QPalette::Window);
    QHBoxLayout *c = new QHBoxLayout;
    c->addWidget(label120);
    c->addWidget(label121);
    c->addWidget(label122);
    c->addWidget(label123);

    QLabel *label130=new QLabel();
    label130->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label130-> setBackgroundRole (QPalette::Window);
    QLabel *label131=new QLabel();
    label131->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label131-> setBackgroundRole (QPalette::Window);
    QLabel *label132=new QLabel();
    label132->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label132-> setBackgroundRole (QPalette::Window);
    QLabel *label133=new QLabel();
    label133->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label133-> setBackgroundRole (QPalette::Window);
    QHBoxLayout *d = new QHBoxLayout;
    d->addWidget(label130);
    d->addWidget(label131);
    d->addWidget(label132);
    d->addWidget(label133);


    QVBoxLayout *full=new QVBoxLayout(frame1);
    full->addLayout(a);
    full->addLayout(b);
    full->addLayout(c);
    full->addLayout(d);
    toolBox->addItem(frame1, "Stipple");



  //---------------Animation---------------------
    QFrame *frame2=new QFrame(this);
    QLabel *label00=new QLabel(tr("None"));
    label00->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label00-> setBackgroundRole (QPalette::Window);
    label00->setAutoFillBackground (true);
    QLabel *label01=new QLabel(tr("Scroll"));
    label01->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label01-> setBackgroundRole (QPalette::Window);
    QLabel *label20=new QLabel(tr("Blink"));
    label20->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label20-> setBackgroundRole (QPalette::Window);
    QLabel *label21=new QLabel(tr("/Blink"));
    label21->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label21-> setBackgroundRole (QPalette::Window);
    QGridLayout *AniLayout=new QGridLayout(frame2);
    AniLayout->setSpacing(20);
    AniLayout->addWidget(label00,0,0);
    AniLayout->addWidget(label01,0,1);
    AniLayout->addWidget(label20,2,0);
    AniLayout->addWidget(label21,2,1);
    toolBox->addItem(frame2, "Animation");
   //---------------Style---------------------
    QFrame *frame3=new QFrame(this);
    QLabel *label30=new QLabel(tr("0px"));
    label30->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label30-> setBackgroundRole (QPalette::Window);
    label30->setAutoFillBackground (true);
    QLabel *label31=new QLabel(tr("1px"));
    label31->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label31-> setBackgroundRole (QPalette::Window);
    QLabel *label32=new QLabel(tr("2px"));
    label32->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label32-> setBackgroundRole (QPalette::Window);
    QLabel *label33=new QLabel(tr("3px"));
    label33->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label33-> setBackgroundRole (QPalette::Window);
    QHBoxLayout *a3 = new QHBoxLayout;
    a3->addWidget(label30);
    a3->addWidget(label31);
    a3->addWidget(label32);
    a3->addWidget(label33);

    QLabel *label34=new QLabel(tr("Simple"));
    label34->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label34-> setBackgroundRole (QPalette::Window);
    QLabel *label35=new QLabel(tr("Marked"));
    label35->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label35-> setBackgroundRole (QPalette::Window);
    QHBoxLayout *b3 = new QHBoxLayout;
    b3->addWidget(label34);
    b3->addWidget(label35);

    QVBoxLayout *c3=new QVBoxLayout(frame3);
    c3->addLayout(a3);
    c3->addLayout(b3);
    toolBox->addItem(frame3, "Style");
   //-----------------Visibility----------------------------
    QFrame *frame4=new QFrame(this);
    QLabel *label40=new QLabel(tr("Show"));
    label40->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label40-> setBackgroundRole (QPalette::Window);
    label40->setAutoFillBackground (true);
    QLabel *label41=new QLabel(tr("Hide"));
    label41->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label41-> setBackgroundRole (QPalette::Window);
    QLabel *label42=new QLabel(tr("Transp"));
    label42->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label42-> setBackgroundRole (QPalette::Window);
    QLabel *label43=new QLabel(tr("Opaque"));
    label43->setFrameStyle (QFrame::Panel | QFrame::Raised);
    label43-> setBackgroundRole (QPalette::Window);
    QGridLayout *VisLayout=new QGridLayout(frame4);
    VisLayout->setSpacing(20);
    VisLayout->addWidget(label40,0,0);
    VisLayout->addWidget(label41,0,1);
    VisLayout->addWidget(label42,2,0);
    VisLayout->addWidget(label43,2,1);
    toolBox->addItem(frame4, "Visibility");

    layerdock->setWidget(toolBox);

}
