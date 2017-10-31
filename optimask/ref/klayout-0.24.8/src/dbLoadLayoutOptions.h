
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


#ifndef HDR_dbLoadLayoutOptions
#define HDR_dbLoadLayoutOptions

#include <string>
#include <map>

#include "config.h"

#include "dbLayout.h"
#include "dbStreamLayers.h"

namespace db
{

/**
 *  @brief Base class for specific options for a certain format
 */
class KLAYOUT_DLL FormatSpecificReaderOptions
{
public:
  FormatSpecificReaderOptions () { }  
  virtual ~FormatSpecificReaderOptions () { }  //  to enable RTTI

  virtual FormatSpecificReaderOptions *clone () const = 0;
};

/**
 *  @brief Options for loading layouts
 */
class KLAYOUT_DLL LoadLayoutOptions
{
public:
  /**
   *  @brief Default constructor
   */
  LoadLayoutOptions ();

  /**
   *  @brief Copy constructor
   */
  LoadLayoutOptions (const LoadLayoutOptions &d);

  /**
   *  @brief Assignment 
   */
  LoadLayoutOptions &operator= (const LoadLayoutOptions &d);

  /**
   *  @brief Destructor
   */
  ~LoadLayoutOptions ();

  /**
   *  @brief Select a format
   *
   *  By default, automatic determination of format selection is enabled.
   *  If a certain format is specified, the reader for that format is used and
   *  no automatic detection is performed.
   */
  void set_format (const std::string &format_name)
  {
    m_format = format_name;
  }

  /**
   *  @brief Set the format from the file name by checking the extension
   *
   *  Returns true, if the suffix indicates a known format.
   */
  bool set_format_from_filename (const std::string &fn);

  /**
   *  @brief Get the format selected currently (empty string for no specific format)
   */
  const std::string format ()
  {
    return m_format;
  }

  /**
   *  @brief Specify a layer mapping
   *
   *  If a layer mapping is specified, only the given layers are read.
   *  Otherwise, all layers are read.
   *  Setting "create_other_layers" to true will make the reader
   *  create other layers for all layers not given in the layer map.
   *  Setting an empty layer map and create_other_layers to true effectively
   *  enables all layers for reading.
   *
   *  @param map See above.
   *  @param create_other_layers See above.
   */
  void set_layer_map (const db::LayerMap &map, bool create_other_layers)
  {
    m_layer_map = map;
    m_create_other_layers = create_other_layers;
  }

  /**
   *  @brief Select all layers
   *
   *  This disables any layer map and enables reading of all layers where
   *  new layers will be created.
   */
  void select_all_layers ()
  {
    set_layer_map (db::LayerMap (), true);
  }

  /**
   *  @brief Get the layer map
   *
   *  The layer map delivered does not have valid layer indices set.
   *  It is supposed to act rather as a selection (and potentially multiple layer compression) set.
   */
  const db::LayerMap &layer_map () const
  {
    return m_layer_map;
  }

  /**
   *  @brief Get the layer map (non-const version)
   *
   *  The layer map delivered does not have valid layer indices set.
   *  It is supposed to act rather as a selection (and potentially multiple layer compression) set.
   */
  db::LayerMap &layer_map () 
  {
    return m_layer_map;
  }

  /**
   *  @brief Set the flag which tells if new layers should be created
   */
  void create_other_layers (bool f) 
  {
    m_create_other_layers = f;
  }

  /**
   *  @brief Get the flag which tells if new layers should be created
   */
  bool create_other_layers () const
  {
    return m_create_other_layers;
  }

  /**
   *  @brief Select text objects for reading
   *
   *  This flag is meaningful only for formats supporting text objects.
   */
  void enable_text_objects (bool enable)
  {
    m_enable_text_objects = enable;
  }

  /**
   *  @brief Get the flag which tells if text objects should be enabled for reading
   */
  bool enable_text_objects () const
  {
    return m_enable_text_objects;
  }

  /**
   *  @brief Select properties for reading
   *
   *  This flag is meaningful only for formats supporting properties.
   */
  void enable_properties (bool enable)
  {
    m_enable_properties = enable;
  }

  /**
   *  @brief Get the flag which tells if properties should be enabled for reading
   */
  bool enable_properties () const
  {
    return m_enable_properties;
  }

  /**
   *  @brief Set format specific options for a given format
   *
   *  In this version, the ownership over the options object is not transferred to the LoadLayoutOptions object.
   *
   *  @param options The options to use for reading the file
   *  @param format The format name for which to use these options
   */
  void set_specific_options (const FormatSpecificReaderOptions &options, const std::string &format);

  /**
   *  @brief Set format specific options for a given format
   *
   *  The ownership over the options object is transferred to the LoadLayoutOptions object.
   *
   *  @param options The options to use for reading the file
   *  @param format The format name for which to use these options
   */
  void set_specific_options (const FormatSpecificReaderOptions *options, const std::string &format);

  /**
   *  @brief Get the format specific option object for the given format
   *
   *  @return 0, if there is no such object attached
   */
  const FormatSpecificReaderOptions *get_specific_options (const std::string &format) const;

private:
  std::string m_format;
  db::LayerMap m_layer_map;
  bool m_create_other_layers;
  bool m_enable_text_objects;
  bool m_enable_properties;
  std::map <std::string, const FormatSpecificReaderOptions *> m_options;

  void release ();
};

}

#endif


