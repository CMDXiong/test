/* ========================================================================== */
/* Optimask.h -- Optimask主界面的头文件(动作,菜单,界面和窗口等)
 *
 * REFERENCE:
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.01$ (20171001) */
/* HISTORY: 20161027(v1.00) Initial Version; 20171001 Optimization;
 * ========================================================================== */

#ifndef OPTIMASK_H
#define OPTIMASK_H

#include <QMainWindow>
#include <QFile>
#include <QDockWidget>
#include <QString>
#include <QTreeWidget>

#include "./File/gds2reader.h"
#include "./File/gds2writer.h"
#include "./Draw/graphicscene.h"
#include "./Draw/graphicsview.h"
#include "./Layer/laytreeview.h"
#include "./Layer/layertoolbox.h"
#include "./Layer/laynavigator.h"
#include "./cell/ccelldockwidget.h"

class QAction;
class QMenu;
class QToolBar;
class QStatusBar;
class QDockWidget;

class COptimaskView;
class CCellTreeWidget;
class CGdsDocument;

class AlterDockWidget;
class TransformWidget;
class BrushWidget;
class PenWidget;
class Navigator;

class Matrix;
class LibraryWidget;
class MultiLine;

/* Optimask 主窗口界面和操作 */
class Optimask : public QMainWindow
{
    Q_OBJECT

public:
    Optimask(QWidget *parent = 0);
    ~Optimask();
private:

    void init();                       //初始化场景
    void createActions();              //创建动作       
    void createMenusAndToolBars();     //创建菜单和工具条
    void createStatusBar();            //创建应用程序状态栏
    void populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar, QList<QAction*> actions);

    //所有主要面板Dock(Layer,Cell,Work,Navi,Altr,Code,Info,Hint)
    void createMainDocks();            //创建主界面上所有主要面板
    void createLayerPanel();           //创建构层面板(Layer Panel, Layer View)
    void createLayerPalette();         //创建构层色板(Layer Palette, Color&Pattern)
    void createCellDock();             //创建构元结构面板(Cell Structure)
    void createWorkDock();             //需要允许多重工作区域(Workspace, Work Panel)
    void createNaviDock();             //创建视图导航区域(Navigator, Aerial View)
    void createAltrDock();             //创建变换面板(Alter, Vary, Change), 包括(Boolean, Regenerate, Transform, Calculate)
    void createCodeDock();             //创建编程区域(Command, Script, Macro, Code, Programming)
    void createInfoDock();             //创建信息输出栏
    void createHintDock();             //创建信息提示报警栏

    //数据操作
    void createConnections();          //创建动作和函数的联接
    void GetCellStructure();           //获取构元结构
    void colorFrameAndFill();          //边框和填充上色

