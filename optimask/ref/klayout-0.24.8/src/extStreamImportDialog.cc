
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


#include "StreamImportDialog.h"
#include "extStreamImporter.h"
#include "extStreamImportDialog.h"

#include "layExceptions.h"
#include "layFileDialog.h"
#include "layDialogs.h"
#include "layConverters.h"
#include "layMainWindow.h"
#include "layLoadLayoutOptionsDialog.h"

#include <QtGui/QFileDialog>

#include <fstream>

namespace ext
{

// -----------------------------------------------------------------------------------------
//  StreamImportData implementation

StreamImportData::StreamImportData ()
  : mode (Simple), layer_mode (Original)
{
  // .. nothing yet ..
}

void 
StreamImportData::setup_importer (StreamImporter *importer)
{
  importer->set_global_trans (explicit_trans);
  importer->set_reference_points (reference_points);
  importer->set_cell_mapping (mode);
  importer->set_layer_mapping (layer_mode);
  importer->set_file (file);
  importer->set_topcell (topcell);
  importer->set_layer_offset (layer_offset);
  importer->set_reader_options (options);
}

void  
StreamImportData::from_string (const std::string &s)
{
  tl::Extractor ex (s.c_str ());
  while (! ex.at_end ()) {

    if (ex.test ("file")) {

      ex.test ("=");
      ex.read_quoted (file);
      ex.test (";");

    } else if (ex.test ("cell-name")) {

      ex.test ("=");
      ex.read_quoted (topcell);
      ex.test (";");

    } else if (ex.test ("cell-name")) {

      ex.test ("=");
      ex.read_quoted (topcell);
      ex.test (";");

    } else if (ex.test ("import-mode")) {

      ex.test ("=");
      int m = 0;
      ex.read (m);
      mode = (mode_type) m;
      ex.test (";");

    } else if (ex.test ("layer-mode")) {

      ex.test ("=");
      int m = 0;
      ex.read (m);
      layer_mode = (layer_mode_type) m;
      ex.test (";");

    } else if (ex.test ("layer-offset")) {

      ex.test ("=");
      layer_offset = db::LayerOffset ();
      layer_offset.read (ex);
      ex.test (";");

    } else if (ex.test ("reference-points")) {

      ex.test ("=");
      reference_points.clear ();
      while (! ex.test (";") && ! ex.at_end ()) {
        double x1, y1, x2, y2;
        ex.test ("(");
        ex.test ("(");
        ex.read (x1);
        ex.test (",");
        ex.read (y1);
        ex.test (")");
        ex.test (",");
        ex.test ("(");
        ex.read (x2);
        ex.test (",");
        ex.read (y2);
        ex.test (")");
        ex.test (")");
        ex.test (",");
        reference_points.push_back (std::make_pair (db::DPoint (x1, y1), db::DPoint (x2, y2)));
      }
      ex.test (";");

    } else if (ex.test ("explicit-trans")) {

      ex.test ("=");
      ex.read (explicit_trans);
      ex.test (";");
  
    } else if (ex.test ("options")) {

      lay::PluginRoot config_root (true /*standalone*/);

      ex.test ("=");
      ex.test ("(");
      while (! ex.test (")") && ! ex.at_end ()) {
        std::string n, v;
        ex.read_word_or_quoted (n, "_.$-");
        ex.test (":");
        ex.read_word_or_quoted (v);
        config_root.config_set (n, v);
        ex.test (";");
      }
      ex.test (";");

      lay::get_from_config (options, &config_root);

    } else {
      ex.expect_end ();
    }

  }
}

std::string  
StreamImportData::to_string () const
{
  std::string s;
  s += "file=" + tl::to_quoted_string(file) + ";";
  s += "cell-name=" + tl::to_quoted_string(topcell) + ";";
  s += "import-mode=" + tl::to_string(int (mode)) + ";";

  s += "reference-points=";
  for (std::vector <std::pair <db::DPoint, db::DPoint> >::const_iterator rp = reference_points.begin (); rp != reference_points.end (); ++rp) {
    if (rp != reference_points.begin ()) {
      s += ",";
    }
    s += "((" + tl::to_string (rp->first.x ()) + "," + tl::to_string (rp->first.y ()) + "),(" 
              + tl::to_string (rp->second.x ()) + "," + tl::to_string (rp->second.y ()) + "))";
  }
  s += ";";

  s += "explicit-trans=" + explicit_trans.to_string () + ";";

  s += "layer-mode=" + tl::to_string(int (layer_mode)) + ";";
  s += "layer-offset=" + layer_offset.to_string () + ";";

  s += "options=(";

  lay::PluginRoot config_root (true /*standalone*/);
  lay::commit_to_config (options, &config_root);

  for (lay::PluginRoot::iterator kv = config_root.begin (); kv != config_root.end (); ++kv) {
    s += kv->first;
    s += ":";
    s += tl::to_quoted_string (kv->second);
    s += ";";
  }

  s += ");";

  return s;
}

// -----------------------------------------------------------------------------------------
//  StreamImportDialog implementation

StreamImportDialog::StreamImportDialog (QWidget *parent, StreamImportData *data)
  : QDialog (parent), mp_data (data)
{
  mp_ui = new Ui::StreamImportDialog ();
  mp_ui->setupUi (this);

  connect (mp_ui->last_pb, SIGNAL (clicked ()), this, SLOT (last_page ()));
  connect (mp_ui->next_pb, SIGNAL (clicked ()), this, SLOT (next_page ()));
  connect (mp_ui->file_pb, SIGNAL (clicked ()), this, SLOT (browse_filename ()));
  connect (mp_ui->edit_options_pb, SIGNAL (clicked ()), this, SLOT (edit_options ()));
  connect (mp_ui->reset_options_pb, SIGNAL (clicked ()), this, SLOT (reset_options ()));
  connect (mp_ui->reset_pb, SIGNAL (clicked ()), this, SLOT (reset ()));
  connect (mp_ui->offset_rb, SIGNAL (clicked ()), this, SLOT (mapping_changed ()));
  connect (mp_ui->no_mapping_rb, SIGNAL (clicked ()), this, SLOT (mapping_changed ()));
}

StreamImportDialog::~StreamImportDialog ()
{
  delete mp_ui;
  mp_ui = 0;
}

void
StreamImportDialog::edit_options ()
{
  lay::LoadLayoutOptionsDialog dialog (this, tl::to_string (QObject::tr ("Import Layout Options")));

  dialog.get_options (mp_data->options);
}

void
StreamImportDialog::reset_options ()
{
  mp_data->options = db::LoadLayoutOptions ();
}

void  
StreamImportDialog::browse_filename ()
{
  QString file = mp_ui->file_le->text ();
  file = QFileDialog::getOpenFileName (this, QObject::tr ("File To Import"), file, QObject::tr ("All files (*)"));
  if (! file.isNull ()) {
    mp_ui->file_le->setText (file);
  }
}

void 
StreamImportDialog::reject ()
{
  try {
    commit_page ();
  } catch (...) {
    // .. nothing yet ..
  }

  QDialog::reject ();
}

void 
StreamImportDialog::accept ()
{
  BEGIN_PROTECTED

  commit_page ();
  QDialog::accept ();

  END_PROTECTED
}

int 
StreamImportDialog::exec ()
{
  mp_ui->central_stack->setCurrentIndex (0);
  update ();

  return QDialog::exec ();
}

/* 
 * 0 - General
 * 1 - Layers 
 * 2 - Reference points
 */
static int next_pages[] = {  1, 2, -1 };
static int prev_pages[] = { -1, 0, 1  };

void
StreamImportDialog::next_page ()
{
  BEGIN_PROTECTED 
  commit_page ();

  int index = mp_ui->central_stack->currentIndex ();
  if (index >= 0 && index < int (sizeof (next_pages) / sizeof (next_pages [0]))) {
    index = next_pages [index];
    if (index >= 0) {
      mp_ui->central_stack->setCurrentIndex (index);
      enter_page ();
    }
  } 

  update ();
  END_PROTECTED
}

void 
StreamImportDialog::last_page ()
{
  // "safe" commit
  try {
    commit_page ();
  } catch (...) { }

  BEGIN_PROTECTED 

  int index = mp_ui->central_stack->currentIndex ();
  if (index >= 0 && index < int (sizeof (next_pages) / sizeof (next_pages [0]))) {
    index = prev_pages [index];
    if (index >= 0) {
      mp_ui->central_stack->setCurrentIndex (index);
    }
  } 

  update ();
  END_PROTECTED
}

void
StreamImportDialog::enter_page ()
{
  // .. nothing yet ..
}

void 
StreamImportDialog::commit_page ()
{
  int page = mp_ui->central_stack->currentIndex ();

  if (page == 0) {

    //  --- General page
    mp_data->file = tl::to_string (mp_ui->file_le->text ());
    mp_data->topcell = tl::to_string (mp_ui->topcell_le->text ());
    if (mp_ui->import_simple_rb->isChecked ()) {
      mp_data->mode = StreamImportData::Simple;
    } else if (mp_ui->import_instantiate_rb->isChecked ()) {
      mp_data->mode = StreamImportData::Instantiate;
    } else if (mp_ui->import_extra_rb->isChecked ()) {
      mp_data->mode = StreamImportData::Extra;
    } else if (mp_ui->import_merge_rb->isChecked ()) {
      mp_data->mode = StreamImportData::Merge;
    }

  } else if (page == 1) {

    //  --- Layer mapping
    if (mp_ui->no_mapping_rb->isChecked ()) {
      mp_data->layer_mode = StreamImportData::Original;
    } else if (mp_ui->offset_rb->isChecked ()) {
      mp_data->layer_mode = StreamImportData::Offset;
    }

    mp_data->layer_offset = db::LayerOffset ();
    std::string offset = tl::to_string (mp_ui->offset_le->text ());
    tl::Extractor ex (offset.c_str ());
    mp_data->layer_offset.read (ex);

  } else if (page == 2) {

    //  --- Coordinate Mapping page
    QLineEdit *(coord_editors[][4]) = {
      { mp_ui->pcb_x1_le, mp_ui->pcb_y1_le, mp_ui->layout_x1_le, mp_ui->layout_y1_le },
      { mp_ui->pcb_x2_le, mp_ui->pcb_y2_le, mp_ui->layout_x2_le, mp_ui->layout_y2_le },
      { mp_ui->pcb_x3_le, mp_ui->pcb_y3_le, mp_ui->layout_x3_le, mp_ui->layout_y3_le }
    };

    mp_data->reference_points.clear ();
    for (unsigned int i = 0; i < sizeof (coord_editors) / sizeof (coord_editors [0]); ++i) {

      std::string t_pcb_x (tl::to_string (coord_editors [i][0]->text ()));
      tl::Extractor pcb_x (t_pcb_x.c_str ());

      std::string t_pcb_y (tl::to_string (coord_editors [i][1]->text ()));
      tl::Extractor pcb_y (t_pcb_y.c_str ());

      std::string t_layout_x (tl::to_string (coord_editors [i][2]->text ()));
      tl::Extractor layout_x (t_layout_x.c_str ());

      std::string t_layout_y (tl::to_string (coord_editors [i][3]->text ()));
      tl::Extractor layout_y (t_layout_y.c_str ());

      if (pcb_x.at_end () || pcb_y.at_end () || layout_x.at_end () || layout_y.at_end ()) {

        if (! pcb_x.at_end () || ! pcb_y.at_end () || ! layout_x.at_end () || ! layout_y.at_end ()) {
          throw tl::Exception (tl::to_string (QObject::tr ("All coordinates (imported and existing layout) must be specified for a reference point")));
        }

      } else {

        double x, y;

        pcb_x.read (x);
        pcb_x.expect_end ();
        pcb_y.read (y);
        pcb_y.expect_end ();
        db::DPoint pcb (x, y);

        layout_x.read (x);
        layout_x.expect_end ();
        layout_y.read (y);
        layout_y.expect_end ();
        db::DPoint layout (x, y);

        mp_data->reference_points.push_back (std::make_pair (pcb, layout));

      }
    }

    std::string t (tl::to_string (mp_ui->explicit_trans_le->text ()));
    tl::Extractor ex (t.c_str ());
    mp_data->explicit_trans = db::DCplxTrans ();
    if (! ex.at_end ()) {
      ex.read (mp_data->explicit_trans);
      ex.expect_end ();
    }

  } 
}

void 
StreamImportDialog::update ()
{
  std::string section_headers[] = {
    tl::to_string (QObject::tr ("General")),
    tl::to_string (QObject::tr ("Layers")),
    tl::to_string (QObject::tr ("Coordinate Mapping")),
  };

  int page = mp_ui->central_stack->currentIndex ();
  if (page < 0 || page >= int (sizeof (section_headers) / sizeof (section_headers [0]))) {
    return;
  }

  mp_ui->last_pb->setEnabled (page > 0);
  mp_ui->next_pb->setEnabled (page < int (sizeof (section_headers) / sizeof (section_headers [0]) - 1));
  mp_ui->section_header_lbl->setText (tl::to_qstring (section_headers [page]));

  //  --- General page
  mp_ui->file_le->setText (tl::to_qstring (mp_data->file));
  mp_ui->topcell_le->setText (tl::to_qstring (mp_data->topcell));
  mp_ui->import_simple_rb->setChecked (mp_data->mode == StreamImportData::Simple);
  mp_ui->import_extra_rb->setChecked (mp_data->mode == StreamImportData::Extra);
  mp_ui->import_instantiate_rb->setChecked (mp_data->mode == StreamImportData::Instantiate);
  mp_ui->import_merge_rb->setChecked (mp_data->mode == StreamImportData::Merge);

  //  --- Layers page
  mp_ui->no_mapping_rb->setChecked (mp_data->layer_mode == StreamImportData::Original);
  mp_ui->offset_rb->setChecked (mp_data->layer_mode == StreamImportData::Offset);
  mp_ui->offset_le->setText (tl::to_qstring (mp_data->layer_offset.to_string ()));
  mapping_changed ();

  //  --- Coordinate Mapping page
  QLineEdit *(coord_editors[][4]) = {
    { mp_ui->pcb_x1_le, mp_ui->pcb_y1_le, mp_ui->layout_x1_le, mp_ui->layout_y1_le },
    { mp_ui->pcb_x2_le, mp_ui->pcb_y2_le, mp_ui->layout_x2_le, mp_ui->layout_y2_le },
    { mp_ui->pcb_x3_le, mp_ui->pcb_y3_le, mp_ui->layout_x3_le, mp_ui->layout_y3_le }
  };

  for (unsigned int i = 0; i < sizeof (coord_editors) / sizeof (coord_editors [0]); ++i) {
    if (mp_data->reference_points.size () > i) {
      coord_editors[i][0]->setText (tl::to_qstring (tl::to_string (mp_data->reference_points [i].first.x ())));
      coord_editors[i][1]->setText (tl::to_qstring (tl::to_string (mp_data->reference_points [i].first.y ())));
      coord_editors[i][2]->setText (tl::to_qstring (tl::to_string (mp_data->reference_points [i].second.x ())));
      coord_editors[i][3]->setText (tl::to_qstring (tl::to_string (mp_data->reference_points [i].second.y ())));
    } else {
      for (unsigned int j = 0; j < 4; ++j) {
        coord_editors[i][j]->setText (QString ());
      }
    }
  }

  if (mp_data->explicit_trans == db::DCplxTrans ()) {
    mp_ui->explicit_trans_le->setText (QString ());
  } else {
    mp_ui->explicit_trans_le->setText (tl::to_qstring (mp_data->explicit_trans.to_string ()));
  }
}

void 
StreamImportDialog::mapping_changed ()
{
  mp_ui->offset_le->setEnabled (mp_ui->offset_rb->isChecked ());
}

void 
StreamImportDialog::reset ()
{
  //  Commit everything that is not loaded
  try {
    commit_page ();
  } catch (...) {
    // ..
  }

  *mp_data = StreamImportData ();
  mp_ui->central_stack->setCurrentIndex (0);
  update ();
}

}

