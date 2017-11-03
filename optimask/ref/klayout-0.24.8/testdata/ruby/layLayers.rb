
$:.push(File::dirname($0))

load("test_prologue.rb")

class LAYLayers_TestClass < TestBase

  def lnode_str( space, l )
    return space + l.current.source( true ) + "\n";
  end

  def lnodes_str( space, l )
    res = ""
    while ! l.at_end?  
      res += lnode_str( space, l )
      if (l.current.has_children?) 
        l.down_first_child
        res += lnodes_str( "  #{space}", l )
        l.up
      end
      l.next_sibling( 1 )
    end
    return res
  end

  def test_1

    app = RBA::Application.instance
    mw = app.main_window

    mw.load_layout( "../../testdata/gds/t11.gds", 1 )  # @@@
    mw.load_layout( "../../testdata/gds/t10.gds", 2 )  # @@@

    cv = mw.current_view

    cv.clear_layers

    assert_equal( lnodes_str( "", cv.begin_layers ), "" )

    pos = cv.end_layers
    assert_equal( pos.parent.is_null?, true )
    p = pos.dup
    p.up
    assert_equal( p.is_null?, true )
    assert_equal( pos.is_null?, false )

    assert_equal( pos == cv.begin_layers, true )
    assert_equal( pos != cv.begin_layers, false )

    l1 = cv.insert_layer( pos, RBA::LayerProperties::new )

    assert_equal( pos == cv.begin_layers, true )
    assert_equal( pos != cv.begin_layers, false )
    assert_equal( pos == cv.end_layers, false )
    assert_equal( pos != cv.end_layers, true )
    assert_equal( pos < cv.end_layers, true )
    assert_equal( cv.end_layers < pos, false )
    assert_equal( pos < cv.begin_layers, false )
    assert_equal( cv.begin_layers < pos, false )
    assert_equal( pos.at_top?, true )

    assert_equal( lnodes_str( "", cv.begin_layers ), "*/*@*\n" )

    new_p = RBA::LayerProperties::new
    new_p.source = "1/0@1"
    l11 = cv.insert_layer( pos.last_child, new_p )

    p12 = pos.last_child
    assert_equal( p12.parent.is_null?, false )
    assert_equal( p12.parent == pos, true )

    pp = pos.dup
    pp.down_last_child
    assert_equal( pp == p12, true )
    assert_equal( pp == pos, false )
    assert_equal( pp.parent == pos, true )
    pp.up
    assert_equal( pp == pos, true )

    assert_equal( p12.at_top?, false )
    p12.to_sibling( 0 )
    assert_equal( p12 == pos.first_child, true )
    assert_equal( p12.child_index, 0 )
    p12.to_sibling( 1 )
    assert_equal( p12.child_index, 1 )
    assert_equal( p12 == pos.last_child, true )
    assert_equal( p12.num_siblings, 1 )

    l12 = cv.insert_layer( p12, RBA::LayerProperties::new )
    l12_new = RBA::LayerProperties::new
    l12_new.source = "1/0@2"
    cv.set_layer_properties( p12, l12_new )

    assert_equal( p12.current.cellview, 1 )
    assert_equal( p12.current.has_upper_hier_level?(true), false )
    assert_equal( p12.current.has_lower_hier_level?(true), false )

    l12_new.source = "@* #1..2"
    cv.set_layer_properties( p12, l12_new )

    assert_equal( p12.current.cellview, 0 )
    assert_equal( p12.current.has_upper_hier_level?(true), true )
    assert_equal( p12.current.has_upper_hier_level?, true )
    assert_equal( p12.current.upper_hier_level(true), 2 )
    assert_equal( p12.current.upper_hier_level, 2 )
    assert_equal( p12.current.has_lower_hier_level?(true), true )
    assert_equal( p12.current.has_lower_hier_level?, true )
    assert_equal( p12.current.lower_hier_level(true), 1 )
    assert_equal( p12.current.lower_hier_level, 1 )

    l12_new.source = "@* (0,0 *0.5) (0,5 r45 *2.5)"
    cv.set_layer_properties( p12, l12_new )
    trans = p12.current.trans( true )
    assert_equal( trans.to_s, p12.current.trans.to_s )
    assert_equal( trans.size, 2 )
    assert_equal( trans [0].to_s, "r0 *0.5 0,0" )
    assert_equal( trans [1].to_s, "r45 *2.5 0,5" )

    l12_new.source = "1/0@2"
    cv.set_layer_properties( p12, l12_new )

    assert_equal( p12.num_siblings, 2 )

    pos = cv.end_layers

    new_p = RBA::LayerProperties::new
    new_p.source = "@1"
    l2 = cv.insert_layer( pos, new_p )

    new_p = RBA::LayerProperties::new
    new_p.source = "7/0@*"
    l21 = cv.insert_layer( pos.first_child, new_p )

    p22 = pos.last_child
    new_p = RBA::LayerProperties::new
    l22 = cv.insert_layer( pos.last_child, new_p )

    assert_equal( lnodes_str( "", cv.begin_layers ), "*/*@*\n  1/0@1\n  1/0@2\n*/*@1\n  7/0@1\n  */*@1\n" )

    new_p = l2.dup
    new_p.source = "@2"
    cv.set_layer_properties( pos, new_p )

    assert_equal( lnodes_str( "", cv.begin_layers ), "*/*@*\n  1/0@1\n  1/0@2\n*/*@2\n  7/0@2\n  */*@2\n" )

    l22_new = RBA::LayerProperties::new
    l22_new.source = "7/1@*"
    cv.replace_layer_node( p22, l22_new )

    assert_equal( lnodes_str( "", cv.begin_layers ), "*/*@*\n  1/0@1\n  1/0@2\n*/*@2\n  7/0@2\n  7/1@2\n" )

    cv.delete_layer( p22 )

    assert_equal( lnodes_str( "", cv.begin_layers ), "*/*@*\n  1/0@1\n  1/0@2\n*/*@2\n  7/0@2\n" )

    new_p = l2.dup
    new_p.source = "%5@2"
    cv.set_layer_properties( pos, new_p )

    assert_equal( lnodes_str( "", cv.begin_layers ), "*/*@*\n  1/0@1\n  1/0@2\n%5@2\n  %5@2\n" )

    mw.close_all

  end

  def test_1a

    app = RBA::Application.instance
    mw = app.main_window

    mw.load_layout( "../../testdata/gds/t11.gds", 1 )  # @@@
    mw.load_layout( "../../testdata/gds/t10.gds", 2 )  # @@@

    cv = mw.current_view

    cv.clear_layers

    cv.insert_layer_list(1)
    cv.rename_layer_list(1, "x")
    assert_equal(cv.current_layer_list, 1)
    cv.set_current_layer_list(0)
    assert_equal(cv.current_layer_list, 0)
    cv.set_current_layer_list(1)
    assert_equal(cv.current_layer_list, 1)

    assert_equal( lnodes_str( "", cv.begin_layers(0) ), "" )
    assert_equal( lnodes_str( "", cv.begin_layers(1) ), "" )

    pos = cv.end_layers(0)
    assert_equal( pos.parent.is_null?, true )
    p = pos.dup
    p.up
    assert_equal( p.is_null?, true )
    assert_equal( pos.is_null?, false )

    assert_equal( pos == cv.begin_layers(0), true )
    assert_equal( pos != cv.begin_layers(0), false )

    l1 = cv.insert_layer( 0, pos, RBA::LayerProperties::new )

    assert_equal( pos == cv.begin_layers(0), true )
    assert_equal( pos != cv.begin_layers(0), false )
    assert_equal( pos == cv.end_layers(0), false )
    assert_equal( pos != cv.end_layers(0), true )
    assert_equal( pos < cv.end_layers(0), true )
    assert_equal( cv.end_layers(0) < pos, false )
    assert_equal( pos < cv.begin_layers(0), false )
    assert_equal( cv.begin_layers(0) < pos, false )
    assert_equal( pos.at_top?, true )

    assert_equal( lnodes_str( "", cv.begin_layers(0) ), "*/*@*\n" )
    assert_equal( lnodes_str( "", cv.begin_layers(1) ), "" )
    assert_equal( lnodes_str( "", cv.begin_layers() ), "" )

    new_p = RBA::LayerProperties::new
    new_p.source = "1/0@1"
    l11 = cv.insert_layer( 0, pos.last_child, new_p )

    p12 = pos.last_child
    assert_equal( p12.parent.is_null?, false )
    assert_equal( p12.parent == pos, true )

    pp = pos.dup
    pp.down_last_child
    assert_equal( pp == p12, true )
    assert_equal( pp == pos, false )
    assert_equal( pp.parent == pos, true )
    pp.up
    assert_equal( pp == pos, true )

    assert_equal( p12.at_top?, false )
    p12.to_sibling( 0 )
    assert_equal( p12 == pos.first_child, true )
    assert_equal( p12.child_index, 0 )
    p12.to_sibling( 1 )
    assert_equal( p12.child_index, 1 )
    assert_equal( p12 == pos.last_child, true )
    assert_equal( p12.num_siblings, 1 )

    l12 = cv.insert_layer( 0, p12, RBA::LayerProperties::new )
    l12_new = RBA::LayerProperties::new
    l12_new.source = "1/0@2"
    cv.set_layer_properties( 0, p12, l12_new )

    assert_equal( p12.current.cellview, 1 )
    assert_equal( p12.current.has_upper_hier_level?(true), false )
    assert_equal( p12.current.has_upper_hier_level?, false )
    assert_equal( p12.current.has_lower_hier_level?(true), false )
    assert_equal( p12.current.has_lower_hier_level?, false )

    l12_new.source = "@* #1..2"
    cv.set_layer_properties( 0, p12, l12_new )

    assert_equal( p12.current.cellview, 0 )
    assert_equal( p12.current.has_upper_hier_level?(true), true )
    assert_equal( p12.current.has_upper_hier_level?, true )
    assert_equal( p12.current.upper_hier_level(true), 2 )
    assert_equal( p12.current.upper_hier_level, 2 )
    assert_equal( p12.current.has_lower_hier_level?(true), true )
    assert_equal( p12.current.has_lower_hier_level?, true )
    assert_equal( p12.current.lower_hier_level(true), 1 )
    assert_equal( p12.current.lower_hier_level, 1 )

    l12_new.source = "@* (0,0 *0.5) (0,5 r45 *2.5)"
    cv.set_layer_properties( 0, p12, l12_new )
    trans = p12.current.trans( true )
    assert_equal( trans.size, 2 )
    assert_equal( trans [0].to_s, "r0 *0.5 0,0" )
    assert_equal( trans [1].to_s, "r45 *2.5 0,5" )

    l12_new.source = "1/0@2"
    cv.set_layer_properties( 0, p12, l12_new )

    assert_equal( p12.num_siblings, 2 )

    pos = cv.end_layers(0)

    new_p = RBA::LayerProperties::new
    new_p.source = "@1"
    l2 = cv.insert_layer( 0, pos, new_p )

    new_p = RBA::LayerProperties::new
    new_p.source = "7/0@*"
    l21 = cv.insert_layer( 0, pos.first_child, new_p )

    p22 = pos.last_child
    new_p = RBA::LayerProperties::new
    l22 = cv.insert_layer( 0, pos.last_child, new_p )

    assert_equal( lnodes_str( "", cv.begin_layers(0) ), "*/*@*\n  1/0@1\n  1/0@2\n*/*@1\n  7/0@1\n  */*@1\n" )
    assert_equal( lnodes_str( "", cv.begin_layers(1) ), "" )
    assert_equal( lnodes_str( "", cv.begin_layers ), "" )

    new_p = l2.dup
    new_p.source = "@2"
    cv.set_layer_properties( 0, pos, new_p )

    assert_equal( lnodes_str( "", cv.begin_layers(0) ), "*/*@*\n  1/0@1\n  1/0@2\n*/*@2\n  7/0@2\n  */*@2\n" )

    l22_new = RBA::LayerProperties::new
    l22_new.source = "7/1@*"
    cv.replace_layer_node( 0, p22, l22_new )

    assert_equal( lnodes_str( "", cv.begin_layers(0) ), "*/*@*\n  1/0@1\n  1/0@2\n*/*@2\n  7/0@2\n  7/1@2\n" )

    cv.delete_layer( 0, p22 )

    assert_equal( lnodes_str( "", cv.begin_layers(0) ), "*/*@*\n  1/0@1\n  1/0@2\n*/*@2\n  7/0@2\n" )

    new_p = l2.dup
    new_p.source = "%5@2"
    cv.set_layer_properties( 0, pos, new_p )

    assert_equal( lnodes_str( "", cv.begin_layers(0) ), "*/*@*\n  1/0@1\n  1/0@2\n%5@2\n  %5@2\n" )

    # build a tree by building a separate tree
    new_p = RBA::LayerPropertiesNode::new
    assert_equal(new_p.has_children?, false)
    n1 = new_p.add_child(RBA::LayerProperties::new)
    assert_equal(n1.has_children?, false)
    n1.source = "101/0"
    n2 = RBA::LayerPropertiesNode::new
    assert_equal(n2.has_children?, false)
    n21 = n2.add_child(RBA::LayerProperties::new)
    n21.source = "102/0"
    assert_equal(n2.has_children?, true)
    n22 = n2.add_child(RBA::LayerProperties::new)
    assert_equal(n2.has_children?, true)
    n22.source = "103/0"
    new_p.add_child(n2)
    assert_equal(new_p.has_children?, true)

    p = pos.last_child
    ll = cv.insert_layer( 0, p, new_p )
    assert_equal(p.current.has_children?, true)
    assert_equal(p.first_child.current.has_children?, false)
    assert_equal(p.first_child.current.source(false), "101/0@1")
    assert_equal(p.first_child.current.source, "%5@1")

    # (test clear_children):
    new_p.clear_children
    assert_equal(new_p.has_children?, false)

    assert_equal(ll.has_children?, true)

    cv.transaction("Delete")
    li = cv.begin_layers(0)
    a = []
    while !li.at_end?
      a.push(li.dup)
      li.next
    end
    assert_equal(a.size, 10)
    cv.delete_layers(0, a)
    assert_equal(cv.begin_layers(0).at_end?, true)
    cv.commit
    mw.cm_undo 
    assert_equal(cv.begin_layers(0).at_end?, false)

    cv.transaction("Delete")
    i = 0
    while !cv.begin_layers(0).at_end?
      cv.delete_layer(0, cv.begin_layers(0))
      i += 1
    end
    assert_equal(i, 2)
    assert_equal(cv.begin_layers(0).at_end?, true)
    cv.commit
    mw.cm_undo 
    assert_equal(cv.begin_layers(0).at_end?, false)

    mw.close_all

  end

  def test_2

    p = RBA::LayerPropertiesNode::new

    assert_equal( p.source( false ), "*/*@*" )
    assert_equal( p.source, "*/*@*" )
    assert_equal( p.has_source_name?( false ), false )
    assert_equal( p.has_source_name?, false )

    p.source_name = "x"
    assert_equal( p.source_name( false ), "x" )
    assert_equal( p.source_name, "x" )
    assert_equal( p.source( false ), "x@*" )
    assert_equal( p.source, "x@*" )
    assert_equal( p.flat.source, "x@*" )
    assert_equal( p.dup.source, "x@*" )
    assert_equal( p.has_source_name?( false ), true )
    assert_equal( p.has_source_name?, true )

    p.clear_source_name
    assert_equal( p.source( false ), "*/*@*" )
    assert_equal( p.has_source_name?( false ), false )

    p.source_layer_index = 6
    assert_equal( p.source( false ), "%6@*" )
    assert_equal( p.source_layer_index( false ), 6 )

    p.source_layer = 6
    p.source_datatype = 5
    assert_equal( p.source( false ), "%6@*" )

    p.source_layer_index = -1
    assert_equal( p.source( false ), "6/5@*" )
    assert_equal( p.source_layer_index( false ), -1 )

    arr = [ RBA::CplxTrans.new_f( RBA::CplxTrans::m45 ), RBA::CplxTrans.new_f( RBA::CplxTrans::r270 ) ]
    p.trans = arr
    assert_equal( p.source( false ), "6/5@* (m45 *1 0,0) (r270 *1 0,0)" )
    assert_equal( arr == p.trans( false ), true )

    p.source_cellview = 1 
    assert_equal( p.source( false ), "6/5@2 (m45 *1 0,0) (r270 *1 0,0)" )
    assert_equal( p.flat.source, "6/5@2 (m45 *1 0,0) (r270 *1 0,0)" )
    p.source_cellview = -1 
    assert_equal( p.source( false ), "6/5@* (m45 *1 0,0) (r270 *1 0,0)" )

    p.upper_hier_level = 17
    assert_equal( p.source( false ), "6/5@* (m45 *1 0,0) (r270 *1 0,0) #..17" )
    assert_equal( p.upper_hier_level( false ), 17 )
    assert_equal( p.has_upper_hier_level?( false ), true )
    p.clear_upper_hier_level 
    assert_equal( p.source( false ), "6/5@* (m45 *1 0,0) (r270 *1 0,0)" )
    assert_equal( p.has_upper_hier_level?( false ), false )

    p.lower_hier_level = 17
    assert_equal( p.source( false ), "6/5@* (m45 *1 0,0) (r270 *1 0,0) #17.." )
    assert_equal( p.source, "6/5@* (m45 *1 0,0) (r270 *1 0,0) #17.." )
    assert_equal( p.lower_hier_level( false ), 17 )
    assert_equal( p.lower_hier_level, 17 )
    assert_equal( p.has_lower_hier_level?( false ), true )
    assert_equal( p.has_lower_hier_level?, true )
    p.clear_lower_hier_level 
    assert_equal( p.source( false ), "6/5@* (m45 *1 0,0) (r270 *1 0,0)" )
    assert_equal( p.source, "6/5@* (m45 *1 0,0) (r270 *1 0,0)" )
    assert_equal( p.has_lower_hier_level?( false ), false )
    assert_equal( p.has_lower_hier_level?, false )

    p.dither_pattern = 18
    assert_equal( p.dither_pattern(true), 18)
    assert_equal( p.flat.dither_pattern(true), 18)
    assert_equal( p.dither_pattern, 18)

    p.animation = 2
    assert_equal( p.animation(true), 2)
    assert_equal( p.flat.animation(true), 2)
    assert_equal( p.animation, 2)

    p.marked = true
    assert_equal( p.marked?(true), true)
    assert_equal( p.flat.marked?(true), true)
    assert_equal( p.marked?, true)

    p.marked = false
    assert_equal( p.marked?(false), false)
    assert_equal( p.flat.marked?(false), false)
    assert_equal( p.marked?, false)

    p.transparent = true
    assert_equal( p.transparent?(true), true)
    assert_equal( p.flat.transparent?(true), true)
    assert_equal( p.transparent?, true)

    p.transparent = false
    assert_equal( p.transparent?(false), false)
    assert_equal( p.flat.transparent?(false), false)
    assert_equal( p.transparent?, false)

    p.visible = true
    assert_equal( p.visible?(true), true)
    assert_equal( p.flat.visible?(true), true)
    assert_equal( p.visible?, true)

    p.visible = false
    assert_equal( p.visible?(false), false)
    assert_equal( p.flat.visible?(false), false)
    assert_equal( p.visible?, false)

    p.valid = true
    assert_equal( p.valid?(true), true)
    assert_equal( p.flat.valid?(true), true)
    assert_equal( p.valid?, true)

    p.valid = false
    assert_equal( p.valid?(false), false)
    assert_equal( p.flat.valid?(false), false)
    assert_equal( p.valid?, false)

    p.width = 3
    assert_equal( p.width(true), 3)
    assert_equal( p.flat.width(true), 3)
    assert_equal( p.width, 3)

    p.frame_color = 0xff000031
    assert_equal( p.frame_color(true), 0xff000031)
    assert_equal( p.flat.frame_color(true), 0xff000031)
    assert_equal( p.frame_color, 0xff000031)

    p.fill_color = 0xff000032
    assert_equal( p.fill_color(true), 0xff000032)
    assert_equal( p.flat.fill_color(true), 0xff000032)
    assert_equal( p.fill_color, 0xff000032)

    p.frame_brightness = 41
    assert_equal( p.frame_brightness(true), 41)
    assert_equal( p.flat.frame_brightness(true), 41)
    assert_equal( p.frame_brightness, 41)

    p.fill_brightness = 42
    assert_equal( p.fill_brightness(true), 42)
    assert_equal( p.flat.fill_brightness(true), 42)
    assert_equal( p.fill_brightness, 42)

  end

end

load("test_epilogue.rb")