private slots: 
    //【文件(File)】相关槽函数
    void fileNew();                    //新建文件
    void fileOpen();                   //打开文件
    void fileClose();                  //关闭文件
    void fileSave();                   //保存文件
    void fileSaveAs();                 //另存为文件
    void filePrint();                  //打印文件
    void filePrintPreview();           //打印预览
    void fileInfo();                   //文件信息
    void fileOptn();                   //文件选项
    void fileExit();                   //文件退出

    //【编辑(Edit)】相关槽函数
    void UndoRedoStateChange(bool undo, bool redo); //撤销重做状态
    void editUndo();                   //编辑撤销
    void editRedo();                   //编辑重做
    void editSelect();                 //编辑选择
    void editMoveView();               //编辑移动视图
    void editSelectedItem();           //编辑选中图元
    void editSelectCell();             //编辑选中构元
    void editCopy();                   //编辑复制
    void editCut();                    //编辑剪切
    void editPaste();                  //编辑粘贴
    void editPasteTo();                //编辑粘贴至指定（构元、构层、位置）

    //【视图(View)】和【显示(Display)】相关槽函数
    //隐藏和显示面板(Panel, Dock, Pad)
    void dispPanel();                  //显示(Display)面板(Panels, Docks)
    //隐藏和显示工具栏(ToolBar)
    void dispToolBar();                //显示(Display)工具栏(ToolBar)

    //【绘制(Draw)】相关槽函数
    void drawClear();                  //清除场景中所有绘制的图元
    void drawShape();                  //绘制形状
    void drawLine();                   //绘制线段
    void drawRect();                   //绘制矩形
    void drawPoly();                   //绘制多边形
    void drawCircle();                 //绘制圆形
    void drawEllipse();                //绘制椭圆形
    void drawCurve();                  //绘制曲线
    void drawMatrix();                 //通过矩阵数组绘制
    void drawText();                   //加入文字
    void drawLogo();                   //加入图标Logo
    void drawMultiLine();              //绘制多条线段
    void drawMultiCurve();             //绘制多条曲线
    void drawLibrary();                //图形库(Library)
    void drawScan();                   //扫描读入图形文件(高级功能)

    //【变化(Alter)】相关槽函数
    void alterAlign();                 //对齐(Align)至格点(Grid)
    void alterConvert();               //参见LEdit(Draw->Convert)
    void alterRegen();                 //重塑,再生(Regenerate)
    void alterGroup();                 //组合(Group)
    void alterUngroup();               //分组(UnGroup)
    void alterClone();                 //克隆(Clone), 整体复制(Duplicate), 复制(Copy)
    void alterSplit();                 //分裂(Split), 剪切(Cut)
    void alterCutX();                  //沿X线切开分裂(CutX,SplitX)
    void alterCutY();                  //沿Y线切开分裂(CutY,SplitY)
    void alterCutLine();               //沿任意线y=kx+b切开分裂(CutLine,SplitLine)
    void alterCrop();                  //裁剪(裁切)(Crop)
    void alterMerge();                 //整合,合并(Merge)
    void alterFract();                 //细片,碎片(Fracture)所选图形. (请区分Cell菜单下的cellFractureAct)
    void alterExLay();                 //导出选定层上所有图元(Export Layer)
    void alterBoolean();               //布尔运算(boolean),与非运算,逻辑运算(logic)
    void alterBoolAnd();               //交集(Intersection),    逻辑与(And):  保留公共部分
    void alterBoolOr();                //联合(Union), 相加(Add), 逻辑或(Or):   保留各自部分和公共部分
    void alterBoolXor();               //互斥(Exclusive),       逻辑异或(Xor): 保留各自部分(删除公共部分)
    void alterBoolANotB();             //相减(SUBtract A-B),    逻辑非(NOT):  保留A不属于B的部分(删除B)
    void alterBoolBNotA();             //相减(SUBtract B-A),    逻辑非(NOT):  保留B不属于A的部分(删除A)
    void alterTransform();             //转变(Transform)
    void alterMove();                  //移位(Move, Translation, Slide), (X,Y) + (dx, dy)
    void alterTurn();                  //旋转(Turn, Rotate), (X+j*Y)*exp(j*ANG)
    void alterFlip();                  //镜像(Flip, Mirror, Reflect)
    void alterScale();                 //缩放(Scaling, Grow, Shrink) (ReSize), (X,Y)*ScaleFactor
    void alterStretch();               //展绷(Stretch) (X*a, Y*b)
    void alterCompute();               //计算(Compute) 根据计算y=f(x)来实现变形

    //【构元(Cell)】相关槽函数
    void cellNew();                    //新建构元
    void cellOpen();                   //打开构元
    void cellCopy();                   //复制构元
    void cellPaste();                  //粘贴构元
    void cellArrange();                //安排构元(位置)
    void cellMoveOrg();                //改变构元基点
    void cellInstance();               //引用构元
    void cellChangeInstance();         //改变引用
    void cellMultiAlter();             //多元操作
    void cellAssemble();               //组装构元
    void cellHierarchy();              //构元从属结构关系
    void cellFlatten();                //构元平整显示
    void cellFracture();               //细片,碎片,分割(Fracture) 所选构元. (请区分Alter菜单下的alterFractAct)
    void cellInfo();                   //构元信息
    void cellStat();                   //构元统计
    void cellFrame();                  //构元边界
    void cellShow();                   //构元显示
    void cellHide();                   //构元隐藏
    void cellProtect();                //构元保护
    void cellUnprotect();              //构元解保
    void cellLock();                   //构元锁定
    void cellUnlock();                 //构元解锁

    //【构层(Layer)】相关槽函数
    void layerOptn();                  //构层选项
    void layerLoadFile();              //导入构层配置文件
    void layerSaveFile();              //保存构层配置文件
    //void layerPanel();               //构层面板
    //void layerPalette();             //构层色板
    void layerSetup();                 //构层设置
    void layerDensity();               //构层密度
    void layerDummyFill();             //冗余填充
    void layerStat();                  //构层统计
    void layerMapping();               //构层对应
    void layerCtrl();                  //构层控制
    void layerSelect();                //构层选择
    void layerDeselect();              //构层解选
    void layerShow();                  //构层显示
    void layerHide();                  //构层隐藏
    void layerProtect();               //构层保护
    void layerUnprotect();             //构层解保
    void layerLock();                  //构层锁定
    void layerUnlock();                //构层解锁
    void layerFill();                  //构层实心显示
    void layerUnfill();                //构层空心显示

    //【编程(Script), 代码(Code)】相关槽函数
    void codeOptn();                   //代码选项
    void codeNew();                    //新建代码
    void codeOpen();                   //打开代码
    void codeSave();                   //保存代码
    void codeRun();                    //运行代码
    void codeDebug();                  //调试代码

    //【配置(Config)】, 设置(Setup)】相关槽函数
    void cnfgOptn();                   //配置选项
    void cnfgFont();                   //配置字体
    void cnfgDRC();                    //配置设计规则检查(DRC: Design Rule Check)

    //【工具(Tool)】相关槽函数
    void toolUtility();                //小工具集
    void toolLayoutStat();             //版图统计
    void toolLayoutCompare();          //版图比较
    void toolLayoutDiff();             //版图差分
    void toolWafer();                  //晶圆工具

    //【窗口(Window)】相关槽函数
    void wndwSplit();                  //窗口分裂
    void wndwTile();                   //窗口排列
    void wndwCascade();                //窗口级联
    void wndwArrange();                //窗口整理

    //【帮助(Help)】相关槽函数
    void helpIndex();                  //帮助索引
    void helpQuickRef();               //快速参考
    void helpManual();                 //帮助手册
    void helpExample();                //帮助实例
    void helpAppNote();                //应用笔记
    void helpUpdate();                 //软件更新
    void helpHistory();                //版本历史
    void helpLicence();                //软件许可
    void helpSupport();                //软件支持
    void helpReportBug();              //汇报缺陷
    void helpAbout();                  //关于软件

