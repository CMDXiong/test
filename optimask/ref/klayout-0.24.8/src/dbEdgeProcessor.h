
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



#ifndef HDR_dbEdgeProcessor
#define HDR_dbEdgeProcessor

#include "config.h"

#include "dbTypes.h"
#include "dbEdge.h"
#include "dbShape.h"
#include "dbShapes.h"

#include <vector>

namespace db
{

struct WorkEdge;
struct CutPoints;
class EdgeSink;
class PolygonSink;
class SimplePolygonSink;
class PGPolyContour;
class PGContourList;
struct PGPoint;
class Layout;
class Cell;

/**
 *  @brief A destination for a (sorted) set of edges
 *
 *  This receiver can be used as destination for the edge processor.
 *  It will receive edge events in the scanline order, this is bottom to 
 *  top and left to right. Edges will be non-intersecting.
 *
 *  This is the base class for such edge receivers.
 */
class KLAYOUT_DLL EdgeSink 
{
public:
  /** 
   *  @brief Constructor
   */
  EdgeSink () { }

  /** 
   *  @brief Destructor
   */
  virtual ~EdgeSink () { };

  /**
   *  @brief Start event
   *
   *  This method is called shortly before the first edge is delivered.
   *  Specifically, all edges are already cached before this method is called.
   *  Thus, inside an implementation of this method, the original source can be
   *  discarded.
   */
  virtual void start () { }

  /**
   *  @brief End event
   *
   *  This method is called after the last edge has been delivered.
   */
  virtual void flush () { }

  /**
   *  @brief Deliver an edge
   *
   *  This method delivers an edge that ends or starts at the current scanline.
   */
  virtual void put (const db::Edge &e) { }

  /**
   *  @brief Deliver an edge that crosses the scanline
   *
   *  This method is called to deliver an edge that is not starting or ending at the
   *  current scanline.
   *  Another delivery of a set of crossing edges may happen through the skip_n 
   *  method which delivers a set of (unspecified) edges which are guaranteed to form
   *  a closed sequence, that is one which is starting and ending at a wrap count of 0.
   */
  virtual void crossing_edge (const db::Edge &e) { }

  /**
   *  @brief Deliver an edge set forming a closed sequence
   *
   *  See description of "crossing_egde" for details.
   */
  virtual void skip_n (size_t n) { }

  /**
   *  @brief Signal the start of a scanline at the given y coordinate
   */
  virtual void begin_scanline (db::Coord y) { }

  /**
   *  @brief Signal the end of a scanline at the given y coordinate
   */
  virtual void end_scanline (db::Coord y) { }
};

/**
 *  @brief A edge container that can be used as a receiver for edges
 *
 *  This class reimplements the EdgeSink interface.
 *  This receiver simply collects the edges in a container (a vector of edges)
 *  which is either kept internally or supplied from the outside.
 */
class KLAYOUT_DLL EdgeContainer 
  : public EdgeSink
{
public:
  /**
   *  @brief Constructor connecting this receiver to an external edge vector
   */
  EdgeContainer (std::vector<db::Edge> &edges, bool clear = false) 
    : EdgeSink (), mp_edges (&edges), m_clear (clear) 
  { }

  /**
   *  @brief Constructor using an internal edge vector
   */
  EdgeContainer ()
    : EdgeSink (), mp_edges (&m_edges), m_clear (false)
  { }

  /**
   *  @brief Get the edges collected so far (const version)
   */
  const std::vector<db::Edge> &edges () const 
  {
    return *mp_edges;
  }

  /**
   *  @brief Get the edges collected so far (non-const version)
   */
  std::vector<db::Edge> &edges () 
  {
    return *mp_edges;
  }

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void start () 
  {
    if (m_clear) {
      mp_edges->clear ();
      //  The single-shot scheme is a easy way to overcome problems with multiple start/flush brackets (i.e. on size filter)
      m_clear = false;
    }
  }

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void put (const db::Edge &e) 
  {
    mp_edges->push_back (e);
  }

private:
  std::vector<db::Edge> m_edges;
  std::vector<db::Edge> *mp_edges;
  bool m_clear;
};

/**
 *  @brief Form polygons from a edge set
 *
 *  This class implements EdgeSink. It builds polygons from the edges delivered to it 
 *  and outputs the polygons to another receiver (PolygonSink). 
 *  The way how touching corners are resolved can be specified (minimum and maximum coherence).
 *  In addition, it can be specified if the resulting polygons contain holes are whether the 
 *  holes are attached to the hull contour by stich lines.
 */
class KLAYOUT_DLL PolygonGenerator
  : public EdgeSink
{
public:
  typedef std::list <PGPoint> open_map_type;
  typedef open_map_type::iterator open_map_iterator_type;

  /**
   *  @brief Constructor
   *
   *  This constructor takes the polygon receiver (from which is keeps a reference).
   *  It allows to specify how holes are resolved and how touching corners are resolved.
   *
   *  @param psink The polygon receiver
   *  @param resolve_holes true, if holes should be resolved into the hull using stich lines
   *  @param min_coherence true, if the resulting polygons should be minimized (less holes, more polygons)
   */
  PolygonGenerator (PolygonSink &psink, bool resolve_holes = true, bool min_coherence = true); 

  /**
   *  @brief Constructor
   *
   *  This constructor takes the simple polygon receiver (from which is keeps a reference).
   *  It allows to specify how touching corners are resolved. Holes are always resolved since this is
   *  the only way to create a simple polygon.
   *
   *  @param spsink The simple polygon receiver
   *  @param resolve_holes true, if holes should be resolved into the hull using stich lines
   *  @param min_coherence true, if the resulting polygons should be minimized (less holes, more polygons)
   */
  PolygonGenerator (SimplePolygonSink &spsink, bool min_coherence = true); 

  /**
   *  @brief Destructor
   */
  ~PolygonGenerator ();

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void start ();

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void flush ();

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void begin_scanline (db::Coord y);

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void end_scanline (db::Coord y);

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void crossing_edge (const db::Edge &e);

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void skip_n (size_t n);

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void put (const db::Edge &e);

  /**
   *  @brief Set the way how holes are resolved dynamically
   *
   *  This property should not be changed why polygons are created (between start and flush)
   */
  void resolve_holes (bool f) { m_resolve_holes = f; }

  /**
   *  @brief Set the way how touching corners are resolved dynamically
   *
   *  This property should not be changed why polygons are created (between start and flush)
   */
  void min_coherence (bool f) { m_min_coherence = f; }

  /**
   *  @brief Disable or enable compression for polygon contours
   *
   *  If compression is disabled, no vertices will be dropped.
   */
  void enable_compression (bool enable) { m_compress = enable; }

  /**
   *  @brief Disable or enable compression for polygon contours
   *
   *  This method switches the global flag and is intended for regression test purposes only!
   */
  static void enable_compression_global (bool enable) { ms_compress = enable; }

private:
  PGContourList *mp_contours;
  open_map_type m_open;
  db::Coord m_y;
  open_map_iterator_type m_open_pos;
  PolygonSink *mp_psink;
  SimplePolygonSink *mp_spsink;
  bool m_resolve_holes;
  bool m_min_coherence;
  db::Polygon m_poly;
  db::SimplePolygon m_spoly;
  static bool ms_compress;
  bool m_compress;

  void join_contours (db::Coord x);
  void produce_poly (const PGPolyContour &c);

  PolygonGenerator &operator= (const PolygonGenerator &);
  PolygonGenerator (const PolygonGenerator &);
};

/**
 *  @brief Declaration of the simple polygon sink interface
 */
class KLAYOUT_DLL SimplePolygonSink
{
public:
  /**
   *  @brief Constructor
   */
  SimplePolygonSink () { }

  /**
   *  @brief Destructor
   */
  virtual ~SimplePolygonSink () { }

  /**
   *  @brief Deliver a simple polygon
   *
   *  This method is called to deliver a new polygon
   */
  virtual void put (const db::SimplePolygon &polygon) { }

