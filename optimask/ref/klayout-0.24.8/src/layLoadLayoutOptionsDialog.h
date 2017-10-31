
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



#ifndef HDR_layLoadLayoutOptionsDialog
#define HDR_layLoadLayoutOptionsDialog

#include "LoadLayoutOptionsDialog.h"
#include "dbStream.h"
#include "dbLayout.h"
#include "layStream.h"

#include <string>

class QScrollArea;
class QWidget;

namespace db
{
  class LoadLayoutOptions;
}

namespace lay
{

class LayoutView;
class PluginRoot;
class FileDialog;

class LoadLayoutOptionsDialog
  : public QDialog, private Ui::LoadLayoutOptionsDialog
{
  Q_OBJECT 

public:
  LoadLayoutOptionsDialog (QWidget *parent, const std::string &title);
  ~LoadLayoutOptionsDialog ();

  bool get_global_options (db::LoadLayoutOptions &options, std::string &technology);
  bool get_options (db::LoadLayoutOptions &options);

  void show_always (bool sa)
  {
    m_show_always = sa;
  }

  bool show_always () const
  {
    return m_show_always;
  }

public slots:
  void ok_button_pressed ();
  void reset_button_pressed ();
  void button_pressed (QAbstractButton *button);
  void layer_list_changed ();
  void layer_item_added ();
  void layer_item_deleted ();
  void current_tech_changed (int index);

private:
  std::vector< std::pair<StreamReaderOptionsPage *, std::string> > m_pages;
  std::vector<db::LayerProperties> m_selected_layers;
  bool m_show_always;

  void commit_global_options (db::LoadLayoutOptions &options);
  void update (const db::LoadLayoutOptions &options, const std::string &technology, lay::PluginRoot *config_root);
  bool get_options_internal (db::LoadLayoutOptions &options, std::string &technology, lay::PluginRoot *config_root, bool global);
};

}

#endif

