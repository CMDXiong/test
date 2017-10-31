
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


#ifndef HDR_layCellView
#define HDR_layCellView

#include "config.h"

#include <string>
#include <vector>

#include "layTechnology.h"
#include "dbLayout.h"
#include "dbMetaInfo.h"
#include "dbReader.h"
#include "dbSaveLayoutOptions.h"
#include "dbLoadLayoutOptions.h"
#include "dbInstElement.h"
#include "gsi.h"

namespace lay 
{

/**
 *  @brief A layout handle
 *
 *  This object controls a layout object. A layout object can be
 *  identified through a name. Additionally, a reference count
 *  is maintained that controls when the layout object is deleted.
 */
class KLAYOUT_DLL LayoutHandle
  : public tl::Observer, public tl::Observed
{
public:
  typedef db::Reader::meta_info meta_info;
  typedef db::Reader::meta_info_iterator meta_info_iterator;
  typedef db::MetaInfo meta_info_item;
  
  /**
   *  @brief Creates a layout handle to the given object
   *
   *  This constructor creates a new handle to the given 
   *  layout object. The handle takes over the ownership over the
   *  layout object.
   *  The initial reference count is zero (see remove_ref).
   *  The filename is a string that is supposed to identify
   *  the layout further. It can be retrieved with the filename 
   *  method.
   */
  LayoutHandle (db::Layout *layout, const std::string &filename);

  /**
   *  @brief Destructor
   *
   *  The destructor will delete the layout object that
   *  was associated with this handle.
   */
  ~LayoutHandle ();

  /**
   *  @brief Rename the layout object
   *
   *  If "force" is set to true, the layout will be given that name, irregardless if
   *  the name already is being used. If "force" is false, a new unique name is created.
   */
  void rename (const std::string &name, bool force = false) throw (tl::Exception);

  /**
   *  @brief Get the name of the handle
   */
  const std::string &name () const;

  /**
   *  @brief Get the layout object that this handle points to
   */
  db::Layout &layout () const;

  /**
   *  @brief Set the file name associated with this handle
   */
  void set_filename (const std::string &);

  /**
   *  @brief Get the file name associated with this handle
   */
  const std::string &filename () const;

  /**
   *  @brief "begin" iterator for the meta info
   */
  meta_info_iterator begin_meta () const
  {
    return m_meta_info.begin ();
  }

  /**
   *  @brief "end" iterator for the meta info
   */
  meta_info_iterator end_meta () const
  {
    return m_meta_info.end ();
  }

  /**
   *  @brief Add a meta information item
   */
  void add_meta_info (const meta_info_item &m)
  {
    m_meta_info.push_back (m);
  }

  /**
   *  @brief Clears the meta information
   */
  void clear_meta_info ()
  {
    m_meta_info.clear ();
  }

  /**
   *  @brief Get the technology attached to this layout
   */
  const lay::Technology *technology () const;

  /**
   *  @brief Get the technology name for this layout
   *
   *  An empty name indicates the default technology should be used.
   */
  const std::string &tech_name () const
  {
    return m_tech_name;
  }

  /**
   *  @brief Apply the given technology
   *
   *  This will set the technology to the new one and send the apply event.
   *  This event is sent always, even if the technology did not change.
   */
  void apply_technology (const std::string &tn);

  /**
   *  @brief Set the technology name
   *
   *  If there is no technology with that name, the default technology
   *  will be used.
   */
  void set_tech_name (const std::string &tn);

  /**
   *  @brief Find a layout object by name
   *
   *  @param name The name under which to find the layout object
   *  @return 0, if there is no layout object with this name. Otherwise a pointer to its handle
   */
  static LayoutHandle *find (const std::string &name);

  /**
   *  @brief Get the names of all registered layout objects
   */
  static void get_names (std::vector <std::string> &names);

  /**
   *  @brief Get the reference count
   */
  int get_ref_count () const
  {
    return m_ref_count;
  }

  /**
   *  @brief Add a reference to the layout handle
   * 
   *  This method will increment the reference counter of this handle
   */
  void add_ref ();

  /**
   *  @brief Remove a reference to the layout handle
   *
   *  This method will decrement the reference counter. Once the 
   *  reference count reaches zero, the layout object and the
   *  handle is deleted.
   *  Upon initialization, the reference count is zero.
   *  Hint: it is generally not safe to access the handle after
   *  a remove_ref was issued.
   */
  void remove_ref ();

  /**
   *  @brief Return true, if the layout is "dirty"
   *
   *  A layout is "dirty", if it needs to be saved. 
   *  It is set dirty if one of the signal handlers is triggered.
   */
  bool is_dirty () const
  {
    return m_dirty;
  }

  /**
   *  @brief Load the layout
   *
   *  Load the layout from the file given in the constructor using the given layer map.
   *  The dirty flag is reset.
   *
   *  @param lmap The layer map specifying the layers to read
   *  @param create True, if new layers shall be created
   *  @return The new layer map (can differ from the input since layers may be created)
   */
  db::LayerMap load (const db::LoadLayoutOptions &options);

  /**
   *  @brief Load the layout
   *
   *  Load the layout from the file given in the constructor.
   *  The dirty flag is reset.
   *
   *  @return The new layer map
   */
  db::LayerMap load ();

  /**
   *  @brief Save the layout 
   *
   *  Save the layout under the given file name and with the given options.
   *  If update is true, this method updates the cell view's filename, title, save options and dirty flag.
   */
  void save_as (const std::string &filename, tl::OutputStream::OutputStreamMode om, const db::SaveLayoutOptions &options, bool update = true);

  /**
   *  @brief Get the current saving options
   *
   *  The saving options are set by the last save_as method call.
   */
  const db::SaveLayoutOptions &save_options () const
  {
    return m_save_options;
  }

  /**
   *  @brief Get the current reader options
   *
   *  The reader options are set by the load method call.
   */
  const db::LoadLayoutOptions &load_options () const
  {
    return m_load_options;
  }

  /**
   *  @brief Add an observer for a change in the technology
   */
  void add_technology_changed_observer (tl::Observer &observer)
  {
    m_technology_changed_observed.add_observer (observer);
  }

  /**
   *  @brief Remove an observer for a change in the technology
   */
  void remove_technology_changed_observer (tl::Observer &observer)
  {
    m_technology_changed_observed.remove_observer (observer);
  }

  /**
   *  @brief Add an observer for the "apply_technology" event
   */
  void add_apply_technology_observer (tl::Observer &observer)
  {
    m_apply_technology_observer.add_observer (observer);
  }

  /**
   *  @brief Remove an observer for the "apply_technology" event
   */
  void remove_apply_technology_observer (tl::Observer &observer)
  {
    m_apply_technology_observer.remove_observer (observer);
  }

private:
  db::Layout *mp_layout;
  int m_ref_count;
  std::string m_name;
  std::string m_filename;
  std::string m_tech_name;
  meta_info m_meta_info;
  bool m_dirty;
  db::SaveLayoutOptions m_save_options;
  db::LoadLayoutOptions m_load_options;
  tl::Observed m_technology_changed_observed;
  tl::Observed m_apply_technology_observer;

  static std::map <std::string, LayoutHandle *> ms_dict;

  virtual void signal_observed ();
};

/**
 *  @brief A layout handle reference
 *
 *  This class encapsulates a reference to a layout handle.
 *  The main purpose for this class is to automate the reference
 *  counting on the handle.
 */
class KLAYOUT_DLL LayoutHandleRef 
{
public:
  LayoutHandleRef ();
  LayoutHandleRef (LayoutHandle *h);
  LayoutHandleRef (const LayoutHandleRef &r);
  ~LayoutHandleRef ();