  /**
   *  @brief Start event
   *
   *  This method is called before the first polygon is delivered. 
   *  The PolygonGenerator will simply forward the EdgeSink's start method to the
   *  polygon sink.
   */
  virtual void start () { }

  /**
   *  @brief End event
   *
   *  This method is called after the last polygon was delivered. 
   *  The PolygonGenerator will deliver all remaining polygons and call flush then.
   */
  virtual void flush () { }
};

/**
 *  @brief A simple polygon receiver collecting the simple polygons in a vector
 *
 *  This class implements the SimplePolygonSink interface.
 *  Like EdgeContainer, this receiver collects the objects either in an external
 *  or an internal vector of polygons.
 */
class KLAYOUT_DLL SimplePolygonContainer
  : public SimplePolygonSink
{
public:
  /**
   *  @brief Constructor specifying an external vector for storing the polygons
   */
  SimplePolygonContainer (std::vector<db::SimplePolygon> &polygons, bool clear = false) 
    : SimplePolygonSink (), mp_polygons (&polygons), m_clear (clear) 
  { }

  /**
   *  @brief Constructor which tells the container to use the internal vector for storing the polygons
   */
  SimplePolygonContainer () 
    : SimplePolygonSink (), mp_polygons (&m_polygons), m_clear (false) 
  { }

  /**
   *  @brief Start the sequence
   */
  virtual void start ()
  {
    if (m_clear) {
      mp_polygons->clear ();
      //  The single-shot scheme is a easy way to overcome problems with multiple start/flush brackets (i.e. on size filter)
      m_clear = false;
    }
  }

  /**
   *  @brief The polygons collected so far (const version)
   */
  const std::vector<db::SimplePolygon> &polygons () const
  { 
    return *mp_polygons; 
  }

  /**
   *  @brief The polygons collected so far (non-const version)
   */
  std::vector<db::SimplePolygon> &polygons () 
  { 
    return *mp_polygons; 
  }

  /**
   *  @brief Implementation of the PolygonSink interface
   */
  virtual void put (const db::SimplePolygon &polygon) 
  {
    mp_polygons->push_back (polygon);
  }

private:
  std::vector<db::SimplePolygon> m_polygons;
  std::vector<db::SimplePolygon> *mp_polygons;
  bool m_clear;
};

/**
 *  @brief Declaration of the polygon sink interface
 */
class KLAYOUT_DLL PolygonSink
{
public:
  /**
   *  @brief Constructor
   */
  PolygonSink () { }

  /**
   *  @brief Destructor
   */
  virtual ~PolygonSink () { }

  /**
   *  @brief Deliver a polygons
   *
   *  This method is called to deliver a new polygon
   */
  virtual void put (const db::Polygon &polygon) { }

  /**
   *  @brief Start event
   *
   *  This method is called before the first polygon is delivered. 
   *  The PolygonGenerator will simply forward the EdgeSink's start method to the
   *  polygon sink.
   */
  virtual void start () { }

  /**
   *  @brief End event
   *
   *  This method is called after the last polygon was delivered. 
   *  The PolygonGenerator will deliver all remaining polygons and call flush then.
   */
  virtual void flush () { }
};

/**
 *  @brief A polygon receiver collecting the polygons in a vector
 *
 *  This class implements the PolygonSink interface.
 *  Like EdgeContainer, this receiver collects the objects either in an external
 *  or an internal vector of polygons.
 */
class KLAYOUT_DLL PolygonContainer
  : public PolygonSink
{
public:
  /**
   *  @brief Constructor specifying an external vector for storing the polygons
   */
  PolygonContainer (std::vector<db::Polygon> &polygons, bool clear = false) 
    : PolygonSink (), mp_polygons (&polygons), m_clear (clear)
  { }

  /**
   *  @brief Constructor which tells the container to use the internal vector for storing the polygons
   */
  PolygonContainer () 
    : PolygonSink (), mp_polygons (&m_polygons), m_clear (false) 
  { }

  /**
   *  @brief The polygons collected so far (const version)
   */
  const std::vector<db::Polygon> &polygons () const
  { 
    return *mp_polygons; 
  }

  /**
   *  @brief The polygons collected so far (non-const version)
   */
  std::vector<db::Polygon> &polygons () 
  { 
    return *mp_polygons; 
  }

  /**
   *  @brief Start the sequence
   */
  virtual void start ()
  {
    if (m_clear) {
      mp_polygons->clear ();
      //  The single-shot scheme is a easy way to overcome problems with multiple start/flush brackets (i.e. on size filter)
      m_clear = false;
    }
  }

  /**
   *  @brief Implementation of the PolygonSink interface
   */
  virtual void put (const db::Polygon &polygon) 
  {
    mp_polygons->push_back (polygon);
  }

private:
  std::vector<db::Polygon> m_polygons;
  std::vector<db::Polygon> *mp_polygons;
  bool m_clear;
};

/**
 *  @brief A polygon receiver creating shapes from the polygons inside a db::Shapes container
 *
 *  This class implements the PolygonSink interface.
 */
class KLAYOUT_DLL ShapeGenerator
  : public PolygonSink
{
public:
  /**
   *  @brief Constructor associating the receiver with a shapes container
   *
   *  See the description of the EdgeSink and PolygonSink interface for a explanation when
   *  the start method is called and when the shape container is cleared if "clear_shapes"
   *  is set.
   *
   *  @param clear_shapes If true, the shapes container is cleared on the start event.
   */
  ShapeGenerator (db::Shapes &shapes, bool clear_shapes = false)
    : PolygonSink (), mp_shapes (&shapes), m_clear_shapes (clear_shapes)
  { }

  /**
   *  @brief Implementation of the PolygonSink interface
   */
  virtual void put (const db::Polygon &polygon) 
  {
    mp_shapes->insert (polygon);
  }

  /**
   *  @brief Implementation of the PolygonSink interface
   */
  virtual void start () 
  { 
    if (m_clear_shapes) {
      mp_shapes->clear ();
      //  The single-shot scheme is a easy way to overcome problems with multiple start/flush brackets (i.e. on size filter)
      m_clear_shapes = false;
    }
  }

private:
  db::Shapes *mp_shapes;
  bool m_clear_shapes;
};

/**
 *  @brief An edge receiver creating shapes from the edges inside a db::Shapes container
 *
 *  This class implements the EdgeSink interface.
 */
class KLAYOUT_DLL EdgeShapeGenerator
  : public EdgeSink
{
public:
  /**
   *  @brief Constructor associating the receiver with a shapes container
   *
   *  See the description of the EdgeSink and PolygonSink interface for a explanation when
   *  the start method is called and when the shape container is cleared if "clear_shapes"
   *  is set.
   *
   *  @param clear_shapes If true, the shapes container is cleared on the start event.
   */
  EdgeShapeGenerator (db::Shapes &shapes, bool clear_shapes = false)
    : EdgeSink (), mp_shapes (&shapes), m_clear_shapes (clear_shapes)
  { }

  /**
   *  @brief Implementation of the EdgeSink interface
   */
  virtual void put (const db::Edge &edge) 
  {
    mp_shapes->insert (edge);
  }

  /**
   *  @brief Implementation of the PolygonSink interface
   */
  virtual void start () 
  { 
    if (m_clear_shapes) {
      mp_shapes->clear ();
      //  The single-shot scheme is a easy way to overcome problems with multiple start/flush brackets (i.e. on size filter)
      m_clear_shapes = false;
    }
  }

private:
  db::Shapes *mp_shapes;
  bool m_clear_shapes;
};

/**
 *  @brief The edge set operator base class
 *
 *  This class is an internal class that specifies how the output is formed from 
 *  a set of intersecting-free input edges. 
 *  Basically, this object receives events for the edges along the scan line.
 *  At the beginning of the scan line, the "reset" method is called to bring the
 *  evaluator into a defined state. Each edge has an integer property that can be
 *  used to distinguish edges from different polygons or layers.
 */
class KLAYOUT_DLL EdgeEvaluatorBase
{
public:
  typedef size_t property_type;

