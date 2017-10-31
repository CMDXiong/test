
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


#ifndef HDR_layStream_h
#define HDR_layStream_h

#include "config.h"

#include "layPlugin.h"

namespace db
{
  class StreamFormatDeclaration;
  class FormatSpecificWriterOptions;
  class FormatSpecificReaderOptions;
  class LoadLayoutOptions;
  class SaveLayoutOptions;
}

namespace lay
{

class PluginRoot;
class LayoutHandle;

/**
 *  @brief The base class for writer configuration pages
 *
 *  This interface defines some services the configuration page
 *  must provide (i.e. setup, commit)
 */
class KLAYOUT_DLL StreamWriterOptionsPage 
  : public QFrame
{
public:
  StreamWriterOptionsPage (QWidget *parent) 
    : QFrame (parent)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Load the page
   *
   *  The implementation is supposed to fetch the configuration from the
   *  Plugin object provided and load the widgets accordingly.
   *  The options object can be cast to the specific format object.
   */
  virtual void setup (const db::FormatSpecificWriterOptions *options)
  {
    //  the default implementation does nothing.
  }

  /**
   *  @brief Commit the page
   *
   *  The implementation is supposed to read the configuration (and 
   *  throw exceptions if the configuration something is invalid)
   *  and commit the changes through 
   *  The options object can be cast to the specific format object.
   */
  virtual void commit (db::FormatSpecificWriterOptions *options, bool gzip)
  {
    //  the default implementation does nothing.
  }
};

/**
 *  @brief The base class for reader configuration pages
 *
 *  This interface defines some services the configuration page
 *  must provide (i.e. setup, commit)
 */
class KLAYOUT_DLL StreamReaderOptionsPage 
  : public QFrame
{
public:
  StreamReaderOptionsPage (QWidget *parent) 
    : QFrame (parent)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Load the page
   *
   *  The implementation is supposed to fetch the configuration from the
   *  Plugin object provided and load the widgets accordingly.
   *  The options object can be cast to the specific format object.
   */
  virtual void setup (const db::FormatSpecificReaderOptions *options)
  {
    //  the default implementation does nothing.
  }

  /**
   *  @brief Commit the page
   *
   *  The implementation is supposed to read the configuration (and 
   *  throw exceptions if the configuration something is invalid)
   *  and commit the changes through 
   *  The options object can be cast to the specific format object.
   */
  virtual void commit (db::FormatSpecificReaderOptions *options)
  {
    //  the default implementation does nothing.
  }
};

/**
 *  This plugin specializations add the stream readers and writers to the configuration
 *  system. The plugins can provide menu entries, configuration parameters, configuration
 *  pages etc.
 */  
  
class KLAYOUT_DLL StreamPluginDeclarationBase
  : public PluginDeclaration
{
public:
  StreamPluginDeclarationBase (const char *format_name)
    : PluginDeclaration (), m_format_name (format_name), mp_stream_fmt (0)
  { 
    //  .. nothing yet ..
  }

  db::StreamFormatDeclaration &stream_fmt ();

  const db::StreamFormatDeclaration &stream_fmt () const
  {
    //  dirty hack:
    return const_cast <StreamPluginDeclarationBase *> (this)->stream_fmt ();
  }

  const std::string &format_name () const
  {
    return m_format_name;
  }

private:
  std::string m_format_name;
  db::StreamFormatDeclaration *mp_stream_fmt;

  //  don't allow to override - use a special configuration page for that purpose.
  virtual ConfigPage *config_page (QWidget *parent, std::string &title) const
  {
    return 0;
  }
};
  
/**
 *  @brief A specialisation of Plugin declaration for stream reader plugins
 */
class KLAYOUT_DLL StreamReaderPluginDeclaration
  : public StreamPluginDeclarationBase
{
public:
  StreamReaderPluginDeclaration (const char *format_name)
    : StreamPluginDeclarationBase (format_name)
  { 
    //  .. nothing yet ..
  }

  /**
   *  @brief Create a format specific options page 
   */
  virtual StreamReaderOptionsPage *format_specific_options_page (QWidget *parent) const 
  {
    return 0;
  }

  /**
   *  @brief Create a format specific options object from the configuration
   *
   *  This method is supposed to create a format specific options object and fill it from the current configuration
   *  which is accessed through the configuration root object given.
   *  config_root may be 0 to indicate that a default options object should be created without being filled.
   */
  virtual db::FormatSpecificReaderOptions *create_specific_options (lay::PluginRoot *config_root) const 
  {
    return 0;
  }

  /**
   *  @brief Commit the settings from the given format specific options object to the configuration
   *
   *  This method is supposed to take the options and write it to the configuration addressed by the given
   *  configuration root object.
   *  The options object can be cast to the actual implementation class for the given format.
   *  Any reimplementation only needs to calls config_root->config_set. The configuration change is finalized by the 
   *  caller.
   */
  virtual void commit_to_config (const db::FormatSpecificReaderOptions *options, lay::PluginRoot *config_root) const 
  {
    //  the default implementation does nothing.
  }
};

/**
 *  @brief A specialisation of Plugin declaration for stream reader plugins
 */
class KLAYOUT_DLL StreamWriterPluginDeclaration
  : public StreamPluginDeclarationBase
{
public:
  StreamWriterPluginDeclaration (const char *format_name)
    : StreamPluginDeclarationBase (format_name)
  { 
    //  .. nothing yet ..
  }

  /**
   *  @brief Create a format specific options page 
   */
  virtual StreamWriterOptionsPage *format_specific_options_page (QWidget *parent) const 
  {
    return 0;
  }

  /**
   *  @brief Create a format specific options object from the configuration
   *
   *  This method is supposed to create a format specific options object and fill it from the current configuration
   *  which is accessed through the configuration root object given.
   *  config_root may be 0 to indicate that a default options object should be created without being filled.
   */
  virtual db::FormatSpecificWriterOptions *create_specific_options (lay::PluginRoot *config_root) const 
  {
    return 0;
  }

  /**
   *  @brief Commit the settings from the given format specific options object to the configuration
   *
   *  This method is supposed to take the options and write it to the configuration addressed by the given
   *  configuration root object.
   *  The options object can be cast to the actual implementation class for the given format.
   *  Any reimplementation only needs to calls config_root->config_set. The configuration change is finalized by the 
   *  caller.
   */
  virtual void commit_to_config (const db::FormatSpecificWriterOptions *options, lay::PluginRoot *config_root) const 
  {
    //  the default implementation does nothing.
  }

  /**
   *  @brief Initialize the writer options from a layout handle
   *
   *  The layout handle carries information about meta data read and similar. This
   *  method gives the plugin a chance to modify the options based on the meta data
   *  of the layout.
   */
  virtual void initialize_options_from_layout_handle (db::FormatSpecificWriterOptions *options, const lay::LayoutHandle &lh) const
  {
    //  the default implementation does nothing.
  }
};

/**
 *  @brief Write a reader options object to the configuration
 */
KLAYOUT_DLL void commit_to_config (const db::LoadLayoutOptions &options, lay::PluginRoot *config_root);

/**
 *  @brief Initialize a reader options object from the configuration
 */
KLAYOUT_DLL void get_from_config (db::LoadLayoutOptions &options, lay::PluginRoot *config_root);

/**
 *  @brief Write a writer options object to the configuration
 */
KLAYOUT_DLL void commit_to_config (const db::SaveLayoutOptions &options, lay::PluginRoot *config_root);

/**
 *  @brief Initialize a writer options object from the configuration
 */
KLAYOUT_DLL void get_from_config (db::SaveLayoutOptions &options, lay::PluginRoot *config_root);

}

#endif