private:
    QColor pencolor, brushcolor;

    /* 【文件(File)】主菜单动作集 (缩进为子菜单动作) */
    QAction *fileNewAct;               //新建(New)
    QAction *fileOpenAct;              //打开(Open)
    QAction *fileCloseAct;             //关闭(Close)
    QAction *fileSaveAct;              //保存(Save)
    QAction *fileSaveAsAct;            //另存为(Save As)
    QAction *fileInfoAct;              //文件信息(File Info)
    QAction *fileOptnAct;              //文件格式选项(File Option)

    QAction *fileImportAct;            //导入(Import)格式
    QAction *fileImportLayerAct;       //导入(Import)Layer Setup (Technology File)层设置
    QAction *fileImportGDSIIAct;       //导入(Import)GDSII格式
    QAction *fileImportOASISAct;       //导入(Import)OASIS格式
    QAction *fileImportCIFAct;         //导入(Import)CIF格式
    QAction *fileImportDXFAct;         //导入(Import)DXF格式
    QAction *fileImportGerberAct;      //导入(Import)Gerber格式
    QAction *fileImportPictureAct;     //导入(Import)图形格式(BMP,JPG,PNG,TIF,etc.)

    QAction *fileExportAct;            //导出(Export)格式
    QAction *fileExportLayerAct;       //导出(Export)Layer Setup (Technology File)层设置
    QAction *fileExportGDSIIAct;       //导出(Export)GDSII格式
    QAction *fileExportOASISAct;       //导出(Export)OASIS格式
    QAction *fileExportCIFAct;         //导出(Export)CIF格式
    QAction *fileExportDXFAct;         //导出(Export)DXF格式
    QAction *fileExportGerberAct;      //导出(Export)Gerber格式
    QAction *fileExportPictureAct;     //导出(Export)图形格式(BMP,JPG,PNG,TIF,etc.)
    QAction *filePrintAct;             //打印(Print)
    //打印预览和打印设置(Print Preview & Print Setup)将在打印(Print)命令执行时出现
    QAction *filePrintPreviewAct;      //打印预览(Print Preview)
    QAction *filePrintSetupAct;        //打印设置(Print Setup)
    QAction *fileExitAct;              //退出(Exit)软件

    /* 【编辑(Edit)】主菜单动作集 (缩进为子菜单动作) */
    QAction *editSelectAction;         //编辑选择
    QAction *editMoveViewAct;          //编辑移动视图
    QAction *editSelectedItemAction;   //编辑选中的图元
    QAction *editUndoAct;              //撤销(Undo)上一个操作
    QAction *editRedoAct;              //恢复(Redo, Recover)上一个操作
    QAction *editAgainAct;             //重复(Repeat, Again)最近的操作
    QAction *editCutAct;               //剪切(Cut)选中目标
    QAction *editCopyAct;              //复制(Copy)选中目标
    QAction *editPasteAct;             //粘贴(Paste)选中目标
    QAction *editPastetoAct;           //粘贴至(PasteTo)特定目标(比如另外一个Origin/Basepoint,Layer,Cell)
    QAction *editDuplicateAct;         //重复(Duplicate, Clone)选中目标
    QAction *editDeleteAct;            //删除(Delete)选中目标
    QAction *editSelectAct;            //选择(Select)指定目标
    QAction *editSelectLayersAct;      //选择指定构层或多层(Select Layers)
    QAction *editSelectCellsAct;       //选择指定单元或多单元(Select Cells)
    QAction *editSelectInAct;          //选择选择框里面(Select In/Box/Range)
    QAction *editSelectSideAct;        //选择该边接触的所有物件(Select Side)
    QAction *editSelectLastAct;        //选择刚才的物件(Select Last/New/Recent)
    QAction *editSelectAllAct;         //选择所有(Select All)
    QAction *editDeselectAct;          //解选(Deselect)选中目标(to be unselected) (ICED: unselect; L-Edit: deselect)
    QAction *editDeselectLayersAct;    //解选指定构层或多层(Deselect Layers)
    QAction *editDeselectCellsAct;     //解选指定单元或多单元(Deselect Cells)
    QAction *editDeselectInAct;        //解选选择框里面(Deselect In/Box/Range)
    QAction *editDeselectSideAct;      //解选该边接触的所有物件(Deselect Side)
    QAction *editDeselectLastAct;      //解选刚才的物件(Deselect Last/New/Recent)
    QAction *editDeselectAllAct;       //解选所有(Deselect All)

    /* 【视图(View)】 和 【显示(Display)】 相关 */
    /* 【视图(View)】主菜单动作集 (缩进为子菜单动作) */
    QAction *viewOptnAct;              //屏幕显示(View, Display, Screen)选项(Option)
    //(Parity,Tone,Orientation,Rotation,Magnification) are basics in Mask View
    QAction *viewParityAct;            //奇偶性(Parity) (是否镜像) (Right/Wrong-Reading以及Chrome-Up/Down)
    QAction *viewToneAct;              //极性(Polarity) (数字色调 Digitized Tone) (Clear透明， Dark黑暗) (Display Polygon as Clear or Dark)
    QAction *viewOrientationAct;       //取向(Orientation) (参见Windows Display Settings: Landscape, Portrait, Landscape Flipped, Portrait Flipped)
    QAction *viewRotationAct;          //旋转(Rotation) (Rotate View/Display/Screen)
    QAction *viewMagnificationAct;     //放大(Magnification) (比如Reticle 1X, 4X, 5X)
    QAction *viewAspectRatio;          //宽高比(Aspect Ratio) = X:Y
    QAction *viewGridAct;              //网格(Grid)显示
    QAction *viewFillAct;              //填充(Fill)显示
    QAction *viewAnntAct;              //批注(Annotation)显示
    QAction *viewNoteAct;              //注释(Note)显示
    QAction *viewPortAct;              //端口(Port)显示
    QAction *viewMarkAct;              //标记(Mark)显示
    QAction *viewRulerAct;             //标尺(Ruler)显示
    QAction *viewFrameAct;             //边框(Frame, Outline)显示
    QAction *viewErrorAct;             //错误(Error)显示
    QAction *viewStructAct;            //结构(Structure)显示
    QAction *viewHierarchyAct;         //从属(hierarchy)结构显示
    QAction *viewFlatAct;              //平整(flat)结构显示 (hierarchy level=0)
    QAction *viewLevelAct;             //层级(hierarchy level)结构显示(设定起始和终止层级)
    QAction *viewStepIntoAct;          //进入(Step Into)指定的从属深度(hierarchy depth)
    QAction *viewFilterAct;            //过滤(Filter)显示(只显示或隐藏符合条件的结构,可以传输过滤条件去修改结构树内各单元属性.)
    QAction *viewHomeAct;              //缺省中心(Home)显示
    QAction *viewGotoAct;              //转至(GoTo)指定坐标为中心显示
    QAction *viewZoomAct;              //缩放(Zoom)(变焦视图)
    //(When using Mouse center Scroll Wheel, the view change is zooming.)
    QAction *viewZoomInAct;            //拉近(In)焦距(放大)
    QAction *viewZoomOutAct;           //推远(Out)焦距(缩小)
    QAction *viewZoomBoxAct;           //缩放矩形(Box)框内
    QAction *viewZoomLastAct;          //恢复上次(Last)焦距
    QAction *viewZoomSelAct;           //缩放选择目标(Selection)
    QAction *viewZoomAllAct;           //显示全景(All, Fit, Home)
    QAction *viewPanAct;               //平移(Pan)镜头
    //(When using Keyboard <left>, <right>, <up>, <down>, the view change is panning.)
    QAction *viewPanLeftAct;           //往左(Left)平移
    QAction *viewPanRightAct;          //往右(Right)平移
    QAction *viewPanUpAct;             //往上(Up)平移
    QAction *viewPanDownAct;           //往下(Down)平移
    QAction *viewPanLastAct;           //恢复至上次(Last)平移位置
    QAction *viewPanSelAct;            //平移至选择目标(Selection)
    QAction *viewPanDistAct;           //平移指定距离(Distance) (dX,dY)
    QAction *viewTiltAct;              //倾斜(Tilt)镜头(2D实际不需要; 保留给3D扩展)
    QAction *viewRefreshAct;           //刷新(Refresh)视图

    //【显示(Display)】工具箱(Toolbox)(显示或隐藏)
    QAction *dispToolboxAct;           //工具箱(Toolbox)显示
    //【显示(Display)】菜单工具条(Toolbar)(显示或隐藏)
    QAction *dispToolbarAct;           //菜单工具条(Toolbar)显示
    QAction *dispFileToolBarAct;       //显示File工具条
    QAction *dispEditToolBarAct;       //显示Edit工具条
    QAction *dispViewToolBarAct;       //显示View工具条
    QAction *dispDrawToolBarAct;       //显示Draw工具条
    QAction *dispAltrToolBarAct;       //显示Alter工具条
    QAction *dispCellToolBarAct;       //显示Cell工具条
    QAction *dispLayerToolBarAct;      //显示Layer工具条
    QAction *dispCodeToolBarAct;       //显示Script/Code/Command工具条
    QAction *dispCnfgToolBarAct;       //显示Config工具条
    QAction *dispToolToolBarAct;       //显示Tool工具条
    QAction *dispWndwToolBarAct;       //显示Window工具条
    QAction *dispHelpToolBarAct;       //显示Help工具条
    //【显示(Display)】面板(Panel,Pad,Dock)(显示或隐藏)
    QAction *dispPanelAct;             //面板(Panel)(Pad)显示选择
    //构层面板之命名和配置 (L-Edit: Layer Palette + Setup Layers; KLayout: Layers + Layer Toolbox)
    QAction *dispLayerPanelAct;        //构层面板(Layer Panel)显示
    QAction *dispLayerPaletteAct;      //构层色板(Layer Palette)显示
     //构元面板之命名和配置 (L-Edit: Design Navigator; KLayout: Cells)
    QAction *dispCellDockAct;          //构元面板(CellDock)显示; 结构树(Structure Tree), 构元组织(Cell Hierarchy);
    //工作区域面板(Main Workspace; Work Panel)可以有多个，可浮动层叠，也可全部关闭；通过双击某构元自动打开。
    QAction *dispWorkDockAct;          //工作区域面板 (Main Workspace; Work Panel)显示
    //导航面板(Navigator)显示当前构元的全景视图；工作区域面板窗口显示的局部大小显示在导航面板相对位置上;
    QAction *dispNaviDockAct;          //导航面板(Navigator, Aerial View)显示
    QAction *dispAltrDockAct;          //变化面板(AltrDock)显示
    QAction *dispCodeDockAct;          //计算和编程面板(Computation, Programming, Script, Macro, Code)显示
    QAction *dispInfoDockAct;          //信息输出栏(Information, Status, Result, Output)显示
    QAction *dispHintDockAct;          //提示栏(报警栏)(Hint, Error, Warning)显示

    QAction *dispRulerAct;

    /* 【绘制(Draw)】主菜单动作集 (Draw & Add) (缩进为子菜单动作) */
    QAction *drawClearAct;             //清空(Clear)
    QAction *drawLibraryAct;           //库元(Library)
    QAction *drawLineAct;              //绘制线段(Draw Line, Path，Wire, PLine)
    QAction *drawRectAct;              //绘制矩形(Draw Box, Rect)
    QAction *drawPolyAct;              //绘制多边形(Draw Polygon, Boundary)
    QAction *drawCircleAct;            //绘制圆形(Draw Circle)
    QAction *drawPathAct;              //绘制Path
    QAction *drawEllipseAct;           //绘制椭圆(Draw Ellipse)
    QAction *drawCurveAct;             //绘制曲线(Draw Curve)
    QAction *drawMatrixAct;            //通过矩阵数组绘制(Draw from Matrix Array)
    QAction *drawTextAct;              //添加文本(Add Text)
    //QAction *drawLogoAct;              //添加图标(Add Logo)
    QAction *drawMultiLineAct;         //绘制多条线段(Draw Multiple Lines)
    QAction *drawMultiCurveAct;        //绘制多条曲线(Draw Multiple Curves)
    QAction *drawScanAct;              //扫描读入图形文件(Scan/Import Pictures)

    /* 【变化(Alter)】主菜单动作集 (缩进为子菜单动作) */
    QAction *alterAlignmentAct;        //对齐
    QAction *alterAlignGridAct;        //对齐(Align)至格点(Grid) (Snap to Grid)
    QAction *alterAlignLeftAct;        //左对齐
    QAction *alterAlignRightAct;       //右对齐
    QAction *alterAlignTopAct;         //顶端对齐
    QAction *alterAlignBottomAct;      //底端对齐
    QAction *alterConvertAct;          //参见LEdit(Draw->Convert)
    QAction *alterRegenAct;            //重塑,再生(Regenerate)
    QAction *alterGroupAct;            //组合(Group)
    QAction *alterUngroupAct;          //分组(UnGroup)
    QAction *alterCloneAct;            //克隆(Clone), 整体复制(Duplicate), 复制(Copy)
    QAction *alterSplitAct;            //分裂(Split), 剪切(Cut)
    QAction *alterCutXAct;             //沿X线切开分裂(CutX,SplitX)
    QAction *alterCutYAct;             //沿Y线切开分裂(CutY,SplitY)
    QAction *alterCutLineAct;          //沿任意线y=kx+b切开分裂(CutLine,SplitLine)
    //CROP keep all shapes within crop box, and delete all shapes outside crop box.
    //CROP basically has 2 CUTX and 2 CUTY, according to the box's 4 edges. AutoCAD has Crop operation.
    QAction *alterCropAct;             //裁剪(裁切)(Crop)
    QAction *alterMergeAct;            //整合,合并(Merge)
    //Fracture: 将选定图形全部根据规则切分为细片(碎片)多边形.
    QAction *alterFractAct;            //细片,碎片,分割(Fracture) 所选构元. (请区分Cell菜单下的cellFractureAct)
    //Advanced Function: ExLAY (EXport_LAYer_to_New_Cell). Not needed in minimum version.
    //Export selected (or specified) layer(s)' all objects (in a source cell) into a new cell.
    //The source cell will add the instance of the new cell, and delete the objects before this operation.
    //The objects on the original cell's layers could belong to different subcells.
    QAction *alterExLayAct;            //导出选定构层上所有图元(Export Layer)
    QAction *alterBooleanAct;          //布尔运算(boolean),与非运算,逻辑运算(logic)
    QAction *alterBoolAndAct;          //交集(Intersection),    逻辑与(And):  保留公共部分
    QAction *alterBoolOrAct;           //联合(Union), 相加(Add), 逻辑或(Or):   保留各自部分和公共部分
    QAction *alterBoolXorAct;          //互斥(Exclusive),       逻辑异或(Xor): 保留各自部分(删除公共部分)
    QAction *alterBoolAnotBAct;        //相减(SUBtract A-B),    逻辑非(NOT):  保留A不属于B的部分(删除B)
    QAction *alterBoolBnotAAct;        //相减(SUBtract B-A),    逻辑非(NOT):  保留B不属于A的部分(删除A)
    QAction *alterTransformAct;        //转变(Transform)
    QAction *alterMoveAct;             //移位(Move, Translation, Slide), (X,Y) + (dx, dy)
    QAction *alterTurnAct;             //旋转(Turn, Rotate), (X+j*Y)*exp(j*ANG)
    QAction *alterFlipAct;             //镜像(Flip, Mirror, Reflect)
    QAction *alterScaleAct;            //缩放(Scaling, Grow, Shrink) (ReSize), (X,Y)*ScaleFactor
    QAction *alterStretchAct;          //展绷(Stretch) (X*a, Y*b)
    QAction *alterComputeAct;           //计算(Compute) 根据计算y=f(x)来实现变形

    /* 【构元(Cell)】主菜单动作集 (缩进为子菜单动作) */
    QAction *cellNewAct;               //新建构元(New Cell)
    QAction *cellOpenAct;              //打开构元(Open Cell)
    QAction *cellCopyAct;              //复制构元(Copy Cell)
    QAction *cellPasteAct;             //粘贴构元(Paste Cell)
    QAction *cellArrangeAct;           //安排构元(位置)(Arrange Cell)
    QAction *cellMoveOrgAct;           //改变构元基点(Move Origin)
    QAction *cellInstanceAct;          //引用构元(Instance Cell)
    QAction *cellChangeInstanceAct;    //改变引用(Change Cell's Instance)
    QAction *cellMultiAlterAct;        //多元操作(Multiple Cells Operation)
    QAction *cellAssembleAct;          //组装构元(Assemble Cells)
    QAction *cellHierarchyAct;         //构元从属结构关系(Show Hierarchy)
    QAction *cellFlattenAct;           //构元平整显示(Show All as Flat)
    QAction *cellFractureAct;          //细片,碎片,分割(Fracture) 所选构元. (请区分Alter菜单下的alterFractAct)
    QAction *cellInfoAct;              //构元信息(Cell Info)
    QAction *cellStatAct;              //构元统计(Cell Statistics)
    QAction *cellFrameAct;             //构元边界(Cell Outline/Frame)
    QAction *cellShowAct;              //构元显示(Show the Cell)
    QAction *cellHideAct;              //构元隐藏(Hide/Unshow the Cell)
    QAction *cellProtectAct;           //构元保护(Protect the Cell)
    QAction *cellUnprotectAct;         //构元解保(Unprotect the Cell)
    QAction *cellLockAct;              //构元锁定(Lock the Cell)
    QAction *cellUnlockAct;            //构元解锁(Unlock the Cell)

    /* 【构层(Layer)】主菜单动作集 (缩进为子菜单动作) */
    QAction *layerOptnAct;             //构层选项(Layer Option)
    QAction *layerLoadFileAct;         //导入构层配置文件(Load/Import Layer Setup/Config/Technology File)
    QAction *layerSaveFileAct;         //保存构层配置文件(Save/Export Layer Setup/Config/Technology File)
    QAction *layerPanelAct;            //构层面板(Layer Panel)
    QAction *layerPaletteAct;          //构层面板(Layer Palette)
    QAction *layerSetupAct;            //构层设置(Layer Setup)
    QAction *layerDensityAct;          //构层密度(Layer Density = PolygonArea / TotalArea)
    QAction *layerDummyFillAct;        //冗余填充(填充冗余构图以达到版图密度目标, Dummy Fill for Target Density)
    QAction *layerStatAct;             //构层统计(Layer Statistics)
    QAction *layerMappingAct;          //构层对应(Layer Mapping, 对应到指定的层设置上去)
    QAction *layerCtrlAct;             //构层控制(Layer Control)
    //Show/Hide--VIEWable？ Prot/Unprot--SELECTable? Lock/Unlock--EDITable?
    QAction *layerSelectAct;           //构层选择(Select)
    QAction *layerDeselectAct;         //构层解选(Deselect, to make it Unselected)
    QAction *layerShowAct;             //构层显示(Show, Unhide)
    QAction *layerHideAct;             //构层隐藏(Hide, Unshow)
    QAction *layerProtectAct;          //构层保护(Protect)
    QAction *layerUnprotectAct;        //构层解保(Unprotect)
    QAction *layerLockAct;             //构层锁定(Lock)
    QAction *layerUnlockAct;           //构层解锁(Unlock)
    QAction *layerFillAct;             //构层实心显示(Fill)
    QAction *layerUnfillAct;           //构层空心显示(Unfill)

    /* 【编程(Script), 代码(Code)】主菜单动作集 (缩进为子菜单动作) */
    QAction *codeOptnAct;              //代码选项(Code Option)
    QAction *codeNewAct;               //新建代码(New)
    QAction *codeOpenAct;              //打开代码(Open)
    QAction *codeSaveAct;              //保存代码(Save)
    QAction *codeRunAct;               //运行代码(Run)
    QAction *codeDebugAct;             //调试代码(Debug)

    /* 【配置(Config)】, 设置(Setup)】主菜单动作集 (缩进为子菜单动作) */
    QAction *cnfgOptnAct;              //配置选项(Configuration Option)
    QAction *cnfgFontAct;              //配置字体(Configure Font)
    QAction *cnfgDRCAct;               //配置设计规则检查(DRC: Design Rule Check)

    /* 【工具(Tool)】主菜单动作集 (缩进为子菜单动作) */
    QAction *toolUtilityAct;           //小工具集(Unilities)
    QAction *toolLayoutStatAct;        //版图统计(Layout Statistics)
    QAction *toolLayoutCompareAct;     //版图比较(Layout Comparison)
    QAction *toolLayoutDiffAct;        //版图差分(Layout Difference)
    QAction *toolWaferAct;             //晶圆工具(Wafer Tool)

    /* 【窗口(Window)】主菜单动作集 (缩进为子菜单动作) */
    QAction *wndwSplitAct;             //窗口分裂(Split the Window)
    QAction *wndwTileAct;              //窗口排列(Tile the Window)
    QAction *wndwCascadeAct;           //窗口级联(Cascade the Window)
    QAction *wndwArrangeAct;           //窗口整理(Arrange the Window)

    /* 【帮助(Help)】主菜单动作集 (缩进为子菜单动作) */
    QAction *helpIndexAct;             //帮助索引(Help Index)
    QAction *helpQuickRefAct;          //快速参考(Quick Reference, Quick Guide)
    QAction *helpManualAct;            //帮助手册(User's Manual)
    QAction *helpExampleAct;           //帮助实例(Examples)
    QAction *helpAppNoteAct;           //应用笔记(Application Notes)
    QAction *helpUpdateAct;            //软件更新(Software Update)
    QAction *helpHistoryAct;           //版本历史(Revision History)
    QAction *helpLicenceAct;           //软件许可(License Key)
    QAction *helpSupportAct;           //软件支持(Software Support)
    QAction *helpReportBugAct;         //汇报缺陷(Report Bug/Defect)
    QAction *helpAboutAct;             //关于软件(About the Software)

    /* 工具条(Toolbar) */
    QToolBar *fileToolBar;             //【文件(File)】工具条
    QToolBar *editToolBar;             //【编辑(Edit)】工具条
    QToolBar *viewToolBar;             //【视图(View)】工具条
    QToolBar *drawToolBar;             //【绘制(Draw)】工具条
    QToolBar *altrToolBar;             //【变化(Alter)】工具条
    QToolBar *cellToolBar;             //【构元(Cell)】工具条
    QToolBar *layerToolBar;            //【构层(Layer)】工具条
    QToolBar *codeToolBar;             //【编程(Script), 代码(Code)】工具条
    QToolBar *cnfgToolBar;             //【配置(Config)】, 设置(Setup)】工具条
    QToolBar *toolToolBar;             //【工具(Tool)】工具条
    QToolBar *wndwToolBar;             //【窗口(Window)】工具条
    QToolBar *helpToolBar;             //【帮助(Help)】工具条