  EdgeEvaluatorBase () { }
  virtual ~EdgeEvaluatorBase () { }

  virtual void reset () { }
  virtual void reserve (size_t n) { }
  virtual int edge (bool north, bool enter, property_type p) { return 0; }
  virtual bool select_edge (bool horizontal, property_type p) { return false; }
  virtual int compare_ns () const { return 0; }
  virtual bool is_reset () const { return false; }
  virtual bool prefer_touch () const { return false; }
  virtual bool selects_edges () const { return false; }
};

/**
 *  @brief An intersection detector
 *
 *  This edge evaluator will not produce output edges but rather record the 
 *  property pairs of polygons intersecting. Only intersections (overlaps)
 *  are recorded. Touching contacts are not recorded.
 *
 *  It will build a set of property pairs, where the lower property value
 *  is the first one of the pairs. 
 */
class KLAYOUT_DLL InteractionDetector
  : public EdgeEvaluatorBase
{
public:
  typedef std::set<std::pair<property_type, property_type> > interactions_type;
  typedef interactions_type::const_iterator iterator;

  /**
   *  @brief Constructor
   *
   *  The mode parameter selects the interaction check mode.
   *  0 is "overlapping". 
   *  -1 will select all polygons inside polygons from the other layer.
   *  +1 will select all polygons outside polygons from the other layer.
   *
   *  In mode -1 and +1, finish () needs to be called before the interactions
   *  can be used. In mode -1 and +1, the interactions delivered will contain
   *  interactions of the reference property vs. the property of the respective
   *  input polygons (property != reference property). In mode +1 these are 
   *  pseudo-interactions, because "outside" by definition means non-interacting.
   *
   *  Mode -1 (inside) and +1 (outside) requires a single property value for the containing region.
   *  This property value must be specified in the container_id parameter. 
   *  For correct operation, the container_id must be the lowest property ID and
   *  the interacting edges must have higher property id's.
   *  The reported interactions will be (container_id,polygon_id) even for outside mode.
   */
  InteractionDetector (int mode = 0, property_type container_id = 0);

  /**
   *  @brief Sets the "touching" flag
   *  
   *  If this flag is set, the interaction will include "touching" interactions in mode 0, i.e.
   *  touching shapes will be counted as interacting.
   */
  void set_include_touching (bool f) 
  {
    m_include_touching = f;
  }

  /**
   *  @brief Gets the "touching" flag
   */
  bool include_touching () const
  {
    return m_include_touching;
  }

  /**
   *  @brief Finish the collection
   *
   *  This method must be called in mode -1 and +1 in order to finish the collection.
   */
  void finish ();

  /**
   *  @brief Iterator delivering the interactions (begin iterator)
   *
   *  The iterator delivers pairs of property values. The lower value will be the first one of the pair.
   */
  iterator begin () const
  {
    return m_interactions.begin ();
  }

  /**
   *  @brief Iterator delivering the interactions (end iterator)
   */
  iterator end () const
  {
    return m_interactions.end ();
  }

  virtual void reset ();
  virtual void reserve (size_t n);
  virtual int edge (bool north, bool enter, property_type p);
  virtual int compare_ns () const;
  virtual bool is_reset () const { return m_inside.empty (); }
  virtual bool prefer_touch () const { return m_mode == 0 && m_include_touching; }

private:
  int m_mode;
  bool m_include_touching;
  property_type m_container_id;
  std::vector <int> m_wcv_n, m_wcv_s;
  std::set <property_type> m_inside;
  std::set<std::pair<property_type, property_type> > m_interactions;
  std::set<property_type> m_non_interactions;
};

/**
 *  @brief A generic inside operator
 *
 *  This incarnation of the evaluator class implements an "inside" function
 *  based on a generic operator.
 */
template <class F>
class KLAYOUT_DLL GenericMerge
  : public EdgeEvaluatorBase
{
public:
  /**
   *  @brief Constructor
   */
  GenericMerge (const F &function) 
    : m_wc_n (0), m_wc_s (0), m_function (function)
  { }

  virtual void reset ()
  {
    m_wc_n = m_wc_s = 0;
  }

  virtual void reserve (size_t n)
  {
    // .. nothing yet ..
  }

  virtual int edge (bool north, bool enter, property_type p)
  {
    int *wc = north ? &m_wc_n : &m_wc_s;
    bool t0 = m_function (*wc);
    if (enter) {
      ++*wc;
    } else {
      --*wc;
    }
    bool t1 = m_function (*wc);
    if (t1 && ! t0) {
      return 1;
    } else if (! t1 && t0) {
      return -1;
    } else {
      return 0;
    }
  }

  virtual int compare_ns () const
  {
    if (m_function (m_wc_s) && ! m_function (m_wc_n)) {
      return -1;
    } else if (! m_function (m_wc_s) && m_function (m_wc_n)) {
      return 1;
    } else {
      return 0;
    }
  }

  virtual bool is_reset () const
  {
    return (m_wc_n == 0 && m_wc_s == 0);
  }

private:
  int m_wc_n, m_wc_s;
  F m_function;
};

/**
 *  @brief A helper class to implement the SimpleMerge operator
 */
struct ParametrizedInsideFunc 
{
  ParametrizedInsideFunc (int mode)
    : m_mode (mode)
  {
    //  .. nothing yet ..
  }

