
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


#ifndef HDR_dbLayoutStateModel
#define HDR_dbLayoutStateModel

#include "config.h"

#include "tlObserver.h"

namespace db 
{

/**
 *  @brief The layout state model
 *
 *  The layout state model is supposed to track the state of a layout object.
 *  The layout object to be tracked should be derived from db::LayoutStateModel
 *  and implement the do_update method. 
 *  The state model can track the state of any layout (or similar) object in two
 *  ways: once, if the bounding boxes become invalid and in another way, if the
 *  cell tree becomes invalid. These events are issued by containers used within
 *  the layout object (i.e. db::Shapes) and "collected" in the layout state model.
 *  Calling "update" will reset this state and call "do_update" to bring the 
 *  layout object into a consistent state.
 *  In addition, observers using the tl::Observer interface can attach to this
 *  state model to track if the layout changes it's state.
 */

class KLAYOUT_DLL LayoutStateModel
{
public:
  /**
   *  @brief Constructor
   */
  LayoutStateModel ();

  /**
   *  @brief Copy constructor
   *
   *  This constructor does not copy the attachments to the tl::Observed members!
   */
  LayoutStateModel (const LayoutStateModel &d);

  /**
   *  @brief Destructor
   */
  virtual ~LayoutStateModel ();

  /**
   *  @brief Assignment constructor
   *
   *  This operator does not copy the attachments to the tl::Observed members!
   */
  LayoutStateModel &operator= (const LayoutStateModel &d);

  /**
   *  @brief Add an observer for the "hierarchy invalid" event
   */
  void add_hier_observer (tl::Observer &observer);

  /**
   *  @brief Remove an observer for the "hierarchy invalid" event
   */
  void remove_hier_observer (tl::Observer &observer);

  /**
   *  @brief Add an observer for the "bboxes invalid" event
   */
  void add_bboxes_observer (tl::Observer &observer);

  /**
   *  @brief Remove an observer for the "bboxes invalid" event
   */
  void remove_bboxes_observer (tl::Observer &observer);

  /**
   *  @brief Add an observer for the "dbu changed" event
   */
  void add_dbu_changed_observer (tl::Observer &observer);

  /**
   *  @brief Remove an observer for the "dbu changed" event
   */
  void remove_dbu_changed_observer (tl::Observer &observer);

  /**
   *  @brief Add an observer for the "prop id's changed" event
   */
  void add_prop_ids_observer (tl::Observer &observer);

  /**
   *  @brief Remove an observer for the "prop id's changed" event
   */
  void remove_prop_ids_observer (tl::Observer &observer);

  /**
   *  @brief Add an observer for the "cell name changed" event
   */
  void add_cell_name_observer (tl::Observer &observer);

  /**
   *  @brief Remove an observer for the "cell name changed" event
   */
  void remove_cell_name_observer (tl::Observer &observer);

  /**
   *  @brief Add an observer for the "layer properties changed" event
   */
  void add_layer_properties_observer (tl::Observer &observer);

  /**
   *  @brief Remove an observer for the "layer properties changed" event
   */
  void remove_layer_properties_observer (tl::Observer &observer);

  /**
   *  @brief Invalidate the hierarchy information
   * 
   *  This method is supposed to be called if something on the
   *  hierarchy has been changed - i.e. cells have been inserted
   *  or cell instances have been inserted.
   */
  void invalidate_hier ()
  {
    if (! m_hier_dirty) {
      do_invalidate_hier ();  //  must be called before the hierarchy is invalidated (stopping of redraw thread requires this)
      m_hier_dirty = true;
    }
  }

  /**
   *  @brief Invalidate the bounding boxes
   *
   *  This method is supposed to be called by shape containers for example if 
   *  some event has occured that changed the bounding boxes.
   */
  void invalidate_bboxes ()
  {
    if (! m_bboxes_dirty) {
      do_invalidate_bboxes ();  //  must be called before the bboxes are invalidated (stopping of redraw thread requires this)
      m_bboxes_dirty = true;
    }
  }

  /**
   *  @brief Signal that the database unit has changed
   */
  void dbu_changed ()
  {
    m_dbu_changed_observed.signal_observers ();
  }

  /**
   *  @brief This method resets the layout's state back to valid hierarchy and bounding boxes
   *
   *  This method will call do_update if necessary and reset the invalid flags. 
   */
  void update ()
  {
    if (m_bboxes_dirty || m_hier_dirty) {
      do_update ();
      m_bboxes_dirty = false;
      m_hier_dirty = false;
    }
  }

  /**
   *  @brief The "dirty hierarchy" attribute
   *
   *  This attribute is true, if the hierarchy has changed since the last "update" call
   */
  bool hier_dirty () const
  {
    return m_hier_dirty;
  }

  /**
   *  @brief The "dirty bounding box" attribute
   *
   *  This attribute is true, if the bounding boxes have changed since the last "update" call
   */
  bool bboxes_dirty () const
  {
    return m_bboxes_dirty;
  }

protected:
  friend class PropertiesRepository;

  /**
   *  @brief Reimplement this method to update anything related to the hierarchy or bounding boxes.
   */
  virtual void do_update () { }

  /**
   *  @brief Issue a "prop id's changed event"
   */
  void prop_ids_changed ()
  {
    m_prop_ids_observed.signal_observers ();
  }

  /**
   *  @brief Issue a "prop id's changed event"
   */
  void cell_name_changed ()
  {
    m_cell_name_observed.signal_observers ();
  }

  /**
   *  @brief Issue a "layer properties changed event"
   */
  void layer_properties_changed ()
  {
    m_layer_properties_observed.signal_observers ();
  }

private:
  bool m_hier_dirty;
  bool m_bboxes_dirty;
  tl::Observed m_hier_observed;
  tl::Observed m_bboxes_observed;
  tl::Observed m_dbu_changed_observed;
  tl::Observed m_prop_ids_observed;
  tl::Observed m_cell_name_observed;
  tl::Observed m_layer_properties_observed;

  void do_invalidate_hier ();
  void do_invalidate_bboxes ();
};

}

#endif

