
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


#include "dbLayoutStateModel.h"

namespace db 
{

LayoutStateModel::LayoutStateModel ()
  : m_hier_dirty (false), m_bboxes_dirty (false)
{
  //  .. nothing yet ..
}

LayoutStateModel::LayoutStateModel (const LayoutStateModel &d)
  : m_hier_dirty (d.m_hier_dirty), m_bboxes_dirty (d.m_bboxes_dirty)
{
  //  .. nothing yet ..
}

LayoutStateModel &
LayoutStateModel::operator= (const LayoutStateModel &d)
{
  m_hier_dirty = d.m_hier_dirty;
  m_bboxes_dirty = d.m_bboxes_dirty;
  return *this;
}

LayoutStateModel::~LayoutStateModel ()
{
  //  .. nothing yet ..
}

void 
LayoutStateModel::add_hier_observer (tl::Observer &observer)
{
  m_hier_observed.add_observer (observer);
}

void 
LayoutStateModel::remove_hier_observer (tl::Observer &observer)
{
  m_hier_observed.remove_observer (observer);
}

void 
LayoutStateModel::add_bboxes_observer (tl::Observer &observer)
{
  m_bboxes_observed.add_observer (observer);
}

void 
LayoutStateModel::remove_bboxes_observer (tl::Observer &observer)
{
  m_bboxes_observed.remove_observer (observer);
}

void 
LayoutStateModel::add_dbu_changed_observer (tl::Observer &observer)
{
  m_dbu_changed_observed.add_observer (observer);
}

void 
LayoutStateModel::remove_dbu_changed_observer (tl::Observer &observer)
{
  m_dbu_changed_observed.remove_observer (observer);
}

void 
LayoutStateModel::add_cell_name_observer (tl::Observer &observer)
{
  m_cell_name_observed.add_observer (observer);
}

void 
LayoutStateModel::remove_cell_name_observer (tl::Observer &observer)
{
  m_cell_name_observed.remove_observer (observer);
}

void 
LayoutStateModel::add_prop_ids_observer (tl::Observer &observer)
{
  m_prop_ids_observed.add_observer (observer);
}

void 
LayoutStateModel::remove_prop_ids_observer (tl::Observer &observer)
{
  m_prop_ids_observed.remove_observer (observer);
}

void 
LayoutStateModel::add_layer_properties_observer (tl::Observer &observer)
{
  m_layer_properties_observed.add_observer (observer);
}

void 
LayoutStateModel::remove_layer_properties_observer (tl::Observer &observer)
{
  m_layer_properties_observed.remove_observer (observer);
}

void 
LayoutStateModel::do_invalidate_hier ()
{
  m_hier_observed.signal_observers ();
}

void 
LayoutStateModel::do_invalidate_bboxes ()
{
  m_bboxes_observed.signal_observers ();
}

}