  inline bool operator() (int wc) const
  {
    if (m_mode > 0) {
      return wc >= m_mode;
    } else if (m_mode < 0) {
      return wc <= m_mode || -wc <= m_mode;
    } else {
      return (wc < 0 ? ((-wc) % 2) : (wc % 2)) != 0;
    }
  }

public:
  int m_mode;
};

/**
 *  @brief Simple merge operator
 *
 *  This incarnation of the evaluator class implements a simple 
 *  merge criterion for a set of edges. A result is generated if the wrap count (wc)
 *  of the edge set satisfies a condition given by "mode". Specifically:
 *    mode == 0: even-odd rule (wc = ... -3, -1, 1, 3, ...)
 *    mode == 1: wc >= 1
 *    mode == -1: wc >= 1 || wc <= -1
 *    mode == n: wc >= n
 *    mode == -n: wc >= n || wc <= -n
 */
class KLAYOUT_DLL SimpleMerge
  : public GenericMerge<ParametrizedInsideFunc>
{
public:
  /**
   *  @brief Constructor
   */
  SimpleMerge (int mode = -1)
    : GenericMerge<ParametrizedInsideFunc> (ParametrizedInsideFunc (mode))
  { }
};

/**
 *  @brief Boolean operations
 *
 *  This incarnation of the evaluator class implements a boolean operation
 *  (AND, A NOT B, B NOT A, XOR, OR). The mode can be specified in the constructor.
 *  It relies on the properties being set in a certain way: bit 0 codes the layer (0 for A, 1 for B)
 *  while the other bits are used to distinguish the polygons. For each polygon, a non-zero
 *  wrap count rule is applied before the boolean operation's output is formed.
 */
class KLAYOUT_DLL BooleanOp 
  : public EdgeEvaluatorBase 
{
public:
  enum BoolOp {
    And = 1, ANotB = 2, BNotA = 3, Xor = 4, Or = 5
  };
 
  /**
   *  @brief Constructor
   *
   *  @param mode The boolean operation that this object represents
   */
  BooleanOp (BoolOp mode);

  virtual void reset ();
  virtual void reserve (size_t n);
  virtual int edge (bool north, bool enter, property_type p);
  virtual int compare_ns () const;
  virtual bool is_reset () const { return m_zeroes == m_wcv_n.size () + m_wcv_s.size (); }

protected:
  template <class InsideFunc> bool result (int wca, int wcb, const InsideFunc &inside_a, const InsideFunc &inside_b) const;
  template <class InsideFunc> int edge_impl (bool north, bool enter, property_type p, const InsideFunc &inside_a, const InsideFunc &inside_b);
  template <class InsideFunc> int compare_ns_impl (const InsideFunc &inside_a, const InsideFunc &inside_b) const;

private:
  int m_wc_na, m_wc_nb, m_wc_sa, m_wc_sb;
  std::vector <int> m_wcv_n, m_wcv_s;
  BoolOp m_mode;
  size_t m_zeroes;
};

/**
 *  @brief Edge vs. Polygon intersection
 *
 *  This operator detects edges inside or outside polygons.
 *  The polygon edges must be given with property 0, the other edges
 *  with properties 1 and higher.
 *
 *  The operator will deliver edges inside polygons or outside polygons.
 *  It can be configured to include edges on the border of the polygons
 *  as being considered "inside the polygon".
 */
class KLAYOUT_DLL EdgePolygonOp 
  : public db::EdgeEvaluatorBase
{
public:
  /**
   *  @brief Constructor
   *
   *  @param outside If true, the operator will deliver edges outside the polygon
   *  @param include_touching If true, edges on the polygon's border will be considered "inside" of polygons
   *  @param polygon_mode Determines how the polygon edges on property 0 are interpreted (see merge operators)
   */
  EdgePolygonOp (bool outside = false, bool include_touching = true, int polygon_mode = -1);

  virtual void reset ();
  virtual bool select_edge (bool horizontal, property_type p);
  virtual int edge (bool north, bool enter, property_type p);
  virtual bool is_reset () const;
  virtual bool prefer_touch () const;
  virtual bool selects_edges () const;

private:
  bool m_outside, m_include_touching;
  db::ParametrizedInsideFunc m_function;
  int m_wcp_n, m_wcp_s;
};

/**
 *  @brief Boolean operations
 *
 *  This class implements a boolean operation similar to BooleanOp, but 
 *  in addition it allows to specify the merge mode for the two inputs.
 *  See "SimpleMergeOp" for the definition of the merge modes.
 *  This operator is especially useful to implement boolean operations
 *  with sized polygons which required a >0 interpretation.
 */
class KLAYOUT_DLL BooleanOp2
  : public BooleanOp
{
public:
  /**
   *  @brief Constructor
   *
   *  @param mode The boolean operation that this object represents
   */
  BooleanOp2 (BoolOp mode, int wc_mode_a, int wc_mode_b);

  virtual int edge (bool north, bool enter, property_type p);
  virtual int compare_ns () const;

private:
  int m_wc_mode_a, m_wc_mode_b;
};

/**
 *  @brief Merge operation
 *
 *  This incarnation of the evaluator class implements a merge operation
 *  which allows to distinguish polygons (through edge properties) and 
 *  allows to specify a overlap value. Default is 0 which means that the
 *  merge is equivalent to producing all polygins. A overlap value of 1 means
 *  that at least two polygons must overlap to produce a result.
 */
class KLAYOUT_DLL MergeOp 
  : public EdgeEvaluatorBase 
{
public:
  /**
   *  @brief Constructor
   *
   *  @param min_overlap See class description
   */
  MergeOp (unsigned int min_overlap = 0);

  virtual void reset ();
  virtual void reserve (size_t n);
  virtual int edge (bool north, bool enter, property_type p);
  virtual int compare_ns () const;
  virtual bool is_reset () const { return m_zeroes == m_wcv_n.size () + m_wcv_s.size (); }

private:
  int m_wc_n, m_wc_s;
  std::vector <int> m_wcv_n, m_wcv_s;
  unsigned int m_min_wc;
  size_t m_zeroes;
};

/**
 *  @brief The basic edge processor
 *
 *  An edge processor takes a set of edges, processes them by removing intersections and 
 *  applying a custom operator for computing the output edge sets which then are delivered
 *  to an EdgeSink receiver object.
 */
class KLAYOUT_DLL EdgeProcessor
{
public:
  typedef size_t property_type;

  /**
   *  @brief Default constructor
   *
   *  @param report_progress If true, a tl::Progress object will be created to report any progress (warning: this will impose a performance penalty)
   *  @param progress_text The description text of the progress object
   */
  EdgeProcessor (bool report_progress = false, const std::string &progress_desc = std::string ());

  /**
   *  @brief Destructor
   */
  ~EdgeProcessor ();

  /**
   *  @brief Enable progress reporting
   *
   *  @param progress_text The description text of the progress object
   */
  void enable_progress (const std::string &progress_desc = std::string ());

  /**
   *  @brief Disable progress reporting
   */
  void disable_progress ();

  /**
   *  @brief Reserve space for at least n edges
   */
  void reserve (size_t n);

  /**
   *  @brief Insert an edge 
   */
  void insert (const db::Edge &e, property_type p = 0);

  /**
   *  @brief Insert an polygon 
   */
  void insert (const db::Polygon &q, property_type p = 0);

  /**
   *  @brief Insert a sequence of edges
   *
   *  This method does not reserve for the number of elements required. This must
   *  be done explicitly for performance benefits.
   */
  template <class Iter>
  void insert_sequence (Iter from, Iter to, property_type p = 0)
  {
    for (Iter i = from; i != to; ++i) {
      insert (*i, p);
    }
  }

  /**
   *  @brief Insert a sequence of edges (iterator with at_end semantics)
   *
   *  This method does not reserve for the number of elements required. This must
   *  be done explicitly for performance benefits.
   */
  template <class Iter>
  void insert_sequence (Iter i, property_type p = 0)
  {
    for ( ; !i.at_end (); ++i) {
      insert (*i, p);
    }
  }

  /**
   *  @brief Clear all edges stored currently in this processor
   */
  void clear (); 

  /**
   *  @brief Process the edges stored currently
   */
  void process (db::EdgeSink &es, EdgeEvaluatorBase &op);

  /**
   *  @brief Merge the given polygons in a simple "non-zero wrapcount" fashion
   *
   *  The wrapcount is computed over all polygons, i.e. overlapping polygons may "cancel" if they
   *  have different orientation (since a polygon is oriented by construction that is not easy to achieve).
   *  The other merge operation provided for this purpose is "merge" which normalizes each polygon individually before
   *  merging them. "simple_merge" is somewhat faster and consumes less memory.
   *
   *  The result is presented as a set of edges forming closed contours. Hulls are oriented clockwise while
   *  holes are oriented counter-clockwise.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a SimpleMerge operator and puts the result into an output vector.
   *
   *  @param in The input polygons
   *  @param out The output edges
   *  @param mode The merge mode (see SimpleMerge constructor)
   */
  void simple_merge (const std::vector<db::Polygon> &in, std::vector <db::Edge> &out, int mode = -1);

  /**
   *  @brief Merge the given polygons in a simple "non-zero wrapcount" fashion into polygons
   *
   *  The wrapcount is computed over all polygons, i.e. overlapping polygons may "cancel" if they
   *  have different orientation (since a polygon is oriented by construction that is not easy to achieve).
   *  The other merge operation provided for this purpose is "merge" which normalizes each polygon individually before
   *  merging them. "simple_merge" is somewhat faster and consumes less memory.
   *
   *  This method produces polygons and allows to fine-tune the parameters for that purpose.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a SimpleMerge operator and puts the result into an output vector.
   *
   *  @param in The input polygons
   *  @param out The output polygons
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if touching corners should be resolved into less connected contours
   *  @param mode The merge mode (see SimpleMerge constructor)
   */
  void simple_merge (const std::vector<db::Polygon> &in, std::vector <db::Polygon> &out, bool resolve_holes = true, bool min_coherence = true, int mode = -1);

  /**
   *  @brief Merge the given edges in a simple "non-zero wrapcount" fashion
   *
   *  The egdes provided must form valid closed contours. Contours oriented differently "cancel" each other. 
   *  Overlapping contours are merged when the orientation is the same.
   *
   *  The result is presented as a set of edges forming closed contours. Hulls are oriented clockwise while
   *  holes are oriented counter-clockwise.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a SimpleMerge operator and puts the result into an output vector.
   *
   *  @param in The input edges
   *  @param out The output edges
   *  @param mode The merge mode (see SimpleMerge constructor)
   */
  void simple_merge (const std::vector<db::Edge> &in, std::vector <db::Edge> &out, int mode = -1);

