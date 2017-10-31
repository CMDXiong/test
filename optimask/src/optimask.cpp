/* ========================================================================== */
/* Optimask.cpp -- Optimask主界面文件(动作,菜单,界面和窗口等)
 *
 * REFERENCE:
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.01$ (20171001) */
/* HISTORY: 20161027(v1.00) Initial Version; 20171001 Optimization;
 * ========================================================================== */

#include <QtWidgets>
#include <QPrintDialog>
#include <QPrinter>
#include <QPaintDevice>
#include <QTableView>
#include <QHeaderView>
#include <QColorDialog>
#include <QFrame>
#include <QMimeData>

#include "optimask.h"
#include "./Edit/commands.h"
#include "./Edit/CommandManager.h"
#include "./Edit/pastetodlg.h"
#include "./Draw/graphicsview.h"
#include "./Draw/gdsdocument.h"
#include "./Draw/celltreewidget.h"
#include "./Draw/matrix.h"
#include "./Draw/librarywidget.h"
#include "./Draw/multiline.h"
#include "./Alter/alterdockwidget.h"
#include "./Layer/laynavigator.h"
#include "./Script/command.h"
#include "./Script/hint.h"
#include "./Script/info.h"

using namespace std;

//QtCreater中的“./”这个路径是bulid directory的路径。
//Qt资源的相对路径：必须在工程的qrc文件中增加你需要的文件或者资源，引用方法是：":/路径/你的资源.png"
#ifdef RELEASE
  const QString rsrcPath = "./Icon/";     //指定图标目录
  //const QString rsrcPath = ":/Icon/";     //指定图标目录
#else
  const QString rsrcPath = "../src/Icon";     //指定图标目录
  //const QString rsrcPath = QCoreApplication::applicationDirPath() + "/Icon";
  //const QString rsrcPath = QDir::currentPath()+ "/Icon";
#endif

const int OffsetIncrement = 5;
const QString MimeType = "application/vnd.qtrac.pagedesigner";

/* Class Constructor (::optimask) for Optimask class.
   ":" is inheritance, inherited constructor, initialization list, or constructor chaining */
Optimask::Optimask(QWidget *parent)
    : QMainWindow(parent),addOffset(OffsetIncrement),pasteOffset(OffsetIncrement)
{

    resize(1000,600);                  //设置主窗口大小
    setWindowTitle(tr("Optimask[*]")); //设置主窗口标题
    init();

    //显示菜单工具条(Toolbar)显示
    dispFileToolBar  = false;          //显示File工具条
    dispEditToolBar  = false;          //显示Edit工具条
    dispViewToolBar  = false;          //显示View工具条
    dispDrawToolBar  = false;          //显示Draw工具条
    dispAltrToolBar  = false;          //显示Alter工具条
    dispCellToolBar  = false;          //显示Cell工具条
    dispLayerToolBar = false;          //显示Layer工具条
    dispCodeToolBar  = false;          //显示Script/Code/Command工具条
    dispCnfgToolBar  = false;          //显示Config工具
    dispToolToolBar  = false;          //显示Tool工具条
    dispWndwToolBar  = false;          //显示Window工具
    dispHelpToolBar  = false;          //显示Help工具条

    //面板(Panel,Pad,Dock)显示或隐藏
    //构层面板之命名和配置 (L-Edit: Layer Palette + Setup Layers; KLayout: Layers + Layer Toolbox)
    dispLayerPanel   = false;          //显示构层面板(Layer Panel, Layer View)
    dispLayerPalette = false;          //显示构层色板(Layer Palette, Color&Pattern)
     //构元面板之命名和配置 (L-Edit: Design Navigator; KLayout: Cells)
    dispCellDock     = false;          //显示构元结构面板(Cell Structure Tree Panel), 构元组织(Cell Hierarchy)
    //工作区域面板(Main Workspace; Work Panel)可以有多个，可浮动层叠，也可全部关闭；通过双击某构元自动打开。
    //dispWorkDock     = false;          //显示工作区域(Workspace, Work Panel)
    //导航面板(Navigator)显示当前构元的全景视图；工作区域面板窗口显示的局部大小显示在导航面板相对位置上;
    dispNaviDock     = false;          //显示导航区域(Navigator, Aerial View)
    dispAltrDock     = false;          //变化面板(AltrDock)显示
    dispCodeDock     = false;          //显示编程区域(Command, Script, Macro, Code, Programming)
    dispInfoDock     = false;          //显示信息输出栏(Information, Status, Result, Output)
    dispHintDock     = false;          //显示提示报警栏(Hint, Error, Warning, Issue)

    //    centralWidget()->setAttribute(Qt::WA_TransparentForMouseEvents);
    //    setMouseTracking(true);
    //    brushWidget        = alterItem->getbrushWidget();
    //    penWidget            = alterItem->getPenWidget();
    transformWidget = alterItem->getTransformWidget();
}

Optimask::~Optimask()
{
    m_vecDocument.clear();
}

void Optimask::init()
{
    createActions();                   //创建动作
    createMenusAndToolBars();          //创建菜单和工具条
    createStatusBar();                 //创建状态栏
    createMainDocks();                 //创建停靠窗口
    createConnections();               //连接动作函数
}

