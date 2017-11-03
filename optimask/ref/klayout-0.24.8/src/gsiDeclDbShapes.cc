
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


#include "gsiDecl.h"
#include "dbShapes.h"
#include "dbShape.h"
#include "dbLayout.h"
#include "dbRegion.h"
#include "dbEdgePairs.h"
#include "dbEdges.h"

namespace gsi
{

// ---------------------------------------------------------------
//  db::Shapes binding

static void dump_mem_statistics (const db::Shapes *shapes)
{
  db::MemStatistics ms;
  shapes->collect_mem_stat (ms);
  ms.print ();
}

template<class Sh>
static db::Shape insert (db::Shapes *s, const Sh &p)
{
  return s->insert (p);
}

template<class Sh>
static db::Shape replace (db::Shapes *s, const db::Shape &sh, const Sh &p)
{
  return s->replace (sh, p);
}

template<class Sh>
static db::Shape insert_with_properties (db::Shapes *s, const Sh &p, db::properties_id_type id)
{
  return s->insert (db::object_with_properties<Sh> (p, id));
}

static db::Shapes::shape_iterator begin (const db::Shapes *s, unsigned int flags)
{
  return s->begin (flags);
}

static db::Shapes::shape_iterator begin_all (const db::Shapes *s)
{
  return s->begin (db::ShapeIterator::All);
}

static db::Shapes::shape_iterator begin_overlapping (const db::Shapes *s, unsigned int flags, const db::Box &region)
{
  return s->begin_overlapping (region, flags);
}

static db::Shapes::shape_iterator begin_overlapping_all (const db::Shapes *s, const db::Box &region)
{
  return s->begin_overlapping (region, db::ShapeIterator::All);
}

static db::Shapes::shape_iterator begin_touching (const db::Shapes *s, unsigned int flags, const db::Box &region)
{
  return s->begin_touching (region, flags);
}

static db::Shapes::shape_iterator begin_touching_all (const db::Shapes *s, const db::Box &region)
{
  return s->begin_touching (region, db::ShapeIterator::All);
}

static void transform_shapes (db::Shapes *s, const db::Trans &trans)
{
  db::Shapes d (*s);
  s->assign_transformed (d, trans);
}

static void transform_shapes_icplx (db::Shapes *s, const db::ICplxTrans &trans)
{
  db::Shapes d (*s);
  s->assign_transformed (d, trans);
}

static db::Shape transform_shape_cplx (db::Shapes *s, const db::Shape &shape, const db::CplxTrans &trans)
{
  return s->transform (shape, db::ICplxTrans (trans));
}

static db::Shape transform_shape_icplx (db::Shapes *s, const db::Shape &shape, const db::ICplxTrans &trans)
{
  return s->transform (shape, trans);
}

static db::Shape insert_shape_with_trans (db::Shapes *s, const db::Shape &shape, const db::Trans &trans)
{
  tl::ident_map<db::properties_id_type> pm;
  return s->insert (shape, trans, pm);
}

static db::Shape insert_shape_with_cplx_trans (db::Shapes *s, const db::Shape &shape, const db::CplxTrans &trans)
{
  tl::ident_map<db::properties_id_type> pm;
  return s->insert (shape, db::ICplxTrans (trans), pm);
}

static db::Shape insert_shape_with_icplx_trans (db::Shapes *s, const db::Shape &shape, const db::ICplxTrans &trans)
{
  tl::ident_map<db::properties_id_type> pm;
  return s->insert (shape, trans, pm);
}

static void insert_region (db::Shapes *sh, const db::Region &r)
{
  for (db::Region::const_iterator s = r.begin (); ! s.at_end (); ++s) {
    sh->insert (*s);
  }
}

static void insert_region_with_trans (db::Shapes *sh, const db::Region &r, const db::ICplxTrans &trans)
{
  for (db::Region::const_iterator s = r.begin (); ! s.at_end (); ++s) {
    sh->insert (s->transformed (trans));
  }
}

static void insert_edges (db::Shapes *sh, const db::Edges &r)
{
  for (db::Edges::const_iterator s = r.begin (); ! s.at_end (); ++s) {
    sh->insert (*s);
  }
}

static void insert_edges_with_trans (db::Shapes *sh, const db::Edges &r, const db::ICplxTrans &trans)
{
  for (db::Edges::const_iterator s = r.begin (); ! s.at_end (); ++s) {
    sh->insert (s->transformed (trans));
  }
}

static void insert_edge_pairs_as_polygons (db::Shapes *sh, const db::EdgePairs &r, db::Coord e)
{
  for (db::EdgePairs::const_iterator s = r.begin (); s != r.end (); ++s) {
    sh->insert (s->normalized ().to_simple_polygon (e));
  }
}

static void insert_edge_pairs_as_polygons_with_trans (db::Shapes *sh, const db::EdgePairs &r, const db::ICplxTrans &trans, db::Coord e)
{
  for (db::EdgePairs::const_iterator s = r.begin (); s != r.end (); ++s) {
    sh->insert (s->normalized ().to_simple_polygon (e).transformed (trans));
  }
}

static void insert_edge_pairs_as_edges (db::Shapes *sh, const db::EdgePairs &r)
{
  for (db::EdgePairs::const_iterator s = r.begin (); s != r.end (); ++s) {
    sh->insert (s->first ());
    sh->insert (s->second ());
  }
}

static void insert_edge_pairs_as_edges_with_trans (db::Shapes *sh, const db::EdgePairs &r, const db::ICplxTrans &trans)
{
  for (db::EdgePairs::const_iterator s = r.begin (); s != r.end (); ++s) {
    sh->insert (s->first ().transformed (trans));
    sh->insert (s->second ().transformed (trans));
  }
}

static unsigned int s_all ()                 { return db::ShapeIterator::All; }
static unsigned int s_all_with_properties () { return db::ShapeIterator::AllWithProperties; }
static unsigned int s_properties ()          { return db::ShapeIterator::Properties; }
static unsigned int s_polygons ()            { return db::ShapeIterator::Polygons; }
static unsigned int s_boxes ()               { return db::ShapeIterator::Boxes; }
static unsigned int s_edges ()               { return db::ShapeIterator::Edges; }
static unsigned int s_paths ()               { return db::ShapeIterator::Paths; }
static unsigned int s_texts ()               { return db::ShapeIterator::Texts; }
static unsigned int s_user_objects ()        { return db::ShapeIterator::UserObjects; }

Class<db::Shapes> decl_Shapes ("Shapes", 
  gsi::method ("insert", (db::Shape (db::Shapes::*)(const db::Shape &)) &db::Shapes::insert,
    "@brief Insert a shape from a shape reference into the shapes list\n"
    "@args shape\n"
    "@return A reference (a \\Shape object) to the newly created shape\n"
    "This method has been introduced in version 0.16.\n"
  ) +
  gsi::method_ext ("insert", &insert_shape_with_trans,
    "@brief Insert a shape from a shape reference into the shapes list with a transformation\n"
    "@args shape, trans\n"
    "@param shape The shape to insert\n"
    "@param trans The transformation to apply before the shape is inserted\n"
    "@return A reference (a \\Shape object) to the newly created shape\n"
    "This method has been introduced in version 0.22.\n"
  ) +
  gsi::method_ext ("insert", &insert_shape_with_cplx_trans,
    "@brief Insert a shape from a shape reference into the shapes list with a complex transformation\n"
    "@args shape, trans\n"
    "@param shape The shape to insert\n"
    "@param trans The transformation to apply before the shape is inserted\n"
    "@return A reference (a \\Shape object) to the newly created shape\n"
    "This method has been introduced in version 0.22.\n"
  ) +
  gsi::method_ext ("insert", &insert_shape_with_icplx_trans,
    "@brief Insert a shape from a shape reference into the shapes list with a complex integer transformation\n"
    "@args shape, trans\n"
    "@param shape The shape to insert\n"
    "@param trans The transformation to apply before the shape is inserted\n"
    "@return A reference (a \\Shape object) to the newly created shape\n"
    "This method has been introduced in version 0.22.\n"
  ) +
  gsi::method_ext ("insert", &insert_region,
    "@brief Insert the polygons from the region into this shape container\n"
    "@args region\n"
    "@param region The region to insert\n"
    "\n"
    "This method inserts all polygons from the region into this shape container.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("insert", &insert_region_with_trans,
    "@brief Insert the polygons from the region into this shape container with a transformation\n"
    "@args region, trans\n"
    "@param region The region to insert\n"
    "@param trans The transformation to apply\n"
    "\n"
    "This method inserts all polygons from the region into this shape container.\n"
    "Before a polygon is inserted, the given transformation is applied.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("insert", &insert_edges,
    "@brief Insert the edges from the edge collection into this shape container\n"
    "@args edges\n"
    "@param edges The edges to insert\n"
    "\n"
    "This method inserts all edges from the edge collection into this shape container.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("insert", &insert_edges_with_trans,
    "@brief Insert the edges from the edge collection into this shape container with a transformation\n"
    "@args edges, trans\n"
    "@param edges The edges to insert\n"
    "@param trans The transformation to apply\n"
    "\n"
    "This method inserts all edges from the edge collection into this shape container.\n"
    "Before an edge is inserted, the given transformation is applied.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("insert_as_polygons", &insert_edge_pairs_as_polygons,
    "@brief Insert the edge pairs from the edge pair collection as polygons into this shape container\n"
    "@args edge_pairs, e\n"
    "@param edge_pairs The edge pairs to insert\n"
    "@param The extension to apply when converting the edges to polygons\n"
    "\n"
    "This method inserts all edge pairs from the edge pair collection into this shape container.\n"
    "The edge pairs are converted to polygons covering the area between the edges.\n"
    "The extension parameter specifies a sizing which is applied when converting the edge pairs to polygons. This way, "
    "degenerated edge pairs (i.e. two point-like edges) do not vanish.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("insert_as_polygons", &insert_edge_pairs_as_polygons_with_trans,
    "@brief Insert the edge pairs from the edge pair collection as polygons into this shape container with a transformation\n"
    "@args edge_pairs, e, trans\n"
    "@param edges The edge pairs to insert\n"
    "@param trans The transformation to apply\n"
    "\n"
    "This method inserts all edge pairs from the edge pair collection into this shape container.\n"
    "The edge pairs are converted to polygons covering the area between the edges.\n"
    "The extension parameter specifies a sizing which is applied when converting the edge pairs to polygons. This way, "
    "degenerated edge pairs (i.e. two point-like edges) do not vanish.\n"
    "Before a polygon is inserted into the shape collection, the given transformation is applied.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("insert_as_edges", &insert_edge_pairs_as_edges,
    "@brief Insert the edge pairs from the edge pair collection as individual edges into this shape container\n"
    "@args edge_pairs\n"
    "@param edge_pairs The edge pairs to insert\n"
    "\n"
    "This method inserts all edge pairs from the edge pair collection into this shape container.\n"
    "Each edge from the edge pair is inserted individually into the shape container.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("insert_as_edges", &insert_edge_pairs_as_edges_with_trans,
    "@brief Insert the edge pairs from the edge pair collection as individual into this shape container with a transformation\n"
    "@args edge_pairs, trans\n"
    "@param edges The edge pairs to insert\n"
    "@param trans The transformation to apply\n"
    "\n"
    "This method inserts all edge pairs from the edge pair collection into this shape container.\n"
    "Each edge from the edge pair is inserted individually into the shape container.\n"
    "Before each edge is inserted into the shape collection, the given transformation is applied.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("transform", &transform_shapes,
    "@brief Transform all shapes with the given transformation\n"
    "@args trans\n"
    "This method will invalidate all references to shapes inside this collection.\n\n"
    "It has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("transform", &transform_shapes_icplx,
    "@brief Transform all shapes with the given complex integer transformation\n"
    "@args trans\n"
    "This method will invalidate all references to shapes inside this collection.\n\n"
    "It has been introduced in version 0.23.\n"
  ) +
  gsi::method ("transform", (db::Shape (db::Shapes::*)(const db::Shape &, const db::Trans &)) &db::Shapes::transform,
    "@brief Transform the shape given by the reference with the given transformation\n"
    "@args shape, trans\n"
    "@return A reference (a \\Shape object) to the new shape\n"
    "This method has been introduced in version 0.16.\n"
    "The original shape may be deleted and re-inserted by this method. Therefore, a new reference is returned.\n"
    "It is permitted in editable mode only."
  ) +
  gsi::method_ext ("transform", &transform_shape_cplx,
    "@brief Transform the shape given by the reference with the given complex transformation\n"
    "@args shape, trans\n"
    "@return A reference (a \\Shape object) to the new shape\n"
    "This method has been introduced in version 0.16.\n"
    "The original shape may be deleted and re-inserted by this method. Therefore, a new reference is returned.\n"
    "It is permitted in editable mode only."
  ) +
  gsi::method_ext ("transform", &transform_shape_icplx,
    "@brief Transform the shape given by the reference with the given complex integer space transformation\n"
    "@args shape, trans\n"
    "@return A reference (a \\Shape object) to the new shape\n"
    "This method has been introduced in version 0.22.\n"
    "The original shape may be deleted and re-inserted by this method. Therefore, a new reference is returned.\n"
    "It is permitted in editable mode only."
  ) +
  gsi::method_ext ("replace", &replace<db::Box>,
    "@brief Replace the given shape with a box\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args shape,box\n"
    "\n"
    "This method has been introduced with version 0.16. It replaces the given shape with the "
    "object specified. It does not change the property Id. To change the property Id, "
    "use the \\replace_prop_id method. To replace a shape and discard the property Id, erase the "
    "shape and insert a new shape."
    "\n"
    "This method is permitted in editable mode only."
  ) +
  gsi::method_ext ("replace", &replace<db::Path>,
    "@brief Replace the given shape with a path\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args shape,path\n"
    "\n"
    "This method has been introduced with version 0.16. It replaces the given shape with the "
    "object specified. It does not change the property Id. To change the property Id, "
    "use the \\replace_prop_id method. To replace a shape and discard the property Id, erase the "
    "shape and insert a new shape."
    "\n"
    "This method is permitted in editable mode only."
  ) +
  gsi::method_ext ("replace", &replace<db::Edge>,
    "@brief Replace the given shape with an edge object\n"
    "@args shape,edge\n"
    "\n"
    "This method has been introduced with version 0.16. It replaces the given shape with the "
    "object specified. It does not change the property Id. To change the property Id, "
    "use the \\replace_prop_id method. To replace a shape and discard the property Id, erase the "
    "shape and insert a new shape."
    "\n"
    "This method is permitted in editable mode only."
  ) +
  gsi::method_ext ("replace", &replace<db::Text>,
    "@brief Replace the given shape with a text object\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args shape,text\n"
    "\n"
    "This method has been introduced with version 0.16. It replaces the given shape with the "
    "object specified. It does not change the property Id. To change the property Id, "
    "use the \\replace_prop_id method. To replace a shape and discard the property Id, erase the "
    "shape and insert a new shape."
    "\n"
    "This method is permitted in editable mode only."
  ) +
  gsi::method_ext ("replace", &replace<db::SimplePolygon>,
    "@brief Replace the given shape with a simple polygon\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args shape,simple_polygon\n"
    "\n"
    "This method has been introduced with version 0.16. It replaces the given shape with the "
    "object specified. It does not change the property Id. To change the property Id, "
    "use the \\replace_prop_id method. To replace a shape and discard the property Id, erase the "
    "shape and insert a new shape."
    "\n"
    "This method is permitted in editable mode only."
  ) +
  gsi::method_ext ("replace", &replace<db::Polygon>,
    "@brief Replace the given shape with a polygon\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args shape,polygon\n"
    "\n"
    "This method has been introduced with version 0.16. It replaces the given shape with the "
    "object specified. It does not change the property Id. To change the property Id, "
    "use the \\replace_prop_id method. To replace a shape and discard the property Id, erase the "
    "shape and insert a new shape."
    "\n"
    "This method is permitted in editable mode only."
  ) +
  gsi::method_ext ("insert|#insert_box", &insert<db::Box>,
    "@brief Insert a box into the shapes list\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args box\n"
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_path", &insert<db::Path>,
    "@brief Insert a path into the shapes list\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args path\n"
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_edge", &insert<db::Edge>,
    "@brief Insert a edge into the shapes list\n"
    "@args edge\n"
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_text", &insert<db::Text>,
    "@brief Insert a text into the shapes list\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args text\n"
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_simple_polygon", &insert<db::SimplePolygon>,
    "@brief Insert a simple polygon into the shapes list\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args simple_polygon\n"
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_polygon", &insert<db::Polygon>,
    "@brief Insert a polygon into the shapes list\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "@args polygon\n"
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_box_with_properties", &insert_with_properties<db::Box>,
    "@brief Insert a box with properties into the shapes list\n"
    "@args box, property_id\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "The property Id must be obtained from the \\Layout object's property_id method which "
    "associates a property set with a property Id."
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_path_with_properties", &insert_with_properties<db::Path>,
    "@brief Insert a path with properties into the shapes list\n"
    "@args path, property_id\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "The property Id must be obtained from the \\Layout object's property_id method which "
    "associates a property set with a property Id."
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_edge_with_properties", &insert_with_properties<db::Edge>,
    "@brief Insert a edge with properties into the shapes list\n"
    "@args edge, property_id\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "The property Id must be obtained from the \\Layout object's property_id method which "
    "associates a property set with a property Id."
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_text_with_properties", &insert_with_properties<db::Text>,
    "@brief Insert a text with properties into the shapes list\n"
    "@args text, property_id\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "The property Id must be obtained from the \\Layout object's property_id method which "
    "associates a property set with a property Id."
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_simple_polygon_with_properties", &insert_with_properties<db::SimplePolygon>,
    "@brief Insert a simple polygon with properties into the shapes list\n"
    "@args simple_polygon, property_id\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "The property Id must be obtained from the \\Layout object's property_id method which "
    "associates a property set with a property Id."
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::method_ext ("insert|#insert_polygon_with_properties", &insert_with_properties<db::Polygon>,
    "@brief Insert a polygon with properties into the shapes list\n"
    "@args polygon, property_id\n"
    "@return A reference to the new shape (a \\Shape object)\n"
    "The property Id must be obtained from the \\Layout object's property_id method which "
    "associates a property set with a property Id."
    "\n"
    "Starting with version 0.16, this method returns a reference to the newly created shape\n"
  ) +
  gsi::iterator_ext ("each", &begin, 
    "@brief Get all shapes\n"
    "@args flags\n"
    "\n"
    "@param flags An \"or\"-ed combination of the S... constants\n"
  ) +
  gsi::iterator_ext ("each", &begin_all, 
    "@brief Get all shapes\n"
    "\n"
    "This call is equivalent to each(SAll). This convenience method has been introduced in version 0.16\n"
  ) +
  gsi::iterator_ext ("each_touching", &begin_touching, 
    "@brief Get all shapes that touch the search box (region)\n"
    "@args flags,region\n"
    "This method was introduced in version 0.16\n"
    "\n"
    "@param flags An \"or\"-ed combination of the S... constants\n"
    "@param region The rectangular search region\n"
  ) +
  gsi::iterator_ext ("each_touching", &begin_touching_all, 
    "@brief Get all shapes that touch the search box (region)\n"
    "@args region\n"
    "@param region The rectangular search region\n"
    "\n"
    "This call is equivalent to each_touching(SAll,region). This convenience method has been introduced in version 0.16\n"
  ) +
  gsi::iterator_ext ("each_overlapping", &begin_overlapping, 
    "@brief Get all shapes that overlap the search box (region)\n"
    "@args flags,region\n"
    "This method was introduced in version 0.16\n"
    "\n"
    "@param flags An \"or\"-ed combination of the S... constants\n"
    "@param region The rectangular search region\n"
  ) +
  gsi::iterator_ext ("each_overlapping", &begin_overlapping_all, 
    "@brief Get all shapes that overlap the search box (region)\n"
    "@args region\n"
    "@param region The rectangular search region\n"
    "\n"
    "This call is equivalent to each_overlapping(SAll,region). This convenience method has been introduced in version 0.16\n"
  ) +
  gsi::method ("erase", &db::Shapes::erase_shape, 
    "@brief Erase the shape pointed to by the given \\Shape object\n"
    "@args shape\n"
    "This method has been introduced in version 0.16. It can only be used in editable mode.\n"
    "Erasing a shape will invalidate the shape reference. Access to this reference may then render invalid results.\n"
    "\n"
    "@param shape The shape which to destroy"
  ) +
  gsi::method ("find", (db::Shape (db::Shapes::*)(const db::Shape &) const) &db::Shapes::find, 
    "@brief Finds a shape inside this collected\n"
    "@args shape\n"
    "This method has been introduced in version 0.21.\n"
    "This method tries to find the given shape in this collection. The original shape may be located in another collection. "
    "If the shape is found, this method returns a reference to the shape in this collection, otherwise a null reference is returned."
  ) +
  gsi::method ("is_valid?", &db::Shapes::is_valid, 
    "@brief Test if the given \\Shape object is still pointing to a valid object\n"
    "@args shape\n"
    "This method has been introduced in version 0.16.\n"
    "If the shape represented by the given reference has been deleted, this method returns false. "
    "If however, another shape has been inserted already that occupies the original shape's position, "
    "this method will return true again.\n"
  ) +
  gsi::method ("is_empty?", &db::Shapes::empty, 
    "@brief Returns a value indicating whether the shapes container is empty\n"
    "This method has been introduced in version 0.20.\n"
  ) +
  gsi::method ("clear", &db::Shapes::clear, 
    "@brief Clear the shape container\n"
    "This method has been introduced in version 0.16. It can only be used in editable mode."
  ) +
  gsi::method ("size", (size_t (db::Shapes::*)() const) &db::Shapes::size, 
    "@brief Report the number of shapes in this container\n"
    "This method was introduced in version 0.16\n"
    "@return The number of shapes in this container\n"
  ) +
  gsi::method ("replace_prop_id", (db::Shape (db::Shapes::*) (const db::Shape &, db::properties_id_type)) &db::Shapes::replace_prop_id,
    "@brief Replace (or install) the properties of a shape\n"
    "@args shape,property_id\n"
    "@return A \\Shape object representing the new shape\n"
    "This method has been introduced in version 0.16. It can only be used in editable mode.\n"
    "Changes the properties Id of the given shape or install a properties Id on that shape if it does not have one yet.\n"
    "The property Id must be obtained from the \\Layout object's property_id method which "
    "associates a property set with a property Id.\n"
    "This method will potentially invalidate the shape reference passed to it. Use the reference "
    "returned for future references."
  ) +
  gsi::method ("SAll|#s_all", &s_all,
    "@brief Indicates that all shapes shall be retrieved"
  ) +
  gsi::method ("SAllWithProperties|#s_all_with_properties", &s_all_with_properties,
    "@brief Indicates that all shapes with properties shall be retrieved"
  ) +
  gsi::method ("SPolygons|#s_polygons", &s_polygons,
    "@brief Indicates that polygons shall be retrieved"
  ) +
  gsi::method ("SBoxes|#s_boxes", &s_boxes,
    "@brief Indicates that boxes shall be retrieved"
  ) +
  gsi::method ("SEdges|#s_edges", &s_edges,
    "@brief Indicates that edges shall be retrieved"
  ) +
  gsi::method ("SPaths|#s_paths", &s_paths,
    "@brief Indicates that paths shall be retrieved"
  ) +
  gsi::method ("STexts|#s_texts", &s_texts,
    "@brief Indicates that texts be retrieved"
  ) +
  gsi::method ("SUserObjects|#s_user_objects", &s_user_objects,
    "@brief Indicates that user objects shall be retrieved"
  ) +
  gsi::method ("SProperties|#s_properties", &s_properties,
    "@brief Indicates that only shapes with properties shall be retrieved"
  ) +
  gsi::method_ext ("dump_mem_statistics", &dump_mem_statistics,
    "@hide"
  ),
  "@brief A collection of shapes\n"
  "\n"
  "A shapes collection is a collection of geometrical objects, such as "
  "polygons, boxes, paths, edges or text objects.\n"
  "\n"
  "Shapes objects are the basic containers for geometrical objects of a cell. Inside a cell, there is "
  "one Shapes object per layer.\n"
);

}