  /**
   *  @brief Merge the given edges in a simple "non-zero wrapcount" fashion into polygons
   *
   *  The egdes provided must form valid closed contours. Contours oriented differently "cancel" each other. 
   *  Overlapping contours are merged when the orientation is the same.
   *
   *  This method produces polygons and allows to fine-tune the parameters for that purpose.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a SimpleMerge operator and puts the result into an output vector.
   *
   *  @param in The input edges
   *  @param out The output polygons
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if touching corners should be resolved into less connected contours
   *  @param mode The merge mode (see SimpleMerge constructor)
   */
  void simple_merge (const std::vector<db::Edge> &in, std::vector <db::Polygon> &out, bool resolve_holes = true, bool min_coherence = true, int mode = -1);

  /**
   *  @brief Merge the given polygons 
   *
   *  In contrast to "simple_merge", this merge implementation considers each polygon individually before merging them.
   *  Thus self-overlaps are effectively removed before the output is computed and holes are correctly merged with the
   *  hull. In addition, this method allows to select areas with a higher wrap count which allows to compute overlaps
   *  of polygons on the same layer. Because this method merges the polygons before the overlap is computed, self-overlapping
   *  polygons do not contribute to higher wrap count areas.
   *
   *  The result is presented as a set of edges forming closed contours. Hulls are oriented clockwise while
   *  holes are oriented counter-clockwise.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a Merge operator and puts the result into an output vector.
   *
   *  @param in The input polygons
   *  @param out The output edges
   *  @param min_wc The minimum wrap count for output (0: all polygons, 1: at least two overlapping)
   */
  void merge (const std::vector<db::Polygon> &in, std::vector <db::Edge> &out, unsigned int min_wc = 0);

  /**
   *  @brief Merge the given polygons 
   *
   *  In contrast to "simple_merge", this merge implementation considers each polygon individually before merging them.
   *  Thus self-overlaps are effectively removed before the output is computed and holes are correctly merged with the
   *  hull. In addition, this method allows to select areas with a higher wrap count which allows to compute overlaps
   *  of polygons on the same layer. Because this method merges the polygons before the overlap is computed, self-overlapping
   *  polygons do not contribute to higher wrap count areas.
   *
   *  This method produces polygons and allows to fine-tune the parameters for that purpose.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a Merge operator and puts the result into an output vector.
   *
   *  @param in The input polygons
   *  @param out The output polygons
   *  @param min_wc The minimum wrap count for output (0: all polygons, 1: at least two overlapping)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if touching corners should be resolved into less connected contours
   */
  void merge (const std::vector<db::Polygon> &in, std::vector <db::Polygon> &out, unsigned int min_wc = 0, bool resolve_holes = true, bool min_coherence = true);

  /**
   *  @brief Size the given polygons 
   *
   *  This method sizes a set of polygons. Before the sizing is applied, the polygons are merged. After that, sizing is applied 
   *  on the individual result polygons of the merge step. The result may contain overlapping contours, but no self-overlaps. 
   *
   *  dx and dy describe the sizing. A positive value indicates oversize (outwards) while a negative one describes undersize (inwards).
   *  The sizing applied can be choosen differently in x and y direction. In this case, the sign must be identical for both
   *  dx and dy.
   *
   *  The result is presented as a set of edges forming closed contours. Hulls are oriented clockwise while
   *  holes are oriented counter-clockwise.
   *
   *  This is a convenience method that bundles filling of the edges and processing them 
   *  and which puts the result into an output vector.
   *
   *  @param in The input polygons
   *  @param dx The sizing value in x direction
   *  @param dy The sizing value in y direction
   *  @param out The output edges
   *  @param mode The sizing mode (see db::Polygon for a description)
   */
  void size (const std::vector<db::Polygon> &in, db::Coord dx, db::Coord dy, std::vector <db::Edge> &out, unsigned int mode = 2);

  /**
   *  @brief Size the given polygons into polygons
   *
   *  This method sizes a set of polygons. Before the sizing is applied, the polygons are merged. After that, sizing is applied 
   *  on the individual result polygons of the merge step. The result may contain overlapping polygons, but no self-overlapping ones. 
   *  Polygon overlap occures if the polygons are close enough, so a positive sizing makes polygons overlap.
   *  
   *  dx and dy describe the sizing. A positive value indicates oversize (outwards) while a negative one describes undersize (inwards).
   *  The sizing applied can be choosen differently in x and y direction. In this case, the sign must be identical for both
   *  dx and dy.
   *
   *  This method produces polygons and allows to fine-tune the parameters for that purpose.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a SimpleMerge operator and puts the result into an output vector.
   *
   *  @param in The input polygons
   *  @param dx The sizing value in x direction
   *  @param dy The sizing value in y direction
   *  @param out The output polygons
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if touching corners should be resolved into less connected contours
   */
  void size (const std::vector<db::Polygon> &in, db::Coord dx, db::Coord dy, std::vector <db::Polygon> &out, unsigned int mode = 2, bool resolve_holes = true, bool min_coherence = true);

  /**
   *  @brief Size the given polygons (isotropic)
   *
   *  This method is equivalent to calling the anisotropic version with identical dx and dy.
   *
   *  @param in The input polygons
   *  @param d The sizing value in x direction
   *  @param out The output edges
   *  @param mode The sizing mode (see db::Polygon for a description)
   */
  void size (const std::vector<db::Polygon> &in, db::Coord d, std::vector <db::Edge> &out, unsigned int mode = 2)
  {
    size (in, d, d, out, mode);
  }

  /**
   *  @brief Size the given polygons into polygons (isotropic)
   *
   *  This method is equivalent to calling the anisotropic version with identical dx and dy.
   *
   *  @param in The input polygons
   *  @param d The sizing value in x direction
   *  @param out The output polygons
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if touching corners should be resolved into less connected contours
   */
  void size (const std::vector<db::Polygon> &in, db::Coord d, std::vector <db::Polygon> &out, unsigned int mode = 2, bool resolve_holes = true, bool min_coherence = true)
  {
    size (in, d, d, out, mode, resolve_holes, min_coherence);
  }

  /**
   *  @brief Boolean operation for a set of given polygons, creating edges
   *
   *  This method computes the result for the given boolean operation on two sets of polygons.
   *  The result is presented as a set of edges forming closed contours. Hulls are oriented clockwise while
   *  holes are oriented counter-clockwise.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a Boolean operator and puts the result into an output vector.
   *
   *  @param a The input polygons (first operand)
   *  @param b The input polygons (second operand)
   *  @param out The output edges
   *  @param mode The boolean mode
   */
  void boolean (const std::vector<db::Polygon> &a, const std::vector<db::Polygon> &b, std::vector <db::Edge> &out, int mode);

  /**
   *  @brief Boolean operation for a set of given polygons, creating polygons
   *
   *  This method computes the result for the given boolean operation on two sets of polygons.
   *  This method produces polygons on output and allows to fine-tune the parameters for that purpose.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a Boolean operator and puts the result into an output vector.
   *
   *  @param a The input polygons (first operand)
   *  @param b The input polygons (second operand)
   *  @param out The output polygons
   *  @param mode The boolean mode
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if touching corners should be resolved into less connected contours
   */
  void boolean (const std::vector<db::Polygon> &a, const std::vector<db::Polygon> &b, std::vector <db::Polygon> &out, int mode, bool resolve_holes = true, bool min_coherence = true);

  /**
   *  @brief Boolean operation for a set of given edges, creating edges
   *
   *  This method computes the result for the given boolean operation on two sets of edges.
   *  The input edges must form closed contours where holes and hulls must be oriented differently. 
   *  The input edges are processed with a simple non-zero wrap count rule as a whole.
   *
   *  The result is presented as a set of edges forming closed contours. Hulls are oriented clockwise while
   *  holes are oriented counter-clockwise.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a Boolean operator and puts the result into an output vector.
   *
   *  @param a The input edges (first operand)
   *  @param b The input edges (second operand)
   *  @param out The output edges
   *  @param mode The boolean mode
   */
  void boolean (const std::vector<db::Edge> &a, const std::vector<db::Edge> &b, std::vector <db::Edge> &out, int mode);