/* 创建动作 */
void Optimask::createActions()
{
    /* 【文件(File)】主菜单动作集 (缩进为子菜单动作) */
    fileNewAct            = new QAction(QIcon(rsrcPath + "/filenew.png"), tr("New(&N)"), this);              //新建(New)
    fileNewAct->setShortcuts(QKeySequence::New);
    fileNewAct->setToolTip("New");                        //设置工具栏按钮的提示文本
    fileNewAct->setStatusTip(tr("Create a New Layout.")); //设置状态栏提示文本
    fileOpenAct           = new QAction(QIcon(rsrcPath + "/fileopen.png"), tr("Open(&O)"), this);            //打开(Open)
    fileOpenAct->setShortcuts(QKeySequence::Open);
    fileCloseAct          = new QAction(QIcon(rsrcPath + "/fileclose.png"), tr("&Close"), this);             //关闭(Close)
    fileSaveAct           = new QAction(QIcon(rsrcPath + "/document-save.png"), tr("Save(&S)"), this);       //保存(Save)
    fileSaveAct->setShortcuts(QKeySequence::Save);
    fileSaveAsAct         = new QAction(QIcon(rsrcPath + "/document-save-as.png"), tr("Save As(&A)"), this); //另存为(Save As)
    fileInfoAct           = new QAction(QIcon(rsrcPath + "/fileinfo.png"), tr("File Info"), this);           //文件信息(File Info)
    fileOptnAct           = new QAction(QIcon(rsrcPath + "/fileoption.png"), tr("File Option"), this);       //文件格式选项(File Option)
    fileImportAct         = new QAction(QIcon(rsrcPath + "/fileoption.png"), tr("Import"), this);            //导入(Import)格式
    fileImportLayerAct    = new QAction(tr("Layer Tech"), this);                      //导入(Import)Layer Setup (Technology File)层设置
    fileImportGDSIIAct    = new QAction(tr("GDSII"), this);                           //导入(Import)GDSII格式
    fileImportOASISAct    = new QAction(tr("OASIS"), this);                           //导入(Import)OASIS格式
    fileImportCIFAct      = new QAction(tr("CIF"), this);                             //导入(Import)CIF格式
    fileImportDXFAct      = new QAction(tr("DXF"), this);                             //导入(Import)DXF格式
    fileImportGerberAct   = new QAction(tr("Gerber"), this);                          //导入(Import)Gerber格式
    fileImportPictureAct  = new QAction(tr("Picture"), this);                         //导入(Import)图形格式(BMP,JPG,PNG,TIF,etc.)
    fileExportAct         = new QAction(QIcon(rsrcPath + "/fileoption.png"), tr("Export"), this);            //导出(Export)格式
    fileExportLayerAct    = new QAction(tr("Layer Tech"), this);                      //导出(Export)Layer Setup (Technology File)层设置
    fileExportGDSIIAct    = new QAction(tr("GDSII"), this);                           //导出(Export)GDSII格式
    fileExportOASISAct    = new QAction(tr("OASIS"), this);                           //导出(Export)OASIS格式
    fileExportCIFAct      = new QAction(tr("CIF"), this);                             //导出(Export)CIF格式
    fileExportDXFAct      = new QAction(tr("DXF"), this);                             //导出(Export)DXF格式
    fileExportGerberAct   = new QAction(tr("Gerber"), this);                          //导出(Export)Gerber格式
    fileExportPictureAct  = new QAction(tr("Picture"), this);                         //导出(Export)图形格式(BMP,JPG,PNG,TIF,etc.)
    filePrintAct = new QAction(QIcon(rsrcPath + "/fileprint.png"), tr("Print(&P)"), this);  //打印(Print)
    //打印预览和打印设置(Print Preview & Print Setup)将在打印(Print)命令执行时出现
    filePrintPreviewAct   = new QAction(QIcon(rsrcPath + "/fileprintpreview.png"), tr("Print Preview"), this); //打印预览(Print Preview)
    filePrintSetupAct = new QAction(tr("Print Setup"), this);                         //打印设置(Print Setup)
    fileExitAct = new QAction(QIcon(rsrcPath + "/fileexit.png"), tr("Exit(X)"), this);

    /* 【编辑(Edit)】主菜单动作集 (缩进为子菜单动作) */
    editSelectAction = new QAction(QIcon(rsrcPath + "/selectCells.png"),tr("Select"), this);
    editMoveViewAct = new QAction(QIcon(rsrcPath + "/OpenHandCursor.png"),tr("Move"), this);
    editSelectedItemAction = new QAction(QIcon(rsrcPath + "/editselecteditem.png"),tr("Edit Selected Item..."), this);
    editUndoAct = new QAction(QIcon(rsrcPath + "/editundo.png"), tr("Undo(&U)"), this);      //撤销(Undo)上一个操作
    editUndoAct->setEnabled(false);
    editRedoAct = new QAction(QIcon(rsrcPath + "/editredo.png"), tr("Redo(&R)"), this);      //恢复(Redo, Recover)上一个操作
    editRedoAct->setEnabled(false);
    editAgainAct          = new QAction(QIcon(rsrcPath + "/edit-delete.png"), tr("&Again"), this);   //重复(Repeat, Again)最近一个操作
    editCutAct            = new QAction(QIcon(rsrcPath + "/editcut.png"), tr("Cut(&T)"), this);      //剪切(Cut)选中目标
    editCutAct->setShortcuts(QList<QKeySequence>()<< QKeySequence::Cut << Qt::Key_Delete);
    editCopyAct           = new QAction(QIcon(rsrcPath + "/editcopy.png"), tr("Copy(&C)"), this);    //复制(Copy)选中目标
    editCopyAct->setShortcuts(QKeySequence::Copy);
    editPasteAct          = new QAction(QIcon(rsrcPath + "/editpaste.png"), tr("Paste(&P)"), this);  //粘贴(Paste)选中目标
    editPasteAct->setShortcuts(QKeySequence::Paste);
    editPastetoAct        = new QAction(QIcon(rsrcPath + "/pasteto.ico"), tr("Paste to"), this);     //粘贴至(PasteTo)特定目标(比如另外一个Origin/Basepoint,Layer,Cell)
    editDuplicateAct      = new QAction(QIcon(rsrcPath + "/edit-duplicate.png"), tr("Duplicate(&U)"), this); //重复(Duplicate, Clone)选中目标
    editDeleteAct         = new QAction(QIcon(rsrcPath + "/edit-delete.png"), tr("Delete(&D)"), this);//删除(Delete)选中目标
    editSelectAct         = new QAction(QIcon(rsrcPath + "/edit-select.png"), tr("Select"), this);    //选择(Select)指定目标
    editSelectLayersAct   = new QAction(tr("Layers"), this);                                          //选择指定构层或多层(Select Layers)
    editSelectCellsAct    = new QAction(QIcon(rsrcPath + "/selectCells.png"), tr("Cells"), this);     //选择指定单元或多单元(Select Cells)
    editSelectInAct       = new QAction(QIcon(rsrcPath + "/selectCells.png"), tr("In"), this);        //选择选择框里面(Select In/Box/Range)
    editSelectSideAct     = new QAction(tr("Side"), this);                       //选择该边接触的所有物件(Select Side)
    editSelectLastAct     = new QAction(tr("Last"), this);                       //选择刚才的物件(Select Last/New/Recent)
    editSelectAllAct      = new QAction(tr("All"), this);                        //选择所有(Select All)
    editSelectAllAct->setShortcuts(QKeySequence::SelectAll);
    editDeselectAct       = new QAction(QIcon(rsrcPath + "/selectCells.png"), tr("Deselect"), this);  //解选(Deselect)选中目标(to be unselected) (ICED: unselect; L-Edit: deselect)
    editDeselectLayersAct = new QAction(tr("Layers"), this);                     //解选指定构层或多层(Deselect Layers)
    editDeselectCellsAct  = new QAction(tr("Cells"), this);                      //解选指定单元或多单元(Deselect Cells)
    editDeselectInAct     = new QAction(tr("In"), this);                         //解选选择框里面(Deselect In/Box/Range)
    editDeselectSideAct   = new QAction(tr("Side"), this);                       //解选该边接触的所有物件(Deselect Side)
    editDeselectLastAct   = new QAction(tr("Last"), this);                       //解选刚才的物件(Deslect Last/New/Recent)
    editDeselectAllAct    = new QAction(tr("All"), this);                        //解选所有(Deslect All)
    editDeselectAllAct->setShortcuts(QKeySequence::Deselect);

    /* 【视图(View)】 和 【显示(Display)】 相关 */
    /* 【视图(View)】主菜单动作集 (缩进为子菜单动作) */
    viewOptnAct           = new QAction(QIcon(rsrcPath+"/home.png"), tr("Option"), this); //屏幕显示(View, Display, Screen)选项(Option)
    viewParityAct         = new QAction(tr("Parity"), this);                      //奇偶性(Parity) (是否镜像) (Right/Wrong-Reading以及Chrome-Up/Down)
    viewToneAct           = new QAction(tr("Digitized Tone"), this);              //极性(Polarity) (数字色调 Digitized Tone) (Clear透明， Dark黑暗) (Display Polygon as Clear or Dark)
    viewOrientationAct    = new QAction(tr("Orientation"), this);                 //取向(Orientation) (参见Windows Display Settings: Landscape, Portrait, Landscape Flipped, Portrait Flipped)
    viewRotationAct       = new QAction(tr("Rotation"), this);                    //旋转(Rotation) (Rotate View/Display/Screen)
    viewMagnificationAct  = new QAction(tr("Magnification"), this);               //放大(Magnification) (比如Reticle 1X, 4X, 5X)
    viewAspectRatio       = new QAction(tr("Aspect Ratio (X:Y)"), this);          //宽高比(Aspect Ratio) = X:Y
    viewGridAct           = new QAction(tr("Grid"), this);                        //网格(Grid)显示
    viewFillAct           = new QAction(tr("Fill"), this);                        //填充(Fill)显示
    viewAnntAct           = new QAction(QIcon(rsrcPath + "/zoom-in.png"), tr("Annotation"), this);     //批注(Annotation)显示
    viewNoteAct           = new QAction(tr("Note"), this);                        //注释(Note)显示
    viewNoteAct->setCheckable(true);
    viewNoteAct->setChecked(true);
    viewPortAct           = new QAction(tr("Port"), this);                        //端口(Port)显示
    viewPortAct->setCheckable(true);
    viewMarkAct           = new QAction(tr("Mark"), this);                        //标记(Mark)显示
    viewMarkAct->setCheckable(true);
    viewRulerAct          = new QAction(tr("Ruler"), this);                       //标尺(Ruler)显示
    viewRulerAct->setCheckable(true);
    viewFrameAct          = new QAction(tr("Frame"), this);                       //边框(Frame, Outline)显示
    viewFrameAct->setCheckable(true);
    viewErrorAct          = new QAction(tr("Error"), this);                       //错误(Error)显示
    viewErrorAct->setCheckable(true);
    viewStructAct         = new QAction(QIcon(rsrcPath+"/toolbox.png"), tr("Structure"), this);       //结构(Structure)显示, 单元从属结构(hierarchy)显示或者平整(Flat)显示(参DW2000)
    viewFlatAct           = new QAction(tr("Flat"), this);                        //平整(flat)结构显示 (hierarchy level = 0)
    viewHierarchyAct      = new QAction(tr("Hierarchy"), this);                   //从属(hierarchy)结构显示 (hierarchy level >= 1)
    viewLevelAct          = new QAction(tr("Level"), this);                       //层级(hierarchy level)结构显示(设定起始和终止层级)(参KLayout)
    viewStepIntoAct       = new QAction(tr("StepInto"), this);                    //进入(Step Into)指定的从属深度(hierarchy depth)
    viewFilterAct         = new QAction(tr("Filter"), this);                      //过滤(Filter)显示(只显示或隐藏符合条件的结构,可以传输过滤条件去修改结构树内各单元属性.)
    viewHomeAct           = new QAction(QIcon(rsrcPath+"/home.png"), tr("Home"), this);           //转至(Home)缺省坐标为中心显示
    viewGotoAct           = new QAction(QIcon(rsrcPath+"/goto.png"), tr("Go To"), this);          //转至(GoTo)指定坐标为中心显示
    viewZoomAct           = new QAction(QIcon(rsrcPath + "/zoom-in.png"), tr("Zoom"), this);      //缩放(Zoom)(变焦视图)
    //(When using Mouse center Scroll Wheel, the view change is zooming.)
    viewZoomInAct         = new QAction(QIcon(rsrcPath + "/zoom-in.png"), tr("In"), this);        //拉近(In)焦距(放大)
    viewZoomOutAct        = new QAction(QIcon(rsrcPath + "/zoom-out.png"), tr("Out"), this);      //推远(Out)焦距(缩小)
    viewZoomBoxAct        = new QAction(tr("Box"), this);                                         //缩放矩形(Box)框内
    viewZoomLastAct       = new QAction(tr("Last"), this);                                        //恢复上次(Last)焦距
    viewZoomSelAct        = new QAction(tr("Select"), this);                                      //缩放选择目标(Selection)
    viewZoomAllAct        = new QAction(tr("All"), this);                                         //显示全景(All, Fit, Home)
    viewPanAct            = new QAction(QIcon(rsrcPath + "/go-left.png"), tr("Pan"), this);       //平移(Pan)镜头
    viewPanLeftAct        = new QAction(QIcon(rsrcPath + "/go-left.png"), tr("Left"), this);      //往左(Left)平移
    viewPanRightAct       = new QAction(QIcon(rsrcPath + "/go-right.png"), tr("Right"), this);    //往右(Right)平移
    viewPanUpAct          = new QAction(QIcon(rsrcPath + "/go-up.png"), tr("Up"), this);          //往上(Up)平移
    viewPanDownAct        = new QAction(QIcon(rsrcPath + "/go-down.png"), tr("Down"), this);      //往下(Down)平移
    viewPanLastAct        = new QAction(tr("Last"), this);                                        //恢复至上次(Last)平移位置
    viewPanSelAct         = new QAction(tr("Select"), this);                                      //平移至选择目标(Selection)
    viewPanDistAct        = new QAction(tr("Distance"), this);                                    //平移指定距离(Distance) (dX,dY)
    viewTiltAct           = new QAction(tr("Tilt"), this);                                        //倾斜(Tilt)镜头(2D实际不需要; 保留给3D扩展)
    viewRefreshAct        = new QAction(tr("Tilt"), this);                                        //刷新(Refresh)视图

    //【显示(Display)】工具箱(Toolbox)(显示或隐藏)
    dispToolboxAct        = new QAction(QIcon(rsrcPath+"/toolbox.png"), tr("Toolbox"), this);  //工具箱(Toolbox)显示
    //【显示(Display)】菜单工具条(Toolbar)(显示或隐藏)
    dispToolbarAct        = new QAction(QIcon(rsrcPath+"/toolbox.png"), tr("Toolbar"), this);  //菜单工具条(Toolbar)显示
    //【显示(Display)】面板(Panel,Pad,Dock)(显示或隐藏)
    dispFileToolBarAct    = new QAction(tr("File"), this);
    dispFileToolBarAct->setCheckable(true);
    dispFileToolBarAct->setChecked(true);
    //dispFileToolBarAct->setData("FileToolBar");
    dispEditToolBarAct    = new QAction(tr("Edit"), this);
    dispEditToolBarAct->setCheckable(true);
    dispEditToolBarAct->setChecked(true);
    dispViewToolBarAct    = new QAction(tr("View"), this);
    dispViewToolBarAct->setCheckable(true);
    dispViewToolBarAct->setChecked(true);
    dispDrawToolBarAct    = new QAction(tr("Draw"), this);
    dispDrawToolBarAct->setCheckable(true);
    dispDrawToolBarAct->setChecked(true);
    dispAltrToolBarAct   = new QAction(tr("Alter"), this);
    dispAltrToolBarAct->setCheckable(true);
    dispAltrToolBarAct->setChecked(true);
    dispCellToolBarAct    = new QAction(tr("Cell"), this);
    dispCellToolBarAct->setCheckable(true);
    dispCellToolBarAct->setChecked(true);
    dispLayerToolBarAct   = new QAction(tr("Layer"), this);
    dispLayerToolBarAct->setCheckable(true);
    dispLayerToolBarAct->setChecked(true);
    dispCodeToolBarAct  = new QAction(tr("Script"), this);
    dispCodeToolBarAct->setCheckable(true);
    dispCodeToolBarAct->setChecked(true);
    dispCnfgToolBarAct  = new QAction(tr("Config"), this);
    dispCnfgToolBarAct->setCheckable(true);
    dispCnfgToolBarAct->setChecked(true);
    dispToolToolBarAct    = new QAction(tr("Tool"), this);
    dispToolToolBarAct->setCheckable(true);
    dispToolToolBarAct->setChecked(true);
    dispWndwToolBarAct  = new QAction(tr("Window"), this);
    dispWndwToolBarAct->setCheckable(true);
    dispWndwToolBarAct->setChecked(true);
    dispHelpToolBarAct    = new QAction(tr("Help"), this);
    dispHelpToolBarAct->setCheckable(true);
    dispHelpToolBarAct->setChecked(true);
    dispPanelAct = new QAction(QIcon(rsrcPath+"/layers.png"), tr("Panel"), this);      //面板(Panel)(Pad)显示选择
    //构层面板之命名和配置 (L-Edit: Layer Palette + Setup Layers; KLayout: Layers + Layer Toolbox)
    dispLayerPanelAct     = new QAction(tr("Layer Panel"), this); //构层面板(Layer Panel)显示
    dispLayerPanelAct->setCheckable(true);
    dispLayerPanelAct->setChecked(true);
    dispLayerPaletteAct   = new QAction( tr("Layer Palette"), this); //构层色板(Layer Palette)显示
    dispLayerPaletteAct->setCheckable(true);
    dispLayerPaletteAct->setChecked(true);
    dispCellDockAct       = new QAction(tr("Cell Structure"), this);                    //结构树(Structure Tree), 构元组织(Cell Hierarchy)显示;
    dispCellDockAct->setCheckable(true);
    dispCellDockAct->setChecked(true);
    dispWorkDockAct = new QAction(tr("Workspace"), this);                             //工作区域面板 (Main Workspace; Work Panel)显示; 需要允许多重窗口;
    dispWorkDockAct->setCheckable(true);
    dispWorkDockAct->setChecked(true);
    dispNaviDockAct       = new QAction(tr("Navigator"), this);                         //导航区域面板(Navigator, Aerial View)显示;
    dispNaviDockAct->setCheckable(true);
    dispNaviDockAct->setChecked(true);
    dispAltrDockAct       = new QAction(tr("Alter"), this); //变换面板(Alter, Vary, Change), 包括(Boolean, Regenerate, Transform, Calculate)
    dispAltrDockAct->setCheckable(true);
    dispAltrDockAct->setChecked(true);
    dispCodeDockAct       = new QAction(tr("Command/Script"), this);                    //计算和编程区域(Computation, Programming, Script, Macro, Code)显示;
    dispCodeDockAct->setCheckable(true);
    dispCodeDockAct->setChecked(true);
    dispInfoDockAct       = new QAction(tr("Info"), this);                              //信息输出栏(Information, Status, Result, Output)显示
    dispInfoDockAct->setCheckable(true);
    dispInfoDockAct->setChecked(true);
    dispHintDockAct       = new QAction(tr("Hint"), this);                              //提示报警栏(Hint, Error, Warning, Issue)显示
    dispHintDockAct->setCheckable(true);
    dispHintDockAct->setChecked(true);

    dispRulerAct       = new QAction(tr("Ruler"), this);                              //提示报警栏(Hint, Error, Warning, Issue)显示
    dispRulerAct->setCheckable(true);
    dispRulerAct->setChecked(false);

    /* 【绘制(Draw)】主菜单动作集 (Draw & Add) (缩进为子菜单动作) */
    drawClearAct          = new QAction(tr("Clear"), this);                             //清空(Clear)
    drawLibraryAct        = new QAction(QIcon(rsrcPath+"/drawlibrary.png"), tr("Library"), this); //库元(Library)

    /* 自定义绘制单元图形 */
    drawLineAct           = new QAction(QIcon(rsrcPath+"/line.png"), tr("Line"), this); //绘制线段(Draw Line, Path, Wire, PLine)
    drawLineAct->setToolTip(tr("Draw a line (Path, Wire, PLine)."));
    drawLineAct->setStatusTip(tr("Draw a line (Path, Wire, PLine)."));
    drawRectAct           = new QAction(QIcon(rsrcPath+"/box.png"), tr("Box"), this);   //绘制矩形(Draw Box, Rect)
    drawRectAct->setToolTip(tr("Draw a rectangle (box)."));
    drawRectAct->setStatusTip(tr("Draw a rectangle (box)."));
    drawRectAct->setData(BoxItemType);
    drawPolyAct           = new QAction(QIcon(rsrcPath+"/polygon.png"), tr("Polygon"), this); //绘制多边形(Draw Polygon, Boundary)
    drawPolyAct->setToolTip(tr("Draw a polygon (boundary)."));
    drawPolyAct->setStatusTip(tr("Draw a polygon (boundary)."));
    drawCircleAct         = new QAction(QIcon(rsrcPath+"/circle.png"), tr("Circle"), this);   //绘制圆形(Draw Circle)
    drawCircleAct->setToolTip(tr("Draw a circle."));
    drawCircleAct->setStatusTip(tr("Draw a circle."));
    drawPathAct        = new QAction(QIcon(rsrcPath+"/path.png"), tr("Path"), this);          //绘制路径(Draw Path)
    drawPathAct->setToolTip(tr("Draw an Path."));
    drawPathAct->setStatusTip(tr("Draw an path."));
    drawEllipseAct        = new QAction(QIcon(rsrcPath+"/ellipse.png"), tr("Path"), this);    //绘制椭圆(Draw Ellipse)
    drawEllipseAct->setToolTip(tr("Draw an Ellipase."));
    drawEllipseAct->setStatusTip(tr("Draw an path."));
    drawCurveAct          = new QAction(QIcon(rsrcPath+"/curve.png"), tr("Curve"), this);     //绘制曲线(Draw Curve)
    drawCurveAct->setToolTip(tr("Draw a curve."));
    drawCurveAct->setStatusTip(tr("Draw a curve."));
    drawMatrixAct         = new QAction(QIcon(rsrcPath+"/matrix-array.png"), tr("Matrix"), this); //通过矩阵数组绘制(Draw from Matrix Array)
    drawMatrixAct->setToolTip(tr("Draw from Matrix Array."));
    drawMatrixAct->setStatusTip(tr("Draw from Matrix Array."));
    drawTextAct           = new QAction(QIcon(rsrcPath+"/text.png"), tr("Text"), this);       //添加文本(Add Text)
    drawTextAct->setToolTip(tr("Add Text."));
    drawTextAct->setStatusTip(tr("Add Text."));
    drawTextAct->setData(TextItemType);
//    drawLogoAct           = new QAction(QIcon(rsrcPath+"/logo.png"), tr("Logo"), this);       //添加图标(Add Logo)
//    drawLogoAct->setToolTip(tr("Add Logo."));
//    drawLogoAct->setStatusTip(tr("Add Logo."));
    drawMultiLineAct      = new QAction(QIcon(rsrcPath+"/path.png"), tr("Multi-Line"), this); //绘制多条线段(Draw Multiple Lines)
    drawMultiLineAct->setToolTip(tr("Draw multiple lines (Path, Wire, PLine)."));
    drawMultiLineAct->setStatusTip(tr("Draw multiple lines (Path, Wire, PLine)."));
    drawMultiCurveAct     = new QAction(QIcon(rsrcPath+"/multiple-curves.png"), tr("Multi-Curve"), this); //绘制多条曲线(Draw Multiple Curves)
    drawMultiCurveAct->setToolTip(tr("Draw multiple curves (Path, Wire, PLine)."));
    drawMultiCurveAct->setStatusTip(tr("Draw multiple curves (Path, Wire, PLine)."));
    drawScanAct           = new QAction(QIcon(rsrcPath+"/drawscan.png"), tr("Scan"), this);   //扫描读入图形文件(Scan/Import Pictures)
    drawScanAct->setToolTip(tr("Scan Picture."));
    drawScanAct->setStatusTip(tr("Scan Picture."));

    /* 【变化(Alter)】主菜单动作集 (缩进为子菜单动作) */
    alterAlignmentAct      = new QAction(QIcon(rsrcPath + "/align-left.png"), tr("Alignment"), this);
    alterAlignmentAct->setData(Qt::AlignLeft);
    alterAlignGridAct      = new QAction(QIcon(rsrcPath + "/aligntogrid.png"), tr("AlignToGrid"), this); //对齐(Align)至格点(Grid) (Snap to Grid)
    alterAlignLeftAct      = new QAction(QIcon(rsrcPath + "/align-left.png"), tr("Left"), this);
    alterAlignLeftAct->setData(Qt::AlignLeft);
    alterAlignRightAct     = new QAction(QIcon(rsrcPath + "/align-right.png"), tr("Right"), this);
    alterAlignRightAct->setData(Qt::AlignRight);
    alterAlignTopAct       = new QAction(QIcon(rsrcPath + "/align-top.png"), tr("Top"), this);
    alterAlignTopAct->setData(Qt::AlignTop);
    alterAlignBottomAct    = new QAction(QIcon(rsrcPath + "/align-bottom.png"), tr("Bottom"), this);
    alterAlignBottomAct->setData(Qt::AlignBottom);
    alterConvertAct        = new QAction(QIcon(rsrcPath + "/convert.png"), tr("Convert"), this);    //参见LEdit(Draw->Convert)
    alterRegenAct          = new QAction(QIcon(rsrcPath + "/convert.png"), tr("Regenerate"), this); //重塑,再生(Regenerate)
    alterGroupAct          = new QAction(tr("Group"), this);                                  //组合(Group)
    alterUngroupAct        = new QAction(tr("Ungroup"), this);                                //分组(UnGroup)
    alterCloneAct          = new QAction(tr("Duplicate"), this);                              //克隆(Clone), 整体复制(Duplicate), 复制(Copy)
    alterSplitAct          = new QAction(tr("Split"), this);                                  //分裂(Split), 剪切(Cut)
    alterCutXAct           = new QAction(tr("Cut horizontally"), this);                       //沿X线切开分裂(CutX,SplitX)
    alterCutYAct           = new QAction(tr("Cut verically"), this);                          //沿Y线切开分裂(CutY,SplitY)
    alterCutLineAct        = new QAction(tr("Cut at y=kx+b line"), this);                     //沿任意线y=kx+b切开分裂(CutLine,SplitLine)
    alterCropAct           = new QAction(tr("Crop"), this);                                   //裁剪(裁切)(Crop)
    alterMergeAct          = new QAction(tr("Merge"), this);                                  //整合,合并(Merge)
    alterFractAct          = new QAction(tr("Fract"), this);                                  //细片,碎片(Fracture)所选图形. (请区分Cell菜单下的fractureAct)
    alterExLayAct          = new QAction(tr("Export Layer"), this);                           //导出选定层上所有图元(Export Layer)请区分Cell菜单下的fractureAct)
    alterBooleanAct        = new QAction(QIcon(rsrcPath + "/convert.png"), tr("Boolean"), this); //布尔运算(boolean),与非运算,逻辑运算(logic)
    alterBoolAndAct        = new QAction(tr("And"), this);                                    //交集(Intersection),    逻辑与(And):  保留公共部分
    alterBoolOrAct         = new QAction(tr("Or"), this);                                     //联合(Union), 相加(Add), 逻辑或(Or):   保留各自部分和公共部分
    alterBoolXorAct        = new QAction(tr("Xor"), this);                                    //互斥(Exclusive),       逻辑异或(Xor): 保留各自部分(删除公共部分)
    alterBoolAnotBAct      = new QAction(tr("A-B"), this);                                    //相减(SUBtract A-B),    逻辑非(NOT):  保留A不属于B的部分(删除B)
    alterBoolBnotAAct      = new QAction(tr("B-A"), this);                                    //相减(SUBtract B-A),    逻辑非(NOT):  保留B不属于A的部分(删除A)
    alterTransformAct      = new QAction(QIcon(rsrcPath + "/convert.png"), tr("Transform"), this); //转变(Transform)
    alterMoveAct           = new QAction(tr("Move"), this);                                   //移位(Move, Translation, Slide), (X,Y) + (dx, dy)
    alterTurnAct           = new QAction(tr("Rotate"), this);                                 //旋转(Turn, Rotate), (X+j*Y)*exp(j*ANG)
    alterFlipAct           = new QAction(tr("Mirror"), this);                                 //镜像(Flip, Mirror, Reflect)
    alterScaleAct          = new QAction(tr("Scaling"), this);                                //缩放(Scaling, Grow, Shrink) (ReSize), (X,Y)*ScaleFactor
    alterStretchAct        = new QAction(tr("Stretch"), this);                                //展绷(Stretch) (X*a, Y*b)
    alterComputeAct        = new QAction(QIcon(rsrcPath + "/altercompute.png"), tr("Compute"), this); //计算(Compute) 根据计算y=f(x)来实现变形

    /* 【构元(Cell)】主菜单动作集 (缩进为子菜单动作) */
    cellNewAct             = new QAction(QIcon(rsrcPath + "/cellnew.png"), tr("New"), this);                                 //新建构元(New Cell)
    cellOpenAct            = new QAction(QIcon(rsrcPath + "/cellopen.png"), tr("Open"), this);                               //打开构元(Open Cell)
    cellCopyAct            = new QAction(QIcon(rsrcPath + "/cellcopy.png"), tr("Copy"), this);                               //复制构元(Copy Cell)
    cellPasteAct           = new QAction(QIcon(rsrcPath + "/cellpaste.png"), tr("Paste"), this);                             //粘贴构元(Paste Cell)
    cellArrangeAct         = new QAction(QIcon(rsrcPath + "/cellarrange.png"), tr("Arrange"), this);                         //安排构元(位置)(Arrange Cell)
    cellMoveOrgAct         = new QAction(QIcon(rsrcPath + "/cellmoveorigin.png"), tr("Move Origin"), this);                  //改变构元基点(Move Origin)
    cellInstanceAct        = new QAction(QIcon(rsrcPath + "/cellinstance.png"), tr("Instance"), this);                       //引用构元(Instance Cell)
    cellChangeInstanceAct  = new QAction(QIcon(rsrcPath + "/cellchangeinstance.png"), tr("Change Instance"), this);          //改变引用(Change Cell's Instance)
    cellMultiAlterAct      = new QAction(QIcon(rsrcPath + "/cellmulticelloperation.png"), tr("Multi-Cell Operation"), this); //多元操作(Multiple Cells Operation)
    cellAssembleAct        = new QAction(QIcon(rsrcPath + "/cellassemble.png"), tr("Assemble"), this);                       //组装构元(Assemble Cells)
    cellHierarchyAct       = new QAction(QIcon(rsrcPath + "/cellhierarchy.png"), tr("Hierarchy"), this);                     //构元从属结构关系(Show Hierarchy)
    cellFlattenAct         = new QAction(QIcon(rsrcPath + "/cellflatten.png"), tr("Flatten"), this);                         //构元平整显示(Show All as Flat)
    cellFractureAct        = new QAction(QIcon(rsrcPath + "/cellfracture.png"), tr("Fracture"), this);                       //细片,碎片(Fracture)所选构元. (请区分Alter菜单下的alterFractAct)
    cellInfoAct            = new QAction(QIcon(rsrcPath + "/cellinfo.png"), tr("Info"), this);                               //构元信息(Cell Info)
    cellStatAct            = new QAction(QIcon(rsrcPath + "/cellstatistics.png"), tr("Statistics"), this);                   //构元统计(Cell Statistics)
    cellFrameAct           = new QAction(QIcon(rsrcPath + "/cellframe.png"), tr("Frame"), this);                             //构元边界(Cell Outline/Frame)
    cellShowAct            = new QAction(tr("Show"), this);                            //构元显示(Show the Cell)
    cellHideAct            = new QAction(tr("Hide"), this);                            //构元隐藏(Hide/Unshow the Cell)
    cellProtectAct         = new QAction(tr("Protect"), this);                         //构元保护(Protect the Cell)
    cellUnprotectAct       = new QAction(tr("Unprotect"), this);                       //构元解保(Unprotect the Cell)
    cellLockAct            = new QAction(tr("Lock"), this);                            //构元锁定(Lock the Cell)
    cellUnlockAct          = new QAction(tr("Unlock"), this);                          //构元解锁(Unlock the Cell)

    /* 【构层(Layer)】主菜单动作集 (缩进为子菜单动作) */
    layerOptnAct      = new QAction(QIcon(rsrcPath + "/loadlayerfile.png"), tr("Layer Option"), this);     //构层选项(Layer Option)
    layerLoadFileAct  = new QAction(QIcon(rsrcPath + "/loadlayerfile.png"), tr("Load Layer File"), this);  //导入构层配置文件(Load/Import Layer Setup/Config/Technology File)
    layerSaveFileAct  = new QAction(QIcon(rsrcPath + "/savelayerfile.png"), tr("Save Layer File"), this);  //保存构层配置文件(Save/Export Layer Setup/Config/Technology File)
    layerPanelAct     = new QAction(QIcon(rsrcPath+"/layerpanel.png"), tr("layerpanel"), this);            //构层面板(Layer Panel)
    layerPaletteAct   = new QAction(QIcon(rsrcPath+"/palette.png"), tr("Layer Palette"), this);            //构层色板(Layer Palette)
    layerSetupAct     = new QAction(QIcon(rsrcPath+"/layersetup.png"), tr("Layer Setup"), this);           //构层设置(Layer Setup)
    layerDensityAct   = new QAction(QIcon(rsrcPath + "/layerdensity.png"), tr("Density"), this);           //构层密度(Layer Density = PolygonArea / TotalArea)
    layerDummyFillAct = new QAction(QIcon(rsrcPath + "/layerdummyfill.png"), tr("Dummy Fill"), this);      //冗余填充(填充冗余构图以达到版图密度目标, Dummy Fill for Target Density)
    layerStatAct      = new QAction(QIcon(rsrcPath + "/layerstatistics.png"), tr("Statistics"), this);     //构层统计(Layer Statistics)
    layerMappingAct   = new QAction(QIcon(rsrcPath + "/layermapping.png"), tr("Mapping"), this);           //构层对应(Layer Mapping, 对应到指定的层设置上去)
    layerCtrlAct      = new QAction(QIcon(rsrcPath + "/layermapping.png"), tr("Control"), this);           //构层控制(Layer Control)
    //Show/Hide--VIEWable？ Prot/Unprot--SELECTable? Lock/Unlock--EDITable?
    layerSelectAct    = new QAction(tr("Select"), this);                               //构层选择(Select)
    layerDeselectAct  = new QAction(tr("Deselect"), this);                             //构层解选(Deselect, to make it Unselected)
    layerShowAct      = new QAction(tr("Show"), this);                                 //构层显示(Show, Unhide)
    layerHideAct      = new QAction(tr("Hide"), this);                                 //构层隐藏(Hide, Unshow)
    layerProtectAct   = new QAction(tr("Protect"), this);                              //构层保护(Protect)
    layerUnprotectAct = new QAction(tr("Unprotect"), this);                            //构层解保(Unprotect)
    layerLockAct      = new QAction(tr("Lock"), this);                                 //构层锁定(Lock)
    layerUnlockAct    = new QAction(tr("Unlock"), this);                               //构层解锁(Unlock)
    layerFillAct      = new QAction(tr("Fill"), this);                                 //构层实心显示(Fill)
    layerUnfillAct    = new QAction(tr("Unfill"), this);                               //构层空心显示(Unfill)

    /* 【编程(Script), 代码(Code)】主菜单动作集 (缩进为子菜单动作) */
    codeOptnAct  = new QAction(QIcon(rsrcPath + "/scriptoption.png"), tr("Script Option"), this);  //代码选项(Code Option)
    codeNewAct   = new QAction(QIcon(rsrcPath + "/scriptnew.png"), tr("New"), this);               //新建代码(New)
    codeOpenAct  = new QAction(QIcon(rsrcPath + "/scriptopen.png"), tr("Open"), this);             //打开代码(Open)
    codeSaveAct  = new QAction(QIcon(rsrcPath + "/scriptsave.png"), tr("Save"), this);             //保存代码(Save)
    codeRunAct   = new QAction(QIcon(rsrcPath + "/scriptrun.png"), tr("Run"), this);               //运行代码(Run)
    codeDebugAct = new QAction(QIcon(rsrcPath + "/scriptdebug.png"), tr("Debug"), this);           //调试代码(Debug)

    /* 【配置(Config)】, 设置(Setup)】主菜单动作集 (缩进为子菜单动作) */
    cnfgOptnAct  = new QAction(QIcon(rsrcPath + "/configoption.png"), tr("Option"), this); //配置选项(Configuration Option)
    cnfgFontAct  = new QAction(QIcon(rsrcPath + "/configfont.png"), tr("Font"), this);     //配置字体(Configure Font)
    cnfgDRCAct   = new QAction(QIcon(rsrcPath + "/configdrc.png"), tr("DRC"), this);       //配置设计规则检查(DRC: Design Rule Check)

    /* 【工具(Tool)】主菜单动作集 (缩进为子菜单动作) */
    toolUtilityAct       = new QAction(QIcon(rsrcPath + "/utility.png"), tr("Utility"), this);                         //小工具集(Unilities)
    toolLayoutStatAct    = new QAction(QIcon(rsrcPath + "/toollayoutstatistics.png"), tr("Layout Statistics"), this);  //版图统计(Layout Statistics)
    toolLayoutCompareAct = new QAction(QIcon(rsrcPath + "/toollayoutcompare.png"), tr("Layout Compare"), this);        //版图比较(Layout Comparison)
    toolLayoutDiffAct    = new QAction(QIcon(rsrcPath + "/toollayoutdifference.png"), tr("Layout Difference"), this);  //版图差分(Layout Difference)
    toolWaferAct         = new QAction(QIcon(rsrcPath + "/toolWafer.ico"), tr("Wafer Tool"), this);                    //晶圆工具(Wafer Tool)

    /* 【窗口(Window)】主菜单动作集 (缩进为子菜单动作) */
    wndwSplitAct   = new QAction(QIcon(rsrcPath + "/split.png"), tr("Split"), this);           //窗口分裂(Split the Window)
    wndwTileAct    = new QAction(QIcon(rsrcPath + "/tile.png"), tr("Tile"), this);             //窗口排列(Tile the Window)
    wndwCascadeAct = new QAction(QIcon(rsrcPath + "/windowcascade.png"), tr("Cascade"), this); //窗口级联(Cascade the Window)
    wndwArrangeAct = new QAction(QIcon(rsrcPath + "/windowarrange.png"), tr("Arrange"), this); //窗口整理(Arrange the Window)

    /* 【帮助(Help)】主菜单动作集 (缩进为子菜单动作) */
    helpIndexAct     = new QAction(QIcon(rsrcPath + "/helpindex.png"), tr("Index"), this);                     //帮助索引(Help Index)
    helpQuickRefAct  = new QAction(QIcon(rsrcPath + "/helpquickreference.png"), tr("Quick Reference"), this);  //快速参考(Quick Reference, Quick Guide)
    helpManualAct    = new QAction(QIcon(rsrcPath + "/helpmanual.ico"), tr("Manual"), this);                   //帮助手册(User's Manual)
    helpExampleAct   = new QAction(QIcon(rsrcPath + "/helpexample.png"), tr("Example"), this);                 //帮助实例(Examples)
    helpAppNoteAct   = new QAction(QIcon(rsrcPath + "/helpapplicationnote.png"), tr("Application Note"), this);//应用笔记(Application Notes)
    helpUpdateAct    = new QAction(QIcon(rsrcPath + "/update.png"), tr("Update"), this);                       //软件更新(Software Update)
    helpHistoryAct   = new QAction(QIcon(rsrcPath + "/history.png"), tr("History"), this);                     //版本历史(Revision History)
    helpLicenceAct   = new QAction(QIcon(rsrcPath + "/helplicence.png"), tr("Licence"), this);                 //软件许可(License Key)
    helpSupportAct   = new QAction(QIcon(rsrcPath + "/helpsupport.png"), tr("Support"), this);                 //软件支持(Software Support)
    helpReportBugAct = new QAction(QIcon(rsrcPath + "/helpreportbug.png"), tr("Report Bug"), this);            //汇报缺陷(Report Bug/Defect)
    helpAboutAct     = new QAction(QIcon(rsrcPath + "/about.png"), tr("About"), this);                         //关于软件(About the Software)
}

