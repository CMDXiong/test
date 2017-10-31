
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


#include "layCellView.h"
#include "dbLayout.h"
#include "dbWriter.h"
#include "dbReader.h"
#include "tlLog.h"

#include <algorithm>

namespace lay
{

// -------------------------------------------------------------

static std::string 
filename_for_caption (const std::string &fn)
{
  const char *cp = fn.c_str ();
  const char *cpp = cp + fn.size ();
  while (cpp > cp && cpp [-1] != '\\' && cpp [-1] != '/') {
    --cpp;
  }
  return cpp;
}

// -------------------------------------------------------------
//  LayoutHandle implementation


LayoutHandle::LayoutHandle (db::Layout *layout, const std::string &filename)
  : tl::Observer (), 
    mp_layout (layout),
    m_ref_count (0),
    m_filename (filename),
    m_dirty (false)
{
  if (! m_filename.empty ()) {
    rename (filename_for_caption (m_filename));
  } else {

    //  create a unique new name 
    static int nn = 0;

    std::string n;
    do {
      n = tl::sprintf ("L%d", ++nn);
    } while (find (n) != 0);

    m_name = n;
    ms_dict.insert (std::make_pair (n, this));

  }

  mp_layout->add_hier_observer (*this);
  mp_layout->add_bboxes_observer (*this);
  mp_layout->add_cell_name_observer (*this);
  mp_layout->add_prop_ids_observer (*this);
  mp_layout->add_layer_properties_observer (*this);

  if (tl::verbosity () >= 30) {
    tl::info << "Created layout " << name ();
  }
}

LayoutHandle::~LayoutHandle ()
{
  if (tl::verbosity () >= 30) {
    tl::info << "Deleted layout " << name ();
  }

  delete mp_layout;
  mp_layout = 0;

  if (find (m_name) == this) {
    ms_dict.erase (m_name);
  }
}

void 
LayoutHandle::signal_observed ()
{
  m_dirty = true;
}

void 
LayoutHandle::rename (const std::string &name, bool force) throw (tl::Exception)
{
  std::string n (name);

  if (n != m_name) {

    if (find (m_name) == this) {
      ms_dict.erase (m_name);
    }

    if (force || find (n) == 0) {
      if (tl::verbosity () >= 40) {
        tl::info << "Renamed layout from " << m_name << " to " << n;
      }
      m_name = n;
      ms_dict.insert (std::make_pair (n, this));
      return;
    }

    //  rename using suffix "[u]" where u is a unique index
    int nn = 0;
    int ns = 0x40000000;
    do {
      n = name + tl::sprintf ("[%d]", nn + ns);
      if (find (n) != 0) {
        nn += ns;
      }
      ns /= 2;
    } while (ns > 0);

    n = name + tl::sprintf ("[%d]", nn + 1);

    if (tl::verbosity () >= 40) {
      tl::info << "Renamed layout from " << m_name << " to " << n;
    }

    m_name = n;
    ms_dict.insert (std::make_pair (n, this));
    return;

  }
}

db::Layout &
LayoutHandle::layout () const
{
  return *mp_layout;
}

void
LayoutHandle::set_filename (const std::string &fn) 
{
  m_filename = fn;
}

const std::string &
LayoutHandle::filename () const
{
  return m_filename;
}

const std::string &
LayoutHandle::name () const
{
  return m_name;
}

void 
LayoutHandle::add_ref ()
{
  if (tl::verbosity () >= 50) {
    tl::info << "Add reference to " << m_name;
  }
  ++m_ref_count;
}

void 
LayoutHandle::remove_ref ()
{
  if (tl::verbosity () >= 50) {
    tl::info << "Remove reference from " << m_name;
  }
  if (--m_ref_count <= 0) {
    //  not nice, but hopefully we can do so:
    delete this;
  }
}

const lay::Technology *
LayoutHandle::technology () const
{
  return lay::Technologies::instance ()->technology_by_name (m_tech_name);
}

void 
LayoutHandle::apply_technology (const std::string &tn)
{
  set_tech_name (tn);
  m_apply_technology_observer.signal_observers ();
  m_apply_technology_observer.signal_observers ((void *) this);
}

void 
LayoutHandle::set_tech_name (const std::string &tn)
{
  if (tn != m_tech_name) {
    m_tech_name = tn;
    m_technology_changed_observed.signal_observers ();
  }
}

std::map <std::string, LayoutHandle *> LayoutHandle::ms_dict;

LayoutHandle *
LayoutHandle::find (const std::string &name)
{
  std::map <std::string, LayoutHandle *>::const_iterator h = ms_dict.find (name);
  if (h == ms_dict.end ()) {
    return 0;
  } else {
    return h->second;
  }
}

void 
LayoutHandle::get_names (std::vector <std::string> &names)
{
  names.clear ();
  names.reserve (ms_dict.size ());
  for (std::map <std::string, LayoutHandle *>::const_iterator h = ms_dict.begin (); h != ms_dict.end (); ++h) {
    names.push_back (h->first);
  }
}

void 
LayoutHandle::save_as (const std::string &filename, tl::OutputStream::OutputStreamMode om, const db::SaveLayoutOptions &options, bool update)
{
  if (update) {
    m_save_options = options;
    rename (filename_for_caption (filename));
    set_filename (filename);
  }

  db::Writer writer (options);

  tl::OutputStream stream (filename, om);
  writer.write (*mp_layout, stream);

  if (update) {

    clear_meta_info ();
    for (db::Writer::meta_info_iterator meta = writer.begin_meta (); meta != writer.end_meta (); ++meta) {
      add_meta_info (*meta);
    }

    m_dirty = false;

  }
}

db::LayerMap 
LayoutHandle::load (const db::LoadLayoutOptions &options)
{
  m_load_options = options;

  tl::InputStream stream (m_filename);
  db::Reader reader (stream);
  db::LayerMap new_lmap = reader.read (layout (), m_load_options);

  clear_meta_info ();
  for (db::Reader::meta_info_iterator meta = reader.begin_meta (); meta != reader.end_meta (); ++meta) {
    add_meta_info (*meta);
  }

  m_dirty = false;

  return new_lmap;
}

db::LayerMap 
LayoutHandle::load ()
{
  m_load_options = db::LoadLayoutOptions ();

  tl::InputStream stream (m_filename);
  db::Reader reader (stream);
  db::LayerMap new_lmap = reader.read (layout (), m_load_options);

  clear_meta_info ();
  for (db::Reader::meta_info_iterator meta = reader.begin_meta (); meta != reader.end_meta (); ++meta) {
    add_meta_info (*meta);
  }

  m_dirty = false;

  return new_lmap;
}

// -------------------------------------------------------------
//  LayoutHandleRef implementation

LayoutHandleRef::LayoutHandleRef ()
  : mp_handle (0)
{
  // .. nothing yet ..
}

LayoutHandleRef::LayoutHandleRef (LayoutHandle *h)
  : mp_handle (0)
{
  set (h);
}

LayoutHandleRef::LayoutHandleRef (const LayoutHandleRef &r)
  : mp_handle (0)
{
  set (r.mp_handle);
}

LayoutHandleRef::~LayoutHandleRef ()
{
  set (0);
}

bool 
LayoutHandleRef::operator== (const LayoutHandleRef &r) const
{
  return mp_handle == r.mp_handle;
}

LayoutHandleRef &
LayoutHandleRef::operator= (const LayoutHandleRef &r)
{
  if (&r != this) {
    set (r.mp_handle);
  }
  return *this;
}

void
LayoutHandleRef::set (LayoutHandle *h)
{
  if (mp_handle) {
    mp_handle->remove_ref ();
    mp_handle = 0;
  } 
  mp_handle = h;
  if (mp_handle) {
    mp_handle->add_ref ();
  }
}

LayoutHandle *
LayoutHandleRef::operator-> () const
{
  return mp_handle;
}

LayoutHandle *
LayoutHandleRef::get () const
{
  return mp_handle;
}

// -------------------------------------------------------------
//  LayoutHandleRef implementation

CellView::CellView () 
  : mp_ctx_cell (0), m_ctx_cell_index (0), mp_cell (0), m_cell_index (cell_index_type (-1))
{ }

bool 
CellView::operator== (const CellView &cv) const
{
  return m_layout_href == cv.m_layout_href 
        && mp_ctx_cell == cv.mp_ctx_cell && m_ctx_cell_index == cv.m_ctx_cell_index 
        && mp_cell == cv.mp_cell && m_cell_index == cv.m_cell_index 
        && m_unspecific_path == cv.m_unspecific_path && m_specific_path == cv.m_specific_path;
}

bool 
CellView::is_valid () const
{
  if (m_layout_href.get () == 0 || mp_cell == 0) {
    return false;
  }

  //  check, if the path references valid cell indices.
  for (specific_cell_path_type::const_iterator pp = m_specific_path.begin (); pp != m_specific_path.end (); ++pp) {
    if (! m_layout_href.get ()->layout ().is_valid_cell_index (pp->inst_ptr.cell_index ())) {
      return false;
    }
  }
  for (unspecific_cell_path_type::const_iterator pp = m_unspecific_path.begin (); pp != m_unspecific_path.end (); ++pp) {
    if (! m_layout_href.get ()->layout ().is_valid_cell_index (*pp)) {
      return false;
    }
  }

  return true;
}

void 
CellView::set_unspecific_path (const unspecific_cell_path_type &p)
{
  tl_assert (m_layout_href.get () != 0);

  mp_cell = 0;
  m_cell_index = 0;
  m_unspecific_path = p;
  m_specific_path.clear ();

  if (p.size () > 0 && m_layout_href.get () && p.back () < m_layout_href->layout ().cells ()) {
    m_cell_index = p.back ();
    mp_cell = &m_layout_href->layout ().cell (p.back ());
  }

  mp_ctx_cell = mp_cell;
  m_ctx_cell_index = m_cell_index;
}

void 
CellView::set_specific_path (const specific_cell_path_type &p)
{
  tl_assert (m_layout_href.get () != 0);

  m_specific_path = p;
  for (specific_cell_path_type::iterator pp = m_specific_path.begin (); pp != m_specific_path.end (); ++pp) {
    //  fix elements of the path not associated with a certain array instance (this may happen if 
    //  unspecific selections are put into the path)
    if (pp->array_inst.at_end ()) {
      pp->array_inst = pp->inst_ptr.begin ();
    }
  }

  if (p.empty ()) {
    m_cell_index = m_ctx_cell_index;
    mp_cell = mp_ctx_cell;
  } else if (m_layout_href.get () && p.back ().inst_ptr.cell_index () < m_layout_href->layout ().cells ()) {
    m_cell_index = p.back ().inst_ptr.cell_index ();
    mp_cell = &m_layout_href->layout ().cell (m_cell_index);
  } else {
    reset_cell ();
  }
}

CellView::unspecific_cell_path_type 
CellView::combined_unspecific_path () const
{
  CellView::unspecific_cell_path_type path;
  path.reserve (m_unspecific_path.size () + m_specific_path.size ());
  path.insert (path.end (), m_unspecific_path.begin (), m_unspecific_path.end ());
  for (CellView::specific_cell_path_type::const_iterator p = m_specific_path.begin (); p != m_specific_path.end (); ++p) {
    path.push_back (p->inst_ptr.cell_index ());
  }
  return path;
}

void
CellView::set_cell (cell_index_type index)
{
  tl_assert (m_layout_href.get () != 0);

  db::Layout &layout = m_layout_href->layout ();
  
  if (! layout.is_valid_cell_index (index)) {

    reset_cell ();

  } else {

    m_cell_index = index;
    mp_cell = &layout.cell (m_cell_index);

    m_unspecific_path.clear ();
    m_specific_path.clear ();
    m_unspecific_path.push_back (index);

    while (! layout.cell (index).is_top ()) {
      index = *layout.cell (index).begin_parent_cells ();
      m_unspecific_path.push_back (index);
    }

    std::reverse (m_unspecific_path.begin (), m_unspecific_path.end ());

    mp_ctx_cell = mp_cell;
    m_ctx_cell_index = m_cell_index;

  }
}

void 
CellView::set_cell (const std::string &name)
{
  tl_assert (m_layout_href.get () != 0);

  std::pair<bool, db::cell_index_type> cp = m_layout_href->layout ().cell_by_name (name.c_str ());
  if (cp.first) {
    set_cell (cp.second);
  } else {
    reset_cell ();
  }
}

void 
CellView::reset_cell ()
{
  mp_cell = 0;
  m_cell_index = cell_index_type (-1);
  mp_ctx_cell = 0;
  m_ctx_cell_index = 0;
  m_unspecific_path.clear ();
  m_specific_path.clear ();
}

void 
CellView::set (lay::LayoutHandle *handle)
{
  reset_cell ();
  m_layout_href.set (handle);
}

CellView
CellView::deep_copy (db::Manager *manager) const
{
  CellView r;
  r.set (new lay::LayoutHandle (new db::Layout (manager), ""));
  r->layout () = (*this)->layout ();
  r.set_unspecific_path (unspecific_path ());
  r.set_specific_path (specific_path ());
  return r;
}

db::CplxTrans 
CellView::context_trans () const
{
  db::CplxTrans trans;
  for (std::vector <db::InstElement>::const_iterator p = specific_path ().begin (); p != specific_path ().end (); ++p) {
    trans = trans * p->complex_trans ();
  }
  return trans;
}


}