  /**
   *  @brief Boolean operation for a set of given edges, creating polygons
   *
   *  This method computes the result for the given boolean operation on two sets of edges.
   *  The input edges must form closed contours where holes and hulls must be oriented differently. 
   *  The input edges are processed with a simple non-zero wrap count rule as a whole.
   *
   *  This method produces polygons on output and allows to fine-tune the parameters for that purpose.
   *
   *  This is a convenience method that bundles filling of the edges, processing with
   *  a Boolean operator and puts the result into an output vector.
   *
   *  @param a The input polygons (first operand)
   *  @param b The input polygons (second operand)
   *  @param out The output polygons
   *  @param mode The boolean mode
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if touching corners should be resolved into less connected contours
   */
  void boolean (const std::vector<db::Edge> &a, const std::vector<db::Edge> &b, std::vector <db::Polygon> &out, int mode, bool resolve_holes = true, bool min_coherence = true);

private:
  std::vector <WorkEdge> *mp_work_edges;
  std::vector <CutPoints> *mp_cpvector;
  bool m_report_progress;
  std::string m_progress_desc;

  static size_t count_edges (const db::Polygon &q) 
  {
    size_t n = q.hull ().size ();
    for (unsigned int h = 0; h < q.holes (); ++h) {
      n += q.hole (h).size ();
    }
    return n;
  }

  static size_t count_edges (const std::vector<db::Polygon> &v) 
  {
    size_t n = 0;
    for (std::vector<db::Polygon>::const_iterator p = v.begin (); p != v.end (); ++p) {
      n += count_edges (*p);
    }
    return n;
  }
};

/**
 *  @brief A processor for shape objects
 *
 *  Similar to the edge processor, this class deals with shape objects and shape containers 
 *  instead of polygons.
 */
class KLAYOUT_DLL ShapeProcessor 
{
public:
  /**
   *  @brief Constructor
   *
   *  @param report_progress If true, a tl::Progress object will be created to report any progress (warning: this will impose a performance penalty)
   */
  ShapeProcessor (bool report_progress = false, const std::string &progress_desc = std::string ());

  /**
   *  @brief Clear the shapes stored currently
   */
  void clear ();

  /**
   *  @brief Reserve the number of edges
   */
  void reserve (size_t n);

  /**
   *  @brief Enable progress
   *
   *  @param progress_text The description text of the progress object
   */
  void enable_progress (const std::string &progress_desc = std::string ())
  {
    m_processor.enable_progress (progress_desc);
  }

  /**
   *  @brief Disable progress
   */
  void disable_progress ()
  {
    m_processor.disable_progress ();
  }

  /**
   *  @brief Insert a shape without transformation
   */
  void insert (const db::Shape &shape, db::EdgeProcessor::property_type p)
  {
    insert (shape, db::UnitTrans (), p);
  }

  /**
   *  @brief Insert a native shape
   */
  template <class S>
  void insert_native (const S &shape, db::EdgeProcessor::property_type p)
  {
    m_processor.insert (shape, p);
  }

  /**
   *  @brief Insert a shape
   */
  template <class T>
  void insert (const db::Shape &shape, const T &trans, db::EdgeProcessor::property_type p)
  {
    if (shape.is_polygon ()) {

      for (db::Shape::polygon_edge_iterator e = shape.begin_edge (); ! e.at_end (); ++e) {
        m_processor.insert ((*e).transform (trans), p);
      }

    } else if (shape.is_path ()) {

      db::Polygon poly;
      shape.polygon (poly);
      for (db::Polygon::polygon_edge_iterator e = poly.begin_edge (); ! e.at_end (); ++e) {
        m_processor.insert ((*e).transform (trans), p);
      }

    } else if (shape.is_box ()) {

      db::Box b (shape.box ());
      m_processor.insert (db::Edge (b.lower_left (), b.upper_left ()).transform (trans), p);
      m_processor.insert (db::Edge (b.upper_left (), b.upper_right ()).transform (trans), p);
      m_processor.insert (db::Edge (b.upper_right (), b.lower_right ()).transform (trans), p);
      m_processor.insert (db::Edge (b.lower_right (), b.lower_left ()).transform (trans), p);

    }
  }

  /**
   *  @brief Count the edges for a shape
   */
  size_t count_edges (const db::Shape &shape) const;

  /**
   *  @brief Insert a sequence of shapes
   *
   *  This method does not reserve for the number of elements required. This must
   *  be done explicitly for performance benefits.
   */
  template <class Iter>
  void insert_sequence (Iter from, Iter to, db::EdgeProcessor::property_type p = 0)
  {
    for (Iter i = from; i != to; ++i) {
      insert (*i, p);
    }
  }

  /**
   *  @brief Insert a sequence of shapes (iterator with at_end semantics)
   *
   *  This method does not reserve for the number of elements required. This must
   *  be done explicitly for performance benefits.
   */
  template <class Iter>
  void insert_sequence (Iter i, db::EdgeProcessor::property_type p = 0)
  {
    for ( ; !i.at_end (); ++i) {
      insert (*i, p);
    }
  }

  /**
   *  @brief Process the given edges 
   *
   *  This method uses the given sink as target and the given evaluator for defining
   *  the method to use
   */
  void process (db::EdgeSink &es, EdgeEvaluatorBase &op);

  /**
   *  @brief Merge the given shapes
   *
   *  See the EdgeProcessor for a description of the merge method. This implementation takes shapes
   *  rather than polygons for input and produces a polygon set.
   *
   *  @param in The set of shapes to merge
   *  @param trans A corresponding set of transformations to apply on the shapes
   *  @param out The result (a polygon vector)
   *  @param min_wc The minimum wrap count for output (0: all polygons, 1: at least two overlapping)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void merge (const std::vector<db::Shape> &in, const std::vector<db::CplxTrans> &trans,
              std::vector <db::Polygon> &out, unsigned int min_wc = 0, bool resolve_holes = true, bool min_coherence = true);

  /**
   *  @brief Merge the given shapes
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to merge
   *  @param out The result (a polygon vector)
   *  @param min_wc The minimum wrap count for output (0: all polygons, 1: at least two overlapping)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void merge (const std::vector<db::Shape> &in, std::vector <db::Polygon> &out, unsigned int min_wc = 0, bool resolve_holes = true, bool min_coherence = true)
  {
    merge (in, std::vector<db::CplxTrans> (), out, min_wc, resolve_holes, min_coherence);
  }

  /**
   *  @brief Merge the given shapes into an edge set
   *
   *  This is basically a "or" operation on a single layer creating edges.
   *
   *  @param in The set of shapes to merge
   *  @param trans A corresponding set of transformations to apply on the shapes
   *  @param out The result (an edge vector)
   *  @param min_wc The minimum wrap count for output (0: all polygons, 1: at least two overlapping)
   */
  void merge (const std::vector<db::Shape> &in, const std::vector<db::CplxTrans> &trans,
              std::vector <db::Edge> &out, unsigned int min_wc = 0);

  /**
   *  @brief Merge the given shapes into an edge set
   *
   *  This is basically a "or" operation on a single layer creating edges.
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to merge
   *  @param out The result (an edge vector)
   *  @param min_wc The minimum wrap count for output (0: all polygons, 1: at least two overlapping)
   */
  void merge (const std::vector<db::Shape> &in, std::vector<db::Edge> &out, unsigned int min_wc = 0)
  {
    merge (in, std::vector<db::CplxTrans> (), out, min_wc);
  }