/* 创建菜单和工具栏 */
void Optimask::createMenusAndToolBars()
{
    QAction *separator = 0;
    //setUnifiedTitleAndToolBarOnMac(true);

    //【文件(File)】主菜单-------------------------------------------------------------------------
    QMenu *fileImportMenu = new QMenu(tr("Import"), this);
    foreach (QAction *action, QList<QAction*>()
            << fileImportLayerAct << fileImportGDSIIAct << fileImportOASISAct
            << fileImportCIFAct << fileImportDXFAct << fileImportGerberAct
            << fileImportPictureAct)
        fileImportMenu->addAction(action);
    fileImportAct->setMenu(fileImportMenu);

    QMenu *fileExportMenu = new QMenu(tr("Export"), this);
    foreach (QAction *action, QList<QAction*>()
            << fileExportLayerAct << fileExportGDSIIAct << fileExportOASISAct
            << fileExportCIFAct<< fileExportDXFAct << fileExportGerberAct
            << fileExportPictureAct)
        fileExportMenu->addAction(action);
    fileExportAct->setMenu(fileExportMenu);

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    fileToolBar = this->addToolBar("File");
    //QToolBar *fileToolBar = addToolBar(tr("File"));
    populateMenuAndToolBar(fileMenu, fileToolBar, QList<QAction*>()
            << fileNewAct << fileOpenAct << fileCloseAct << fileSaveAct
            << fileSaveAsAct << separator << fileInfoAct << fileOptnAct
            << fileImportAct << fileExportAct << filePrintAct
            << filePrintPreviewAct << filePrintSetupAct << fileExitAct);

    //【编辑(Edit)】主菜单----------------------------------------------------------------------------
    QMenu *editSelectMenu = new QMenu(tr("Import"), this);
    foreach (QAction *action, QList<QAction*>()
            << editSelectLayersAct << editSelectCellsAct << editSelectInAct
            << editSelectSideAct << editSelectLastAct << editSelectAllAct)
        editSelectMenu->addAction(action);
    editSelectAct->setMenu(editSelectMenu);

    QMenu *editDeselectMenu = new QMenu(tr("Export"), this);
    foreach (QAction *action, QList<QAction*>()
            << editDeselectLayersAct << editDeselectCellsAct << editDeselectInAct
            << editDeselectSideAct<< editDeselectLastAct << editDeselectAllAct)
        editDeselectMenu->addAction(action);
    editDeselectAct->setMenu(editDeselectMenu);

    QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
    editToolBar = this->addToolBar("Edit");
    //QToolBar *editToolBar = addToolBar(tr("Edit"));
    populateMenuAndToolBar(editMenu, editToolBar, QList<QAction*>()
            << editSelectAction << editMoveViewAct
            << editSelectedItemAction << editUndoAct << editRedoAct << editAgainAct
            << editCutAct << separator << editCopyAct << editPasteAct << editPastetoAct
            << editDuplicateAct << editDeleteAct << editSelectAct << editDeselectAct);

    //【视图(View)】主菜单---------------------------------------------------------------------------
    QMenu *viewVoptMenu = new QMenu(tr("Voption"), this);
    foreach (QAction *action, QList<QAction*>()
            << viewParityAct << viewToneAct << viewOrientationAct
            << viewRotationAct << viewMagnificationAct << viewAspectRatio
            << viewGridAct<<viewFillAct)
        viewVoptMenu->addAction(action);
    viewOptnAct->setMenu(viewVoptMenu);

    QMenu *viewAnntMenu = new QMenu(tr("Annotation"), this);
    foreach (QAction *action, QList<QAction*>()
            << editDeselectLayersAct << editDeselectCellsAct << editDeselectInAct
            << editDeselectSideAct<< editDeselectLastAct << editDeselectAllAct)
        viewAnntMenu->addAction(action);
    viewAnntAct->setMenu(viewAnntMenu);

    QMenu *viewVstrMenu = new QMenu(tr("View Structure"), this); //结构显示(View Structure)子菜单
    foreach (QAction *action, QList<QAction*>()
            << viewHierarchyAct << viewFlatAct << viewLevelAct
            << viewStepIntoAct << viewFilterAct)
        viewVstrMenu->addAction(action);
    viewStructAct->setMenu(viewVstrMenu);

    QMenu *viewZoomMenu = new QMenu(tr("Zoom"), this);
    foreach (QAction *action, QList<QAction*>()
            << viewZoomInAct << viewZoomOutAct << viewZoomBoxAct
            << viewZoomLastAct<< viewZoomSelAct << viewZoomAllAct)
        viewZoomMenu->addAction(action);
    viewZoomAct->setMenu(viewZoomMenu);

    QMenu *viewPanMenu = new QMenu(tr("Pan"), this);           //平移(Pan)镜头子菜单
    foreach (QAction *action, QList<QAction*>()
            << viewPanLeftAct << viewPanRightAct << viewPanUpAct
            << viewPanDownAct << viewPanLastAct << viewPanSelAct
            << viewPanDistAct)
        viewPanMenu->addAction(action);
    viewPanAct->setMenu(viewPanMenu);

    QMenu *viewVtbxMenu = new QMenu(tr("Toolbox"), this);      //显示工具箱(Toolbox)子菜单
    foreach (QAction *action, QList<QAction*>()<< dispToolboxAct )
        viewVtbxMenu->addAction(action);
    dispToolboxAct->setMenu(viewVtbxMenu);

    QMenu *viewVbarMenu = new QMenu(tr("Toolbar"), this);      //显示工具条(Toolbar)子菜单
    foreach (QAction *action, QList<QAction*>()
            << dispFileToolBarAct << dispEditToolBarAct << dispViewToolBarAct
            << dispDrawToolBarAct << dispAltrToolBarAct << dispCellToolBarAct
            << dispLayerToolBarAct << dispCodeToolBarAct << dispCnfgToolBarAct
            << dispToolToolBarAct << dispWndwToolBarAct << dispHelpToolBarAct)
        viewVbarMenu->addAction(action);
    dispToolbarAct->setMenu(viewVbarMenu);

    QMenu *viewVpadMenu = new QMenu(tr("Panel"), this);        //显示面板(Panel)(Pad)子菜单
    foreach (QAction *action, QList<QAction*>()
            << dispLayerPanelAct << dispLayerPaletteAct << dispCellDockAct
            << dispNaviDockAct << dispAltrDockAct << dispCodeDockAct << dispInfoDockAct << dispHintDockAct
            << dispRulerAct)
        viewVpadMenu->addAction(action);
    dispPanelAct->setMenu(viewVpadMenu);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));
    viewToolBar = this->addToolBar("View");
    populateMenuAndToolBar(viewMenu, viewToolBar, QList<QAction*>()
            << viewOptnAct << separator << viewAnntAct<< separator << viewStructAct
            << separator << viewHomeAct << viewGotoAct << separator << viewZoomAct
            << viewPanAct<< separator << dispToolboxAct<< separator << dispToolbarAct
            << separator << dispPanelAct);

    addToolBarBreak(Qt::TopToolBarArea);

    //【绘制(Draw)】主菜单---------------------------------------------------------------------------
    QMenu *drawMenu = menuBar()->addMenu(tr("Draw"));
    drawToolBar = this->addToolBar("Draw");
    //QToolBar *drawToolBar = addToolBar(tr("Draw"));
    populateMenuAndToolBar(drawMenu, drawToolBar, QList<QAction*>()
            << drawLibraryAct << separator << drawLineAct << drawRectAct
            << drawPolyAct << drawCircleAct<< drawPathAct << drawEllipseAct <<drawCurveAct
            << drawMatrixAct << drawTextAct << separator //<< drawLogoAct
            << drawMultiLineAct<< drawMultiCurveAct << separator << drawScanAct);

    //【变形(Alter)】主菜单---------------------------------------------------------------------------
    // 对齐(Align)子菜单
    QMenu *alterAlignmentMenu = new QMenu(tr("Alignment"), this);
    foreach (QAction *action, QList<QAction*>()
            << alterAlignGridAct << alterAlignLeftAct << alterAlignRightAct
            << alterAlignTopAct<< alterAlignBottomAct)
        alterAlignmentMenu->addAction(action);
    alterAlignmentAct->setMenu(alterAlignmentMenu);

    //分裂(Split), 剪切(Cut)子菜单
    QMenu *alterSplitMenu = new QMenu(tr("Split"), this);          //平移(Pan)镜头子菜单
    foreach (QAction *action, QList<QAction*>()
            << alterCutXAct << alterCutYAct << alterCutLineAct )
        alterSplitMenu->addAction(action);
    alterCutXAct->setMenu(alterSplitMenu);

    //重塑,再生(Regenerate)子菜单
    QMenu *alterRegenMenu = new QMenu(tr("Regenerate"), this);
    foreach (QAction *action, QList<QAction*>()
            << alterGroupAct << alterUngroupAct << alterCloneAct
            << alterSplitAct<< alterCropAct << alterMergeAct << alterFractAct)
        alterRegenMenu->addAction(action);
    alterRegenAct->setMenu(alterRegenMenu);

    //布尔运算(boolean),与非运算,逻辑运算(logic)子菜单
    QMenu *alterBoolMenu = new QMenu(tr("Boolean"), this);
    foreach (QAction *action, QList<QAction*>()
            << alterBoolAndAct << alterBoolOrAct << alterBoolXorAct
            << alterBoolAnotBAct<< alterBoolBnotAAct )
        alterBoolMenu->addAction(action);
    alterBooleanAct->setMenu(alterBoolMenu);

    //转变(Transform, 变换)子菜单
    QMenu *alterTrsfMenu = new QMenu(tr("Transform"), this);
    foreach (QAction *action, QList<QAction*>()
            << alterMoveAct << alterTurnAct << alterFlipAct
            << alterScaleAct << alterStretchAct)
        alterTrsfMenu->addAction(action);
    alterTransformAct->setMenu(alterTrsfMenu);

    QMenu *altrMenu = menuBar()->addMenu(tr("Alter"));
    altrToolBar = this->addToolBar("Alter");
    //QToolBar *altrToolBar = addToolBar(tr("Alter"));
    populateMenuAndToolBar(altrMenu, altrToolBar, QList<QAction*>()
            << alterConvertAct << separator << alterAlignmentAct << separator
            << alterRegenAct << separator << alterBooleanAct << separator
            << alterTransformAct << alterComputeAct);

    //【构元(Cell)】主菜单----------------------------------------------------------------------------
    QMenu *cellMenu = menuBar()->addMenu(tr("Cell"));
        cellToolBar = this->addToolBar("Cell");
        //QToolBar *cellToolBar = addToolBar(tr("Cell"));
        populateMenuAndToolBar(cellMenu, cellToolBar, QList<QAction*>()
                <<  cellNewAct << cellOpenAct << cellCopyAct <<cellPasteAct << separator
                << cellArrangeAct <<cellMoveOrgAct << separator << cellInstanceAct << cellChangeInstanceAct
                <<separator << cellMultiAlterAct <<cellAssembleAct <<separator <<cellHierarchyAct <<cellFlattenAct << separator
                <<cellFractureAct << separator <<cellInfoAct << cellStatAct << cellFrameAct);


    //【构层(Layer)】主菜单---------------------------------------------------------------------------
    //控制构层(Layer Control)子菜单
    QMenu *layerLctrlMenu = new QMenu(tr("Layer Control"), this);
    foreach (QAction *action, QList<QAction*>()
            << layerSelectAct << layerDeselectAct << layerShowAct << layerHideAct
            << layerProtectAct << layerUnprotectAct << layerLockAct << layerUnlockAct
            << layerFillAct << layerUnfillAct)
        layerLctrlMenu->addAction(action);
    layerCtrlAct->setMenu(layerLctrlMenu);

    QMenu *layerMenu = menuBar()->addMenu(tr("Layer"));
    layerToolBar = this->addToolBar("Layer");
    //QToolBar *layerToolBar = addToolBar(tr("Layer"));
    populateMenuAndToolBar(layerMenu, layerToolBar, QList<QAction*>()
            << layerOptnAct << layerLoadFileAct << separator << layerSaveFileAct << layerPanelAct
            << layerPaletteAct << separator << layerSetupAct << layerDensityAct << layerDummyFillAct
            << layerStatAct << layerMappingAct << separator << layerCtrlAct);

    //【编程(Script), 代码(Code)】主菜单---------------------------------------------------------------
    QMenu *codeMenu = menuBar()->addMenu(tr("Script"));
    codeToolBar = this->addToolBar("Script");
    //QToolBar *altrToolBar = addToolBar(tr("Script"));
    populateMenuAndToolBar(codeMenu, codeToolBar, QList<QAction*>()
            << codeOptnAct << separator << codeNewAct << codeOpenAct
            << codeSaveAct << separator << codeRunAct << codeDebugAct);

    //【配置(Config)】主菜单--------------------------------------------------------------------------
    QMenu *cnfgMenu = menuBar()->addMenu(tr("Config"));
    cnfgToolBar = this->addToolBar("Config");
    //QToolBar *cnfgToolBar = addToolBar(tr("Config"));
    populateMenuAndToolBar(cnfgMenu, cnfgToolBar, QList<QAction*>()
            << cnfgOptnAct << separator << cnfgFontAct << separator << cnfgDRCAct);

   //【工具(Tool)】主菜单-----------------------------------------------------------------------------
    QMenu *toolMenu = menuBar()->addMenu(tr("Tool"));
    toolToolBar = this->addToolBar("Tool");
    //QToolBar *toolToolBar = addToolBar(tr("Tool"));
    populateMenuAndToolBar(toolMenu, toolToolBar, QList<QAction*>()
            << toolUtilityAct << separator << toolLayoutStatAct
            << toolLayoutCompareAct << toolLayoutDiffAct << separator << toolWaferAct);

    //【窗口(Window)】主菜单---------------------------------------------------------------------------
    QMenu *wndwMenu = menuBar()->addMenu(tr("Window"));
    wndwToolBar = this->addToolBar("Window");
    //QToolBar *wndwToolBar = addToolBar(tr("Window"));
    populateMenuAndToolBar(wndwMenu, wndwToolBar, QList<QAction*>()
            << wndwSplitAct << wndwTileAct << wndwCascadeAct << wndwArrangeAct);

    //【帮助(Help)】主菜单-----------------------------------------------------------------------------
    QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
    helpToolBar = this->addToolBar("Help");
    //QToolBar *helpToolBar = addToolBar(tr("Help"));
    populateMenuAndToolBar(helpMenu, helpToolBar, QList<QAction*>()
            << helpIndexAct << helpQuickRefAct << helpManualAct << helpExampleAct << helpAppNoteAct
            << helpUpdateAct << helpHistoryAct <<  helpLicenceAct << separator << helpSupportAct
            << helpReportBugAct << separator << helpAboutAct);
}