  LayoutHandleRef &operator= (const LayoutHandleRef &r);
  
  bool operator== (const LayoutHandleRef &r) const;

  bool operator!= (const LayoutHandleRef &r) const
  {
    return !operator== (r);
  }

  LayoutHandle *operator-> () const;

  LayoutHandle *get () const;
  void set (LayoutHandle *h);

private:
  LayoutHandle *mp_handle;
};

/**
 *  @brief A "cell view" reference
 *
 *  A cell view reference points to a certain cell within a certain layout.
 *  The layout pointer can be 0, indicating that it is invalid.
 *  Also, the cell view describes a cell within that layout. The cell
 *  is addressed by an cell_index or a cell pointer. 
 *  The cell is not only identified by it's index or pointer but as well 
 *  by the path leading to that cell. This path describes how to find the
 *  cell in the context of parent cells. 
 *  The path is in fact composed in twofold: once in an unspecific fashion,
 *  just describing which parent cells are used. The target of this path
 *  is called the context cell. It is accessible by the ctx_cell_index
 *  or ctx_cell method.
 *  Additionally the path may further identify a certain instance of a certain
 *  subcell in the context cell. This is done through a set of db::InstElement
 *  objects. The target of this context path is the actual cell addressed by the
 *  cellview.
 *  In the viewer, the target cell is shown in the context of the context cell.
 *  The hierarchy levels are counted from the context cell, which is on level 0.
 *  If the context path is empty, the context cell is identical with the target cell.
 */

class KLAYOUT_DLL CellView
  : public gsi::ObjectBase
{
public:
  typedef db::cell_index_type cell_index_type;
  typedef std::vector <cell_index_type> unspecific_cell_path_type;
  typedef std::vector <db::InstElement> specific_cell_path_type;

  /**
   *  @brief Constructor: create an invalid cellview
   */
  CellView ();

  /**
   *  @brief Equality: compares the cell the cv points to, not the path
   */
  bool operator== (const CellView &cv) const;

  /**
   *  @brief Test if the cv points to a valid cell
   */
  bool is_valid () const;

  /**
   *  @brief Return the layout handle
   */
  lay::LayoutHandle *operator-> () const
  {
    return m_layout_href.get ();
  }

  /**
   *  @brief Return the layout handle (not via operator->)
   */
  lay::LayoutHandle *handle () const
  {
    return m_layout_href.get ();
  }

  /**
   *  @brief Set the unspecific part of the path explicitly
   *
   *  Setting the unspecific part will clear the context part and
   *  update the context and target cell.
   */
  void set_unspecific_path (const unspecific_cell_path_type &p);

  /**
   *  @brief Set the context part of the path explicitly
   *
   *  This method assumes that the unspecific part of the path 
   *  is established already and that the context part starts
   *  from the context cell.
   */
  void set_specific_path (const specific_cell_path_type &p);

  /**
   *  @brief Set the path to the given cell
   *
   *  This method will construct any path to this cell, not a 
   *  particular one. It will clear the context part of the path
   *  and update the context and target cell.
   */
  void set_cell (cell_index_type index);

  /**
   *  @brief Set the cell by name
   *
   *  If the name is not a valid one, the cellview will become
   *  invalid.
   *  This method will construct any path to this cell, not a 
   *  particular one. It will clear the context part of the path
   *  and update the context and target cell.
   */
  void set_cell (const std::string &name);

  /**
   *  @brief Reset the cell 
   *
   *  The cellview will become invalid. The layout object will
   *  still be attached to the cellview.
   */
  void reset_cell ();

  /**
   *  @brief Set the layout handle
   *
   *  Connect the cellview with a certain layout.
   *  This will reset the target and context cell.
   */
  void set (lay::LayoutHandle *handle);

  /**
   *  @brief Get the context cell pointer
   */
  db::Cell *ctx_cell () const
  {
    return mp_ctx_cell;
  }

  /**
   *  @brief Get the context cell index
   */
  cell_index_type ctx_cell_index () const
  {
    return m_ctx_cell_index;
  }

  /**
   *  @brief Get the target cell pointer
   */
  db::Cell *cell () const
  {
    return mp_cell;
  }

  /**
   *  @brief Get the target cell index
   */
  cell_index_type cell_index () const
  {
    return m_cell_index;
  }

  /**
   *  @brief Get the cell's combined path in an unspecific form
   */
  unspecific_cell_path_type combined_unspecific_path () const;

  /**
   *  @brief Get the cell's unspecific part of the path
   */
  const unspecific_cell_path_type &unspecific_path () const
  {
    return m_unspecific_path;
  }

  /**
   *  @brief Get the cell's unspecific part of the path
   */
  const specific_cell_path_type &specific_path () const
  {
    return m_specific_path;
  }

  /**
   *  @brief Retrive the accumulated transformation induced by the context part of the path
   */
  db::CplxTrans context_trans () const;

  /**
   *  @brief Deep copy of the cellview
   *
   *  This method performs a deep copy on the cellview.
   *  A layout must be set already. Rather the creating another reference to the layout
   *  (which is done on operator= for example), this method copies the content of the 
   *  source layout to the current one and transfers cell path and other parameters.
   *
   *  @param manager The database object manager that the new layout is put under
   */
  CellView deep_copy (db::Manager *manager) const;

private:
  lay::LayoutHandleRef m_layout_href;
  db::Cell *mp_ctx_cell;
  cell_index_type m_ctx_cell_index;
  db::Cell *mp_cell;
  cell_index_type m_cell_index;
  unspecific_cell_path_type m_unspecific_path;
  specific_cell_path_type m_specific_path;

};

}

#endif

