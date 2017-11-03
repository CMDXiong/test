
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



#ifndef HDR_dbSaveLayoutOptions
#define HDR_dbSaveLayoutOptions

#include "config.h"

#include <string>
#include <set>

#include "dbLayout.h"
#include "dbStreamLayers.h"

namespace db
{

/**
 *  @brief Base class for specific options for a certain format
 */
class KLAYOUT_DLL FormatSpecificWriterOptions
{
public:
  FormatSpecificWriterOptions () { }  
  virtual ~FormatSpecificWriterOptions () { }  //  to enable RTTI

  virtual FormatSpecificWriterOptions *clone () const = 0;
};

/**
 *  @brief Options for saving layouts
 */
class KLAYOUT_DLL SaveLayoutOptions
{
public:
  /**
   *  @brief Default constructor
   *
   *  By default, the scale factor will be 1.0, the database unit is set to 
   *  "same as original" and all layers are selected.
   */
  SaveLayoutOptions ();

  /**
   *  @brief Copy constructor
   */
  SaveLayoutOptions (const SaveLayoutOptions &d);

  /**
   *  @brief Assignment 
   */
  SaveLayoutOptions &operator= (const SaveLayoutOptions &d);

  /**
   *  @brief Destructor
   */
  ~SaveLayoutOptions ();

  /**
   *  @brief Select a format
   */
  void set_format (const std::string &format_name);

  /**
   *  @brief Get the format name 
   */
  const std::string &format () const
  {
    return m_format;
  }

  /**
   *  @brief Add a layer to be saved 
   *
   *  Add the layer with the given index to the layers to be saved.
   *  If all layers have been selected previously, this state will 
   *  be cleared. By default, the layer properties given in the
   *  layout to be saved will be used. If certain properties are 
   *  specified, these will be used.
   */
  void add_layer (unsigned int layer, const db::LayerProperties &props = db::LayerProperties ());

  /**
   *  @brief Select all layers to be saved
   *
   *  This is the default.
   */
  void select_all_layers ();

  /**
   *  @brief Deselect all layers
   */
  void deselect_all_layers ();

  /**
   *  @brief Add a cell to save (without their children)
   *
   *  Give the cell index of the cell. The actual cell is determined when the layout is given. 
   *  Clears the "all_cells" flag.
   */
  void select_this_cell (db::cell_index_type cell)
  {
    clear_cells ();
    add_this_cell (cell);
  }

  /**
   *  @brief Add a cell to save
   *
   *  Give the cell index of the cell. The actual cell is determined when the layout is given. 
   *  Clears the "all_cells" flag.
   */
  void select_cell (db::cell_index_type cell)
  {
    clear_cells ();
    add_cell (cell);
  }

  /**
   *  @brief Add a cell to save (with it's children)
   *
   *  Give the cell index of the cell. The actual cell is determined when the layout is given. 
   *  Clears the "all_cells" flag.
   */
  void add_cell (db::cell_index_type cell);

  /**
   *  @brief Add a cell to save (without it's children)
   *
   *  Give the cell index of the cell. The actual cell is determined when the layout is given. 
   *  Clears the "all_cells" flag.
   */
  void add_this_cell (db::cell_index_type cell);

  /**
   *  @brief Unselect all cells to save
   */
  void clear_cells ();

  /**
   *  @brief Select all cells to save
   */
  void select_all_cells ();

  /**
   *  @brief Set the database unit to be saved
   */
  void set_dbu (double dbu);

  /**
   *  @brief Get the database unit 
   */
  double dbu () const
  {
    return m_dbu;
  }

  /**
   *  @brief Set the scaling factor for the saving 
   */
  void set_scale_factor (double f);

  /**
   *  @brief Get the scaling factor 
   */
  double scale_factor () const
  {
    return m_scale_factor;
  }

  /**
   *  @brief Set the "don't write empty cells flag"
   */
  void set_dont_write_empty_cells (bool f);

  /**
   *  @brief Get the "don't write empty cells flag"
   */
  bool dont_write_empty_cells () const
  {
    return m_dont_write_empty_cells;
  }

  /**
   *  @brief The "keep instances" property (getter)
   *
   *  With this property set to true, instances of dropped cells (i.e. empty cells, 
   *  hidden cells) are kept if the format allows. This leads to ghost cells.
   */
  bool keep_instances () const
  {
    return m_keep_instances;
  }

  /**
   *  @brief The "keep instances" property (setter)
   *
   *  See keep_instances for a description of that property.
   */
  void set_keep_instances (bool ki) 
  {
    m_keep_instances = ki;
  }

  /**
   *  @brief The "write context information" property (getter)
   *
   *  With this property set to true, context information for cells is written to 
   *  the file in a suitable way. This way it is possible to recover PCell instantiation
   *  information and library references.
   */
  bool write_context_info () const
  {
    return m_write_context_info;
  }

  /**
   *  @brief The "write context information" property (setter)
   *
   *  See write_context_info for a description of that property.
   */
  void set_write_context_info (bool ctx_info) 
  {
    m_write_context_info = ctx_info;
  }

  /**
   *  @brief Set the format (default) from the file name
   *
   *  Returns true, if the suffix indicates a known format.
   */
  bool set_format_from_filename (const std::string &fn);

  /**
   *  @brief Set format specific options
   *
   *  In this version, the ownership over this object is not transferred to the SaveLayoutOptions object.
   */
  void set_specific_options (const FormatSpecificWriterOptions &options);

  /**
   *  @brief Set format specific options
   *
   *  The ownership over this object is transferred to the SaveLayoutOptions object.
   */
  void set_specific_options (const FormatSpecificWriterOptions *options);

  /**
   *  @brief Get the format specific option object
   *
   *  @return 0, if there is no such object attached
   */
  const FormatSpecificWriterOptions *get_specific_options () const;

  /**
   *  @brief See get_valid_layers
   */
  enum LayerAssignmentMode
  {
    LP_OnlyNumbered = 0,
    LP_OnlyNamed = 1,
    LP_AssignName = 2,
    LP_AssignNumber = 3
  };

  /**
   *  @brief Report all valid layers 
   *
   *  Given the layout, report all pairs or layer indices and valid properties.
   *  The lm mode specifies how to create layer properties for "halfway defined" layers - 
   *    - LP_OnlyNamed will only select named ones
   *    - LP_OnlyNumbered will select only numbered ones
   *    - LP_AssignName will assign a name when no name is given
   *    - LP_AssignNumber will assign numbers when no number is given 
   */
  void get_valid_layers (const db::Layout &layout, std::vector <std::pair <unsigned int, db::LayerProperties> > &valid_layers, LayerAssignmentMode lm) const;

  /**
   *  @brief Report all cells to write
   *
   *  It must be given a list of valid layers which is used to determine empty cells if dont_save_empty_cells is true.
   */
  void get_cells (const db::Layout &layout, std::set <db::cell_index_type> &cells, const std::vector <std::pair <unsigned int, db::LayerProperties> > &valid_layers) const;

private:
  std::string m_format;
  std::map<unsigned int, db::LayerProperties> m_layers;
  std::set<db::cell_index_type> m_cells;
  std::set<db::cell_index_type> m_implied_childred;
  bool m_all_layers;
  bool m_all_cells;
  double m_dbu;
  double m_scale_factor;
  const FormatSpecificWriterOptions *mp_options;
  bool m_keep_instances;
  bool m_write_context_info;
  bool m_dont_write_empty_cells;
};

}

#endif