void Optimask::populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar, QList<QAction *> actions)
{
    foreach (QAction *action, actions) {
        if (!action) {
            menu->addSeparator();
            toolBar->addSeparator();
        }
        else {
            menu->addAction(action);
            toolBar->addAction(action);
        }
    }
}

/* 创建状态条 */
void Optimask::createStatusBar()
{
    //设置应用程序的状态栏
//    scenepos = new QLabel;
//    scenepos->setText(tr("scenepos: "));
//    scenepos->setFixedWidth(200);
    viewpos = new QLabel;
    viewpoint = new QLabel;
    viewpos->setText("<font color='blue'>viewpos:</font>");
    viewpoint->setText("");
    viewpos->setFixedWidth(50);
    viewpoint->setFixedWidth(200);
    diffpos = new QLabel;
    diffpoint = new QLabel;
    diffpos->setText("<font color='red'>diffpos:</font>");
    diffpoint->setText("");
    diffpos->setFixedWidth(50);
    diffpoint->setFixedWidth(200);


//     statusBar()->addPermanentWidget(scenepos);
    statusBar()->addPermanentWidget(viewpos);
    statusBar()->addPermanentWidget(viewpoint);
    statusBar()->addPermanentWidget(diffpos);
    statusBar()->addPermanentWidget(diffpoint);


}

/* 创建主界面各面板 */
void Optimask::createMainDocks()
{
    layerPanel   = new QDockWidget(tr("Layer Panel"), this);
    layerPalette = new QDockWidget (QObject::tr ("Layer Palette"), this);
    altrDock     = new QDockWidget(tr("Alter"), this);
    cellDock     = new QDockWidget(tr("Cell Structure"), this);
    workDock     = new QDockWidget(tr("Work"),this);
    naviDock     = new QDockWidget(tr("Navigator"),this);
    codeDock     = new QDockWidget(tr("Command/Script"), this);
    infoDock     = new QDockWidget(tr("Info"), this);
    hintDock     = new QDockWidget(tr("Hint"), this);

    workDock->setMinimumSize(450,450);
    naviDock->setMinimumHeight(100);

    createLayerPanel();                //创建构层面板(Layer Panel, Layer View)
    createLayerPalette();              //创建构层色板(Layer Palette, Color&Pattern)
    createCellDock();                  //创建构元结构面板(Cell Structure Tree Panel), 构元组织(Cell Hierarchy)
    createWorkDock();                  //创建工作区域(Workspace, Work Panel); 需要允许多重窗口
    createNaviDock();                  //创建导航区域(Navigator, Aerial View)
    createAltrDock();                  //创建变换面板(Alter, AltrDock)(Boolean, Transform, Calculate)
    createCodeDock();                  //创建编程区域(Command, Script, Macro, Code, Programming)
    createInfoDock();                  //创建信息输出区域
    createHintDock();                  //创建提示报警区域

    addDockWidget(Qt::LeftDockWidgetArea,layerPanel);
    splitDockWidget(layerPanel,workDock,Qt::Horizontal);
    splitDockWidget(workDock,naviDock,Qt::Horizontal);

    splitDockWidget(layerPanel,layerPalette,Qt::Vertical);
    splitDockWidget(layerPalette,cellDock,Qt::Vertical);
    splitDockWidget(workDock,infoDock,Qt::Vertical);
    splitDockWidget(naviDock,altrDock,Qt::Vertical);
    splitDockWidget(altrDock,codeDock,Qt::Vertical);

    tabifyDockWidget(infoDock,hintDock);
}

/* 创建构层面板(Layer Panel, Layer View) */
void Optimask::createLayerPanel()
{

    layerPanelTabs = new QTabWidget(layerPanel); //将QTabWidget变成中心部件
    layerPanelTabs->setObjectName(QStringLiteral("cellTreeTabs"));
    layerPanel->setWidget(layerPanelTabs);
    //dispLayerPanelAct->setChecked(true); //将viewMenu->vpadMenu下的该项选中
    connect(layerPanelTabs, SIGNAL(currentChanged(int)), this, SLOT(currentLayerPanelChanged(int)));

    //    //NOTE!! 请先检查LayerPanel是否已经存在。
    //    //如果存在，只需显示(viewLayerPanel)； 如果不存在，那么创建(createLayerPanel)。
    //    LayTableView *pTableView=new LayTableView;
    //    layerPanel=new QDockWidget(tr("Layer Panel"), this);
    //    QWidget *Widget= new QWidget();
    //    QVBoxLayout *verticalLayout = new QVBoxLayout(Widget); //垂直布局添加水平布局，下拉列表，行编辑框，laytablemodel
    //    verticalLayout->addWidget(pTableView);
    //    Widget->setLayout(verticalLayout);
    //    // Widget->resize(30,30);
    //    layerPanel->setWidget(Widget);
    //    //setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
    //    addDockWidget(Qt::LeftDockWidgetArea,layerPanel);
    //    dispLayerPanelAct->setChecked(true); //将viewMenu->vpadMenu下的该项选中
}

/* 创建构层色板(Layer Palette, Color&Pattern) */
void Optimask::createLayerPalette()
{
    //NOTE!! 请先检查LayerPalette是否已经存在。
    //如果存在，只需显示(viewLayerPalette)； 如果不存在，那么创建(createLayerPalette)。

    ColorPalette palette=ColorPalette::default_palette();
    layerPalette->setObjectName(QString::fromLatin1("lt_dock_widget"));
    layer_palette = new LayerToolbox (layerPalette,"Layer_Palette");
    layer_palette->set_palette(palette);
    layerPalette->setWidget(layer_palette);
    //connect(layerPalette, SIGNAL(visibilityChanged(bool)), this, SLOT(dock_widget_visibility_changed(bool)));
    //dispLayerPaletteAct->setChecked(true); //将viewMenu->vpadMenu下的该项选中
    pencolor = QColor(Qt::white) ;
    brushcolor =QColor(Qt::white);
    connect(layer_palette,SIGNAL(frameColorInf(QColor &)),this,SLOT(changeFrameColor(QColor &)));
    connect(layer_palette,SIGNAL(fillColorInf(QColor &)),this,SLOT(changeFillColor(QColor &)));
}

void Optimask::createAltrDock()
{
    setDockOptions(QMainWindow::AnimatedDocks);
    QDockWidget::DockWidgetFeatures features =
            QDockWidget::DockWidgetMovable |
            QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable;
    altrDock->setFeatures(features);
    alterItem = new AlterDockWidget(this,"Alter");
    altrDock->setWidget(alterItem);
}

/* 创建构元结构面板(Cell Structure Tree Panel), 构元组织(Cell Hierarchy) */
void Optimask::createCellDock()
{
//    QAction *addSeparator = 0;
//    cellTreeWidget= new QWidget(cellDock);
//    QVBoxLayout *vboxLayout = new QVBoxLayout(cellTreeWidget);
//    vboxLayout->setSpacing(0);
//    vboxLayout->setContentsMargins(0, 0, 0, 0);
//    vboxLayout->setObjectName(QStringLiteral("vboxLayout"));

//    cellTreeTabs = new QTabWidget(cellTreeWidget); //将QTabWidget变成中心部件
//    cellTreeTabs->setObjectName(QStringLiteral("cellTreeTabs"));
//    //cellTreeTabs->setTabsClosable(true);

//    QHBoxLayout *topHoritalLayout = new QHBoxLayout();
//    QToolBar *toolBar = new QToolBar();
//    QAction *hierarchyAct = new QAction(cellDock);
//    hierarchyAct->setIcon(QIcon(rsrcPath+"/hierarchy.png"));
//    hierarchyAct->setToolTip(tr("Hierarchy"));
//    QAction *flatAct = new QAction( cellDock);
//    flatAct->setIcon(QIcon(rsrcPath+"/flat.png"));
//    flatAct->setToolTip(tr("Flat"));
//    QAction *alphabetAct = new QAction(cellDock);
//    alphabetAct->setIcon(QIcon(rsrcPath+"/alphabet.png"));
//    alphabetAct->setToolTip(tr("Alphabet"));
//    QAction *timeAct = new QAction(cellDock);
//    timeAct->setIcon(QIcon(rsrcPath+"/time.png"));
//    timeAct->setToolTip(tr("Time"));
//    QAction *sizeAct = new QAction(cellDock);
//    sizeAct->setIcon(QIcon(rsrcPath+"/size.png"));
//    sizeAct->setToolTip(tr("Size"));
//    QList<QAction*> actions;
//    actions << addSeparator << hierarchyAct << flatAct  << addSeparator
//            << alphabetAct << timeAct << sizeAct;
//    foreach (QAction *action, actions) {
//        if (!action) {
//            toolBar->addSeparator();
//        }
//        else {
//            toolBar->addAction(action);
//        }
//    }

//    topHoritalLayout->addWidget(toolBar);

//    QHBoxLayout *horitalLayout=new QHBoxLayout();
//    QLabel *label1=new QLabel("Levels");
//    QSpinBox *spinbox1=new QSpinBox();
//    spinbox1->setRange(-100,100);
//    QLabel *label2=new QLabel("...");
//    QSpinBox *spinbox2=new QSpinBox();
//    spinbox2->setRange(-100,100);
//    horitalLayout->addWidget(label1);
//    horitalLayout->addWidget(spinbox1);
//    horitalLayout->addWidget(label2);
//    horitalLayout->addWidget(spinbox2);

//    QHBoxLayout *horitalLayout2=new QHBoxLayout();
//    QToolBar *toolBar1 = new QToolBar();
//    QLineEdit *lineEdit = new QLineEdit();
//    lineEdit->setMaximumSize(175, 20);
//    lineEdit->setMinimumSize(175, 20);
//    QAction *findAct = new QAction(cellDock);
//    findAct->setIcon(QIcon(rsrcPath+"/find.png"));
//    findAct->setToolTip(tr("Find"));
//    toolBar1->addAction(findAct);
//    horitalLayout2->addWidget(lineEdit);
//    horitalLayout2->addWidget(toolBar1);

//    vboxLayout->addLayout(topHoritalLayout);
//    vboxLayout->addLayout(horitalLayout2);
//    vboxLayout->addWidget(cellTreeTabs);
//    vboxLayout->addLayout(horitalLayout);

//    cellDock->setWidget(cellTreeWidget);
//    connect(cellTreeTabs, SIGNAL(currentChanged(int)), this, SLOT(currentCellTreeChanged(int)));
    QAction *addSeparator = 0;
    cellTreeWidget= new QWidget(cellDock);
    QVBoxLayout *vboxLayout = new QVBoxLayout(cellTreeWidget);
    vboxLayout->setSpacing(0);
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    vboxLayout->setObjectName(QStringLiteral("vboxLayout"));

    cellTreeTabs = new QTabWidget(cellTreeWidget); //将QTabWidget变成中心部件
    cellTreeTabs->setObjectName(QStringLiteral("cellTreeTabs"));
    //cellTreeTabs->setTabsClosable(true);

    QHBoxLayout *topHoritalLayout = new QHBoxLayout();
    QComboBox *comboBox = new QComboBox();
    comboBox->addItems(QStringList()<<"Top dowm - all cells"<<"Bottom up - all cells"<<"Top dowm - non -instanced"
                       <<"By date modified"<<"DRC status");

    QToolBar *toolBar = new QToolBar();
    QAction *cellCollapseAll = new QAction(QIcon(rsrcPath+"/collapse.png"), tr("Collapse All"), cellDock);
    cellCollapseAll->setToolTip(tr("Collapse All"));
    QAction *cellExpandAll = new QAction(QIcon(rsrcPath+"/Expand.png"), tr("Expand All"), cellDock);
    cellExpandAll->setToolTip(tr("Expand All"));
    QAction *cellNewCell = new QAction(QIcon(rsrcPath+"/NewCell.png"), tr("New Cell"), cellDock);
    cellNewCell->setToolTip(tr("New Cell"));
    QAction *cellDeleteCell = new QAction(QIcon(rsrcPath+"/DeleteCell.png"), tr("Delete Cell"), cellDock);
    cellDeleteCell->setToolTip(tr("Delete Cell"));
    QAction *cellCopyToTextViewl = new QAction(QIcon(rsrcPath+"/CopyToText.png"), tr("Copy"), cellDock);
    cellCopyToTextViewl->setToolTip(tr("Copy To Text View"));
    QAction *cellShowAllCell = new QAction(QIcon(rsrcPath+"/ShowAll.png"), tr("Show"), cellDock);
    cellShowAllCell->setToolTip(tr(" Show All Cell"));
    QList<QAction*> actions;
    actions << addSeparator << cellCollapseAll << cellExpandAll << addSeparator << cellNewCell
                << cellDeleteCell << addSeparator <<  cellCopyToTextViewl << addSeparator <<  cellShowAllCell;
    foreach (QAction *action, actions) {
        if (!action) {
            toolBar->addSeparator();
        }
        else {
            toolBar->addAction(action);
        }
    }

    topHoritalLayout->addWidget(comboBox);
    topHoritalLayout->addWidget(toolBar);

    QHBoxLayout *horitalLayout=new QHBoxLayout();
    QLabel *label1=new QLabel("Levels");
    QSpinBox *spinbox1=new QSpinBox();
    spinbox1->setRange(-100,100);
    QLabel *label2=new QLabel("...");
    QSpinBox *spinbox2=new QSpinBox();
    spinbox2->setRange(-100,100);
    horitalLayout->addWidget(label1);
    horitalLayout->addWidget(spinbox1);
    horitalLayout->addWidget(label2);
    horitalLayout->addWidget(spinbox2);

    QHBoxLayout *horitalLayout2=new QHBoxLayout();
    QToolBar *toolBar1 = new QToolBar();
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setMaximumSize(175, 20);
    lineEdit->setMinimumSize(175, 20);
    QAction *modeAct = new QAction(tr("Mode"), cellDock);
    modeAct->setToolTip(tr("Toggle Show Mode"));
    toolBar1->addSeparator();
    toolBar1->addAction(modeAct);
    horitalLayout2->addWidget(lineEdit);
    horitalLayout2->addWidget(toolBar1);

    vboxLayout->addLayout(topHoritalLayout);
    vboxLayout->addLayout(horitalLayout2);
    vboxLayout->addWidget(cellTreeTabs);
    vboxLayout->addLayout(horitalLayout);

    cellDock->setWidget(cellTreeWidget);
    dispViewToolBarAct->setChecked(true); //将viewMenu->vpadMenu下的该项选中
    connect(cellTreeTabs, SIGNAL(currentChanged(int)), this, SLOT(currentCellTreeChanged(int)));
}

