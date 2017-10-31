
$:.push(File::dirname($0))

load("test_prologue.rb")

# an event filter class

class EventFilter < RBA::QObject

  alias orgEventFilter eventFilter

  def initialize 
    @log = []
  end

  def log
    @log
  end

  def eventFilter(obj, event)
    @log << (event.class.to_s + ": " + event.type.to_s)
    self.orgEventFilter(obj, event)
  end

end

# QAction implementation

class MyAction < RBA::QAction

  def initialize(p, n)
    super(p)
    self.objectName = n
    @ce = nil
  end

  def childEvent(ev)
    if @ce
      @ce.call(ev)
    end
  end
  
  def on_child_event(&ce)
    @ce = ce
  end
  
end

# QObject implementation 

class MyObject < RBA::QObject

  def initialize
    @ef = nil
    super(nil)
  end

  alias baseEventFilter eventFilter
    
  def eventFilter(watched, event)
    if @ef.call(watched, event)
      return true
    end
    baseEventFilter(watched, event)
  end

  def on_event_filter(&ef)
    @ef = ef
  end
  
end

class QtBinding_TestClass < TestBase

  def test_00

    # initial test 

  end

  def test_10

    a = MyAction.new(nil, "a")
    a.text = "mytext"
    a.checkable = true
    assert_equal(a.isChecked, false)
    a.checked = true
    assert_equal(a.text, "mytext")
    assert_equal(a.objectName, "a")
    a.text += "."
    assert_equal(a.text, "mytext.")
    assert_equal(a.isChecked, true)

    t = ""
    a.triggered do |checked|
      t += "[#{checked}]";
    end
    assert_equal(t, "")
    a.trigger # also toggles checked state
    assert_equal(t, "[false]")
    t = ""
    a.trigger # also toggles checked state
    assert_equal(t, "[true]")

    #  force GC and destruction
    a = nil
    GC.start
    a = MyAction.new(nil, "anew")
    assert_equal(a.objectName, "anew")
    a = nil
    GC.start

  end

  def test_11

    a = RBA::QAction.new(nil)
    aa = MyAction.new(a, "aa")
    assert_equal(aa.objectName, "aa")

    # destroying a will also destroy aa
    a.destroy
    assert_equal(a.destroyed?, true)
    assert_equal(aa.destroyed?, true)

  end

  def test_12

    a = RBA::QAction.new(nil)
    aa = RBA::QAction.new(a)
    aa.objectName = "aa"

    # destroying in the GC will also destroy aa
    a = nil
    GC.start
    a = RBA::QAction.new(nil)
    a = nil
    GC.start

    assert_equal(aa.destroyed?, true)

  end

  def test_13

    a = RBA::QAction.new(nil)
    aa = RBA::QAction.new(a)
    aa.objectName = "aa"
    aa.text = "aatext"

    cc = []
    a.children.each do |c|
      cc.push(c.objectName)
    end
    assert_equal(cc.join(","), "aa")

    # aa now is kept by a
    aa = nil
    GC.start
    aa = RBA::QAction.new(nil)
    aa = nil
    GC.start

    # fetch aa again
    a.children.each do |c|
      if c.objectName == "aa"
        aa = c
      end
    end
    assert_equal(aa != nil, true)
    assert_equal(aa.class.to_s, "RBA::QAction")
    assert_equal(aa.text, "aatext")
    assert_equal(aa.destroyed?, false)

  end

  def test_20

    no_event = false

    x = []
    ef = MyObject.new
    ef.on_event_filter do |watched,event|
      x.push(watched.inspect + ":" + event.inspect)
      no_event
    end

    ce_log = []
    a = MyAction.new(nil, "a")
    a.on_child_event do |ce|
      ce_log.push("#{ce.added}:#{ce.child.objectName}")
    end

    a.installEventFilter(ef)

    aa = MyAction.new(nil, "aa")
    assert_equal(ce_log.join(","), "")
    aa.setParent(a)
    assert_equal(ce_log.join(","), "true:aa")
    ce_log = []

    # destroy aa
    aa.destroy
    aa = nil
    assert_equal(ce_log.join(","), "false:aa")
    ce_log = []

    no_event = true
    aa = MyAction.new(nil, "aa")
    aa.setParent(a)
    assert_equal(ce_log.join(","), "")
    ce_log = []

    no_event = false
    aa.destroy
    aa = nil
    assert_equal(ce_log.join(","), "false:aa")
    ce_log = []

  end

  def test_30

    # dialog construction, cleanup, object dependency ...

    mw = nil # more fun than with RBA::Application::instance.main_window because Ruby's GC does all the cleanup

    dialog = RBA::QDialog::new(mw)
    label = RBA::QLabel::new(dialog)
    layout = RBA::QHBoxLayout::new(dialog)
    layout.addWidget(label)
    GC.start

    dialog = RBA::QDialog::new(mw)
    label = RBA::QLabel::new(dialog)
    layout = RBA::QHBoxLayout::new(dialog)
    layout.addWidget(label)
    label.destroy
    GC.start

    dialog = RBA::QDialog::new(mw)
    label = RBA::QLabel::new(dialog)
    layout = RBA::QHBoxLayout::new(dialog)
    layout.addWidget(label)
    layout.destroy
    GC.start

    dialog = RBA::QDialog::new(mw)
    label = RBA::QLabel::new(dialog)
    layout = RBA::QHBoxLayout::new(dialog)
    layout.addWidget(label)
    dialog.destroy
    GC.start

    dialog = RBA::QDialog::new(mw)
    label = RBA::QLabel::new(dialog)
    layout = RBA::QHBoxLayout::new(dialog)
    layout.addWidget(label)
    GC.start

  end

  def test_31

    # Optional arguments, enums, QFlag's

    mw = nil # more fun than with RBA::Application::instance.main_window because Ruby's GC does all the cleanup

    mb = RBA::QMessageBox::new(RBA::QMessageBox::Critical, "title", "text")
    assert_equal(mb.icon.to_i != RBA::QMessageBox::Warning.to_i, true)
    assert_equal(mb.icon.to_i == RBA::QMessageBox::Critical.to_i, true)
    assert_equal(mb.standardButtons.to_i == RBA::QMessageBox::NoButton.to_i, true)

    mb = RBA::QMessageBox::new(RBA::QMessageBox::Critical, "title", "text", RBA::QMessageBox::Ok)
    assert_equal(mb.standardButtons.to_i == RBA::QMessageBox::Ok.to_i, true)

    mb = RBA::QMessageBox::new(RBA::QMessageBox::Critical, "title", "text", RBA::QMessageBox::Ok | RBA::QMessageBox::Cancel)
    assert_equal(mb.standardButtons.to_i == RBA::QMessageBox::Ok.to_i + RBA::QMessageBox::Cancel.to_i, true)

  end

  def test_40

    # Lifetime management of objects/methods not using QObject::parent
    # QTreeWidget (parent)/QTreeWidgetItem (child)

    # constructor with parent-like argument:
    tw = RBA::QTreeWidget::new
    ti = RBA::QTreeWidgetItem::new(tw)
    # strange, but true:
    assert_equal(ti.parent, nil)
    assert_equal(tw.topLevelItemCount, 1)
    ti = nil
    # force delete of the ti item unless it's referenced
    # NOTE: it's not sufficient to just set ti to nil, we also
    # have to create a new QTreeWidgetItem (special for Ruby >1.9?)
    ti = RBA::QTreeWidgetItem::new
    GC.start
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    assert_equal(tw.topLevelItemCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.topLevelItem(0)
    tw._destroy
    # gives true, because tw owns ti too.
    assert_equal(ti._destroyed?, true)

    # The same works for insert too
    tw = RBA::QTreeWidget::new
    ti = RBA::QTreeWidgetItem::new
    tw.insertTopLevelItem(0, ti)
    assert_equal(tw.topLevelItemCount, 1)
    ti = nil
    # force delete of the ti item unless it's referenced
    ti = RBA::QTreeWidgetItem::new
    GC.start
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    assert_equal(tw.topLevelItemCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.topLevelItem(0)
    tw._destroy
    # gives true, because tw owns ti
    assert_equal(ti._destroyed?, true)

    # And add:
    tw = RBA::QTreeWidget::new
    ti = RBA::QTreeWidgetItem::new
    tw.addTopLevelItem(ti)
    assert_equal(tw.topLevelItemCount, 1)
    ti = nil
    # force delete of the ti item unless it's referenced
    ti = RBA::QTreeWidgetItem::new
    GC.start
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    assert_equal(tw.topLevelItemCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.topLevelItem(0)
    tw._destroy
    # gives true, because tw owns ti
    assert_equal(ti._destroyed?, true)

    # But the item is released when we take it and add:
    tw = RBA::QTreeWidget::new
    ti = RBA::QTreeWidgetItem::new
    tw.addTopLevelItem(ti)
    assert_equal(tw.topLevelItemCount, 1)
    ti = nil
    # force delete of the ti item unless it's referenced
    ti = RBA::QTreeWidgetItem::new
    GC.start
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    assert_equal(tw.topLevelItemCount, 1)

    ti = tw.takeTopLevelItem(0)
    tw._destroy
    # gives false, because we took ti and tw no longer owns it
    assert_equal(ti._destroyed?, false)

    # And we can destroy a child too
    tw = RBA::QTreeWidget::new
    ti = RBA::QTreeWidgetItem::new
    tw.addTopLevelItem(ti)
    assert_equal(tw.topLevelItemCount, 1)
    ti._destroy
    assert_equal(tw.topLevelItemCount, 0)

  end

  def test_41

    # Lifetime management of objects/methods not using QObject::parent
    # QTreeWidgetItem (parent)/QTreeWidgetItem (child)

    # constructor with parent-like argument (supported by QObject parent/child relationship):
    tw = RBA::QTreeWidgetItem::new
    ti = RBA::QTreeWidgetItem::new(tw)
    # that's not QObject::parent - this one still is 0 (not seen by RBA)
    assert_equal(ti.parent, tw)
    assert_equal(tw.childCount, 1)
    ti = nil
    # force delete of the ti item unless it's referenced
    # NOTE: it's not sufficient to just set ti to nil, we also
    # have to create a new QTreeWidgetItem (special for Ruby >1.9?)
    ti = RBA::QTreeWidgetItem::new
    GC.start
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    assert_equal(tw.childCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.child(0)
    tw._destroy
    # gives true, because tw owns ti too.
    assert_equal(ti._destroyed?, true)

    # The same works for insert too
    tw = RBA::QTreeWidgetItem::new
    ti = RBA::QTreeWidgetItem::new
    tw.insertChild(0, ti)
    assert_equal(tw.childCount, 1)
    ti = nil
    # force delete of the ti item unless it's referenced
    ti = RBA::QTreeWidgetItem::new
    GC.start
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    assert_equal(tw.childCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.child(0)
    tw._destroy
    # gives true, because tw owns ti
    assert_equal(ti._destroyed?, true)

    # And add:
    tw = RBA::QTreeWidgetItem::new
    ti = RBA::QTreeWidgetItem::new
    tw.addChild(ti)
    assert_equal(tw.childCount, 1)
    ti = nil
    # force delete of the ti item unless it's referenced
    ti = RBA::QTreeWidgetItem::new
    GC.start
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    assert_equal(tw.childCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.child(0)
    tw._destroy
    # gives true, because tw owns ti
    assert_equal(ti._destroyed?, true)

    # But the item is released when we take it and add:
    tw = RBA::QTreeWidgetItem::new
    ti = RBA::QTreeWidgetItem::new
    tw.addChild(ti)
    assert_equal(tw.childCount, 1)
    ti = nil
    # force delete of the ti item unless it's referenced
    ti = RBA::QTreeWidgetItem::new
    GC.start
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    assert_equal(tw.childCount, 1)

    ti = tw.takeChild(0)
    tw._destroy
    # gives false, because we took ti and tw no longer owns it
    assert_equal(ti._destroyed?, false)

    # And we can destroy a child too
    tw = RBA::QTreeWidgetItem::new
    ti = RBA::QTreeWidgetItem::new
    tw.addChild(ti)
    assert_equal(tw.childCount, 1)
    ti._destroy
    assert_equal(tw.childCount, 0)

  end

  def test_42

    # QKeyEvent and related issues

    ef = EventFilter::new

    widget = RBA::QLineEdit::new
    widget.setText("ABC")

    RBA::QApplication::processEvents

    widget.installEventFilter(ef)

    ke = RBA::QKeyEvent::new(RBA::QEvent::KeyPress, RBA::Qt::Key_O.to_i, RBA::Qt::ShiftModifier, "O")
    RBA::QCoreApplication::postEvent(widget, ke)

    ke = RBA::QKeyEvent::new(RBA::QEvent::KeyPress, RBA::Qt::Key_Left.to_i, RBA::Qt::NoModifier)
    RBA::QCoreApplication::postEvent(widget, ke)

    ke = RBA::QKeyEvent::new(RBA::QEvent::KeyPress, RBA::Qt::Key_P.to_i, RBA::Qt::NoModifier, "p")
    RBA::QCoreApplication::postEvent(widget, ke)

    RBA::QApplication::processEvents

    GC.start

    assert_equal(ef.log.join("\n"), "RBA::QKeyEvent: ShortcutOverride (51)\nRBA::QKeyEvent: KeyPress (6)\nRBA::QKeyEvent: ShortcutOverride (51)\nRBA::QKeyEvent: KeyPress (6)\nRBA::QKeyEvent: ShortcutOverride (51)\nRBA::QKeyEvent: KeyPress (6)") 
    ef = nil
    ef = EventFilter::new
    GC.start

    assert_equal(widget.text, "ABCpO")
    ef = nil
    widget = nil
    GC.start

  end

  def test_43

    # QVariant 

    v = RBA::QVariant::new
    assert_equal(v.type.to_s, "Invalid (0)")
    assert_equal(v.toInt, 0)
    assert_equal(v.toDouble, 0.0)
    assert_equal(v.toBool, false)
    assert_equal(v.toString, nil)

    v = RBA::QVariant::new(0)
    assert_equal(v.type.to_s, "Int (2)")
    assert_equal(v.toInt, 0)
    assert_equal(v.toDouble, 0.0)
    assert_equal(v.toBool, false)
    assert_equal(v.toString, "0")

    v = RBA::QVariant::new(17)
    assert_equal(v.type.to_s, "Int (2)")
    assert_equal(v.toInt, 17)
    assert_equal(v.toDouble, 17.0)
    assert_equal(v.toBool, true)
    assert_equal(v.toString, "17")

    v = RBA::QVariant::new(true)
    assert_equal(v.type.to_s, "Bool (1)")
    assert_equal(v.toInt, 1)
    assert_equal(v.toString, "true")

    v = RBA::QVariant::new(42.25)
    assert_equal(v.type.to_s, "Double (6)")
    assert_equal(v.toInt, 42)
    assert_equal(v.toDouble, 42.25)
    assert_equal(v.toBool, true)
    assert_equal(v.toString, "42.25")

    v = RBA::QVariant::new([ RBA::QVariant::new(1), RBA::QVariant::new(2), RBA::QVariant::new(5) ])
    assert_equal(v.type.to_s, "List (9)")
    assert_equal(v.toString, nil)
    assert_equal(v.toList.collect { |i| i.toInt }, [1, 2, 5])

    v = RBA::QVariant::new(RBA::QPoint::new(5, 7))
    assert_equal(v.type.to_s, "Point (25)")
    assert_equal(v.toPoint.x, 5)
    assert_equal(v.toPoint.y, 7)

    v = RBA::QVariant::new(RBA::QColor::new(10, 20, 30))
    assert_equal(v.type.to_s, "Color (67)")
    assert_equal(v.toColor.red, 10)
    assert_equal(v.toColor.green, 20)
    assert_equal(v.toColor.blue, 30)

    v = RBA::QVariant::new(RBA::QFont::new("Helvetica", 22))
    assert_equal(v.type.to_s, "Font (64)")
    assert_equal(v.toFont.key, "Helvetica,22,-1,5,50,0,0,0,0,0")

    v = RBA::QVariant::new(RBA::QBitmap::new)
    assert_equal(v.type.to_s, "Pixmap (65)")
    assert_equal(v.toBitmap.class, RBA::QBitmap_Native)

    v = RBA::QVariant::new(RBA::QIcon::new)
    assert_equal(v.type.to_s, "Icon (69)")
    assert_equal(v.toIcon.class, RBA::QIcon)

    v = RBA::QVariant::new(RBA::QPixmap::new)
    assert_equal(v.type.to_s, "Pixmap (65)")
    assert_equal(v.toPixmap.class, RBA::QPixmap_Native)

    v = RBA::QVariant::new(RBA::QImage::new)
    assert_equal(v.type.to_s, "Image (70)")
    assert_equal(v.toImage.class, RBA::QImage_Native)

    v = RBA::QVariant::new(RBA::QKeySequence::new("Shift+F5"))
    assert_equal(v.type.to_s, "KeySequence (76)")
    assert_equal(v.toKeySequence.toString, "Shift+F5")

    v = RBA::QVariant::new(RBA::QCursor::new)
    assert_equal(v.type.to_s, "Cursor (74)")
    assert_equal(v.toCursor.class, RBA::QCursor)

    v = RBA::QVariant::new(RBA::QBrush::new)
    assert_equal(v.type.to_s, "Brush (66)")
    assert_equal(v.toBrush.class, RBA::QBrush)

    v = RBA::QVariant::new(RBA::QPen::new)
    assert_equal(v.type.to_s, "Pen (77)")
    assert_equal(v.toPen.class, RBA::QPen)

    v = RBA::QVariant::new(RBA::QPalette::new)
    assert_equal(v.type.to_s, "Palette (68)")
    assert_equal(v.toPalette.class, RBA::QPalette)

    v = RBA::QVariant::new(RBA::QTransform::new)
    assert_equal(v.type.to_s, "Transform (81)")
    assert_equal(v.toTransform.class, RBA::QTransform)

    v = RBA::QVariant::new(RBA::QQuaternion::new)
    assert_equal(v.type.to_s, "Quaternion (86)")
    assert_equal(v.toQuaternion.class, RBA::QQuaternion)

    v = RBA::QVariant::new(RBA::QMatrix4x4::new)
    assert_equal(v.type.to_s, "Matrix4x4 (82)")
    assert_equal(v.toMatrix4x4.class, RBA::QMatrix4x4)

    v = RBA::QVariant::new(RBA::QTextFormat::new)
    assert_equal(v.type.to_s, "TextFormat (79)")
    assert_equal(v.toTextFormat.class, RBA::QTextFormat)

    v = RBA::QVariant::new(RBA::QTextLength::new)
    assert_equal(v.type.to_s, "TextLength (78)")
    assert_equal(v.toTextLength.class, RBA::QTextLength)

    v = RBA::QVariant::new(RBA::QVector2D::new)
    assert_equal(v.type.to_s, "Vector2D (83)")
    assert_equal(v.toVector2D.class, RBA::QVector2D)

    v = RBA::QVariant::new(RBA::QVector3D::new)
    assert_equal(v.type.to_s, "Vector3D (84)")
    assert_equal(v.toVector3D.class, RBA::QVector3D)

    v = RBA::QVariant::new(RBA::QVector4D::new)
    assert_equal(v.type.to_s, "Vector4D (85)")
    assert_equal(v.toVector4D.class, RBA::QVector4D)

  end

end

load("test_epilogue.rb")