  /**
   *  @brief Merge the given shapes from a layout to a shape container
   *
   *  This is basically a "or" operation on a single layer.
   *  The input can be taken from a layout and the result is delivered to a 
   *  shape container.
   *
   *  @param layout_in The layout from which to take the input
   *  @param cell_in The cell from which to take the input
   *  @param layer_in The layer from which to take the input
   *  @param out Where to store the results 
   *  @param with_sub_hierarchy Take shapes from the cell and subcells
   *  @param min_wc The minimum wrap count for output (0: all polygons, 1: at least two overlapping)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void merge (const db::Layout &layout_in, const db::Cell &cell_in, unsigned int layer_in, 
              db::Shapes &out, bool with_sub_hierarchy, unsigned int min_wc = 0, bool resolve_holes = true, bool min_coherence = true)
  {
    std::vector<unsigned int> layers_in;
    layers_in.push_back (layer_in);
    merge (layout_in, cell_in, layers_in, out, with_sub_hierarchy, min_wc, resolve_holes, min_coherence);
  }

  /**
   *  @brief Merge the given shapes from a layout to a shape container
   *
   *  This is basically a "or" operation on a single layer.
   *  The input can be taken from a layout and the result is delivered to a 
   *  shape container.
   *
   *  @param layout_in The layout from which to take the input
   *  @param cell_in The cell from which to take the input
   *  @param layers_in The layers from which to take the input
   *  @param out Where to store the results 
   *  @param with_sub_hierarchy Take shapes from the cell and subcells
   *  @param min_wc The minimum wrap count for output (0: all polygons, 1: at least two overlapping)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void merge (const db::Layout &layout_in, const db::Cell &cell_in, const std::vector<unsigned int> &layers_in, 
              db::Shapes &out, bool with_sub_hierarchy, unsigned int min_wc = 0, bool resolve_holes = true, bool min_coherence = true);

  /**
   *  @brief Boolean operation on two given shape sets into a polygon set
   *
   *  @param in_a The set of shapes to use for input A
   *  @param trans_a A set of transformations to apply before the shapes are used
   *  @param in_b The set of shapes to use for input A
   *  @param trans_b A set of transformations to apply before the shapes are used
   *  @param mode The boolean operation
   *  @param out The result (an polygon vector)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void boolean (const std::vector<db::Shape> &in_a, const std::vector<db::CplxTrans> &trans_a,
                const std::vector<db::Shape> &in_b, const std::vector<db::CplxTrans> &trans_b,
                int mode, std::vector <db::Polygon> &out, bool resolve_holes = true, bool min_coherence = true);

  /**
   *  @brief Boolean operation on two given shape sets into a polygon set
   *
   *  @param in_a The set of shapes to use for input A
   *  @param in_b The set of shapes to use for input A
   *  @param mode The boolean operation
   *  @param out The result (an polygon vector)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void boolean (const std::vector<db::Shape> &in_a, const std::vector<db::Shape> &in_b, 
                int mode, std::vector <db::Polygon> &out, bool resolve_holes = true, bool min_coherence = true)
  {
    boolean (in_a, std::vector<db::CplxTrans> (), in_b, std::vector<db::CplxTrans> (), mode, out, resolve_holes, min_coherence);
  }

  /**
   *  @brief Boolean operation on two given shape sets into an edge set
   *
   *  @param in_a The set of shapes to use for input A
   *  @param trans_a A set of transformations to apply before the shapes are used
   *  @param in_b The set of shapes to use for input A
   *  @param trans_b A set of transformations to apply before the shapes are used
   *  @param mode The boolean operation
   *  @param out The result (an edge vector)
   */
  void boolean (const std::vector<db::Shape> &in_a, const std::vector<db::CplxTrans> &trans_a,
                const std::vector<db::Shape> &in_b, const std::vector<db::CplxTrans> &trans_b,
                int mode, std::vector <db::Edge> &out);

  /**
   *  @brief Boolean operation on two given shape sets into an edge set
   *
   *  @param in_a The set of shapes to use for input A
   *  @param in_b The set of shapes to use for input A
   *  @param mode The boolean operation
   *  @param out The result (an edge vector)
   */
  void boolean (const std::vector<db::Shape> &in_a, const std::vector<db::Shape> &in_b, 
                int mode, std::vector <db::Edge> &out)
  {
    boolean (in_a, std::vector<db::CplxTrans> (), in_b, std::vector<db::CplxTrans> (), mode, out);
  }

  /**
   *  @brief Boolean operation on the given shapes from a layout to a shape container
   *
   *  The input can be taken from separate layouts and the result is delivered to a 
   *  shape container.
   *
   *  @param layout_in_a The layout from which to take the input for source A
   *  @param cell_in_a The cell from which to take the input for source A
   *  @param layer_in_a The layer from which to take the input for source A
   *  @param layout_in_b The layout from which to take the input for source B
   *  @param cell_in_b The cell from which to take the input for source B
   *  @param layer_in_b The layer from which to take the input for source B
   *  @param out Where to store the results 
   *  @param mode The boolean operation to apply
   *  @param with_sub_hierarchy Take shapes from the cell and subcells
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void boolean (const db::Layout &layout_in_a, const db::Cell &cell_in_a, unsigned int layer_in_a, 
                const db::Layout &layout_in_b, const db::Cell &cell_in_b, unsigned int layer_in_b, 
                db::Shapes &out, int mode, bool with_sub_hierarchy = false, bool resolve_holes = true, bool min_coherence = true)
  {
    std::vector<unsigned int> layers_in_a;
    layers_in_a.push_back (layer_in_a);
    std::vector<unsigned int> layers_in_b;
    layers_in_b.push_back (layer_in_b);
    boolean (layout_in_a, cell_in_a, layers_in_a, layout_in_b, cell_in_b, layers_in_b, out, mode, with_sub_hierarchy, resolve_holes, min_coherence);
  }

  /**
   *  @brief Boolean operation on the given shapes from a layout to a shape container
   *
   *  The input can be taken from separate layouts and the result is delivered to a 
   *  shape container.
   *
   *  @param layout_in_a The layout from which to take the input for source A
   *  @param cell_in_a The cell from which to take the input for source A
   *  @param layers_in_a The layer from which to take the input for source A
   *  @param layout_in_b The layout from which to take the input for source B
   *  @param cell_in_b The cell from which to take the input for source B
   *  @param layers_in_b The layer from which to take the input for source B
   *  @param out Where to store the results 
   *  @param mode The boolean operation to apply
   *  @param with_sub_hierarchy Take shapes from the cell and subcells
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void boolean (const db::Layout &layout_in_a, const db::Cell &cell_in_a, const std::vector<unsigned int> &layers_in_a, 
                const db::Layout &layout_in_b, const db::Cell &cell_in_b, const std::vector<unsigned int> &layers_in_b, 
                db::Shapes &out, int mode, bool with_sub_hierarchy = false, bool resolve_holes = true, bool min_coherence = true);

  /**
   *  @brief Size the given shapes into an polygon set
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to size
   *  @param trans A set of transformations to apply before the shapes are used
   *  @param dx The sizing to apply (x-direction)
   *  @param dy The sizing to apply (y-direction)
   *  @param out The result (an edge vector)
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void size (const std::vector<db::Shape> &in, const std::vector<db::CplxTrans> &trans,
             db::Coord dx, db::Coord dy, std::vector <db::Polygon> &out, unsigned int mode = 2, bool resolve_holes = true, bool min_coherence = true);

  /**
   *  @brief Size the given shapes into an polygon set
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to size
   *  @param trans A set of transformations to apply before the shapes are used
   *  @param d The sizing to apply 
   *  @param out The result (an edge vector)
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void size (const std::vector<db::Shape> &in, const std::vector<db::CplxTrans> &trans,
             db::Coord d, std::vector <db::Polygon> &out, unsigned int mode = 2, bool resolve_holes = true, bool min_coherence = true)
  {
    size (in, trans, d, d, out, mode, resolve_holes, min_coherence);
  }

  /**
   *  @brief Size the given shapes into an polygon set
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to size
   *  @param d The sizing to apply 
   *  @param out The result (an edge vector)
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void size (const std::vector<db::Shape> &in, 
             db::Coord d, std::vector <db::Polygon> &out, unsigned int mode = 2, bool resolve_holes = true, bool min_coherence = true)
  {
    size (in, std::vector<db::CplxTrans> (), d, out, mode, resolve_holes, min_coherence);
  }

  /**
   *  @brief Size the given shapes into an polygon set
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to size
   *  @param dx The sizing to apply (x-direction)
   *  @param dy The sizing to apply (y-direction)
   *  @param out The result (an edge vector)
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void size (const std::vector<db::Shape> &in, 
             db::Coord dx, db::Coord dy, std::vector <db::Polygon> &out, unsigned int mode = 2, bool resolve_holes = true, bool min_coherence = true)
  {
    size (in, std::vector<db::CplxTrans> (), dx, dy, out, mode, resolve_holes, min_coherence);
  }

  /**
   *  @brief Size the given shapes into an edge set
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to size
   *  @param trans A set of transformations to apply before the shapes are used
   *  @param dx The sizing to apply (x-direction)
   *  @param dy The sizing to apply (y-direction)
   *  @param out The result (an edge vector)
   *  @param mode The sizing mode (see db::Polygon for a description)
   */
  void size (const std::vector<db::Shape> &in, const std::vector<db::CplxTrans> &trans,
             db::Coord dx, db::Coord dy, std::vector <db::Edge> &out, unsigned int mode = 2);