void Optimask::createWorkDock()
{
//    centralwidget = new QWidget(workDock);
//    centralwidget->setObjectName(QStringLiteral("centralwidget"));
//    QVBoxLayout *vboxLayout = new QVBoxLayout(centralwidget);
//    vboxLayout->setSpacing(0);
//    vboxLayout->setContentsMargins(0, 0, 0, 0);
//    vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
//    workTabs = new QTabWidget(centralwidget);                   //将QTabWidget变成中心部件
//    workTabs->setObjectName(QStringLiteral("workTabs"));
//    workTabs->setTabsClosable(true);
//    vboxLayout->addWidget(workTabs);

//    centralwidget->setFixedSize(workDock->width(),workDock->height());

//    workDock->setWidget(centralWidget());
//    connect(workTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(removeWorkSubTab(int)));
//    connect(workTabs, SIGNAL(currentChanged(int)), this, SLOT(currentWorkViewChanged(int)));
//    //centralwidget->setMinimumSize(workDock->width(),workDock->height());

    workTabs = new QTabWidget();
    workTabs->setObjectName(QStringLiteral("workTabs"));
    workTabs->setTabsClosable(true);
//    vboxLayout->addWidget(workTabs);
    workDock->setWidget(workTabs);
    workTabs->setMinimumSize(workDock->width(),workDock->height());

//        setCentralWidget(workDock);
    connect(workTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(removeWorkSubTab(int)));
    connect(workTabs, SIGNAL(currentChanged(int)), this, SLOT(currentWorkViewChanged(int)));

}
/* 创建视图导航区域(Navigator, Aerial View) */
void Optimask::createNaviDock()
{
    layerNaviTabs=new QTabWidget(naviDock);
    naviDock->setWidget(layerNaviTabs);
    connect(layerNaviTabs,SIGNAL(currentChanged(int)),this,SLOT(currentNavigatorChanged(int)));
}

/* 创建编程区域(Command, Script, Macro, Code, Programming) */
void Optimask::createCodeDock()
{
    Command *command     = new Command(this);
    codeDock->setWidget(command);

    connect(command, SIGNAL(drawRectInf(CGeoPt,CGeoPt)), this, SLOT(addRect(CGeoPt,CGeoPt)));
    connect(command, SIGNAL(drawLineInf(CGeoPt,CGeoPt)), this, SLOT(addLine(CGeoPt,CGeoPt)));
    connect(command, SIGNAL(drawPolygonInf(std::vector<CGeoPt>,bool)), this, SLOT(addPolygon(std::vector<CGeoPt>,bool)));
    connect(command, SIGNAL(drawEllipseInf(CGeoPt, double, double)), this, SLOT(addEllipse(CGeoPt ,double ,double )));
    connect(command, SIGNAL(drawArcInf(CGeoPt, double, double, double)), this, SLOT(addArc(CGeoPt, double, double, double)));
    connect(command, SIGNAL(redoInf()), this, SLOT(editRedo()));
    connect(command, SIGNAL(undoInf()), this, SLOT(editUndo()));

}
/********创建信息输出区域******/
void Optimask::createInfoDock()
{
    Info *info = new Info();
    infoDock->setWidget(info);
}
/******创建提示报警区域*******/
void Optimask::createHintDock()
{
    Hint *hint = new Hint();
    hintDock->setWidget(hint);
}
void Optimask::createConnections()
{
    // File
    connect(fileNewAct,            SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(fileOpenAct,           SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(fileSaveAct,           SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(fileSaveAsAct,         SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(fileInfoAct,           SIGNAL(triggered()), this, SLOT(fileInfo()));
    connect(fileOptnAct,           SIGNAL(triggered()), this, SLOT(fileOptn()));
    connect(filePrintAct,          SIGNAL(triggered()), this, SLOT(filePrint()));
    connect(filePrintPreviewAct,   SIGNAL(triggered()), this, SLOT(filePrintPreview()));
    connect(fileExitAct,           SIGNAL(triggered()), this, SLOT(fileExit()));

    // Edit
    connect(editSelectAction,      SIGNAL(triggered()), this, SLOT(editSelect()));
    connect(editMoveViewAct,       SIGNAL(triggered()), this, SLOT(editMoveView()));
    connect(editSelectedItemAction,SIGNAL(triggered()), this, SLOT(editSelectedItem()));
    connect(editUndoAct,           SIGNAL(triggered()), this, SLOT(editUndo()));
    connect(editRedoAct,           SIGNAL(triggered()), this, SLOT(editRedo()));
    connect(editCutAct,            SIGNAL(triggered()), this, SLOT(editCut()));
    connect(editCopyAct,           SIGNAL(triggered()), this, SLOT(editCopy()));
    connect(editPasteAct,          SIGNAL(triggered()), this, SLOT(editPaste()));
    connect(editPastetoAct,        SIGNAL(triggered()), this, SLOT(editPasteTo()));
    connect(editSelectCellsAct,    SIGNAL(triggered()), this, SLOT(editSelectCell()));
    connect(QApplication::clipboard(), SIGNAL(dataChanged()),this, SLOT(updateUi()));

    // View Toolbars
    foreach (QAction *action, QList<QAction*>()
            << dispFileToolBarAct << dispEditToolBarAct << dispViewToolBarAct
            << dispDrawToolBarAct << dispAltrToolBarAct << dispCellToolBarAct
            << dispLayerToolBarAct << dispCodeToolBarAct << dispCnfgToolBarAct
            << dispToolToolBarAct << dispWndwToolBarAct << dispHelpToolBarAct){
        connect(action, SIGNAL(triggered()), this, SLOT(dispToolBar()));
    }

    // View Panels (Docks)
    foreach (QAction *action, QList<QAction*>()
            << dispLayerPanelAct << dispLayerPaletteAct << dispCellDockAct
            << dispNaviDockAct << dispAltrDockAct << dispCodeDockAct
            << dispInfoDockAct << dispHintDockAct){
        connect(action, SIGNAL(triggered()), this, SLOT(dispPanel()));
    }

    //check change ,dockwidget statue change
    connect(dispLayerPanelAct,SIGNAL(triggered()), this, SLOT(dockChanged()));
    connect(dispLayerPaletteAct,SIGNAL(triggered()), this, SLOT(dockChanged()));
    connect(dispCellDockAct,SIGNAL(triggered()), this, SLOT(dockChanged()));
    connect(dispWorkDockAct,SIGNAL(triggered()), this, SLOT(dockChanged()));
    connect(dispNaviDockAct,SIGNAL(triggered()), this, SLOT(dockChanged()));
    connect(dispAltrDockAct,SIGNAL(triggered()), this, SLOT(dockChanged()));
    connect(dispCodeDockAct,SIGNAL(triggered()), this, SLOT(dockChanged()));
    connect(dispInfoDockAct,SIGNAL(triggered()), this, SLOT(dockChanged()));
    connect(dispHintDockAct,SIGNAL(triggered()), this, SLOT(dockChanged()));

    //close dockwidget ,checked change
    connect(layerPanel,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));
    connect(layerPalette,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));
    connect(cellDock,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));
    connect(workDock,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));
    connect(naviDock,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));
    connect(altrDock,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));
    connect(codeDock,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));
    connect(infoDock,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));
    connect(hintDock,SIGNAL(visibilityChanged(bool)), this, SLOT(dockCloseOrOpen()));

    //Ruler
    connect(dispRulerAct,SIGNAL(triggered()), this, SLOT(rulerStatusChanged()));

    // Draw
    connect(drawClearAct,          SIGNAL(triggered()), this, SLOT(drawClear()));
    connect(drawLibraryAct,        SIGNAL(triggered()), this, SLOT(drawLibrary()));
    connect(drawLineAct,           SIGNAL(triggered()), this, SLOT(drawShape()));
    connect(drawRectAct,           SIGNAL(triggered()), this, SLOT(drawShape()));
    connect(drawPolyAct,           SIGNAL(triggered()), this, SLOT(drawShape()));
    connect(drawCircleAct,         SIGNAL(triggered()), this, SLOT(drawShape()));
    connect(drawPathAct,           SIGNAL(triggered()), this, SLOT(drawShape()));
    connect(drawEllipseAct,        SIGNAL(triggered()), this, SLOT(drawShape()));
    connect(drawCurveAct,          SIGNAL(triggered()), this, SLOT(drawShape()));
    connect(drawTextAct,           SIGNAL(triggered()), this, SLOT(drawRect()));
    //connect(drawLogoAct,           SIGNAL(triggered()), this, SLOT(drawShape()));
    connect(drawMultiLineAct,      SIGNAL(triggered()), this, SLOT(drawMultiLine()));
    connect(drawMatrixAct,         SIGNAL(triggered()),this,SLOT(drawMatrix()));

    // Alter
    connect(alterAlignmentAct,     SIGNAL(triggered()), this, SLOT(alterAlign()));
    connect(alterAlignGridAct,     SIGNAL(triggered()), this, SLOT(alterAlign()));
    connect(alterAlignLeftAct,     SIGNAL(triggered()), this, SLOT(alterAlign()));
    connect(alterAlignRightAct,    SIGNAL(triggered()), this, SLOT(alterAlign()));
    connect(alterAlignTopAct,      SIGNAL(triggered()), this, SLOT(alterAlign()));
    connect(alterAlignBottomAct,   SIGNAL(triggered()), this, SLOT(alterAlign()));
    connect(alterConvertAct,       SIGNAL(triggered()), this, SLOT(alterConvert()));
    connect(alterRegenAct,         SIGNAL(triggered()), this, SLOT(alterRegen()));
    connect(alterGroupAct,         SIGNAL(triggered()), this, SLOT(alterGroup()));
    connect(alterUngroupAct,       SIGNAL(triggered()), this, SLOT(alterUngroup()));
    connect(alterCloneAct,         SIGNAL(triggered()), this, SLOT(alterClone()));
    connect(alterSplitAct,         SIGNAL(triggered()), this, SLOT(alterSplit()));
    connect(alterCutXAct,          SIGNAL(triggered()), this, SLOT(alterCutX()));
    connect(alterCutYAct,          SIGNAL(triggered()), this, SLOT(alterCutY()));
    connect(alterCutLineAct,       SIGNAL(triggered()), this, SLOT(alterCutLine()));
    connect(alterCropAct,          SIGNAL(triggered()), this, SLOT(alterCrop()));
    connect(alterMergeAct,         SIGNAL(triggered()), this, SLOT(alterMerge()));
    connect(alterFractAct,         SIGNAL(triggered()), this, SLOT(alterFract()));
    connect(alterExLayAct,         SIGNAL(triggered()), this, SLOT(alterExLay()));
    connect(alterBooleanAct,       SIGNAL(triggered()), this, SLOT(alterBoolean()));
    connect(alterBoolAndAct,       SIGNAL(triggered()), this, SLOT(alterBoolAnd()));
    connect(alterBoolOrAct,        SIGNAL(triggered()), this, SLOT(alterBoolOr()));
    connect(alterBoolXorAct,       SIGNAL(triggered()), this, SLOT(alterBoolXor()));
    connect(alterBoolAnotBAct,     SIGNAL(triggered()), this, SLOT(alterBoolANotB()));
    connect(alterBoolBnotAAct,     SIGNAL(triggered()), this, SLOT(alterBoolBNotA()));
    connect(alterTransformAct,     SIGNAL(triggered()), this, SLOT(alterTransform()));
    connect(alterMoveAct,          SIGNAL(triggered()), this, SLOT(alterMove()));
    connect(alterTurnAct,          SIGNAL(triggered()), this, SLOT(alterTurn()));
    connect(alterFlipAct,          SIGNAL(triggered()), this, SLOT(alterFlip()));
    connect(alterScaleAct,         SIGNAL(triggered()), this, SLOT(alterScale()));
    connect(alterStretchAct,       SIGNAL(triggered()), this, SLOT(alterStretch()));
    connect(alterComputeAct,       SIGNAL(triggered()), this, SLOT(alterCompute()));

    // Cell
    connect(cellNewAct,            SIGNAL(triggered()), this, SLOT(cellNew()));
    connect(cellOpenAct,           SIGNAL(triggered()), this, SLOT(cellOpen()));
    connect(cellCopyAct,           SIGNAL(triggered()), this, SLOT(cellCopy()));
    connect(cellPasteAct,          SIGNAL(triggered()), this, SLOT(cellPaste()));
    connect(cellArrangeAct,        SIGNAL(triggered()), this, SLOT(cellArrange()));
    connect(cellMoveOrgAct,        SIGNAL(triggered()), this, SLOT(cellMoveOrg()));
    connect(cellInstanceAct,       SIGNAL(triggered()), this, SLOT(cellInstance()));
    connect(cellChangeInstanceAct, SIGNAL(triggered()), this, SLOT(cellChangeInstance()));
    connect(cellMultiAlterAct,     SIGNAL(triggered()), this, SLOT(cellMultiAlter()));
    connect(cellAssembleAct,       SIGNAL(triggered()), this, SLOT(cellAssemble()));
    connect(cellAssembleAct,       SIGNAL(triggered()), this, SLOT(cellHierarchy()));
    connect(cellFlattenAct,        SIGNAL(triggered()), this, SLOT(cellFlatten()));
    connect(cellFractureAct,       SIGNAL(triggered()), this, SLOT(cellFracture()));
    connect(cellInfoAct,           SIGNAL(triggered()), this, SLOT(cellInfo()));
    connect(cellInfoAct,           SIGNAL(triggered()), this, SLOT(cellInfo()));
    connect(cellFrameAct,          SIGNAL(triggered()), this, SLOT(cellFrame()));
    connect(cellShowAct,           SIGNAL(triggered()), this, SLOT(cellShow()));
    connect(cellHideAct,           SIGNAL(triggered()), this, SLOT(cellHide()));
    connect(cellProtectAct,        SIGNAL(triggered()), this, SLOT(cellProtect()));
    connect(cellUnprotectAct,      SIGNAL(triggered()), this, SLOT(cellUnprotect()));
    connect(cellLockAct,           SIGNAL(triggered()), this, SLOT(cellLock()));
    connect(cellUnlockAct,         SIGNAL(triggered()), this, SLOT(cellUnlock()));

    // Layer
    connect(layerOptnAct,          SIGNAL(triggered()), this, SLOT(layerOptn()));
    connect(layerLoadFileAct,      SIGNAL(triggered()), this, SLOT(layerLoadFile()));
    connect(layerSaveFileAct,      SIGNAL(triggered()), this, SLOT(layerSaveFile()));
    //connect(layerPanelAct,         SIGNAL(triggered()), this, SLOT(createLayerPanel()));
    //connect(layerPaletteAct,       SIGNAL(triggered()), this, SLOT(createLayerPalette()));
    connect(layerSetupAct,         SIGNAL(triggered()), this, SLOT(layerSetup()));
    connect(layerDensityAct,       SIGNAL(triggered()), this, SLOT(layerDensity()));
    connect(layerDummyFillAct,     SIGNAL(triggered()), this, SLOT(layerDummyFill()));
    connect(layerStatAct,          SIGNAL(triggered()), this, SLOT(layerStat()));
    connect(layerMappingAct,       SIGNAL(triggered()), this, SLOT(layerMapping()));
    connect(layerCtrlAct,          SIGNAL(triggered()), this, SLOT(layerCtrl()));
    connect(layerSelectAct,        SIGNAL(triggered()), this, SLOT(layerSelect()));
    connect(layerSelectAct,        SIGNAL(triggered()), this, SLOT(layerDeselect()));
    connect(layerShowAct,          SIGNAL(triggered()), this, SLOT(layerShow()));
    connect(layerHideAct,          SIGNAL(triggered()), this, SLOT(layerHide()));
    connect(layerProtectAct,       SIGNAL(triggered()), this, SLOT(layerProtect()));
    connect(layerUnprotectAct,     SIGNAL(triggered()), this, SLOT(layerUnprotect()));
    connect(layerLockAct,          SIGNAL(triggered()), this, SLOT(layerLock()));
    connect(layerUnlockAct,        SIGNAL(triggered()), this, SLOT(layerUnlock()));
    connect(layerFillAct,          SIGNAL(triggered()), this, SLOT(layerFill()));
    connect(layerUnfillAct,        SIGNAL(triggered()), this, SLOT(layerUnfill()));

    // Script
    connect(codeOptnAct,     SIGNAL(triggered()), this, SLOT(codeOptn()));
    connect(codeNewAct,      SIGNAL(triggered()), this, SLOT(codeNew()));
    connect(codeOpenAct,     SIGNAL(triggered()), this, SLOT(codeOpen()));
    connect(codeSaveAct,     SIGNAL(triggered()), this, SLOT(codeSave()));
    connect(codeRunAct,      SIGNAL(triggered()), this, SLOT(codeRun()));
    connect(codeDebugAct,    SIGNAL(triggered()), this, SLOT(codeDebug()));

    // Config
    connect(cnfgOptnAct,           SIGNAL(triggered()), this, SLOT(cnfgOptn()));
    connect(cnfgFontAct,           SIGNAL(triggered()), this, SLOT(cnfgFont()));
    connect(cnfgDRCAct,            SIGNAL(triggered()), this, SLOT(cnfgDRC()));

    // Tool
    connect(toolUtilityAct,        SIGNAL(triggered()), this, SLOT(toolUtility()));
    connect(toolLayoutStatAct,     SIGNAL(triggered()), this, SLOT(toolLayoutStat()));
    connect(toolLayoutCompareAct,  SIGNAL(triggered()), this, SLOT(toolLayoutCompare()));
    connect(toolLayoutDiffAct,     SIGNAL(triggered()), this, SLOT(toolLayoutDiff()));
    connect(toolWaferAct,          SIGNAL(triggered()), this, SLOT(toolWafer()));

    // Window
    connect(wndwSplitAct,          SIGNAL(triggered()), this, SLOT(wndwSplit()));
    connect(wndwTileAct,           SIGNAL(triggered()), this, SLOT(wndwTile()));
    connect(wndwCascadeAct,        SIGNAL(triggered()), this, SLOT(wndwCascade()));
    connect(wndwArrangeAct,        SIGNAL(triggered()), this, SLOT(wndwArrange()));

    // Help
    connect(helpIndexAct,          SIGNAL(triggered()), this, SLOT(helpIndex()));
    connect(helpQuickRefAct,       SIGNAL(triggered()), this, SLOT(helpQuickRef()));
    connect(helpManualAct,         SIGNAL(triggered()), this, SLOT(helpManual()));
    connect(helpExampleAct,        SIGNAL(triggered()), this, SLOT(helpExample()));
    connect(helpAppNoteAct,        SIGNAL(triggered()), this, SLOT(helpAppNote()));
    connect(helpUpdateAct,         SIGNAL(triggered()), this, SLOT(helpUpdate()));
    connect(helpHistoryAct,        SIGNAL(triggered()), this, SLOT(helpHistory()));
    connect(helpLicenceAct,        SIGNAL(triggered()), this, SLOT(helpLicence()));
    connect(helpSupportAct,        SIGNAL(triggered()), this, SLOT(helpSupport()));
    connect(helpReportBugAct,      SIGNAL(triggered()), this, SLOT(helpReportBug()));
    connect(helpAboutAct,          SIGNAL(triggered()), this, SLOT(helpAbout()));

}