private:
    QString        fileName;           //文件名称
    QLabel *       msgLabel;           //信息标签
    QLabel *       scenepos;           //状态栏绝对坐标(absolute, scene)显示
    QLabel *       viewpos;            //状态栏view坐标显示
    QLabel *       viewpoint;
    QLabel *       diffpoint;
    QLabel *       diffpos;            //状态栏相对坐标(relative, diff)显示
    QList<QString> List_structureCell; //获取GDSII中构元名字

    QDockWidget *  layerPanel;         //构层面板(Layer Panel, Layer View)
    QDockWidget *  layerPalette;       //构层色板(Layer Palette, Color&Pattern)
    LayerToolbox*  layer_palette;      //创建构层色板的下拉工具框(Color,Stipple/FilPattern,Animation,Style,etc.)调用
    QDockWidget *  cellDock;           //构元结构面板(Cell Structure Tree Panel), 构元组织(Cell Hierarchy)
    QDockWidget *  workDock;           //工作区域面板 (Main Workspace; Work Panel)
    QDockWidget *  naviDock;           //视图导航区域(Navigator, Aerial View)
    QDockWidget *  altrDock;           //变化面板（AltrDock)
    AlterDockWidget *alterItem;        //创建变化面板下的下拉工具框(Boolean,Regenerate,Transform,etc.)调用
    QDockWidget *  codeDock;           //编程区域(Command, Script, Macro, Code, Programming)
    QDockWidget *  infoDock;           //信息输出栏(Information, Status, Result, Output)
    QDockWidget *  hintDock;           //提示报警栏(Hint, Error, Warning, Issue)
    QWidget *      centralwidget;      //中心窗口

    /*--------------------------GDS文件的打开与显示多窗口-----------------------------------*/
