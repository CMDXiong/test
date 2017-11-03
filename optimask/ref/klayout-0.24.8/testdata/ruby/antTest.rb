
$:.push(File::dirname($0))

load("test_prologue.rb")

def annot_obj( p1, p2, fmt, fmt_x, fmt_y, style, outline, snap, ac )
  a = RBA::Annotation::new
  a.p1 = p1
  a.p2 = p2
  a.fmt = fmt
  a.fmt_x = fmt_x
  a.fmt_y = fmt_y
  a.style = style
  a.outline = outline
  a.snap = snap
  a.angle_constraint = ac
  return a
end

class RBA::Annotation
  def to_s
    "p1=" + p1.to_s + ", p2=" + p2.to_s + ", fmt=" + fmt + ", fmt_x=" + fmt_x + ", fmt_y=" + fmt_y +
      ", style=" + style.to_s + ", outline=" + outline.to_s + ", snap=" + snap?.to_s + ", ac=" + angle_constraint.to_s
  end
end

class Ant_TestClass < TestBase

  # Annotation object
  def test_1

    a = RBA::Annotation::new
    assert_equal( a.to_s, "p1=0,0, p2=0,0, fmt=$D, fmt_x=$X, fmt_y=$Y, style=0, outline=0, snap=true, ac=5" )

    a = annot_obj( RBA::DPoint::new_xy( 1.0, 2.0 ), RBA::DPoint::new_xy( 3.0, 4.0 ), "a", "b", "c", 1, 2, false, 3 )
    assert_equal( a.to_s, "p1=1,2, p2=3,4, fmt=a, fmt_x=b, fmt_y=c, style=1, outline=2, snap=false, ac=3" )

    b = a.dup
    a = RBA::Annotation::new

    assert_equal( a == b, false )
    assert_equal( a != b, true )

    a = annot_obj( RBA::DPoint::new_xy( 1.0, 2.0 ), RBA::DPoint::new_xy( 3.0, 4.0 ), "a", "b", "c", 1, 2, false, 3 )
    assert_equal( a == b, true )
    assert_equal( a != b, false )

    c = a.transformed( RBA::DTrans::new_u( RBA::DPoint::new_xy( 10.0, 20.0 ) ) )
    assert_equal( c.to_s, "p1=11,22, p2=13,24, fmt=a, fmt_x=b, fmt_y=c, style=1, outline=2, snap=false, ac=3" )

    c = a.transformed_cplx( RBA::DCplxTrans::new_t( RBA::DTrans::new_u( RBA::DPoint::new_xy( 11.0, 22.0 ) ) ) )
    assert_equal( c.to_s, "p1=12,24, p2=14,26, fmt=a, fmt_x=b, fmt_y=c, style=1, outline=2, snap=false, ac=3" )

    a.fmt = "X=$X,Y=$Y,D=$D,A=$A,P=$P,Q=$Q,U=$U,V=$V"
    a.fmt_x = "$(X*1000)+$(Y*1e3)"
    assert_equal( a.text == "X=2,Y=2,D=2.8284271,A=4e-06,P=3,Q=4,U=1,V=2" || a.text == "X=2,Y=2,D=2.8284271,A=4e-006,P=3,Q=4,U=1,V=2", true )
    assert_equal( a.text_x, "2000+2000" )
    assert_equal( a.text_y, "c" )

  end

  # Test LayoutView integration
  def test_2

    mw = RBA::Application::instance.main_window
    mw.create_layout( 0 )
    lv = mw.current_view

    lv.insert_annotation( RBA::Annotation::new )
    a = annot_obj( RBA::DPoint::new_xy( 1.0, 2.0 ), RBA::DPoint::new_xy( 3.0, 4.0 ), "a", "b", "c", 1, 2, false, 3 )
    lv.insert_annotation( a )

    arr = []
    lv.each_annotation { |a| arr.push( a.to_s ) }
    assert_equal( arr.join( ";" ), "p1=0,0, p2=0,0, fmt=$D, fmt_x=$X, fmt_y=$Y, style=0, outline=0, snap=true, ac=5;p1=1,2, p2=3,4, fmt=a, fmt_x=b, fmt_y=c, style=1, outline=2, snap=false, ac=3" )

    id = nil
    lv.each_annotation { |a| id = a.id }
    a.fmt = "u"
    lv.replace_annotation(id, a)

    arr = []
    lv.each_annotation { |a| arr.push( a.to_s ) }
    assert_equal( arr.join( ";" ), "p1=0,0, p2=0,0, fmt=$D, fmt_x=$X, fmt_y=$Y, style=0, outline=0, snap=true, ac=5;p1=1,2, p2=3,4, fmt=u, fmt_x=b, fmt_y=c, style=1, outline=2, snap=false, ac=3" )

    lv.erase_annotation(id)

    arr = []
    lv.each_annotation { |a| arr.push( a.to_s ) }
    assert_equal( arr.join( ";" ), "p1=0,0, p2=0,0, fmt=$D, fmt_x=$X, fmt_y=$Y, style=0, outline=0, snap=true, ac=5" )

    lv.clear_annotations

    arr = []
    lv.each_annotation { |a| arr.push( a.to_s ) }
    assert_equal( arr.join( ";" ), "" )

  end

end

load("test_epilogue.rb")