/* 新建文件 */
void Optimask::fileNew()
{
    newGdsDocument();
    connect(CommandManager::Instance(), SIGNAL(UndoRedoState(bool,bool)), this,SLOT(UndoRedoStateChange(bool,bool)));
    clear();
    updateUi();
}

/* 打开文件 */
void Optimask::fileOpen()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, "Select one or more files to open", "/home"); //获取文件路径名字
    for(QStringList::iterator itr = fileNames.begin(); itr != fileNames.end(); ++itr){
        QString fileName = (*itr) ;
        if (fileName.isEmpty())
            continue;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) { //只读
            QMessageBox::warning(this,
                                 tr("File error"),
                                 tr("Failed to open\n%1").arg(fileName));
            continue;
        }
        CGdsDocument *gdsdoc = new CGdsDocument(); //新建一个document
        gdsdoc->LoadDocument(fileName);
        m_vecDocument.push_back(gdsdoc);

        COptimaskView *view=new COptimaskView(gdsdoc);
        view->m_parent=true;
        view->EnterDrawSceneEvent();


        if(fileName.contains(".gds", Qt::CaseInsensitive)){

//            Navigator *navigator=new Navigator();
//            navigator->SetGdsDocument(gdsdoc);
//            //gdsdoc->SetViewWid(view);
//            gdsdoc->SetNavigator(navigator);
//            //addGdsDocument(gdsdoc);
        }
        //设置celltree窗口以及其数据
        CCellTreeWidget* celltree=new CCellTreeWidget();
        celltree->SetGdsDocument(gdsdoc);
        celltree->ConstructTreeWidget();
        connect(celltree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showSelectedItem(QTreeWidgetItem*,int)));
        //设置layer窗口以及其数据
        LayTableView *tableview=new LayTableView();
        tableview->SetGdsDocument(gdsdoc);
        tableview->SetModeData();

        //生成停靠窗口
        gdsdoc->SetLayTableViewWid(tableview);    // Layer窗口
        gdsdoc->SetCellTreeWid(celltree);         // celltree窗口
        gdsdoc->SetViewWid(view);
        //view->TestData();
        view->DrawMainStructure();
        addGdsDocument(gdsdoc);
    }
}

/* 关闭文件 */
void Optimask::fileClose()
{

}

/* 保存文件 */
void Optimask::fileSave()
{
    QString filename;
    filename=QFileDialog::getSaveFileName(this, tr("Save File"),
                                          "",
                                          tr("Images (*.png *.xpm *.jpg)"));
}

/* 另存文件 */
void Optimask::fileSaveAs()
{
    QString filename;     // 另存为的文件名
    filename=QFileDialog::getSaveFileName(this, tr("Save File"),"",
                                          tr("GDSII (*.gds *.sf)")); //ttt(*.gds *sf)Images
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        fileSaveGDSII(filename);

    }
}

/* GDSII格式文件存储 */
void Optimask::fileSaveGDSII(QString filename)
{
    if (!(filename.endsWith(".gds", Qt::CaseInsensitive) || filename.endsWith(".sf", Qt::CaseInsensitive))) {
        filename += ".gds";                                       // 默认文件后缀.gds
    }
    //currentView()->shape2structure();
    //write.writeGDS(filename,currentView(), currentView()->GetGdsDocument()->GetGdsInfo());    // 将数据写到filename文件中->GetGDSIRead(),
}

/* 文件信息 */
void Optimask::fileInfo()
{
}

/* 文件选项 */
void Optimask::fileOptn()
{
}

/* 文件打印 */
void Optimask::filePrint()
{
    QPrinter printer;
    if(QPrintDialog(&printer).exec()==QDialog::Accepted)
    {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        this->render(&painter);
    }
}

void Optimask::filePrintPreview()
{

}

void Optimask::fileExit(){}

