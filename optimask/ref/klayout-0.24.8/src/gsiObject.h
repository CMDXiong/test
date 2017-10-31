
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


#ifndef _HDR_gsiObject
#define _HDR_gsiObject

#include <memory>

//  For a comprehensive documentation see gsi.h

namespace gsi
{

/**
 *  @brief The number of gsi clients (bindings)
 *
 *  Every client has a slot number which must be between 0 and num_clients - 1.
 */
const int num_clients = 2;

template <class X> class Class;
class ClassBase;
class ObjectBase;

/**
 *  @brief The handler for the client-side data of Object<X>
 *
 *  If the scripting client desires to attach data to the Object<X> objects, it
 *  must reimplement this handler class and register a handler for each ClassBase
 *  object that is registered.
 */
class ClientHandler
{
public:
  /**
   *  @brief Constructor
   */
  ClientHandler () { }

  /**
   *  @brief Destructor
   */
  virtual ~ClientHandler () { }

  /**
   *  @brief Attach the handler to an object's client data
   */
  virtual void attach (void *&) = 0;

  /**
   *  @brief Detach the handler from an object's client data
   */
  virtual void detach (void *&) = 0;

  /* 
   *  @brief Transfer ownership from script to C++
   *  This method is called by the C++ side to indicate that it wants to keep the object 
   *  and transfer ownership from the script client to a C++ object.
   */
  virtual void keep (void *&) = 0;

  /**
   *  @brief Transfer ownership from C++ to the script
   *  This method is called by the C++ side to indicate that it no longer wants to keep 
   *  the object and transfer ownership from C++ back to the script client.
   */
  virtual void release (void *&) = 0;
};

//  TODO: currently the client data is not copied. As a consequence, copying the C++
//  objects does not copy the Ruby part of the object but rather leaves it unitialized.
//  Since however, the client-side data feature is supposed to be used to non-copying 
//  objects mainly, this problem is not considered serious for now.

/**
 *  @brief A base class for objects that are connected to their script counterpart
 */
class KLAYOUT_DLL ObjectBase
{
public:
  /**
   *  @brief Constructor
   */
  ObjectBase () 
  {
    for (int i = 0; i < num_clients; ++i) {
      mp_data [i] = 0;
    }
    attach ();
  }

  /**
   *  @brief Copy constructor
   */
  ObjectBase (const ObjectBase &) 
  {
    for (int i = 0; i < num_clients; ++i) {
      mp_data [i] = 0;
    }
    attach ();
  }

  /**
   *  @brief Assignment
   */
  ObjectBase &operator= (const ObjectBase &)
  {
    return *this;
  }

  /**
   *  @brief Destructor
   */
  virtual ~ObjectBase ()
  {
    detach ();
  }

  /**
   *  @brief Client-specific data
   *  The client-specific data can be used by scripting clients
   *  to store specific information. "ch" is the client index - every script
   *  binding uses it's specific index.
   *  The data is set and reset by the "client handler" which is registered
   *  by the script binding implementation in the system. The client handler
   *  has method to attach and detach data from or to objects derived from 
   *  gsi::ObjectBase.
   */
  void *data (int ch) const
  {
    return mp_data [ch];
  }

  /**
   *  @brief Client-specific data in non-const context
   */
  void *&data (int ch)
  {
    return mp_data [ch];
  }

  /**
   *  @brief Gets the client handler for a given client index
   */
  static ClientHandler *client_handler (int ch) 
  {
    return ms_client_handler [ch].get();
  }

  /**
   *  @brief Registers a client handler with a given client index
   */
  static void register_client_handler (ClientHandler *handler, int ch) 
  {
    ms_client_handler [ch].reset (handler);
  }

  /**
   *  @brief Attach this object to all client handlers
   */
  void attach () 
  {
    for (int i = 0; i < num_clients; ++i) {
      if (ms_client_handler [i].get()) {
        ms_client_handler [i]->attach (data (i));
      }
    }
  }

  /**
   *  @brief Detach this object from all client handlers
   */
  void detach () 
  {
    for (int i = 0; i < num_clients; ++i) {
      if (ms_client_handler [i].get() && mp_data [i]) {
        ms_client_handler [i]->detach (data (i));
      }
    }
  }

  /**
   *  @brief Transfers ownership of this object from C++ to the client who has a reference to this object
   */
  void keep () 
  {
    for (int i = 0; i < num_clients; ++i) {
      if (ms_client_handler [i].get()) {
        ms_client_handler [i]->keep (data (i));
      }
    }
  }

  /**
   *  @brief Transfers ownership of this object from the clients back to C++
   */
  void release () 
  {
    for (int i = 0; i < num_clients; ++i) {
      if (ms_client_handler [i].get()) {
        ms_client_handler [i]->release (data (i));
      }
    }
  }

private:
  static std::auto_ptr<ClientHandler> ms_client_handler[num_clients];
  void *mp_data [num_clients];
};

}

#endif