  /**
   *  @brief Size the given shapes into an edge set
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to size
   *  @param trans A set of transformations to apply before the shapes are used
   *  @param d The sizing to apply 
   *  @param out The result (an edge vector)
   *  @param mode The sizing mode (see db::Polygon for a description)
   */
  void size (const std::vector<db::Shape> &in, const std::vector<db::CplxTrans> &trans,
             db::Coord d, std::vector <db::Edge> &out, unsigned int mode = 2)
  {
    size (in, trans, d, d, out, mode);
  }

  /**
   *  @brief Size the given shapes into an edge set
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to size
   *  @param d The sizing to apply 
   *  @param out The result (an edge vector)
   *  @param mode The sizing mode (see db::Polygon for a description)
   */
  void size (const std::vector<db::Shape> &in, 
             db::Coord d, std::vector <db::Edge> &out, unsigned int mode = 2)
  {
    size (in, std::vector<db::CplxTrans> (), d, out, mode);
  }

  /**
   *  @brief Size the given shapes into an edge set
   *
   *  This is equivalent to the previous method except that no transformations can be specified.
   *
   *  @param in The set of shapes to size
   *  @param dx The sizing to apply (x-direction)
   *  @param dy The sizing to apply (y-direction)
   *  @param out The result (an edge vector)
   *  @param mode The sizing mode (see db::Polygon for a description)
   */
  void size (const std::vector<db::Shape> &in, 
             db::Coord dx, db::Coord dy, std::vector <db::Edge> &out, unsigned int mode = 2)
  {
    size (in, std::vector<db::CplxTrans> (), dx, dy, out, mode);
  }

  /**
   *  @brief Size operation on the given shapes from a layout to a shape container
   *
   *  The input can be taken from a layout and the result is delivered to a 
   *  shape container.
   *
   *  @param layout_in The layout from which to take the input for source A
   *  @param cell_in The cell from which to take the input for source A
   *  @param layer_in The layer from which to take the input for source A
   *  @param out Where to store the results 
   *  @param d The sizing to apply
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param with_sub_hierarchy Take shapes from the cell and subcells
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void size (const db::Layout &layout_in, const db::Cell &cell_in, unsigned int layer_in, 
             db::Shapes &out, db::Coord d, unsigned int mode = 2, bool with_sub_hierarchy = false, bool resolve_holes = true, bool min_coherence = true)
  {
    size (layout_in, cell_in, layer_in, out, d, d, mode, with_sub_hierarchy, resolve_holes, min_coherence);
  }
             
  /**
   *  @brief Size operation on the given shapes from a layout to a shape container
   *
   *  The input can be taken from a layout and the result is delivered to a 
   *  shape container.
   *
   *  @param layout_in The layout from which to take the input for source A
   *  @param cell_in The cell from which to take the input for source A
   *  @param layers_in The layers from which to take the input for source A
   *  @param out Where to store the results 
   *  @param d The sizing to apply
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param with_sub_hierarchy Take shapes from the cell and subcells
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void size (const db::Layout &layout_in, const db::Cell &cell_in, const std::vector<unsigned int> &layers_in, 
             db::Shapes &out, db::Coord d, unsigned int mode = 2, bool with_sub_hierarchy = false, bool resolve_holes = true, bool min_coherence = true)
  {
    size (layout_in, cell_in, layers_in, out, d, d, mode, with_sub_hierarchy, resolve_holes, min_coherence);
  }
             
  /**
   *  @brief Size operation on the given shapes from a layout to a shape container
   *
   *  The input can be taken from a layout and the result is delivered to a 
   *  shape container.
   *
   *  @param layout_in The layout from which to take the input for source A
   *  @param cell_in The cell from which to take the input for source A
   *  @param layer_in The layer from which to take the input for source A
   *  @param out Where to store the results 
   *  @param dx The sizing to apply (x-direction)
   *  @param dy The sizing to apply (y-direction)
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param with_sub_hierarchy Take shapes from the cell and subcells
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void size (const db::Layout &layout_in, const db::Cell &cell_in, unsigned int layer_in, 
             db::Shapes &out, db::Coord dx, db::Coord dy, unsigned int mode = 2, bool with_sub_hierarchy = false, bool resolve_holes = true, bool min_coherence = true)
  {
    std::vector<unsigned int> layers_in;
    layers_in.push_back (layer_in);
    size (layout_in, cell_in, layers_in, out, dx, dy, mode, with_sub_hierarchy, resolve_holes, min_coherence);
  }

  /**
   *  @brief Size operation on the given shapes from a layout to a shape container
   *
   *  The input can be taken from a layout and the result is delivered to a 
   *  shape container.
   *
   *  @param layout_in The layout from which to take the input for source A
   *  @param cell_in The cell from which to take the input for source A
   *  @param layers_in The layers from which to take the input for source A
   *  @param out Where to store the results 
   *  @param dx The sizing to apply (x-direction)
   *  @param dy The sizing to apply (y-direction)
   *  @param mode The sizing mode (see db::Polygon for a description)
   *  @param with_sub_hierarchy Take shapes from the cell and subcells
   *  @param resolve_holes true, if holes should be resolved into the hull
   *  @param min_coherence true, if minimum polygons should be created for touching corners
   */
  void size (const db::Layout &layout_in, const db::Cell &cell_in, const std::vector<unsigned int> &layers_in, 
             db::Shapes &out, db::Coord dx, db::Coord dy, unsigned int mode = 2, bool with_sub_hierarchy = false, bool resolve_holes = true, bool min_coherence = true);

private:
  EdgeProcessor m_processor;

  void collect_shapes_hier (const db::CplxTrans &tr, const db::Layout &layout, const db::Cell &cell, unsigned int layer, int hier_levels, size_t &pn, size_t pdelta);
  size_t count_edges_hier (const db::Layout &layout, const db::Cell &cell, unsigned int layer, std::map<std::pair<db::cell_index_type, int>, size_t> &cache, int hier_levels) const;
};

/**
 *  @brief A polygon filter that sizes the polygons 
 *
 *  This class implements the PolygonSink interface and delivers the sized polygons to an EdgeSink.
 */
class KLAYOUT_DLL SizingPolygonFilter
  : public PolygonSink
{
public:
  /**
   *  @brief Constructor 
   */
  SizingPolygonFilter (EdgeSink &output, Coord dx, Coord dy, unsigned int mode)
    : PolygonSink (), mp_output (&output), m_dx (dx), m_dy (dy), m_mode (mode)
  { }

  /**
   *  @brief Implementation of the PolygonSink interface
   */
  virtual void put (const db::Polygon &polygon);

private:
  EdgeProcessor m_sizing_processor;
  EdgeSink *mp_output;
  Coord m_dx, m_dy;
  unsigned int m_mode;
};

}

#endif


