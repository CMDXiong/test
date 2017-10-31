
#include <QFileInfo>

#include"gdsdocument.h"
#include "./File/gds2reader.h"
#include "./Layer/laytreeview.h"
#include "./base/GeoFile.h"


CGdsDocument::CGdsDocument()
    :m_cellTreeWidget(NULL),m_gdsView(NULL),m_tableView(NULL),m_scene(new CGeoScene()){}

CGdsDocument::~CGdsDocument(){}

void CGdsDocument::LoadDocument(QString filename)
{
    std::string sfilename = filename.toStdString();
    m_scene =  ReadDataFromFile_A(sfilename, "");
    SetDocumentName(filename);
}
void CGdsDocument::SaveFile(){}

void CGdsDocument::SetDocumentName(const QString &fileName)
{
    m_documentName = fileName;
}
QString CGdsDocument::DocumentName() const
{
    return m_documentName;
}
void CGdsDocument::SetViewWid(COptimaskView* gdsview)
{
    m_gdsView=gdsview;
}
COptimaskView* CGdsDocument::GetViewWid()const
{
    return m_gdsView;
}
 void CGdsDocument::SetCellTreeWid(CCellTreeWidget* celltree)
 {
     m_cellTreeWidget=celltree;
 }
  CCellTreeWidget* CGdsDocument::GetCellTreeWid()const
  {
      return m_cellTreeWidget;
  }
  void CGdsDocument::SetLayTableViewWid(LayTableView* layerview)
  {
      m_tableView=layerview;
  }
  LayTableView* CGdsDocument::GetTableView()const
  {
      return m_tableView;
  }
 void CGdsDocument::SaveCellViewWid(COptimaskView* cellview)
 {
     m_listCellView.push_back(cellview);
 }
 void CGdsDocument::DeleteCellViewWid(COptimaskView* cellview)
 {
     if(m_listCellView.empty()==true)
         return;
     int index=m_listCellView.indexOf(cellview);

     m_listCellView.removeAt(index);
 }

void CGdsDocument::SetNavigator(Navigator* navigator)
{
    m_navigator=navigator;
}
Navigator* CGdsDocument::GetNavigator()const
{
    return m_navigator;
}

ref_ptr<CGeoScene> CGdsDocument::GetSceneData() const
{
    return m_scene;
}
