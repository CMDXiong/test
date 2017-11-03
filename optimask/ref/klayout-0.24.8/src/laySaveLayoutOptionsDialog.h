
/*

  KLayout Layout Viewer
  Copyright (C) 2006-2016 Matthias Koefferlein

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/


#ifndef HDR_laySaveLayoutOptionsDialog
#define HDR_laySaveLayoutOptionsDialog

#include "SaveLayoutOptionsDialog.h"
#include "SaveLayoutAsOptionsDialog.h"
#include "dbStream.h"
#include "layStream.h"
#include "tlStream.h"

#include <string>

class QScrollArea;
class QWidget;

namespace db
{
  class SaveLayoutOptions;
}

namespace lay
{

class LayoutView;
class PluginRoot;

class SaveLayoutAsOptionsDialog
  : public QDialog, private Ui::SaveLayoutAsOptionsDialog
{
  Q_OBJECT 

public:
  SaveLayoutAsOptionsDialog (QWidget *parent, const std::string &title);
  ~SaveLayoutAsOptionsDialog ();

  bool get_options (lay::LayoutView *view, unsigned int cv_index, const std::string &fn, tl::OutputStream::OutputStreamMode &compression, db::SaveLayoutOptions &options, lay::PluginRoot *config_root, bool save_permanently);

public slots:
  void ok_button_pressed ();
  void fmt_cbx_changed (int);

private:
  std::vector< std::pair<StreamWriterOptionsPage *, std::string> > m_pages;
  std::vector<int> m_tab_positions;
  std::string m_filename;
};

class SaveLayoutOptionsDialog
  : public QDialog, private Ui::SaveLayoutOptionsDialog
{
  Q_OBJECT 

public:
  SaveLayoutOptionsDialog (QWidget *parent, const std::string &title);
  ~SaveLayoutOptionsDialog ();

public slots:
  void ok_button_pressed ();
  bool exec (lay::PluginRoot *config_root);
  void current_tab_changed (int index);

private:
  std::vector< std::pair<StreamWriterOptionsPage *, std::string> > m_pages;
  std::string m_filename;
  QWidget *mp_current_tab;

  void commit_current_page (lay::PluginRoot *config_root);
};

}

#endif

