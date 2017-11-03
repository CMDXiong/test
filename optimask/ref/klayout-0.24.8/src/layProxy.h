
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



#ifndef HDR_layProxy
#define HDR_layProxy

template <class Observer, class Proxy>
struct ProxyNode 
{
  size_t count;
  ProxyList <Observer, Proxy>::iterator pos;
};


template <class Observer, class Proxy, class Node = ProxyNode <Observer, Proxy> >
struct ProxyList 
{
  friend class ProxyBase <Observer, Proxy>; 

  typedef std::list <ProxyNode <Observer, Proxy> > List;
  typedef List::iterator iterator;
  typedef List::const_iterator const_iterator;

  Proxy 
  issue (Observer &observer)
  {
    m_nodes.push_front (Node ());
    m_nodes.front ().pos = m_nodes.begin ();

    Proxy p;
    p.init_base (observer, m_nodes.front (), *this);
    return p;
  }

private:
  List m_nodes;
  size_t m_num_nodes;

  size_t 
  remove (Node &p)
  {
    m_nodes.erase (p.pos);
    //  p has deleted through this!
    return m_nodes.size ();
  }
};


template <class Observer, class Proxy, class Node = ProxyNode <Observer, Proxy> >
struct ProxyBase
{
  friend class ProxyList <Observer, Proxy>;

  ProxyBase ()
    : mp_observer (0), mp_node (0), mp_list (0)
  { }

  void 
  init_base (Observer &o, Node &n, ProxyList <Observer, Proxy> &l)
  {
    cleanup ();
    mp_observer = &o;
    mp_node = &n;
    mp_list = &l;
    ++mp_node->count;
  }
  
  ProxyBase (const ProxyBase &d) 
    : mp_observer (d.mp_observer), mp_node (d.mp_node), mp_list (d.mp_list)
  {
    ++mp_node->count;
  }

  ProxyBase &
  operator= (const ProxyBase &d) 
  {
    cleanup ();
    mp_observer = d.mp_observer;
    mp_node = d.mp_node;
    mp_list = d.mp_list;
    ++mp_node->count;
    return *this;
  }

  ~ProxyBase ();
  {
    cleanup ();
  }

  void 
  cleanup ()
  {
    if (mp_node && --mp_node->count == 0) {
      mp_observer->operator() (*this, mp_list->remove (*mp_node));
    }

    mp_node = 0;
    mp_observer = 0;
    mp_list = 0;
  }

private:
  Observer *mp_observer;
  Node *mp_node;
  ProxyList <Object, Proxy> *mp_list;

  void cleanup ();
};

#endif