private:
    void                 addGdsDocument(CGdsDocument *gdsdoc);
    void                 removeGdsDocument(CGdsDocument *&gdsdoc);
    void                 setCurrentGdsDocument(CGdsDocument *gdsdoc);
    CGdsDocument*        currentGdsDocument() const;
    COptimaskView*       currentView() const;

    void                 newGdsDocument();
    void                 closeGdsDocument();
    QString              fixedWindowTitle(const CGdsDocument *gdsdoc) const;

public slots:
    void                 removeWorkSubTab(int index);
    void                 currentWorkViewChanged(int index);
    void                 currentCellTreeChanged(int index);
    void                 currentLayerPanelChanged(int index);
    void                 currentNavigatorChanged(int index);
    void                 showSelectedItem(QTreeWidgetItem* item,int column);

private:
    CCellDockWidget *celldockwidget;

    QList<CGdsDocument*> m_gdsDocument;
    QTabWidget *  workTabs;            // 多页窗口
    QTabWidget *  layerPanelTabs;      // 多窗口
    QTabWidget *  layerNaviTabs;       // 多窗口
    /* 【视图(View)】 和 【显示(Display)】 相关 */
    bool  dispFileToolBar;
    bool  dispEditToolBar;
    bool  dispViewToolBar;
    bool  dispDrawToolBar;
    bool  dispAltrToolBar;
    bool  dispCellToolBar;
    bool  dispLayerToolBar;
    bool  dispCodeToolBar;
    bool  dispCnfgToolBar;
    bool  dispToolToolBar;
    bool  dispWndwToolBar;
    bool  dispHelpToolBar;
    bool  dispLayerPanel;
    bool  dispLayerPalette;
    bool  dispCellDock;
    bool  dispNaviDock;
    bool  dispAltrDock;
    bool  dispCodeDock;
    bool  dispInfoDock;
    bool  dispHintDock;
    /*------------------------------------------------------------------------------*/