void Optimask::editSelect()
{
    if(currentView() != NULL){
        currentView()->SetSelectEvent(true);
        currentView()->SetDrawEvent(false);
//        currentView()->setCursor(Qt::ArrowCursor);
//        currentView()->setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void Optimask::editMoveView()
{
    if(currentView() != NULL){
        currentView()->setCursor(Qt::OpenHandCursor);
    }
}

void Optimask::editSelectedItem()
{
    COptimaskView *view = currentView();
    if(view != NULL){
        QList<QGraphicsItem*> items = view->getGraphicsScene()->selectedItems();
        if (items.count() != 1)
            return;
    #ifdef NO_DYNAMIC_CAST
        if (QObject *item = qObjectFrom(items.at(0))) {
    #else
        if (QObject *item = dynamic_cast<QObject*>(items.at(0))) {
    #endif
            const QMetaObject *metaObject = item->metaObject();
            metaObject->invokeMethod(item, "edit", Qt::DirectConnection);
        }
    }
}

void Optimask::editUndo()
{
    UNDO;
}

void Optimask::editRedo()
{
    REDO;
}

void Optimask::UndoRedoStateChange(bool undo, bool redo)
{
    editUndoAct->setEnabled(undo);
    editRedoAct->setEnabled(redo);
}

void Optimask::editSelectCell()
{

}

void Optimask::editCopy()
{
    COptimaskView *view=currentView();
    if (view==NULL)
        return;

    QList<QGraphicsItem*> items = view->getGraphicsScene()->selectedItems();
    if (items.isEmpty())
        return;
    pasteOffset = OffsetIncrement;
    copyItems(items);
    updateUi();
}

void Optimask::editCut()
{
    QList<QGraphicsItem*> items = currentView()->getGraphicsScene()->selectedItems();
    if (items.isEmpty())
        return;
    copyItems(items);
    QListIterator<QGraphicsItem*> i(items);
    while (i.hasNext()) {
#if QT_VERSION >= 0x040600
        QScopedPointer<QGraphicsItem> item(i.next());
        currentView()->getGraphicsScene()->removeItem(item.data());
#else
        QGraphicsItem *item = i.next();
        scene->removeItem(item);
        delete item;
#endif
    }
    setDirty(true);
}

void Optimask::editPaste()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (!mimeData)
        return;

    if (mimeData->hasFormat(MimeType)) {
        QByteArray copiedItems = mimeData->data(MimeType);
        QDataStream in(&copiedItems, QIODevice::ReadOnly);
        readItems(in, pasteOffset, true);
        pasteOffset += OffsetIncrement;
    }
    else if (mimeData->hasHtml() || mimeData->hasText()) {
//        TextItem *textItem = new TextItem(position(), scene);
//        connectItem(textItem);
//        if (mimeData->hasHtml())
//            textItem->setHtml(mimeData->html());
//        else
//            textItem->setPlainText(mimeData->text());
    }
    else
        return;
    setDirty(true);
}

void Optimask::editPasteTo()
{
    PasteToDlg  *pasteToDlg = new PasteToDlg;
    pasteToDlg->show();
}

//面板(Panel, Dock)
//void Optimask::viewLayerPanel(){}      //构层面板(Layer Panel)显示
//void Optimask::viewLayerPalette(){}    //构层色板(Layer Palette)显示
//void Optimask::viewCellDock(){}        //构元结构树(Cell Structure Tree), 构元组织(Cell Hierarchy)显示;
//void Optimask::viewWorkDock(){}        //工作区域面板 (Main Workspace; Work Panel)显示;
//void Optimask::viewNaviDock(){}        //导航区域面板(Navigator, Aerial View)显示;
//void Optimask::viewCodeDock(){}        //计算和编程面板(Computation, Programming, Script, Macro, Code)显示;
//void Optimask::viewInfoDock(){}        //信息输出栏(Information, Status, Result, Output)显示
//void Optimask::viewHintDock(){}        //提示报警栏(Hint, Error, Warning, Issue)显示

//隐藏和显示工具栏(ToolBar)
void Optimask::dispToolBar()
{
    if(sender() == dispFileToolBarAct){
        fileToolBar->setVisible(dispFileToolBar);
        dispFileToolBar = !dispFileToolBar;
    }
    if(sender() == dispEditToolBarAct){
        editToolBar->setVisible(dispEditToolBar);
        dispEditToolBar = !dispEditToolBar;
    }
    if(sender() == dispViewToolBarAct){
        viewToolBar->setVisible(dispViewToolBar);
        dispViewToolBar = !dispViewToolBar;
    }
    if(sender() == dispDrawToolBarAct){
        drawToolBar->setVisible(dispDrawToolBar);
        dispDrawToolBar = !dispDrawToolBar;
    }
    if(sender() == dispAltrToolBarAct){
        altrToolBar->setVisible(dispAltrToolBar);
        dispAltrToolBar = !dispAltrToolBar;
    }
    if(sender() == dispCellToolBarAct){
        cellToolBar->setVisible(dispCellToolBar);
        dispCellToolBar = !dispCellToolBar;
    }
    if(sender() == dispLayerToolBarAct){
        layerToolBar->setVisible(dispLayerToolBar);
        dispLayerToolBar = !dispLayerToolBar;
    }
    if(sender() == dispCodeToolBarAct){
        codeToolBar->setVisible(dispCodeToolBar);
        dispCodeToolBar = !dispCodeToolBar;
    }
    if(sender() == dispCnfgToolBarAct){
        cnfgToolBar->setVisible(dispCnfgToolBar);
        dispCnfgToolBar =! dispCnfgToolBar;
    }
    if(sender() == dispToolToolBarAct){
        toolToolBar->setVisible(dispToolToolBar);
        dispToolToolBar = !dispToolToolBar;
    }
    if(sender() == dispWndwToolBarAct){
        wndwToolBar->setVisible(dispWndwToolBar);
        dispWndwToolBar = !dispWndwToolBar;
    }
    if(sender() == dispHelpToolBarAct){
        helpToolBar->setVisible(dispHelpToolBar);
        dispHelpToolBar = !dispHelpToolBar;
    }
}

//隐藏和显示面板(Dock, Panel, Pad)
void Optimask::dispPanel()
{
//    if(sender() == dispLayerPanelAct){
//        LayerPanel->setVisible(dispLayerPanel);
//        dispLayerPanel = !dispLayerPanel;
//    }
//    if(sender() == dispLayerPaletteAct){
//        LayerPalette->setVisible(dispLayerPalette);
//        dispLayerPalette = !dispLayerPalette;
//    }
    if(sender() == dispCellDockAct){
        cellDock->setVisible(dispCellDock);
        dispCellDock = !dispCellDock;
    }
    if(sender() == dispNaviDockAct){
        naviDock->setVisible(dispNaviDock);
        dispNaviDock = !dispNaviDock;
    }
    if(sender() == dispAltrDockAct){
        altrDock->setVisible(dispAltrDock);
       dispAltrDock = !dispAltrDock;
    }
    if(sender() == dispCodeDockAct){
        codeDock->setVisible(dispCodeDock);
        dispCodeDock = !dispCodeDock;
    }
    if(sender() == dispInfoDockAct){
        infoDock->setVisible(dispInfoDock);
        dispInfoDock = !dispInfoDock;
    }
    if(sender() == dispHintDockAct){
        hintDock->setVisible(dispHintDock);
        dispHintDock = !dispHintDock;
    }
 }

void Optimask::drawShape()
{
    if(currentView() != NULL){
        COptimaskView *view=currentView();
        const QMetaObject *metaObject = view->metaObject();
        int type = -1;
        if (sender() == drawLineAct)
            type = GEO_TYPE_LINE ;
        else if (sender() == drawRectAct)
            type = GEO_TYPE_RECT;
        else if (sender() == drawPolyAct)
            type = GEO_TYPE_POLYGON;
        else if (sender() == drawCircleAct)
            type = GEO_TYPE_CIRCLE;
        else if (sender() == drawPathAct)
            type = GEO_TYPE_PATH;
        else if(sender() == drawEllipseAct)
            type = GEO_TYPE_ELLIPSE;
        else if(sender() == drawCurveAct)
            type = GEO_TYPE_ARC;
        else if (sender() == drawTextAct)
            type = GEO_TYPE_TEXT;
        else return;
   metaObject->invokeMethod(view, "drawShape", Qt::DirectConnection, Q_ARG(const int, type));
    }
}

void Optimask::drawClear()             //清除场景中所有的图元
{

}

void Optimask::drawLibrary()           //图元库
{
    libraryWidget = new LibraryWidget;
    libraryWidget->show();
}
void Optimask::drawLine()              //绘制线段
{

}

void Optimask::drawRect()              //绘制矩形
{
        COptimaskView *view = currentView();
        if (view == NULL)
            return;
        QAction *action = qobject_cast<QAction*>(sender());
        if (!action)
            return;
        QObject *item = 0;
        int type = action->data().toInt();
        if (type == BoxItemType){
            item = new BoxItem(QRect(position(), QSize(90, 30)), view->getGraphicsScene());
            //CGraphicsItem *itemrect = new CGraphicsItem(view->getGraphicsScene());
            //itemrect->update();
            //view->getGraphicsScene()->addItem(itemrect);
        }
//        else if (type == SmileyItemType){
//            item = new SmileyItem(position(), scene);
//        }
        else if (type == TextItemType) {
            TextItemDialog dialog(0, position(), view->getGraphicsScene(), this);
            if (dialog.exec())
                item = dialog.textItem();
        }

        if (item) {
            connectItem(item);
            setDirty(true);
        }
}

void Optimask::drawPoly(){}            //绘制多边形
void Optimask::drawCircle(){}          //绘制圆形
void Optimask::drawEllipse(){}         //绘制椭圆
void Optimask::drawCurve(){}           //绘制曲线
void Optimask::drawMatrix()            //绘制矩阵数组确定的图元
{
    matrixWidget = new Matrix;
    matrixWidget->show();
}
void Optimask::drawText(){}            //加入文字
void Optimask::drawLogo(){}            //加入图标logo
void Optimask::drawMultiLine()         //绘制多条线段(Draw Multiple Lines)
{
    multiLine = new MultiLine;
    multiLine->show();
}
void Optimask::drawMultiCurve(){}      //绘制多条曲线(Draw Multiple Curves)
void Optimask::drawScan(){}            //扫描读入图形文件(Scan/Import Pictures)(高级功能)

void Optimask::alterAlign()            //对齐(Align)至格点(Grid) (Snap to Grid)
{
    COptimaskView *view = currentView();
    if(view != NULL){
        QAction *action = qobject_cast<QAction*>(sender());
        if (!action)
            return;

        Qt::Alignment alignment = static_cast<Qt::Alignment>(
                action->data().toInt());
        if (action != alterAlignmentAct) {
            alterAlignmentAct->setData(action->data());
            alterAlignmentAct->setIcon(action->icon());
        }

        QList<QGraphicsItem*> items = view->getGraphicsScene()->selectedItems();
        QVector<double> coordinates;
        populateCoordinates(alignment, &coordinates, items);
        double offset;
        if (alignment == Qt::AlignLeft || alignment == Qt::AlignTop){
            offset = *std::min_element(coordinates.constBegin(),
                                       coordinates.constEnd());
        }else{
            offset = *std::max_element(coordinates.constBegin(),
                                       coordinates.constEnd());
        }

        if (alignment == Qt::AlignLeft || alignment == Qt::AlignRight) {
            for (int i = 0; i < items.count(); ++i)
                items.at(i)->moveBy(offset - coordinates.at(i), 0);
        }
        else {
            for (int i = 0; i < items.count(); ++i)
                items.at(i)->moveBy(0, offset - coordinates.at(i));
        }
        setDirty(true);
    }
}
void Optimask::alterConvert(){}        //参见LEdit(Draw->Convert)
void Optimask::alterRegen(){}          //重塑,再生(Regenerate)
void Optimask::alterGroup(){}          //组合(Group)
void Optimask::alterUngroup(){}        //分组(UnGroup)
void Optimask::alterClone(){}          //克隆(Clone), 整体复制(Duplicate), 复制(Copy)
void Optimask::alterSplit(){}          //分裂(Split), 剪切(Cut)
void Optimask::alterCutX(){}           //沿X线切开分裂(CutX,SplitX)
void Optimask::alterCutY(){}           //沿Y线切开分裂(CutY,SplitY)
void Optimask::alterCutLine(){}        //沿任意线y=kx+b切开分裂(CutLine,SplitLine)
void Optimask::alterCrop(){}           //裁剪(裁切)(Crop)
void Optimask::alterMerge(){}          //整合,合并(Merge)
void Optimask::alterFract(){}          //细片,碎片(Fracture)所选图形. (请区分Cell菜单下的cellFractureAct)
void Optimask::alterExLay(){}          //导出选定层上所有图元(Export Layer)
void Optimask::alterBoolean(){}        //布尔运算(boolean),与非运算,逻辑运算(logic)
void Optimask::alterBoolAnd(){}        //交集(Intersection),    逻辑与(And):  保留公共部分
void Optimask::alterBoolOr(){}         //联合(Union), 相加(Add), 逻辑或(Or):   保留各自部分和公共部分
void Optimask::alterBoolXor(){}        //互斥(Exclusive),       逻辑异或(Xor): 保留各自部分(删除公共部分)
void Optimask::alterBoolANotB(){}      //相减(SUBtract A-B),    逻辑非(NOT):  保留A不属于B的部分(删除B)
void Optimask::alterBoolBNotA(){}      //相减(SUBtract B-A),    逻辑非(NOT):  保留B不属于A的部分(删除A)
void Optimask::alterTransform(){}      //转变(Transform)
void Optimask::alterMove(){}           //移位(Move, Translation, Slide), (X,Y) + (dx, dy)
void Optimask::alterTurn(){}           //旋转(Turn, Rotate), (X+j*Y)*exp(j*ANG)
void Optimask::alterFlip(){}           //镜像(Flip, Mirror, Reflect)
void Optimask::alterScale(){}          //缩放(Scaling, Grow, Shrink) (ReSize), (X,Y)*ScaleFactor
void Optimask::alterStretch(){}        //展绷(Stretch) (X*a, Y*b)
void Optimask::alterCompute(){}        //计算(Compute) 根据计算y=f(x)来实现变形

void Optimask::cellNew(){}             //新建构元(New Cell)
void Optimask::cellOpen(){}            //打开构元(Open Cell)
void Optimask::cellCopy(){}            //复制构元(Copy Cell)
void Optimask::cellPaste(){}           //粘贴构元(Paste Cell)
void Optimask::cellArrange(){}         //安排构元(位置)(Arrange Cell)
void Optimask::cellMoveOrg(){}         //改变构元基点(Move Origin)
void Optimask::cellInstance(){}        //引用构元(Instance Cell)
void Optimask::cellChangeInstance(){}  //改变引用(Change Cell's Instance)
void Optimask::cellMultiAlter(){}      //多元操作(Multiple Cells Operation)
void Optimask::cellAssemble(){}        //组装构元(Assemble Cells)
void Optimask::cellHierarchy(){}       //构元从属结构关系(Show Hierarchy)
void Optimask::cellFlatten(){}         //构元平整显示(Show All as Flat)
void Optimask::cellFracture(){}        //细片,碎片,分割(Fracture) 所选构元. (请区分Alter菜单下的alterFractAct)
void Optimask::cellInfo(){}            //构元信息(Cell Info)
void Optimask::cellStat(){}            //构元统计(Cell Statistics)
void Optimask::cellFrame(){}           //构元边界(Cell Outline/Frame)
void Optimask::cellShow(){}            //构元显示(Show the Cell)
void Optimask::cellHide(){}            //构元隐藏(Hide/Unshow the Cell)
void Optimask::cellProtect(){}         //构元保护(Protect the Cell)
void Optimask::cellUnprotect(){}       //构元解保(Unprotect the Cell)
void Optimask::cellLock(){}            //构元锁定(Lock the Cell)
void Optimask::cellUnlock(){}          //构元解锁(Unlock the Cell)

void Optimask::layerOptn(){}           //构层选项(Layer Option)
void Optimask::layerLoadFile(){}       //导入构层配置文件(Load/Import Layer Setup/Config/Technology File)
void Optimask::layerSaveFile(){}       //保存构层配置文件(Save/Export Layer Setup/Config/Technology File)
void Optimask::layerSetup(){}          //构层设置(Layer Setup)
void Optimask::layerDensity(){}        //构层密度(Layer Density = PolygonArea / TotalArea)
void Optimask::layerDummyFill(){}      //冗余填充(填充冗余构图以达到版图密度目标, Dummy Fill for Target Density)
void Optimask::layerStat(){}           //构层统计(Layer Statistics)
void Optimask::layerMapping(){}        //构层对应(Layer Mapping, 对应到指定的层设置上去)
void Optimask::layerCtrl(){}           //构层控制(Layer Control)
void Optimask::layerSelect(){}         //构层选择(Select)
void Optimask::layerDeselect(){}       //构层解选(Deselect, to make it Unselected)
void Optimask::layerShow(){}           //构层显示(Show, Unhide)
void Optimask::layerHide(){}           //构层隐藏(Hide, Unshow)
void Optimask::layerProtect(){}        //构层保护(Protect)
void Optimask::layerUnprotect(){}      //构层解保(Unprotect)
void Optimask::layerLock(){}           //构层锁定(Lock)
void Optimask::layerUnlock(){}         //构层解锁(Unlock)
void Optimask::layerFill(){}           //构层实心显示(Fill)
void Optimask::layerUnfill(){}         //构层空心显示(Unfill)

void Optimask::codeOptn(){             //代码选项(Code Option)
    COptimaskView *view=currentView();
    if (view==NULL)
        return;
    view->isSelect(true);
}
void Optimask::codeNew(){}             //新建代码(New)
void Optimask::codeOpen(){}            //打开代码(Open)
void Optimask::codeSave(){}            //保存代码(Save)
void Optimask::codeRun(){}             //运行代码(Run)
void Optimask::codeDebug(){}           //调试代码(Debug)

void Optimask::cnfgOptn(){}            //配置选项(Configuration Option)
void Optimask::cnfgFont(){}            //配置字体(Configure Font)
void Optimask::cnfgDRC(){}             //配置设计规则检查(DRC: Design Rule Check)

void Optimask::toolUtility(){}         //小工具集(Unilities)
void Optimask::toolLayoutStat(){}      //版图统计(Layout Statistics)
void Optimask::toolLayoutCompare(){}   //版图比较(Layout Comparison)
void Optimask::toolLayoutDiff(){}      //版图差分(Layout Difference)
void Optimask::toolWafer(){}           //晶圆工具(Wafer Tool)

void Optimask::wndwSplit(){}           //窗口分裂(Split the Window)
void Optimask::wndwTile(){}            //窗口排列(Tile the Window)
void Optimask::wndwCascade(){}         //窗口级联(Cascade the Window)
void Optimask::wndwArrange(){}         //窗口整理(Arrange the Window)

void Optimask::helpIndex(){}           //帮助索引(Help Index)
void Optimask::helpQuickRef(){}        //快速参考(Quick Reference, Quick Guide)
void Optimask::helpManual(){}          //帮助手册(User's Manual)
void Optimask::helpExample(){}         //帮助实例(Examples)
void Optimask::helpAppNote(){}         //应用笔记(Application Notes)
void Optimask::helpUpdate(){}          //软件更新(Software Update)
void Optimask::helpHistory(){}         //版本历史(Revision History)
void Optimask::helpLicence(){}         //软件许可(License Key)
void Optimask::helpSupport(){}         //软件支持(Software Support)
void Optimask::helpReportBug(){}       //汇报缺陷(Report Bug/Defect)
void Optimask::helpAbout(){}           //关于软件(About the Software)


void Optimask::clear()
{
    COptimaskView *view = currentView();
    if(view == NULL){
        return;
    }
    else{
        view->getGraphicsScene()->clear();
    }
//    gridGroup = 0;
    //    viewShowGrid(viewShowGridAction->isChecked());
}

bool Optimask::okToClearData()
{
//    if (isWindowModified())
//        return AQP::okToClearData(&Optimask::fileSave, this,
//                tr("Unsaved changes"), tr("Save unsaved changes?"));
    return true;
}

void Optimask::populateCoordinates(const Qt::Alignment &alignment, QVector<double> *coordinates, const QList<QGraphicsItem *> &items)
{
    QListIterator<QGraphicsItem*> i(items);
    while (i.hasNext()) {
        QRectF rect = i.next()->sceneBoundingRect();
        switch (alignment) {
            case Qt::AlignLeft:
                coordinates->append(rect.x()); break;
            case Qt::AlignRight:
                coordinates->append(rect.x() + rect.width()); break;
            case Qt::AlignTop:
                coordinates->append(rect.y()); break;
            case Qt::AlignBottom:
                coordinates->append(rect.y() + rect.height()); break;
        }
    }
}

void Optimask::getSelectionProperties(bool *hasBrushProperty, bool *hasPenProperty) const
{
    Q_ASSERT(hasBrushProperty && hasPenProperty);
    *hasBrushProperty = false;
    *hasPenProperty = false;
    COptimaskView *view = currentView();
    if(view !=NULL){
        foreach (QGraphicsItem *item, view->getGraphicsScene()->selectedItems()) {
    #ifdef NO_DYNAMIC_CAST
            if (QObject *object = qObjectFrom(item)) {
    #else
            if (QObject *object = dynamic_cast<QObject*>(item)) {
    #endif
                const QMetaObject *metaObject = object->metaObject();
                if (metaObject->indexOfProperty("brush") > -1)
                    *hasBrushProperty = true;
                if (metaObject->indexOfProperty("pen") > -1)
                    *hasPenProperty = true;
                if (*hasBrushProperty && *hasPenProperty)
                    break;
            }
        }
    }
}

Navigator * Optimask::currentNavi()
{
    Navigator *navi= qobject_cast<Navigator*>(layerNaviTabs->currentWidget());
    return navi;
}


QPoint Optimask::position()
{
    COptimaskView *view=currentView();
    QPoint point = mapFromGlobal(QCursor::pos());
    if (!view->geometry().contains(point)) {
        point = previousPoint.isNull()
                ? view->pos() + QPoint(10, 10) : previousPoint;
    }
    if (!previousPoint.isNull() && point == previousPoint) {
        point += QPoint(addOffset, addOffset);
        addOffset += OffsetIncrement;
    }
    else {
        addOffset = OffsetIncrement;
        previousPoint = point;
    }
    return view->mapToScene(point - view->pos()).toPoint();
}

void Optimask::readItems(QDataStream &in, int offset, bool select)
{
    COptimaskView *view = currentView();
    if(view == NULL)
        return;
    QSet<QGraphicsItem*> items;
    qint32 itemType;
    QGraphicsItem *item = 0;
    while (!in.atEnd()) {
        in >> itemType;
        switch (itemType) {
            case BoxItemType: {
                BoxItem *boxItem = new BoxItem(QRect(), view->getGraphicsScene());
                in >> *boxItem;
                connectItem(boxItem);
                item = boxItem;
                break;
            }
//            case SmileyItemType: {
//                SmileyItem *smileyItem = new SmileyItem(QPoint(),
//                                                        scene);
//                in >> *smileyItem;
//                connectItem(smileyItem);
//                item = smileyItem;
//                break;
//            }
            case TextItemType: {
                TextItem *textItem = new TextItem(QPoint(), view->getGraphicsScene());
                in >> *textItem;
                connectItem(textItem);
                item = textItem;
                break;
            }
        }
        if (item) {
            item->moveBy(offset, offset);
            if (select)
                items << item;
            item = 0;
        }
    }
    if (select)
        selectItems(items);
    else
        selectionChanged();
}

void Optimask::writeItems(QDataStream &out, const QList<QGraphicsItem *> &items)
{
    foreach (QGraphicsItem *item, items) {
//        if (item == gridGroup || item->group() == gridGroup)
//            continue;
        qint32 type = static_cast<qint32>(item->type());
        out << type;
        switch (type) {
            case BoxItemType:
                    out << *static_cast<BoxItem*>(item); break;
//            case SmileyItemType:
//                    out << *static_cast<SmileyItem*>(item); break;
            case TextItemType:
                    out << *static_cast<TextItem*>(item); break;
            default: Q_ASSERT(false);
        }
    }
}

void Optimask::copyItems(const QList<QGraphicsItem *> &items)
{
    QByteArray copiedItems;
    QDataStream out(&copiedItems, QIODevice::WriteOnly);
    writeItems(out, items);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MimeType, copiedItems);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

void Optimask::selectItems(const QSet<QGraphicsItem *> &items)
{
    currentView()->getGraphicsScene()->clearSelection();
    foreach (QGraphicsItem *item, items)
        item->setSelected(true);
    selectionChanged();
}

void Optimask::connectItem(QObject *item)
{
    connect(item, SIGNAL(dirty()), this, SLOT(setDirty()));
    const QMetaObject *metaObject = item->metaObject();
    if (metaObject->indexOfProperty("brush") > -1)
        connect(brushWidget, SIGNAL(brushChanged(const QBrush&)),
                item, SLOT(setBrush(const QBrush&)));
    if (metaObject->indexOfProperty("pen") > -1)
        connect(penWidget, SIGNAL(penChanged(const QPen&)),
                item, SLOT(setPen(const QPen&)));
    if (metaObject->indexOfProperty("angle") > -1) {
        connect(transformWidget, SIGNAL(angleChanged(double)),
                item, SLOT(setAngle(double)));
        connect(transformWidget, SIGNAL(shearChanged(double, double)),
                item, SLOT(setShear(double, double)));
    }
}

bool Optimask::sceneHasItems() const
{
    foreach (QGraphicsItem *item, currentView()->getGraphicsScene()->items())
        if(item !=NULL)
            return true;
//        if (item != gridGroup && item->group() != gridGroup)
//            return true;
    return false;
}

void Optimask::selectionChanged()
{
    QList<QGraphicsItem*> items = currentView()->getGraphicsScene()->selectedItems();
    if (items.count() == 1) {
#ifdef NO_DYNAMIC_CAST
        if (QObject *item = qObjectFrom(items.at(0))) {
#else
        if (QObject *item = dynamic_cast<QObject*>(items.at(0))) {
#endif
            if (item->property("brush").isValid())
                brushWidget->setBrush(
                        item->property("brush").value<QBrush>());
            if (item->property("pen").isValid())
                penWidget->setPen(
                        item->property("pen").value<QPen>());
            if (item->property("angle").isValid()) {
                transformWidget->setAngle(
                        item->property("angle").toDouble());
                transformWidget->setShear(
                        item->property("shearHorizontal").toDouble(),
                        item->property("shearVertical").toDouble());
            }
        }
    }
    updateUi();
}

void Optimask::setDirty(bool on)
{
    setWindowModified(on);
    updateUi();
}

void Optimask::updateUi()
{
//    editUndoAct->setEnabled(false);
//    editRedoAct->setEnabled(false);
    COptimaskView *view = currentView();
    if(view != NULL){
        fileSaveAct->setEnabled(isWindowModified());
        bool hasItems = sceneHasItems();
        fileSaveAsAct->setEnabled(hasItems);
        fileExportAct->setEnabled(hasItems);
        filePrintAct->setEnabled(hasItems);
        int selected = currentView()->getGraphicsScene()->selectedItems().count();
        editSelectedItemAction->setEnabled(selected == 1);
        editCopyAct->setEnabled(selected >= 1);
        editCutAct->setEnabled(selected >= 1);
        QClipboard *clipboard = QApplication::clipboard();
        const QMimeData *mimeData = clipboard->mimeData();
        editPasteAct->setEnabled(mimeData &&
                (mimeData->hasFormat(MimeType) || mimeData->hasHtml() ||
                 mimeData->hasText()));
        alterAlignmentAct->setEnabled(selected >= 2);

        //editClearTransformsAction->setEnabled(selected >= 1);
        transformWidget->setEnabled(selected >= 1);
        bool hasBrushProperty;
        bool hasPenProperty;
        getSelectionProperties(&hasBrushProperty, &hasPenProperty);
//        brushWidget->setEnabled(hasBrushProperty);
//        penWidget->setEnabled(hasPenProperty);
    }
}
/*---------------------------画图形的命令-------------------------------------------*/

void Optimask::addRect(CGeoPt begin,CGeoPt end)
{
    COptimaskView *view = currentView();
    if (view == NULL)
        return;
    const QMetaObject *metaObject = this->metaObject();
    metaObject->invokeMethod(view, "DrawRectWithCommand", Qt::DirectConnection,Q_ARG(CGeoPt, begin),Q_ARG(CGeoPt, end));
}

void Optimask::addLine(CGeoPt begin,CGeoPt end)
{
    COptimaskView *view = currentView();
    if (view == NULL)
        return;
    const QMetaObject *metaObject = this->metaObject();
    metaObject->invokeMethod(view, "DrawLineWithCommand", Qt::DirectConnection,Q_ARG(CGeoPt, begin),Q_ARG(CGeoPt, end));
}

void Optimask::addPolygon(std::vector<CGeoPt> allpoints,bool b_poAndPa)
{
    COptimaskView *view = currentView();
    if (view == NULL)
        return;
    const QMetaObject *metaObject = this->metaObject();
    metaObject->invokeMethod(view, "DrawPolygonWithCommand", Qt::DirectConnection, Q_ARG(std::vector<CGeoPt> ,allpoints),Q_ARG(bool, b_poAndPa));
}

void Optimask::addEllipse(CGeoPt center,double axisX,double  axisY)
{
    COptimaskView *view = currentView();
    if (view == NULL)
        return;
    const QMetaObject *metaObject = this->metaObject();
    metaObject->invokeMethod(view, "DrawEllipseWithCommand", Qt::DirectConnection,Q_ARG(CGeoPt, center),Q_ARG(double, axisX),Q_ARG(double , axisY));
}

void Optimask::addArc(CGeoPt center,double rad,double angleS,double angleE)
{
    COptimaskView *view = currentView();
    if (view == NULL)
        return;
    const QMetaObject *metaObject = this->metaObject();
    metaObject->invokeMethod(view, "DrawArcWithCommand", Qt::DirectConnection,Q_ARG(CGeoPt ,center),Q_ARG(double ,rad),Q_ARG(double ,angleS),Q_ARG(double ,angleE));
}
void Optimask::changeFrameColor(QColor &framecolor )
{
    pencolor = framecolor;
    colorFrameAndFill();
}

void Optimask::changeFillColor(QColor &fillcolor)
{
    brushcolor = fillcolor;
    colorFrameAndFill();
}
void Optimask::colorFrameAndFill()
{
    COptimaskView *view = currentView();
    if (view == NULL)
        return;

    ref_ptr<CGeoBase> currentSelectItem = view->GetSelectItem();
    QPen pen;
    pen.setColor(pencolor);
    view->SetPen(pen);
    view->DrawBasePrimitive(currentSelectItem.get());

//    QGraphicsRectItem *item = new QGraphicsRectItem(-100,-100,100,100);

//    QPen pen;
//    pen.setWidth(2);
//    pen.setColor(pencolor);
//    item->setPen(pen);
//    item->setBrush(brushcolor);
//    item->setFlag(QGraphicsItem::ItemIsMovable);
//    item->setFlag(QGraphicsItem::ItemIsSelectable);
//    view->getGraphicsScene()->clear();
//    view->getGraphicsScene()->addItem(item);
//    item->setPos(0,0);
}

void Optimask::dockChanged()
{
    if(dispLayerPanelAct->isChecked())
    {
        layerPanel->show();
    }
    else
    {
        layerPanel->hide();
    }
    if(dispLayerPaletteAct->isChecked())
    {
        layerPalette->show();
    }
    else
    {
        layerPalette->hide();
    }

    if(dispCellDockAct->isChecked())
    {
        cellDock->show();
    }
    else
    {
        cellDock->hide();
    }
    if(dispWorkDockAct->isChecked())
    {
        workDock->show();
    }
    else
    {
        workDock->hide();
    }
    if(dispNaviDockAct->isChecked())
    {
        naviDock->show();
    }
    else
    {
        naviDock->hide();
    }
    if(dispAltrDockAct->isChecked())
    {
        altrDock->show();
    }
    else
    {
        altrDock->hide();
    }
    if(dispCodeDockAct->isChecked())
    {
        codeDock->show();
    }
    else
    {
        codeDock->hide();
    }
    if(dispInfoDockAct->isChecked())
    {
        infoDock->show();
    }
    else
    {
        infoDock->hide();
    }
    if(dispHintDockAct->isChecked())
    {
        hintDock->show();
    }
    else
    {
        hintDock->hide();
    }

}

void Optimask::dockCloseOrOpen()
{
    if(layerPanel->isHidden())
    {
        dispLayerPanelAct->setChecked(false);
    }
    else
    {
        dispLayerPanelAct->setChecked(true);
    }
    if(layerPalette->isHidden())
    {
        dispLayerPaletteAct->setChecked(false);
    }
    else
    {
        dispLayerPaletteAct->setChecked(true);
    }

    if(cellDock->isHidden())
    {
        dispCellDockAct->setChecked(false);
    }
    else
    {
        dispCellDockAct->setChecked(true);
    }
    if(workDock->isHidden())
    {
        dispWorkDockAct->setChecked(false);
    }
    else
    {
        dispWorkDockAct->setChecked(true);
    }
    if(naviDock->isHidden())
    {
        dispNaviDockAct->setChecked(false);
    }
    else
    {
        dispNaviDockAct->setChecked(true);
    }
    if(altrDock->isHidden())
    {
        dispAltrDockAct->setChecked(false);
    }
    else
    {
        dispAltrDockAct->setChecked(true);
    }
    if(codeDock->isHidden())
    {
        dispCodeDockAct->setChecked(false);
    }
    else
    {
        dispCodeDockAct->setChecked(true);
    }
    if(infoDock->isHidden())
    {
        dispInfoDockAct->setChecked(false);
    }
    else
    {
        dispInfoDockAct->setChecked(true);
    }
    if(hintDock->isHidden())
    {
       dispHintDockAct->setChecked(false);
    }
    else
    {
       dispHintDockAct->setChecked(true);
    }
}

void Optimask::rulerStatusChanged()
{

}
/*--------------------------GDS文件的打开与显示多窗口-----------------------------------*/
void Optimask::addGdsDocument(CGdsDocument *gdsdoc)
{
    if (m_gdsDocument.indexOf(gdsdoc) != -1)//如果在原来的多窗口中存在，则不需要重新新建
        return;
    m_gdsDocument.append(gdsdoc);           // m_gdsDocument中保存数据
    workTabs->addTab(gdsdoc->GetViewWid(),fixedWindowTitle(gdsdoc));
    cellTreeTabs->addTab(gdsdoc->GetCellTreeWid(),fixedWindowTitle(gdsdoc));
    layerPanelTabs->addTab(gdsdoc->GetTableView(),fixedWindowTitle(gdsdoc));
    //layerNaviTabs->addTab(gdsdoc->GetNavigator(),fixedWindowTitle(gdsdoc));
    setCurrentGdsDocument(gdsdoc);//设置当前窗口

}
void Optimask::removeGdsDocument(CGdsDocument *&gdsdoc)
{
    int gdsindex = m_gdsDocument.indexOf(gdsdoc);
    if (gdsindex == -1)
        return;
    m_gdsDocument.removeAt(gdsindex);
    int viewindex=workTabs->indexOf(gdsdoc->GetViewWid());
    workTabs->removeTab(viewindex);
    int celltreeindex=cellTreeTabs->indexOf(gdsdoc->GetCellTreeWid());
    cellTreeTabs->removeTab(celltreeindex);
    int tableviewindex=layerPanelTabs->indexOf(gdsdoc->GetTableView());
    layerPanelTabs->removeTab(tableviewindex);
    int navigatorindex=layerNaviTabs->indexOf(gdsdoc->GetNavigator());
    layerNaviTabs->removeTab(navigatorindex);
    gdsdoc=NULL;
}

void Optimask::setCurrentGdsDocument(CGdsDocument *gdsdoc)
{
    workTabs->setCurrentWidget(gdsdoc->GetViewWid());
    cellTreeTabs->setCurrentWidget(gdsdoc->GetCellTreeWid());
    layerPanelTabs->setCurrentWidget(gdsdoc->GetTableView());
    layerNaviTabs->setCurrentWidget(gdsdoc->GetNavigator());
    connect(currentNavi(),SIGNAL(viewBoxchanged(float)),currentView(),SLOT(zoom(float)));
    connect(currentNavi(),SIGNAL(tranSlate(QPointF)),currentView(),SLOT(tranSlate(QPointF)));
    //connect(currentView(),SIGNAL(signalTranslate(QPointF)),currentNavi(),SLOT(slotTranslate(QPointF)));
}

CGdsDocument* Optimask::currentGdsDocument() const
{
    COptimaskView *view= qobject_cast<COptimaskView*>(workTabs->currentWidget());
    return view->GetGdsDocument();//返回currentGdsDocument
}
COptimaskView* Optimask::currentView() const
{
    COptimaskView *view= qobject_cast<COptimaskView*>(workTabs->currentWidget());
    return view;
}
void Optimask::newGdsDocument()
{
    CGdsDocument *gdsdoc=new CGdsDocument();
    m_vecDocument.push_back(gdsdoc);
    COptimaskView *view=new COptimaskView();
    view->SetGdsDocument(gdsdoc);
    view->SetSceneData(gdsdoc->GetSceneData().get());
    Navigator *navi=new Navigator();
    navi->SetGdsDocument(gdsdoc);
    gdsdoc->SetViewWid(view);
    gdsdoc->SetNavigator(navi);
    addGdsDocument(gdsdoc);
}
void Optimask::closeGdsDocument()
{
    CGdsDocument *gdsdoc = currentGdsDocument();
    if (gdsdoc == 0)
        return;
    removeGdsDocument(gdsdoc);
}

QString Optimask::fixedWindowTitle(const CGdsDocument *gdsdoc) const
{
    QString title = gdsdoc->DocumentName();              // 获取gdsdocument的文件名
    if (title.isEmpty())                                 // 文件名为空
        title = tr("Unnamed");                           // 设置标签为Unnamed
    else
        title = QFileInfo(title).fileName();             // QFileInfo能通过相对/绝对路径指向一个文件
    QString result;
    for (int i = 0; ; ++i) {
        result = title;                                  // 将文件名赋给result
        if (i > 0)
            result += QString::number(i);
        bool unique = true;
        for (int j = 0; j < workTabs->count(); ++j) {
            const QWidget *widget = workTabs->widget(j); // 获取多窗口中的每一页控件
            if (widget == gdsdoc->GetViewWid())          // 如果多窗口中每一页控件与该文件相等，则继续
                continue;
            if (result == workTabs->tabText(j)) {        // 多页窗口的每一页的标签
                unique = false;                          // 如果相等，说明不唯一
                break;
            }
        }
        if (unique)                                      // 如果唯一，则结束循环
            break;
    }
    return result;
}

void Optimask::mousePressEvent(QMouseEvent *e)
{
    QString str="("+QString::number(e->x())+","+QString::number(e->y())+")";
    if(e->button()==Qt::LeftButton)
    {
        statusBar()->showMessage(tr("左键： ")+str);
    }
    else if(e->button()==Qt::RightButton)
    {
        statusBar()->showMessage(tr("右键： ")+str);
    }
    else if(e->button()==Qt::MidButton)
    {
        statusBar()->showMessage(tr("中键： ")+str);
    }

    if(fileToolBar->isFloating())
    {

        fileFltWndw=new QMainWindow();
        fileFltWndw->resize(320,20);
        fileFltWndw->setWindowTitle("File");
        //fileFltWndw->setObjectName("fileToolBar");
        fileFltWndw->addToolBar(fileToolBar);
        fileToolBar->setMovable(false);
        fileFltWndw->show();
    }

    if(editToolBar->isFloating())
    {
        editFltWndw=new QMainWindow();
        editFltWndw->resize(290,20);
        editFltWndw->setWindowTitle("Edit");
        editFltWndw->addToolBar(editToolBar);
        editToolBar->setMovable(false);
        editFltWndw->show();
    }

    if(viewToolBar->isFloating())
    {
        viewFltWndw=new QMainWindow();
        viewFltWndw->resize(350,20);
        viewFltWndw->setWindowTitle("View");
        viewFltWndw->addToolBar(viewToolBar);
        viewToolBar->setMovable(false);
        viewFltWndw->show();
    }

    if(drawToolBar->isFloating())
    {
        drawFltWndw=new QMainWindow();
        drawFltWndw->resize(380,20);
        drawFltWndw->setWindowTitle("Draw");
        drawFltWndw->addToolBar(drawToolBar);
        drawToolBar->setMovable(false);
        drawFltWndw->show();
    }

    if(altrToolBar->isFloating())
    {
        altrFltWndw=new QMainWindow();
        altrFltWndw->resize(160,20);
        altrFltWndw->setWindowTitle("Alter");
        altrFltWndw->addToolBar(altrToolBar);
        altrToolBar->setMovable(false);
        altrFltWndw->show();
    }

    if(cellToolBar->isFloating())
    {
        cellFltWndw=new QMainWindow();
        cellFltWndw->resize(540,20);
        cellFltWndw->setWindowTitle("Cell");
        cellFltWndw->addToolBar(cellToolBar);
        cellToolBar->setMovable(false);
        cellFltWndw->show();
    }
    if(layerToolBar->isFloating())
    {
        layerFltWndw=new QMainWindow();
        layerFltWndw->resize(300,20);
        layerFltWndw->setWindowTitle("Layer");
        layerFltWndw->addToolBar(layerToolBar);
        layerToolBar->setMovable(false);
        layerFltWndw->show();
    }

    if(codeToolBar->isFloating())
    {
        codeFltWndw=new QMainWindow();
        codeFltWndw->resize(200,20);
        codeFltWndw->setWindowTitle("Script");
        codeFltWndw->addToolBar(codeToolBar);
        codeToolBar->setMovable(false);
        codeFltWndw->show();
    }

    if(cnfgToolBar->isFloating())
    {

        cnfgFltWndw=new QMainWindow();
        cnfgFltWndw->resize(100,20);
        cnfgFltWndw->setWindowTitle("Config");
        cnfgFltWndw->addToolBar(cnfgToolBar);
        cnfgToolBar->setMovable(false);
        cnfgFltWndw->show();
    }

    if(toolToolBar->isFloating())
    {
        toolFltWndw=new QMainWindow();
        toolFltWndw->resize(180,20);
        toolFltWndw->setWindowTitle("Tool");
        toolFltWndw->addToolBar(toolToolBar);
        toolToolBar->setMovable(false);
        toolFltWndw->show();
    }

    if(wndwToolBar->isFloating())
    {
        wndwFltWndw=new QMainWindow();
        wndwFltWndw->resize(180,20);
        wndwFltWndw->setWindowTitle("Window");
        wndwFltWndw->addToolBar(wndwToolBar);
        wndwToolBar->setMovable(false);
        wndwFltWndw->show();
    }

    if(helpToolBar->isFloating())
    {
        helpFltWndw=new QMainWindow();
        helpFltWndw->resize(360,20);
        helpFltWndw->setWindowTitle("Help");
        helpFltWndw->addToolBar(helpToolBar);
        helpToolBar->setMovable(false);
        helpFltWndw->show();
    }
}

void Optimask::mouseMoveEvent(QMouseEvent *e)
{//qDebug("11111111");

   // QMainWindow::mouseMoveEvent(e);
    this->setMouseTracking(true);
    QPoint viewpoint1 = workTabs->mapFromGlobal(QCursor::pos());
    viewpoint->setText("("+QString::number(viewpoint1.x())+", "+QString::number(viewpoint1.y())+")");
    diffpoint->setText("("+QString::number(e->globalX())+", "+QString::number(e->globalY())+")");

}

void Optimask::mouseReleaseEvent(QMouseEvent *e)
{
    QString str="("+QString::number(e->x())+","+QString::number(e->y())+")";
    statusBar()->showMessage(tr("释放在： ")+str,3000);
}

//-----------此函数没有写好，需要更改-----------------
void Optimask::removeWorkSubTab(int index)
{
    COptimaskView *view= qobject_cast<COptimaskView*>(workTabs->widget(index));
    CGdsDocument *doc = view->GetGdsDocument();
    if(doc==NULL)
    {
        workTabs->removeTab(index);
        delete view;
        delete doc;
    }
    if(view->m_parent==false)
        workTabs->removeTab(index);
    else
        removeGdsDocument(doc);
}

void Optimask::currentWorkViewChanged(int /*index*/)
{
    if(currentView()->m_parent==FALSE)
        return;
    CGdsDocument *gdsdoc=currentView()->GetGdsDocument();
    setCurrentGdsDocument(gdsdoc);
}

void Optimask::currentCellTreeChanged(int /*index*/)
{
    CCellTreeWidget *celltree;
    celltree= qobject_cast<CCellTreeWidget*>(cellTreeTabs->currentWidget());
    CGdsDocument *gdsdoc=celltree->GetGdsDocument();
    setCurrentGdsDocument(gdsdoc);
}
void Optimask::currentLayerPanelChanged(int /*index*/)
{
    LayTableView *layertable=qobject_cast<LayTableView*>(layerPanelTabs->currentWidget());
    CGdsDocument *gdsdoc=layertable->GetGdsDocument();
    setCurrentGdsDocument(gdsdoc);
}

void Optimask::currentNavigatorChanged(int /*index*/)
{
    Navigator* navi=qobject_cast<Navigator*>(layerNaviTabs->currentWidget());
    CGdsDocument *gdsdoc=navi->GetGdsDocument();
    setCurrentGdsDocument(gdsdoc);
}

void Optimask::showSelectedItem(QTreeWidgetItem* item,int column)
{
    QString qname=item->text(column); //获取点击构元的名字，
    CGdsDocument *gds=currentGdsDocument();
    COptimaskView *view=new COptimaskView();
    view->m_parent=false;
    view->SetGdsDocument(gds);
    gds->SaveCellViewWid(view);
    if(qname.contains(' '))
        qname = qname.split(' ').at(0);
    view->SetSceneFrameByName(qname.toStdString());
    view->DrawStructure(qname.toStdString());
    workTabs->addTab(view,qname);
    workTabs->setCurrentWidget(view);
}

void Optimask::resizeEvent(QResizeEvent *)
{
//    QRectF bounds = scene->itemsBoundingRect();
//    bounds.setWidth(bounds.width()*0.9);         // to tighten-up margins
//    bounds.setHeight(bounds.height()*0.9);       // same as above
//    ui->graphicsView->fitInView(bounds, Qt::KeepAspectRatio);
//    ui->graphicsView->centerOn(0, 0);

//    QRectF bounds = scene->itemsBoundingRect();

//   // bounds.setWidth(bounds.width()*0.9);         // to tighten-up margins

//   // bounds.setHeight(bounds.height()*0.9);       // same as above
//    view->fitInView(bounds, Qt::KeepAspectRatio);
    //    view->centerOn(100, 0);
}

