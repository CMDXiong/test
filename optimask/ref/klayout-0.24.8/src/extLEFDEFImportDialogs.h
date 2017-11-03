
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


#ifndef HDR_extLEFImportDialog
#define HDR_extLEFImportDialog

#include "config.h"
#include "layTechnology.h"

#include "LEFDEFImportOptionsDialog.h"
#include "LEFDEFTechnologyComponentEditor.h"

#include <QtGui/QDialog>

namespace ext
{

/**
 *  @brief A structure containing the LEF importer data
 */
struct KLAYOUT_DLL LEFDEFImportData 
{
  LEFDEFImportData ();

  void from_string (const std::string &s);
  std::string to_string () const;

  int mode;
  std::string file;
  std::vector<std::string> lef_files;
};

/**
 *  @brief The LEF importer dialog
 */
class KLAYOUT_DLL LEFDEFImportOptionsDialog
  : public QDialog,
    private Ui::LEFDEFImportOptionsDialog
{
Q_OBJECT

public:
  LEFDEFImportOptionsDialog (QWidget *parent, bool is_lef_dialog);

  int exec (LEFDEFImportData &data);

private slots:
  void browse_button_clicked ();
  void tech_setup_button_clicked ();
  void add_lef_file_clicked ();
  void del_lef_files_clicked ();
  void move_lef_files_up_clicked ();
  void move_lef_files_down_clicked ();

private:
  bool m_is_lef_dialog;
};

/**
 *  @brief The LEF technology component editor
 */
class LEFDEFTechnologyComponentEditor
  : public lay::TechnologyComponentEditor,
    public Ui::LEFDEFTechnologyComponentEditor
{
Q_OBJECT

public:
  LEFDEFTechnologyComponentEditor (QWidget *parent);

  void commit ();
  void setup ();

private slots:
  void checkbox_changed ();
  void add_lef_file_clicked ();
  void del_lef_files_clicked ();
  void move_lef_files_up_clicked ();
  void move_lef_files_down_clicked ();
};

}

#endif