private:
    QVector<QString> imgFile;          // 获取打开的GDSII文件名字
    CGDSStructures * GDSStructure;     // GDSII Cell Structure
    CGDSStructure *  boxStructure;     // CHECK!! box好像不是General的？
    gds2writer       write;
    QPoint           previousPoint;
    int              addOffset;
    int              pasteOffset;

    TransformWidget *transformWidget;
    BrushWidget *    brushWidget;
    PenWidget *      penWidget;

private:
    void fileSaveGDSII(QString filename); //GDSII格式文件存储
    QPoint position();
    void readItems(QDataStream &in, int offset=0, bool select=false);
    void writeItems(QDataStream &out,
                    const QList<QGraphicsItem*> &items);
    void copyItems(const QList<QGraphicsItem*> &items);
    void selectItems(const QSet<QGraphicsItem*> &items);
    void connectItem(QObject *item);
    bool sceneHasItems() const;
    void selectionChanged();
    void clear();
    bool okToClearData();
    void populateCoordinates(const Qt::Alignment &alignment,
            QVector<double> *coordinates,
            const QList<QGraphicsItem*> &items);
    void getSelectionProperties(bool *hasBrushProperty,bool *hasPenProperty) const;
    Navigator *currentNavi();

public slots:
    //void removeShape();
    //void alterAlign();
    void setDirty(bool on=true);
