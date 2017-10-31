
$:.push(File::dirname($0))

load("test_prologue.rb")

class LAYSession_TestClass < TestBase

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

    mw.reader_options.set_layer_map( RBA::LayerMap::new, true )
    mw.reader_options.text_enabled = true
    mw.reader_options.properties_enabled = true
    mw.load_layout( "../../testdata/gds/t11.gds", 1 )  # @@@

    mw.reader_options.create_other_layers = false
    mw.reader_options.layer_map.map( RBA::LayerInfo::new(1, 0), 1 )
    mw.reader_options.layer_map.map( "2/0", 0 )
    mw.reader_options.layer_map.map( "3/0", 2 )
    mw.reader_options.layer_map.map( "3/1", 10 )
    mw.reader_options.text_enabled = false
    mw.reader_options.properties_enabled = true
    mw.load_layout( "../../testdata/gds/t10.gds", 1 )  # @@@

    mw.select_view(0)

    cv = mw.current_view

    cv.clear_layers

    pos = cv.end_layers

    l1 = cv.insert_layer( pos, RBA::LayerPropertiesNode::new )

    new_p = RBA::LayerPropertiesNode::new
    new_p.source = "1/0@1"
    l11 = cv.insert_layer( pos.last_child, new_p )

    mw.save_session("tmp.lys")

    mw.close_all

    assert_equal(0, mw.views);

    mw.restore_session("tmp.lys")

    assert_equal(2, mw.views);

    cv = mw.current_view
    assert_equal( cv.title, "t11.gds" )
    assert_equal( lnodes_str( "", cv.begin_layers ), "*/*@*\n  1/0@1\n" )

    mw.select_view(1)

    cv = mw.current_view
    assert_equal( cv.title, "t10.gds" )
    assert_equal( lnodes_str( "", cv.begin_layers ), "1/0@1\n2/0@1\n3/0@1\n3/1@1\n" )

  end

end

load("test_epilogue.rb")