private slots:
    void updateUi();

/**************Add Shape Singal**************/
    void addRect(CGeoPt ,CGeoPt);
    void addLine(CGeoPt ,CGeoPt);
    void addPolygon(std::vector<CGeoPt>,bool);
    void addEllipse(CGeoPt, double, double);
    void addArc(CGeoPt,double,double,double);

    void changeFrameColor(QColor &);
    void changeFillColor(QColor &);

    void dockChanged();
    void dockCloseOrOpen();
    void rulerStatusChanged();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *);

private:
    //工具栏浮动窗口
    QMainWindow *fileFltWndw;
    QMainWindow *editFltWndw;
    QMainWindow *viewFltWndw;
    QMainWindow *drawFltWndw;
    QMainWindow *altrFltWndw;
    QMainWindow *cellFltWndw;
    QMainWindow *layerFltWndw;
    QMainWindow *codeFltWndw;
    QMainWindow *cnfgFltWndw;
    QMainWindow *toolFltWndw;
    QMainWindow *wndwFltWndw;
    QMainWindow *helpFltWndw;

private:
    Matrix *           matrixWidget;
    LibraryWidget *    libraryWidget;
    MultiLine *        multiLine;

    //added by xiong 2017/09/09
    std::vector<CGdsDocument*> m_vecDocument;
};

#endif // MAINWINDOW_H
