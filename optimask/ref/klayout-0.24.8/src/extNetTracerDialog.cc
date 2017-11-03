
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


#include "extNetTracerDialog.h"
#include "extNetTracerConfig.h"

#include "layConfigurationDialog.h"
#include "layConfig.h"
#include "layConverters.h"
#include "layFinder.h"
#include "layLayoutView.h"
#include "layTechSetupDialog.h"
#include "layMainWindow.h"
#include "layExceptions.h"
#include "layFileDialog.h"
#include "tlXMLWriter.h"
#include "tlUtils.h"
#include "gsiDecl.h"

#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QtGui/QItemDelegate>
#include <QtGui/QHeaderView>
#include <QtGui/QPainter>

#include <fstream>
#include <sstream>

namespace tl
{
  /**
   *  @brief A specialization of the XMLConverter that is used to serialize the connection info
   */

  template <>
  struct XMLStdConverter<ext::NetTracerConnectionInfo>
  {
    std::string to_string (const ext::NetTracerConnectionInfo &v) const
    {
      return v.to_string ();
    }

    void from_string (const std::string &s, ext::NetTracerConnectionInfo &v) const
    {
      tl::Extractor ex (s.c_str ());
      v.parse (ex);
    }
  };

  /**
   *  @brief A specialization of the XMLConverter that is used to serialize the symbol info
   */

  template <>
  struct XMLStdConverter<ext::NetTracerSymbolInfo>
  {
    std::string to_string (const ext::NetTracerSymbolInfo &v) const
    {
      return v.to_string ();
    }

    void from_string (const std::string &s, ext::NetTracerSymbolInfo &v) const
    {
      tl::Extractor ex (s.c_str ());
      v.parse (ex);
    }
  };
}

namespace ext
{

const std::string net_tracer_component_name ("connectivity");

// -----------------------------------------------------------------------------------
//  NetTracerConnectionInfo implementation

NetTracerConnectionInfo::NetTracerConnectionInfo ()
{
  // .. nothing yet ..
}

NetTracerConnectionInfo::NetTracerConnectionInfo (const NetTracerLayerExpressionInfo &la, const NetTracerLayerExpressionInfo &lb)
  : m_la (la), m_via (), m_lb (lb)
{
  // .. nothing yet ..
}

NetTracerConnectionInfo::NetTracerConnectionInfo (const NetTracerLayerExpressionInfo &la, const NetTracerLayerExpressionInfo &via, const NetTracerLayerExpressionInfo &lb)
  : m_la (la), m_via (via), m_lb (lb)
{
  // .. nothing yet ..
}

static int get_layer_id (const NetTracerLayerExpressionInfo &e, const db::Layout &layout, const NetTracerTechnologyComponent &tech, NetTracerData *data) 
{ 
  std::auto_ptr<NetTracerLayerExpression> expr_in (NetTracerLayerExpressionInfo::compile (e.to_string ()).get (layout, tech));
  int l = expr_in->alias_for ();
  if (l < 0 && data) {
    l = data->find_symbol (e.to_string ());
    if (l < 0) {
      return int (data->register_logical_layer (expr_in.release (), 0));
    } 
  } 
  return l;
}

NetTracerConnection 
NetTracerConnectionInfo::get (const db::Layout &layout, const NetTracerTechnologyComponent &tech, NetTracerData &data) const
{
  int la = get_layer_id (m_la, layout, tech, &data);
  int lb = get_layer_id (m_lb, layout, tech, &data);

  if (! m_via.to_string ().empty ()) {
    int via = get_layer_id (m_via, layout, tech, &data);
    return NetTracerConnection (la, via, lb);
  } else {
    return NetTracerConnection (la, lb);
  }
}

std::string 
NetTracerConnectionInfo::to_string () const
{
  std::string res;
  res += m_la.to_string ();
  res += ",";
  res += m_via.to_string ();
  res += ",";
  res += m_lb.to_string ();

  return res;
}

void 
NetTracerConnectionInfo::parse (tl::Extractor &ex)
{
  m_la = NetTracerLayerExpressionInfo::parse (ex);
  ex.expect (",");
  m_via = NetTracerLayerExpressionInfo::parse (ex);
  ex.expect (",");
  m_lb = NetTracerLayerExpressionInfo::parse (ex);
}

// -----------------------------------------------------------------------------------
//  NetTracerSymbolInfo implementation

NetTracerSymbolInfo::NetTracerSymbolInfo ()
{
  // .. nothing yet ..
}

NetTracerSymbolInfo::NetTracerSymbolInfo (const db::LayerProperties &symbol, const std::string &expression)
  : m_symbol (symbol), m_expression (expression)
{
  // .. nothing yet ..
}

std::string 
NetTracerSymbolInfo::to_string () const
{
  std::string res;
  res += m_symbol.to_string ();
  res += "=";
  res += tl::to_quoted_string(m_expression);

  return res;
}

void 
NetTracerSymbolInfo::parse (tl::Extractor &ex)
{
  m_symbol.read (ex);
  ex.expect ("=");
  ex.read_word_or_quoted (m_expression);
}

// -----------------------------------------------------------------------------------------
//  NetTracerLayerExpressionInfo implementation

NetTracerLayerExpressionInfo::NetTracerLayerExpressionInfo ()
  : mp_a (0), mp_b (0), m_op (NetTracerLayerExpression::OPNone)
{
  //  .. nothing yet ..
}

NetTracerLayerExpressionInfo::~NetTracerLayerExpressionInfo ()
{
  delete mp_a; 
  mp_a = 0;
  delete mp_b;
  mp_b = 0;
}

NetTracerLayerExpressionInfo::NetTracerLayerExpressionInfo (const NetTracerLayerExpressionInfo &other)
  : m_expression (other.m_expression), m_a (other.m_a), m_b (other.m_b), mp_a (0), mp_b (0), m_op (other.m_op)
{
  if (other.mp_a) {
    mp_a = new NetTracerLayerExpressionInfo (*other.mp_a);
  }
  if (other.mp_b) {
    mp_b = new NetTracerLayerExpressionInfo (*other.mp_b);
  }
}

NetTracerLayerExpressionInfo &
NetTracerLayerExpressionInfo::operator= (const NetTracerLayerExpressionInfo &other)
{
  if (this != &other) {

    m_expression = other.m_expression;

    delete mp_a; 
    mp_a = 0;
    delete mp_b;
    mp_b = 0;

    m_a = other.m_a; 
    m_b = other.m_b; 
    m_op = other.m_op;

    if (other.mp_a) {
      mp_a = new NetTracerLayerExpressionInfo (*other.mp_a);
    }
    if (other.mp_b) {
      mp_b = new NetTracerLayerExpressionInfo (*other.mp_b);
    }

  }

  return *this;
}

void 
NetTracerLayerExpressionInfo::merge (NetTracerLayerExpression::Operator op, const NetTracerLayerExpressionInfo &other)
{
  if (m_op != NetTracerLayerExpression::OPNone) {
    NetTracerLayerExpressionInfo *e = new NetTracerLayerExpressionInfo (*this);
    *this = NetTracerLayerExpressionInfo ();
    mp_a = e;
  }

  m_op = op;

  if (other.m_op == NetTracerLayerExpression::OPNone) {
    if (other.mp_a) {
      mp_b = new NetTracerLayerExpressionInfo (*other.mp_a);
    } else {
      m_b = other.m_a;
    }
  } else {
    mp_b = new NetTracerLayerExpressionInfo (other);
  }
}

NetTracerLayerExpressionInfo 
NetTracerLayerExpressionInfo::parse_add (tl::Extractor &ex)
{
  NetTracerLayerExpressionInfo e = parse_mult (ex);
  while (true) {
    if (ex.test ("+")) {
      NetTracerLayerExpressionInfo ee = parse_mult (ex);
      e.merge (NetTracerLayerExpression::OPOr, ee);
    } else if (ex.test ("-")) {
      NetTracerLayerExpressionInfo ee = parse_mult (ex);
      e.merge (NetTracerLayerExpression::OPNot, ee);
    } else {
      break;
    }
  }

  return e;
}

NetTracerLayerExpressionInfo 
NetTracerLayerExpressionInfo::parse_mult (tl::Extractor &ex)
{
  NetTracerLayerExpressionInfo e = parse_atomic (ex);
  while (true) {
    if (ex.test ("*")) {
      NetTracerLayerExpressionInfo ee = parse_atomic (ex);
      e.merge (NetTracerLayerExpression::OPAnd, ee);
    } else if (ex.test ("^")) {
      NetTracerLayerExpressionInfo ee = parse_atomic (ex);
      e.merge (NetTracerLayerExpression::OPXor, ee);
    } else {
      break;
    }
  }

  return e;
}

NetTracerLayerExpressionInfo 
NetTracerLayerExpressionInfo::parse_atomic (tl::Extractor &ex)
{
  NetTracerLayerExpressionInfo e;
  if (ex.test ("(")) {
    e = parse_add (ex);
    ex.expect (")");
  } else {
    e.m_a.read (ex);
  }
  return e;
}

NetTracerLayerExpressionInfo
NetTracerLayerExpressionInfo::parse (tl::Extractor &ex)
{
  const char *start = ex.skip ();
  NetTracerLayerExpressionInfo e = parse_add (ex);
  e.m_expression = std::string (start, ex.get () - start);
  return e;
}

NetTracerLayerExpressionInfo
NetTracerLayerExpressionInfo::compile (const std::string &s)
{
  tl::Extractor ex (s.c_str ());
  const char *start = ex.skip ();
  NetTracerLayerExpressionInfo e = parse_add (ex);
  e.m_expression = std::string (start, ex.get () - start);
  ex.expect_end ();
  return e;
}

NetTracerLayerExpression *
NetTracerLayerExpressionInfo::get_expr (const db::LayerProperties &lp, const db::Layout &layout, const NetTracerTechnologyComponent &tech, const std::set<std::string> &used_symbols) const
{
  for (NetTracerTechnologyComponent::const_symbol_iterator s = tech.begin_symbols (); s != tech.end_symbols (); ++s) {
    if (s->symbol ().log_equal (lp)) {
      std::set<std::string> us = used_symbols;
      if (! us.insert (s->symbol ().to_string ()).second) {
        throw tl::Exception (tl::to_string (QObject::tr ("Recursive expression through symbol %s")), s->symbol ());
      }
      return NetTracerLayerExpressionInfo::compile (s->expression ()).get (layout, tech, us);
    }
  }

  for (db::Layout::layer_iterator l = layout.begin_layers (); l != layout.end_layers (); ++l) {
    if ((*l).second->log_equal (lp)) {
      return new NetTracerLayerExpression ((*l).first);
    }
  }

  return new NetTracerLayerExpression (-1);
}
 
NetTracerLayerExpression *
NetTracerLayerExpressionInfo::get (const db::Layout &layout, const NetTracerTechnologyComponent &tech) const
{
  std::set<std::string> us;
  return get (layout, tech, us);
}

NetTracerLayerExpression *
NetTracerLayerExpressionInfo::get (const db::Layout &layout, const NetTracerTechnologyComponent &tech, const std::set<std::string> &used_symbols) const
{
  NetTracerLayerExpression *e = 0;

  if (mp_a) {
    e = mp_a->get (layout, tech, used_symbols);
  } else {
    e = get_expr (m_a, layout, tech, used_symbols);
  }

  if (m_op != NetTracerLayerExpression::OPNone) {
    if (mp_b) {
      e->merge (m_op, mp_b->get (layout, tech, used_symbols));
    } else {
      e->merge (m_op, get_expr (m_b, layout, tech, used_symbols));
    }
  }

  return e;
}

// -----------------------------------------------------------------------------------
//  NetTracerTechnologyComponent implementation

NetTracerTechnologyComponent::NetTracerTechnologyComponent ()
  : lay::TechnologyComponent (net_tracer_component_name, tl::to_string (QObject::tr ("Connectivity")))
{
  // .. nothing yet ..
}

NetTracerTechnologyComponent::NetTracerTechnologyComponent (const NetTracerTechnologyComponent &d)
  : lay::TechnologyComponent (net_tracer_component_name, tl::to_string (QObject::tr ("Connectivity")))
{
  m_connections = d.m_connections;
  m_symbols = d.m_symbols;
}

NetTracerData
NetTracerTechnologyComponent::get (const db::Layout &layout) const
{
  NetTracerData data;

  //  register a logical layer for each original one as alias and one for each expression with a new ID
  for (ext::NetTracerTechnologyComponent::const_symbol_iterator s = begin_symbols (); s != end_symbols (); ++s) {
    ext::NetTracerLayerExpression *expr = ext::NetTracerLayerExpressionInfo::compile (s->expression ()).get (layout, *this);
    data.register_logical_layer (expr, s->symbol ().to_string ().c_str ());
  }

  for (ext::NetTracerTechnologyComponent::const_iterator c = begin (); c != end (); ++c) {
    data.add_connection (c->get (layout, *this, data));
  }

  return data;
}

lay::TechnologyComponentEditor * 
NetTracerTechnologyComponent::create_editor (QWidget *parent) 
{
  return new NetTracerTechComponentEditor (parent);
}

// -----------------------------------------------------------------------------------------
//  NetTracerConnectivityColumnDelegate definition and implementation

class NetTracerConnectivityColumnDelegate
  : public QItemDelegate
{
public:
  NetTracerConnectivityColumnDelegate (QWidget *parent, NetTracerTechnologyComponent *data)
    : QItemDelegate (parent), mp_data (data)
  {
    //  .. nothing yet ..
  }

  QWidget *createEditor (QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const 
  {
    return new QLineEdit (parent);
  }

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    editor->setGeometry(option.rect);
  }

  void setEditorData (QWidget *widget, const QModelIndex &index) const
  {
    QLineEdit *editor = dynamic_cast<QLineEdit *> (widget);
    if (editor) {
      int n = index.model ()->data (index, Qt::UserRole).toInt ();
      if (mp_data->size () > size_t (n)) { 
        if (index.column () == 0) {
          editor->setText (tl::to_qstring (mp_data->begin () [n].layer_a ().to_string ()));
        } else if (index.column () == 1) {
          editor->setText (tl::to_qstring (mp_data->begin () [n].via_layer ().to_string ()));
        } else if (index.column () == 2) {
          editor->setText (tl::to_qstring (mp_data->begin () [n].layer_b ().to_string ()));
        }
      }
    }
  }

  void setModelData (QWidget *widget, QAbstractItemModel *model, const QModelIndex &index) const
  {
    QLineEdit *editor = dynamic_cast<QLineEdit *> (widget);
    if (editor) {

      int n = model->data (index, Qt::UserRole).toInt ();
      if (mp_data->size () > size_t (n)) { 

        NetTracerLayerExpressionInfo expr;

        std::string text = tl::to_string (editor->text ());
        tl::Extractor ex (text.c_str ());
        bool error = false;
        try {
          expr = NetTracerLayerExpressionInfo::compile (text);
        } catch (...) {
          error = true;
        }

        if (error) {
          model->setData (index, QVariant (tl::to_qstring (text)), Qt::DisplayRole);
          model->setData (index, QVariant (Qt::red), Qt::ForegroundRole);
          model->setData (index, QVariant (QColor (Qt::red).lighter (180)), Qt::BackgroundRole);
        } else if ((index.column () == 0 || index.column () == 2) && expr.to_string ().empty ()) {
          model->setData (index, QVariant (QObject::tr ("Enter expression")), Qt::DisplayRole);
          model->setData (index, QVariant (Qt::red), Qt::ForegroundRole);
          model->setData (index, QVariant (QColor (Qt::red).lighter (180)), Qt::BackgroundRole);
        } else if (index.column () == 1 && expr.to_string ().empty ()) {
          model->setData (index, QVariant (QObject::tr ("None")), Qt::DisplayRole);
          model->setData (index, QVariant (), Qt::ForegroundRole);
          model->setData (index, QVariant (), Qt::BackgroundRole);
        } else {
          model->setData (index, QVariant (tl::to_qstring (expr.to_string ())), Qt::DisplayRole);
          model->setData (index, QVariant (), Qt::ForegroundRole);
          model->setData (index, QVariant (), Qt::BackgroundRole);
        }

        if (index.column () == 0) {
          mp_data->begin () [n].set_layer_a (expr);
        } else if (index.column () == 1) {
          mp_data->begin () [n].set_via_layer (expr);
        } else if (index.column () == 2) {
          mp_data->begin () [n].set_layer_b (expr);
        }

      }

    }
  }

  QSize sizeHint (const QStyleOptionViewItem &option, const QModelIndex &index) const 
  {
    QWidget *editor = createEditor (0, option, index);
    QSize size = editor->sizeHint ();
    delete editor;
    return size - QSize (2, 2);
  }

private:
  NetTracerTechnologyComponent *mp_data;
};

// -----------------------------------------------------------------------------------------
//  NetTracerConnectivitySymbolColumnDelegate definition and implementation

class NetTracerConnectivitySymbolColumnDelegate
  : public QItemDelegate
{
public:
  NetTracerConnectivitySymbolColumnDelegate (QWidget *parent, NetTracerTechnologyComponent *data)
    : QItemDelegate (parent), mp_data (data)
  {
    //  .. nothing yet ..
  }

  QWidget *createEditor (QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const 
  {
    return new QLineEdit (parent);
  }

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    editor->setGeometry(option.rect);
  }

  void setEditorData (QWidget *widget, const QModelIndex &index) const
  {
    QLineEdit *editor = dynamic_cast<QLineEdit *> (widget);
    if (editor) {
      int n = index.model ()->data (index, Qt::UserRole).toInt ();
      if (mp_data->symbols () > size_t (n)) { 
        if (index.column () == 0) {
          editor->setText (tl::to_qstring (mp_data->begin_symbols () [n].symbol ().to_string ()));
        } else if (index.column () == 1) {
          editor->setText (tl::to_qstring (mp_data->begin_symbols () [n].expression ()));
        }
      }
    }
  }

  void setModelData (QWidget *widget, QAbstractItemModel *model, const QModelIndex &index) const
  {
    QLineEdit *editor = dynamic_cast<QLineEdit *> (widget);
    if (editor) {

      int n = model->data (index, Qt::UserRole).toInt ();
      if (mp_data->symbols () > size_t (n)) { 

        std::string text = tl::to_string (editor->text ());

        if (index.column () == 0 && text.empty ()) {

          model->setData (index, QVariant (QObject::tr ("Enter symbol")), Qt::DisplayRole);
          model->setData (index, QVariant (Qt::red), Qt::ForegroundRole);
          model->setData (index, QVariant (QColor (Qt::red).lighter (180)), Qt::BackgroundRole);

        } else if (index.column () == 1 && text.empty ()) {

          model->setData (index, QVariant (QObject::tr ("Enter expression")), Qt::DisplayRole);
          model->setData (index, QVariant (Qt::red), Qt::ForegroundRole);
          model->setData (index, QVariant (QColor (Qt::red).lighter (180)), Qt::BackgroundRole);

        } else if (index.column () == 1) {

          bool ok = true;
          //  check the expression
          try {
            NetTracerLayerExpressionInfo::compile (text);
          } catch (tl::Exception &) {
            ok = false;
          }

          model->setData (index, QVariant (tl::to_qstring (text)), Qt::DisplayRole);

          if (! ok) {
            model->setData (index, QVariant (Qt::red), Qt::ForegroundRole);
            model->setData (index, QVariant (QColor (Qt::red).lighter (180)), Qt::BackgroundRole);
          } else {
            model->setData (index, QVariant (), Qt::ForegroundRole);
            model->setData (index, QVariant (), Qt::BackgroundRole);
          }

        } else {

          model->setData (index, QVariant (tl::to_qstring (text)), Qt::DisplayRole);
          model->setData (index, QVariant (), Qt::ForegroundRole);
          model->setData (index, QVariant (), Qt::BackgroundRole);

        }

        if (index.column () == 0) {
          db::LayerProperties lp;
          tl::Extractor ex (text.c_str ());
          lp.read (ex);
          mp_data->begin_symbols () [n].set_symbol (lp);
        } else if (index.column () == 1) {
          mp_data->begin_symbols () [n].set_expression (text);
        }

      }

    }
  }

  QSize sizeHint (const QStyleOptionViewItem &option, const QModelIndex &index) const 
  {
    QWidget *editor = createEditor (0, option, index);
    QSize size = editor->sizeHint ();
    delete editor;
    return size - QSize (2, 2);
  }

private:
  NetTracerTechnologyComponent *mp_data;
};

// -----------------------------------------------------------------------------------
//  NetTracerTechComponentEditor implementation

NetTracerTechComponentEditor::NetTracerTechComponentEditor (QWidget *parent)
  : TechnologyComponentEditor (parent)
{
  Ui::NetTracerTechComponentEditor::setupUi (this);

  connect (add_conductor_pb, SIGNAL (clicked ()), this, SLOT (add_clicked ()));
  connect (del_conductor_pb, SIGNAL (clicked ()), this, SLOT (del_clicked ()));
  connect (move_conductor_up_pb, SIGNAL (clicked ()), this, SLOT (move_up_clicked ()));
  connect (move_conductor_down_pb, SIGNAL (clicked ()), this, SLOT (move_down_clicked ()));
  connect (add_symbol_pb, SIGNAL (clicked ()), this, SLOT (symbol_add_clicked ()));
  connect (del_symbol_pb, SIGNAL (clicked ()), this, SLOT (symbol_del_clicked ()));
  connect (move_symbol_up_pb, SIGNAL (clicked ()), this, SLOT (symbol_move_up_clicked ()));
  connect (move_symbol_down_pb, SIGNAL (clicked ()), this, SLOT (symbol_move_down_clicked ()));
  connect (help_label, SIGNAL (linkActivated (const QString &)), lay::MainWindow::instance (), SLOT (show_help (const QString &)));
  connect (symbol_help_label, SIGNAL (linkActivated (const QString &)), lay::MainWindow::instance (), SLOT (show_help (const QString &)));

  connectivity_table->horizontalHeader ()->setHighlightSections (false);
  connectivity_table->horizontalHeader ()->setStretchLastSection (true);
  connectivity_table->verticalHeader ()->hide ();
  symbol_table->horizontalHeader ()->setHighlightSections (false);
  symbol_table->horizontalHeader ()->setStretchLastSection (true);
  symbol_table->verticalHeader ()->hide ();
}

void 
NetTracerTechComponentEditor::commit ()
{
  NetTracerTechnologyComponent *data = dynamic_cast <NetTracerTechnologyComponent *> (tech_component ());
  if (! data) {
    return;
  }

  *data = m_data;
}

void 
NetTracerTechComponentEditor::setup ()
{
  NetTracerTechnologyComponent *data = dynamic_cast <NetTracerTechnologyComponent *> (tech_component ());
  if (! data) {
    return;
  }

  m_data = *data;

  for (int c = 0; c < 3; ++c) {
    if (connectivity_table->itemDelegateForColumn (c) != 0) {
      delete connectivity_table->itemDelegateForColumn (c);
    }
    connectivity_table->setItemDelegateForColumn (c, new NetTracerConnectivityColumnDelegate (connectivity_table, &m_data));
  }

  for (int c = 0; c < 2; ++c) {
    if (symbol_table->itemDelegateForColumn (c) != 0) {
      delete symbol_table->itemDelegateForColumn (c);
    }
    symbol_table->setItemDelegateForColumn (c, new NetTracerConnectivitySymbolColumnDelegate (symbol_table, &m_data));
  }

  update ();
}

void 
NetTracerTechComponentEditor::add_clicked ()
{
  //  removes focus from the tree view - commits the data
  add_conductor_pb->setFocus (); 

  int row = connectivity_table->currentItem () ? connectivity_table->row (connectivity_table->currentItem ()) : -1;
  if (row < 0) {
    m_data.add (NetTracerConnectionInfo ());
    row = int (m_data.size () - 1);
  } else {
    row += 1;
    m_data.insert (m_data.begin () + row, NetTracerConnectionInfo ());
  }

  update ();
  connectivity_table->setCurrentItem (connectivity_table->item (row, 0));
}

void 
NetTracerTechComponentEditor::del_clicked ()
{
  //  removes focus from the tree view - commits the data
  del_conductor_pb->setFocus (); 

  std::set<int> selected_rows;
  QModelIndexList selected_indices = connectivity_table->selectionModel ()->selectedIndexes ();
  for (QModelIndexList::const_iterator i = selected_indices.begin (); i != selected_indices.end (); ++i) {
    selected_rows.insert (i->row ());
  }

  connectivity_table->setCurrentIndex (QModelIndex ());

  int offset = 0;
  for (std::set<int>::const_iterator r = selected_rows.begin (); r != selected_rows.end (); ++r) {
    m_data.erase (m_data.begin () + (*r - offset));
    ++offset;
  }

  update ();
}

void 
NetTracerTechComponentEditor::move_up_clicked ()
{
  //  removes focus from the tree view - commits the data
  move_conductor_up_pb->setFocus (); 

  std::set<int> selected_rows;
  QModelIndexList selected_indices = connectivity_table->selectionModel ()->selectedIndexes ();
  for (QModelIndexList::const_iterator i = selected_indices.begin (); i != selected_indices.end (); ++i) {
    selected_rows.insert (i->row ());
  }

  QTableWidgetItem *current = connectivity_table->currentItem ();
  int n_current = current ? current->data (Qt::UserRole).toInt () : -1;

  connectivity_table->setCurrentIndex (QModelIndex ());

  int n = 0;
  for (NetTracerTechnologyComponent::iterator l = m_data.begin (); l != m_data.end (); ++l, ++n) {
    if (selected_rows.find (n + 1) != selected_rows.end () && selected_rows.find (n) == selected_rows.end ()) {
      std::swap (m_data.begin () [n + 1], m_data.begin () [n]);
      selected_rows.erase (n + 1);
      selected_rows.insert (n);
      if (n_current == n + 1) {
        n_current = n;
      }
    }
  }

  update ();

  // select the new items
  for (std::set <int>::const_iterator s = selected_rows.begin (); s != selected_rows.end (); ++s) {
    connectivity_table->selectionModel ()->select (connectivity_table->model ()->index (*s, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
  }

  if (n_current >= 0) {
    connectivity_table->selectionModel ()->select (connectivity_table->model ()->index (n_current, 0), QItemSelectionModel::Current | QItemSelectionModel::Rows);
  }
}

void 
NetTracerTechComponentEditor::move_down_clicked ()
{
  //  removes focus from the tree view - commits the data
  move_conductor_down_pb->setFocus (); 

  std::set<int> selected_rows;
  QModelIndexList selected_indices = connectivity_table->selectionModel ()->selectedIndexes ();
  for (QModelIndexList::const_iterator i = selected_indices.begin (); i != selected_indices.end (); ++i) {
    selected_rows.insert (i->row ());
  }

  QTableWidgetItem *current = connectivity_table->currentItem ();
  int n_current = current ? current->data (Qt::UserRole).toInt () : -1;

  connectivity_table->setCurrentIndex (QModelIndex ());

  int n = m_data.size ();
  for (NetTracerTechnologyComponent::iterator l = m_data.end (); l != m_data.begin (); ) {
    --l;
    --n;
    if (selected_rows.find (n - 1) != selected_rows.end () && selected_rows.find (n) == selected_rows.end ()) {
      std::swap (m_data.begin () [n - 1], m_data.begin () [n]);
      selected_rows.erase (n - 1);
      selected_rows.insert (n);
      if (n_current == n - 1) {
        n_current = n;
      }
    }
  }

  update ();

  // select the new items
  for (std::set <int>::const_iterator s = selected_rows.begin (); s != selected_rows.end (); ++s) {
    connectivity_table->selectionModel ()->select (connectivity_table->model ()->index (*s, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
  }

  if (n_current >= 0) {
    connectivity_table->selectionModel ()->select (connectivity_table->model ()->index (n_current, 0), QItemSelectionModel::Current | QItemSelectionModel::Rows);
  }
}

void 
NetTracerTechComponentEditor::symbol_add_clicked ()
{
  //  removes focus from the tree view - commits the data
  add_symbol_pb->setFocus (); 

  int row = symbol_table->currentItem () ? symbol_table->row (symbol_table->currentItem ()) : -1;
  if (row < 0) {
    m_data.add_symbol (NetTracerSymbolInfo ());
    row = int (m_data.symbols () - 1);
  } else {
    row += 1;
    m_data.insert_symbol (m_data.begin_symbols () + row, NetTracerSymbolInfo ());
  }

  update ();
  symbol_table->setCurrentItem (symbol_table->item (row, 0));
}

void 
NetTracerTechComponentEditor::symbol_del_clicked ()
{
  //  removes focus from the tree view - commits the data
  del_symbol_pb->setFocus (); 

  std::set<int> selected_rows;
  QModelIndexList selected_indices = symbol_table->selectionModel ()->selectedIndexes ();
  for (QModelIndexList::const_iterator i = selected_indices.begin (); i != selected_indices.end (); ++i) {
    selected_rows.insert (i->row ());
  }

  symbol_table->setCurrentIndex (QModelIndex ());

  int offset = 0;
  for (std::set<int>::const_iterator r = selected_rows.begin (); r != selected_rows.end (); ++r) {
    m_data.erase_symbol (m_data.begin_symbols () + (*r - offset));
    ++offset;
  }

  update ();
}

void 
NetTracerTechComponentEditor::symbol_move_up_clicked ()
{
  //  removes focus from the tree view - commits the data
  move_symbol_up_pb->setFocus (); 

  std::set<int> selected_rows;
  QModelIndexList selected_indices = symbol_table->selectionModel ()->selectedIndexes ();
  for (QModelIndexList::const_iterator i = selected_indices.begin (); i != selected_indices.end (); ++i) {
    selected_rows.insert (i->row ());
  }

  QTableWidgetItem *current = symbol_table->currentItem ();
  int n_current = current ? current->data (Qt::UserRole).toInt () : -1;

  symbol_table->setCurrentIndex (QModelIndex ());

  int n = 0;
  for (NetTracerTechnologyComponent::symbol_iterator l = m_data.begin_symbols (); l != m_data.end_symbols (); ++l, ++n) {
    if (selected_rows.find (n + 1) != selected_rows.end () && selected_rows.find (n) == selected_rows.end ()) {
      std::swap (m_data.begin_symbols () [n + 1], m_data.begin_symbols () [n]);
      selected_rows.erase (n + 1);
      selected_rows.insert (n);
      if (n_current == n + 1) {
        n_current = n;
      }
    }
  }

  update ();

  // select the new items
  for (std::set <int>::const_iterator s = selected_rows.begin (); s != selected_rows.end (); ++s) {
    symbol_table->selectionModel ()->select (symbol_table->model ()->index (*s, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
  }

  if (n_current >= 0) {
    symbol_table->selectionModel ()->select (symbol_table->model ()->index (n_current, 0), QItemSelectionModel::Current | QItemSelectionModel::Rows);
  }
}

void 
NetTracerTechComponentEditor::symbol_move_down_clicked ()
{
  //  removes focus from the tree view - commits the data
  move_symbol_down_pb->setFocus (); 

  std::set<int> selected_rows;
  QModelIndexList selected_indices = symbol_table->selectionModel ()->selectedIndexes ();
  for (QModelIndexList::const_iterator i = selected_indices.begin (); i != selected_indices.end (); ++i) {
    selected_rows.insert (i->row ());
  }

  QTableWidgetItem *current = symbol_table->currentItem ();
  int n_current = current ? current->data (Qt::UserRole).toInt () : -1;

  symbol_table->setCurrentIndex (QModelIndex ());

  int n = m_data.symbols ();
  for (NetTracerTechnologyComponent::symbol_iterator l = m_data.end_symbols (); l != m_data.begin_symbols (); ) {
    --l;
    --n;
    if (selected_rows.find (n - 1) != selected_rows.end () && selected_rows.find (n) == selected_rows.end ()) {
      std::swap (m_data.begin_symbols () [n - 1], m_data.begin_symbols () [n]);
      selected_rows.erase (n - 1);
      selected_rows.insert (n);
      if (n_current == n - 1) {
        n_current = n;
      }
    }
  }

  update ();

  // select the new items
  for (std::set <int>::const_iterator s = selected_rows.begin (); s != selected_rows.end (); ++s) {
    symbol_table->selectionModel ()->select (symbol_table->model ()->index (*s, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
  }

  if (n_current >= 0) {
    symbol_table->selectionModel ()->select (symbol_table->model ()->index (n_current, 0), QItemSelectionModel::Current | QItemSelectionModel::Rows);
  }
}

void
NetTracerTechComponentEditor::update ()
{
  QStringList labels;
  int n;

  connectivity_table->clear ();
  connectivity_table->setRowCount (m_data.size ());

  connectivity_table->setColumnCount (3);
  labels.clear ();
  labels << QObject::tr ("Conductor 1");
  labels << QObject::tr ("Via (optional)");
  labels << QObject::tr ("Conductor 2");
  connectivity_table->setHorizontalHeaderLabels (labels);

  n = 0;
  for (NetTracerTechnologyComponent::iterator l = m_data.begin (); l != m_data.end (); ++l, ++n) {

    for (int c = 0; c < 3; ++c) {

      QTableWidgetItem *item = new QTableWidgetItem ();
      item->setFlags (item->flags () | Qt::ItemIsEditable);
      connectivity_table->setItem (n, c, item);

      item->setData (Qt::ForegroundRole, QVariant ());

      if (c == 0) {
        if (l->layer_a ().to_string ().empty ()) {
          item->setData (Qt::DisplayRole, QVariant (QObject::tr ("Enter layer")));
          item->setData (Qt::ForegroundRole, QVariant (Qt::red));
          item->setData (Qt::BackgroundRole, QVariant (QColor (Qt::red).lighter (180)));
        } else {
          item->setData (Qt::DisplayRole, QVariant (tl::to_qstring (l->layer_a ().to_string ())));
        }
      } else if (c == 1) {
        if (l->via_layer ().to_string ().empty ()) {
          item->setData (Qt::DisplayRole, QVariant (QObject::tr ("None")));
        } else {
          item->setData (Qt::DisplayRole, QVariant (tl::to_qstring (l->via_layer ().to_string ())));
        }
      } else if (c == 2) {
        if (l->layer_b ().to_string ().empty ()) {
          item->setData (Qt::DisplayRole, QVariant (QObject::tr ("Enter layer")));
          item->setData (Qt::ForegroundRole, QVariant (Qt::red));
          item->setData (Qt::BackgroundRole, QVariant (QColor (Qt::red).lighter (180)));
        } else {
          item->setData (Qt::DisplayRole, QVariant (tl::to_qstring (l->layer_b ().to_string ())));
        }
      }

      item->setData (Qt::UserRole, QVariant (n));

    }

  }

  connectivity_table->clearSelection ();

  symbol_table->clear ();
  symbol_table->setRowCount (m_data.symbols ());

  symbol_table->setColumnCount (2);
  labels.clear ();
  labels << QObject::tr ("Symbol");
  labels << QObject::tr ("Expression");
  symbol_table->setHorizontalHeaderLabels (labels);

  n = 0;
  for (NetTracerTechnologyComponent::symbol_iterator l = m_data.begin_symbols (); l != m_data.end_symbols (); ++l, ++n) {

    for (int c = 0; c < 2; ++c) {

      QTableWidgetItem *item = new QTableWidgetItem ();
      item->setFlags (item->flags () | Qt::ItemIsEditable);
      symbol_table->setItem (n, c, item);

      item->setData (Qt::ForegroundRole, QVariant ());
      item->setData (Qt::BackgroundRole, QVariant ());

      if (c == 0) {

        if (l->symbol ().log_equal (db::LayerProperties ())) {
          item->setData (Qt::DisplayRole, QVariant (QObject::tr ("Enter symbol")));
          item->setData (Qt::ForegroundRole, QVariant (Qt::red));
          item->setData (Qt::BackgroundRole, QVariant (QColor (Qt::red).lighter (180)));
        } else {
          item->setData (Qt::DisplayRole, QVariant (tl::to_qstring (l->symbol ().to_string ())));
        }

      } else if (c == 1) {

        if (l->expression ().empty ()) {

          item->setData (Qt::DisplayRole, QVariant (QObject::tr ("Enter expression")));
          item->setData (Qt::ForegroundRole, QVariant (Qt::red));
          item->setData (Qt::BackgroundRole, QVariant (QColor (Qt::red).lighter (180)));

        } else {

          bool ok = true;
          //  check the expression
          try {
            NetTracerLayerExpressionInfo::compile (l->expression ());
          } catch (tl::Exception &) {
            ok = false;
          }

          if (! ok) {
            item->setData (Qt::ForegroundRole, QVariant (Qt::red));
            item->setData (Qt::BackgroundRole, QVariant (QColor (Qt::red).lighter (180)));
          }

          item->setData (Qt::DisplayRole, QVariant (tl::to_qstring (l->expression ())));

        }

      }

      item->setData (Qt::UserRole, QVariant (n));

    }

  }

  symbol_table->clearSelection ();
}

// -----------------------------------------------------------------------------------
//  Net implementation

Net::Net ()
  : m_dbu (0.001), m_incomplete (true), m_trace_path (false)
{
  //  .. nothing yet ..
}

Net::Net (const NetTracer &tracer, const db::ICplxTrans &trans, const db::Layout &layout, db::cell_index_type cell_index, const std::string &layout_filename, const std::string &layout_name, const NetTracerData &data)
  : m_name (tracer.name ()), m_incomplete (tracer.incomplete ())
{
  m_dbu = layout.dbu ();
  m_top_cell_name = layout.cell_name (cell_index);
  m_layout_filename = layout_filename;
  m_layout_name = layout_name;

  size_t n = 0;
  for (NetTracer::iterator s = tracer.begin (); s != tracer.end (); ++s) {
    ++n;
  }
  m_net_shapes.reserve (n);

  for (NetTracer::iterator s = tracer.begin (); s != tracer.end (); ++s) {

    // @@@ should reset propery ID:
    tl::ident_map<db::properties_id_type> pm;
    db::Shape new_shape = m_shapes.insert (s->shape (), trans, pm);
    m_net_shapes.push_back (*s);
    m_net_shapes.back ().shape (new_shape);

    if (m_cell_names.find (s->cell_index ()) == m_cell_names.end ()) {
      m_cell_names.insert (std::make_pair (s->cell_index (), layout.cell_name (s->cell_index ())));
    }

    if (m_layers.find (s->layer ()) == m_layers.end ()) {

      unsigned int l = s->layer ();
      db::LayerProperties lp;
      db::LayerProperties lprep;

      if (layout.is_valid_layer (l)) {

        lp = layout.get_properties (l);
        lprep = lp;

      } else {

        int lrep = data.expression (l).representative_layer ();
        if (layout.is_valid_layer (lrep)) {
          lprep = layout.get_properties (lrep);
        }

        for (std::map<std::string, unsigned int>::const_iterator sy = data.symbols ().begin (); sy != data.symbols ().end (); ++sy) {
          if (sy->second == l) {
            tl::Extractor ex (sy->first.c_str ());
            lp.read (ex);
            break;
          }
        }

      }

      define_layer (l, lp, lprep);

    }

  }
}

std::vector<unsigned int>
Net::export_net (db::Layout &layout, db::Cell &export_cell)
{
  std::vector<unsigned int> new_layers;
  std::map<unsigned int, unsigned int> layer_map;

  for (iterator net_shape = begin (); net_shape != end (); ++net_shape) {

    if (net_shape->is_pseudo ()) {
      continue;
    }

    std::map<unsigned int, unsigned int>::const_iterator lm = layer_map.find (net_shape->layer ());
    if (lm == layer_map.end ()) {

      int layer_index = -1;
      for (db::Layout::layer_iterator l = layout.begin_layers (); l != layout.end_layers (); ++l) {
        if ((*l).second->log_equal (representative_layer_for (net_shape->layer ()))) {
          layer_index = int ((*l).first);
          break;
        }
      }

      if (layer_index < 0) {
        layer_index = int (layout.insert_layer (representative_layer_for (net_shape->layer ())));
        new_layers.push_back (layer_index);
      }

      lm = layer_map.insert (std::make_pair (net_shape->layer (), (unsigned int)layer_index)).first;

    }

    tl::ident_map<db::properties_id_type> pm;
    export_cell.shapes (lm->second).insert (net_shape->shape (), db::ICplxTrans (net_shape->trans ()), pm);

  }

  return new_layers;
}

const std::string &
Net::cell_name (db::cell_index_type cell_index) const
{
  std::map <unsigned int, std::string>::const_iterator cn = m_cell_names.find (cell_index);
  if (cn != m_cell_names.end ()) {
    return cn->second;
  } else {
    static std::string n;
    return n;
  }
}

db::LayerProperties 
Net::representative_layer_for (unsigned int log_layer) const
{
  std::map <unsigned int, std::pair <db::LayerProperties, db::LayerProperties> >::const_iterator l = m_layers.find (log_layer);
  if (l != m_layers.end ()) {
    return l->second.second;
  } else {
    return db::LayerProperties ();
  }
}

db::LayerProperties 
Net::layer_for (unsigned int log_layer) const
{
  std::map <unsigned int, std::pair <db::LayerProperties, db::LayerProperties> >::const_iterator l = m_layers.find (log_layer);
  if (l != m_layers.end ()) {
    return l->second.first;
  } else {
    return db::LayerProperties ();
  }
}

void 
Net::define_layer (unsigned int l, const db::LayerProperties &lp, const db::LayerProperties &lp_representative)
{
  m_layers.insert (std::make_pair (l, std::make_pair (lp, lp_representative)));
}

// -----------------------------------------------------------------------------------
//  NetTracerDialog implementation

NetTracerDialog::NetTracerDialog (lay::MainWindow *main_window, lay::LayoutView *view)
  : lay::Browser (main_window, view, "net_tracer_dialog"),
    lay::ViewService (view->view_object_widget ()), 
    m_cv_index (0), 
    m_net_index (1),
    m_window (ext::NTFitNet),
    m_window_dim (0.0),
    m_max_marker_count (0),
    m_marker_line_width (-1),
    m_marker_vertex_size (-1),
    m_marker_halo (-1),
    m_marker_dither_pattern (-1),
    m_marker_intensity (0),
    m_auto_color_enabled (false),
    m_auto_color_index (0),
    m_mouse_state (0)
{
  mp_export_file_dialog = new lay::FileDialog (this, tl::to_string (QObject::tr ("Export Net")), tl::to_string (QObject::tr ("KLayout net files (*.lyn);;All files (*)")));

  Ui::NetTracerDialog::setupUi (this);

  connect (add_pb, SIGNAL (clicked ()), this, SLOT (trace_net_button_clicked ()));
  connect (add2_pb, SIGNAL (clicked ()), this, SLOT (trace_path_button_clicked ()));
  connect (del_pb, SIGNAL (clicked ()), this, SLOT (delete_button_clicked ()));
  connect (clear_all_pb, SIGNAL (clicked ()), this, SLOT (clear_all_button_clicked ()));
  connect (detailed_cb, SIGNAL (clicked ()), this, SLOT (detailed_mode_clicked ()));
  connect (export_pb, SIGNAL (clicked ()), this, SLOT (export_clicked ()));
  connect (redo_pb, SIGNAL (clicked ()), this, SLOT (redo_trace_clicked ()));
  connect (export_text_pb, SIGNAL (clicked ()), this, SLOT (export_text_clicked ()));
  connect (configure_pb, SIGNAL (clicked ()), this, SLOT (configure_clicked ()));
  connect (stack_pb, SIGNAL (clicked ()), this, SLOT (layer_stack_clicked ()));
  connect (net_list, SIGNAL (itemSelectionChanged ()), this, SLOT (item_selection_changed ()));
  connect (net_color, SIGNAL (color_changed (QColor)), this, SLOT (net_color_changed (QColor)));
  connect (net_list, SIGNAL (itemDoubleClicked (QListWidgetItem *)), this, SLOT (item_double_clicked (QListWidgetItem *)));
  connect (sticky_cbx, SIGNAL (clicked ()), this, SLOT (sticky_mode_clicked ()));

  view->add_layer_list_observer (*this);

  update_info ();
}

NetTracerDialog::~NetTracerDialog ()
{
  view ()->remove_layer_list_observer (*this);

  clear_markers ();
  clear_nets ();
}

void
NetTracerDialog::clear_nets ()
{
  for (std::vector <Net *>::iterator n = mp_nets.begin (); n != mp_nets.end (); ++n) {
    delete *n;
  }
  mp_nets.clear ();
}

void 
NetTracerDialog::item_double_clicked (QListWidgetItem *item)
{
  int item_index = net_list->row (item);
  if (item_index >= 0 && item_index < int (mp_nets.size ())) {

    QString name = tl::to_qstring (mp_nets [item_index]->name ());

    bool ok = false;
    name = QInputDialog::getText (this,
                                  QObject::tr ("Net Name"),
                                  QObject::tr ("Enter new net name"),
                                  QLineEdit::Normal, name,
                                  &ok);

    if (ok) {

      mp_nets [item_index]->set_name (tl::to_string (name));

      update_list ();
      item_selection_changed ();

    }

  }
}

bool 
NetTracerDialog::mouse_move_event (const db::DPoint &p, unsigned int buttons, bool prio) 
{ 
  if (prio && m_mouse_state != 0) {
    set_cursor (lay::Cursor::cross);
  }

  return false;
}

bool 
NetTracerDialog::mouse_click_event (const db::DPoint &p, unsigned int buttons, bool prio) 
{ 
  if (prio && (buttons & lay::LeftButton) != 0 && m_mouse_state != 0) {

    if (m_mouse_state == 2) {

      m_mouse_first_point = p;
      m_mouse_state = 3;

      view ()->message (tl::to_string (QObject::tr ("Click on the second point in the net")));

    } else {

      bool trace_path = (m_mouse_state == 3);

      if (trace_path || ! sticky_cbx->isChecked ()) {
        release_mouse ();
      }

      //  prepare for the net tracing 
      clear_markers ();

      double l = double (lay::search_range) / widget ()->mouse_event_trans ().mag ();

      db::DBox start_search_box = db::DBox (p, p).enlarged (db::DPoint (l, l));

      db::DBox stop_search_box;
      if (trace_path) {
        stop_search_box = db::DBox (m_mouse_first_point, m_mouse_first_point).enlarged (db::DPoint (l, l));
      }

      Net *net = do_trace (start_search_box, stop_search_box, trace_path);
      if (net) {

        //  create a new net taking the shapes from the tracer
        mp_nets.push_back (net);

        //  do auto coloring
        if (m_auto_color_enabled) {
          if (m_auto_color_index < int (m_auto_colors.colors ())) {
            mp_nets.back ()->set_color (QColor (m_auto_colors.color_by_index (m_auto_color_index)));
          }
          ++m_auto_color_index;
          if (m_auto_color_index >= int (m_auto_colors.colors ())) {
            m_auto_color_index = 0;
          }
        }

        std::string n = mp_nets.back ()->name ();
        if (n.empty ()) {
          mp_nets.back ()->set_name (tl::sprintf (tl::to_string (QObject::tr ("Net%d")), m_net_index++));
        }

        update_list ();
        item_selection_changed ();
        net_list->setCurrentItem (net_list->item (mp_nets.size () - 1));

      }

    }

    return true;

  } else {
    return false; 
  }
}

void 
NetTracerDialog::redo_trace_clicked ()
{
BEGIN_PROTECTED

  std::set <Net *> selected_nets;

  QList<QListWidgetItem *> selected_items = net_list->selectedItems ();
  for (QList<QListWidgetItem *>::const_iterator item = selected_items.begin (); item != selected_items.end (); ++item) {
    int item_index = net_list->row (*item);
    if (item_index >= 0 && item_index < int (mp_nets.size ())) {
      selected_nets.insert (mp_nets [item_index]);
    }
  }

  std::vector <Net *> nets;
  nets.swap (mp_nets);

  m_net_index = 1;

  std::vector <size_t> new_selection;

  for (std::vector <Net *>::const_iterator n = nets.begin (); n != nets.end (); ++n) {

    try {

      Net *net = do_trace ((*n)->start_search_box (), (*n)->stop_search_box (), (*n)->trace_path_flag ());
      if (net) {

        //  create a new net taking the shapes from the tracer
        mp_nets.push_back (net);

        net->set_color ((*n)->color ());

        std::string name = mp_nets.back ()->name ();
        if (name.empty ()) {
          mp_nets.back ()->set_name (tl::sprintf (tl::to_string (QObject::tr ("Net%d")), m_net_index++));
        }

        if (selected_nets.find (*n) != selected_nets.end ()) {
          new_selection.push_back (mp_nets.size () - 1);
        }

      }

    } catch (...) {
      //  ignore errors on redo
    }

    delete *n;

  }

  //  re-establish the selection
  net_list->blockSignals (true);
  update_list ();
  for (std::vector <size_t>::const_iterator i = new_selection.begin (); i != new_selection.end (); ++i) {
    net_list->item (int (*i))->setSelected (true);
  }
  net_list->blockSignals (false);

  item_selection_changed ();

END_PROTECTED
}

Net *
NetTracerDialog::do_trace (const db::DBox &start_search_box, const db::DBox &stop_search_box, bool trace_path)
{
  unsigned int start_layer = 0;
  db::Point start_point;

  //  locate the seed
  {

    lay::ShapeFinder finder (true /*point mode*/, false /*all levels*/, db::ShapeIterator::All);

    //  go through all visible layers of all cellviews and find a seed shape
    for (lay::LayerPropertiesConstIterator lprop = view ()->begin_layers (); ! lprop.at_end (); ++lprop) {
      if (lprop->is_visual ()) {
        finder.find (view (), *lprop, start_search_box);
      }
    }

    //  return, if no shape was found
    lay::ShapeFinder::iterator r = finder.begin ();
    if (r == finder.end ()) {
      return 0;
    }

    m_cv_index = r->cv_index ();

    start_layer = r->layer ();

  }

  //  determine the cellview
  lay::CellView cv = view ()->cellview (m_cv_index);
  if (! cv.is_valid ()) {
    return 0;
  }

  //  determine the start point
  {

    std::vector<db::CplxTrans> tv = view ()->cv_transform_variants (m_cv_index, start_layer);
    if (tv.empty ()) {
      return 0;
    }

    db::DCplxTrans tt = db::CplxTrans (cv->layout ().dbu ()) * tv.front () * cv.context_trans ();

    start_point = tt.inverted ().trans (start_search_box.center ());

  }

  //  fetch the net tracer data from the technology and apply to the current layout
  const lay::Technology *tech = cv->technology ();
  if (! tech) {
    return 0;
  }
  const NetTracerTechnologyComponent *tech_component = dynamic_cast <const NetTracerTechnologyComponent *> (tech->component_by_name (net_tracer_component_name));
  if (! tech_component) {
    return 0;
  }

  //  test run on the expressions to verify their syntax
  int n = 1;
  for (NetTracerTechnologyComponent::const_iterator c = tech_component->begin (); c != tech_component->end (); ++c, ++n) {
    if (c->layer_a ().to_string ().empty ()) {
      throw tl::Exception (tl::to_string (QObject::tr ("Missing first layer specification on connectivity specification #%d")), n);
    } 
    if (c->layer_b ().to_string ().empty ()) {
      throw tl::Exception (tl::to_string (QObject::tr ("Missing second layer specification on connectivity specification #%d")), n);
    }
  }

  n = 1;
  for (NetTracerTechnologyComponent::const_symbol_iterator s = tech_component->begin_symbols (); s != tech_component->end_symbols (); ++s, ++n) {
    if (s->symbol ().to_string ().empty ()) {
      throw tl::Exception (tl::to_string (QObject::tr ("Missing symbol name on symbol specification #%d")), n);
    } 
    if (s->expression ().empty ()) {
      throw tl::Exception (tl::to_string (QObject::tr ("Missing expression on symbol specification #%d")), n);
    } 
    try {
      std::auto_ptr<NetTracerLayerExpression> expr_in (NetTracerLayerExpressionInfo::compile (s->expression ()).get (cv->layout (), *tech_component));
    } catch (tl::Exception &ex) {
      throw tl::Exception (tl::to_string (QObject::tr ("Error compiling expression '%s' (symbol #%d): %s")), s->expression (), n, ex.msg ());
    }
  }

  //  Set up the net tracer environment
  NetTracerData tracer_data;

  //  register a logical layer for each original one as alias and one for each expression with a new ID
  for (NetTracerTechnologyComponent::const_symbol_iterator s = tech_component->begin_symbols (); s != tech_component->end_symbols (); ++s) {
    NetTracerLayerExpression *expr = NetTracerLayerExpressionInfo::compile (s->expression ()).get (cv->layout (), *tech_component);
    tracer_data.register_logical_layer (expr, s->symbol ().to_string ().c_str ());
  }

  for (NetTracerTechnologyComponent::const_iterator c = tech_component->begin (); c != tech_component->end (); ++c) {
    tracer_data.add_connection (c->get (cv->layout (), *tech_component, tracer_data));
  }

  unsigned int stop_layer = 0;
  db::Point stop_point;

  //  locate the stop shape (the second mouse click)
  if (trace_path) {

    lay::ShapeFinder finder (true /*point mode*/, false /*all levels*/, db::ShapeIterator::All);

    //  go through all visible layers of all cellviews and find a seed shape
    for (lay::LayerPropertiesConstIterator lprop = view ()->begin_layers (); ! lprop.at_end (); ++lprop) {
      if (lprop->is_visual ()) {
        finder.find (view (), *lprop, stop_search_box);
      }
    }

    //  return, if no shape was found
    lay::ShapeFinder::iterator r = finder.begin ();
    if (r == finder.end ()) {
      return 0;
    }

    if (r->cv_index () != m_cv_index) {
      throw tl::Exception (tl::to_string (QObject::tr ("Both shapes for path tracing must come from the same layout")));
    }

    std::vector<db::CplxTrans> tv = view ()->cv_transform_variants (m_cv_index, r->layer ());
    if (tv.empty ()) {
      return 0;
    }

    db::DCplxTrans tt = db::CplxTrans (cv->layout ().dbu ()) * tv.front () * cv.context_trans ();

    stop_point = tt.inverted ().trans (stop_search_box.center ());
    stop_layer = r->layer ();

  }

  NetTracer net_tracer;

  //  and trace
  try {
    if (trace_path) {
      net_tracer.trace (cv->layout (), *cv.cell (), start_point, start_layer, stop_point, stop_layer, tracer_data);
    } else {
      net_tracer.trace (cv->layout (), *cv.cell (), start_point, start_layer, tracer_data);
    }
  } catch (tl::BreakException &) {
    //  just keep the found shapes on break (user abort)
  }

  if (net_tracer.begin () == net_tracer.end ()) {

    return 0;

  } else {

    //  create a new net taking the shapes from the tracer
    Net *net = new Net (net_tracer, db::ICplxTrans (cv.context_trans ()), cv->layout (), cv.cell_index (), cv->filename (), cv->name (), tracer_data);
    net->set_start_search_box (start_search_box);
    net->set_stop_search_box (stop_search_box);
    net->set_trace_path_flag (trace_path);

    return net;

  }
}

bool 
NetTracerDialog::configure (const std::string &name, const std::string &value)
{
  bool need_update = false;
  bool taken = true;

  if (name == lay::cfg_background_color) {

    need_update = true;

  } else if (name == cfg_nt_marker_cycle_colors) {
      
    m_auto_colors.from_string (value, true);

  } else if (name == cfg_nt_marker_cycle_colors_enabled) {

    bool en = false;
    tl::from_string (value, en);
    if (en != m_auto_color_enabled) {
      m_auto_color_index = 0;
      m_auto_color_enabled = en;
    }

  } else if (name == cfg_nt_window_mode) {

    nt_window_type window = m_window;
    NetTracerWindowModeConverter ().from_string (value, window);
    need_update = lay::test_and_set (m_window, window);

  } else if (name == cfg_nt_window_dim) {

    double wdim = m_window_dim;
    tl::from_string (value, wdim);
    if (fabs (wdim - m_window_dim) > 1e-6) {
      m_window_dim = wdim;
      need_update = true;
    }

  } else if (name == cfg_nt_max_shapes_highlighted) {

    unsigned int mc = 0;
    tl::from_string (value, mc);
    need_update = lay::test_and_set (m_max_marker_count, mc);

  } else if (name == cfg_nt_marker_color) {

    QColor color;
    if (! value.empty ()) {
      lay::ColorConverter ().from_string (value, color);
    }

    if (color != m_marker_color) {
      m_marker_color = color;
      need_update = true;
    }

  } else if (name == cfg_nt_marker_line_width) {

    int lw = 0;
    tl::from_string (value, lw);

    if (lw != m_marker_line_width) {
      m_marker_line_width = lw;
      need_update = true;
    }

  } else if (name == cfg_nt_marker_vertex_size) {

    int vs = 0;
    tl::from_string (value, vs);

    if (vs != m_marker_vertex_size) {
      m_marker_vertex_size = vs;
      need_update = true;
    }

  } else if (name == cfg_nt_marker_halo) {

    int halo = 0;
    tl::from_string (value, halo);

    if (halo != m_marker_halo) {
      m_marker_halo = halo;
      need_update = true;
    }

  } else if (name == cfg_nt_marker_dither_pattern) {

    int dp = 0;
    tl::from_string (value, dp);

    if (dp != m_marker_dither_pattern) {
      m_marker_dither_pattern = dp;
      need_update = true;
    }

  } else if (name == cfg_nt_marker_intensity) {

    int bo = 0;
    tl::from_string (value, bo);

    if (bo != m_marker_intensity) {
      m_marker_intensity = bo;
      need_update = true;
    }

  } else {
    taken = false;
  }

  if (active () && need_update) {
    update_highlights ();
    adjust_view ();
    update_info ();
  }

  return taken;
}

void  
NetTracerDialog::menu_activated (const std::string &symbol)
{
  if (symbol == "ext::net_trace") {

    lay::CellView cv = view ()->cellview (view ()->active_cellview_index ());
    if (cv.is_valid ()) {
      show ();
      activateWindow ();
      raise ();
      activate ();
    }

  } else {
    lay::Browser::menu_activated (symbol);
  }
}

void 
NetTracerDialog::net_color_changed (QColor color)
{
  bool changed = false;

  QList<QListWidgetItem *> selected_items = net_list->selectedItems ();
  for (QList<QListWidgetItem *>::const_iterator item = selected_items.begin (); item != selected_items.end (); ++item) {
    int item_index = net_list->row (*item);
    if (item_index >= 0 && item_index < int (mp_nets.size ())) {
      if (color != mp_nets [item_index]->color ()) {
        mp_nets [item_index]->set_color (color);
        changed = true;
      }
    }
  }

  if (changed) {
    update_highlights ();
    adjust_view ();
    update_list ();
  }
}

void 
NetTracerDialog::item_selection_changed ()
{
  if (active ()) {
    update_highlights ();
    adjust_view ();
    update_info ();
  }
}

void
NetTracerDialog::detailed_mode_clicked ()
{
  update_info ();
}

void
NetTracerDialog::update_info ()
{
  bool detailed = detailed_cb->isChecked ();

  std::ostringstream info_stream;
  info_stream.imbue (std::locale ("C"));

  tl::XMLWriter info (info_stream); 
  
  info.start_document ("");
  info.start_element ("html");
  info.start_element ("body");

  QList<QListWidgetItem *> selected_items = net_list->selectedItems ();

  if (selected_items.size () == 0) {

    info.start_element ("p");
    info.cdata (tl::to_string (QObject::tr ("No net selected")));
    info.end_element ("p");

  } else {

    size_t ntot = 0;

    info.start_element ("p");
    bool first = true;

    for (QList<QListWidgetItem *>::const_iterator item = selected_items.begin (); item != selected_items.end (); ++item) {

      int item_index = net_list->row (*item);
      if (item_index >= 0 && item_index < int (mp_nets.size ())) {

        if (! first) {
          info.start_element ("br");
          info.end_element ("br");
        }
        first = false;

        info.cdata (tl::sprintf (tl::to_string (QObject::tr ("%ld Shapes")), mp_nets[item_index]->size ()));
        if (selected_items.size () > 1) {
          info.cdata (" (" + mp_nets[item_index]->name () + ")");
        }
        if (mp_nets[item_index]->incomplete ()) {
          info.start_element ("span");
          info.write_attribute ("style", "color:red; font-weight: bold");
          info.cdata(" (" + tl::to_string (QObject::tr ("Net is incomplete")) + ") ");
          info.end_element ("span");
        }

        ntot += mp_nets[item_index]->size ();

      }
    }

    info.end_element ("p");

    if (selected_items.size () > 1) {
      info.start_element ("p");
      info.cdata (tl::sprintf (tl::to_string (QObject::tr ("%ld Shapes (total)")), ntot));
      info.end_element ("p");
    }

    if (ntot > m_max_marker_count) {
      info.start_element ("p");
      info.write_attribute ("style", "color:red; font-weight: bold");
      info.cdata (tl::to_string (QObject::tr ("Not all shapes are highlighted")));
      info.end_element ("p");
    }

    if (selected_items.size () == 1) {

      int item_index = net_list->row (selected_items [0]);
      if (item_index >= 0 && item_index < int (mp_nets.size ())) {

        db::CplxTrans dbu = mp_nets [item_index]->dbu ();
        db::CplxTrans dbuinv (1.0 / mp_nets [item_index]->dbu ());
        double dbu_unidir = mp_nets [item_index]->dbu ();

        size_t max_labels = 1000;
        size_t max_cells = 1000;
        size_t max_shapes = 2000;

        if (detailed) {

          info.start_element ("h3");
          info.cdata (tl::to_string (QObject::tr ("General:")));
          info.end_element ("h3");

          info.start_element ("p");
          info.start_element ("b");
          info.cdata (tl::to_string (QObject::tr ("Net name: ")));
          info.end_element ("b");
          info.cdata (mp_nets[item_index]->name ());
          info.start_element ("br");
          info.end_element ("br");
          info.start_element ("b");
          info.cdata (tl::to_string (QObject::tr ("Top cell: ")));
          info.end_element ("b");
          info.cdata (mp_nets[item_index]->top_cell_name ());
          info.start_element ("br");
          info.end_element ("br");
          info.start_element ("b");
          info.cdata (tl::to_string (QObject::tr ("Layout: ")));
          info.end_element ("b");
          info.cdata (mp_nets[item_index]->layout_name ());
          info.start_element ("br");
          info.end_element ("br");
          info.start_element ("b");
          info.cdata (tl::to_string (QObject::tr ("Layout file: ")));
          info.end_element ("b");
          info.cdata (mp_nets[item_index]->layout_filename ());
          info.end_element ("p");

          bool incomplete = false;
          std::map<std::string, std::set<std::string> > shapes;

          //  map as (layernumber, group of shapes by layer): 
          std::map<unsigned int, std::vector<db::Polygon> > shapes_by_layer;
          std::map<unsigned int, std::string> layer_names;
          std::map<unsigned int, db::coord_traits<db::Coord>::area_type> statinfo_area;
          std::map<unsigned int, db::coord_traits<db::Coord>::perimeter_type> statinfo_perimeter;

          size_t tot_shapes = 0;
          for (Net::iterator net_shape = mp_nets [item_index]->begin (); net_shape != mp_nets [item_index]->end (); ++net_shape) {

            if (tot_shapes++ >= max_shapes) {
              incomplete = true;
              break;
            } 

            std::string l (mp_nets [item_index]->layer_for (net_shape->layer ()).to_string ());
            if (l.empty ()) {
              l = "<anonymous>";
            }

            //  Get layer number, to be used as key for map of merged_shapes 
            unsigned int lay_num = net_shape->layer ();

            //  Check if layer is already detected, otherwise create vector-of-Shape object to hold shapes 
            //  plus initialize the perimeter and area sums
            std::map<unsigned int, std::vector<db::Polygon> >::iterator s = shapes_by_layer.find (lay_num); 
            if (s == shapes_by_layer.end ()) {
              s = shapes_by_layer.insert (std::make_pair (lay_num, std::vector<db::Polygon> ())).first;
              layer_names.insert (std::make_pair (lay_num, l));
              statinfo_perimeter.insert (std::make_pair (lay_num, db::coord_traits<db::Coord>::perimeter_type (0)));
              statinfo_area.insert (std::make_pair (lay_num, db::coord_traits<db::Coord>::area_type (0)));
            }

            //  As layer now certainly exists, insert the shape 
            if (net_shape->shape ().is_box () || net_shape->shape ().is_path () || net_shape->shape ().is_polygon ()) {
              s->second.push_back (db::Polygon ());
              net_shape->shape ().polygon (s->second.back ());
              s->second.back ().transform (net_shape->trans ());
            }

            std::string c (std::string (mp_nets [item_index]->cell_name (net_shape->cell_index ())));
            c += " (with ";
            c += (dbu * db::CplxTrans (net_shape->trans ()) * dbuinv).to_string ();
            c += ")";

            std::string t;

            if (net_shape->shape ().is_text ()) {
              db::Text text;
              net_shape->shape ().text (text);
              t = tl::to_string (QObject::tr ("text on ")) + l + ": " + (dbu * text).to_string ();
            } else if (net_shape->shape ().is_box ()) {
              db::Box box;
              net_shape->shape ().box (box);
              t = tl::to_string (QObject::tr ("box on ")) + l + ": " + (dbu * box).to_string ();
            } else if (net_shape->shape ().is_path ()) {
              db::Path path;
              net_shape->shape ().path (path);
              t = tl::to_string (QObject::tr ("path on ")) + l + ": " + (dbu * path).to_string ();
            } else if (net_shape->shape ().is_polygon ()) {
              db::Polygon polygon;
              net_shape->shape ().polygon (polygon);
              t = tl::to_string (QObject::tr ("polygon on ")) + l + ": " + (dbu * polygon).to_string ();
            }

            if (! t.empty ()) {
              shapes.insert (std::make_pair (c, std::set<std::string> ())).first->second.insert (t);
            }

          }

          //  Try to merge all shaped to polygons, use Map of (layernumber, group of polygons by layer) 
          std::map<unsigned int, std::vector<db::Polygon> > polygons_by_layer;
          for (std::map<unsigned int, std::vector<db::Polygon> >::iterator i = shapes_by_layer.begin(); i != shapes_by_layer.end (); ++i) { 

            unsigned int l = i->first;

            db::EdgeProcessor ep;
            std::vector <db::Polygon> &merged = polygons_by_layer.insert (std::make_pair (l, std::vector <db::Polygon> ())).first->second;
            ep.merge(i->second, merged, 0, true, true);

            db::coord_traits<db::Coord>::area_type area = 0;
            db::coord_traits<db::Coord>::perimeter_type perimeter = 0;

            //  Despite merging, a multitude of seperate non-touching polygons can exist.
            for (std::vector <db::Polygon>::iterator j = merged.begin (); j != merged.end (); ++j) {
              //  Sum area 
              area += j->area ();
              //  Sum perimeter for the merged polygon 
              perimeter += j->perimeter ();
            }

            statinfo_area [l] += area;
            statinfo_perimeter [l] += perimeter;

          }

          if (! shapes.empty ()) {

            if (! incomplete) {

              info.start_element ("h3");
              info.cdata (tl::to_string (QObject::tr ("Statistics:")));
              info.end_element ("h3");
              
              db::coord_traits<db::Coord>::area_type total_area = 0;
              db::coord_traits<db::Coord>::perimeter_type total_perimeter = 0;

              //  Print perimeter and area and sum up total
              info.start_element ("table");

              info.start_element ("tr");
              info.start_element ("td");
              info.start_element ("b");
              info.cdata (tl::to_string (QObject::tr ("Layer")));
              info.end_element ("b");
              info.end_element ("td");
              info.start_element ("td");
              info.start_element ("b");
              info.cdata (tl::to_string (QObject::tr ("Perimeter")));
              info.start_element ("br");
              info.end_element ("br");
              info.cdata (tl::to_string (QObject::tr ("(micron)")));
              info.end_element ("b");
              info.end_element ("td");
              info.start_element ("td");
              info.start_element ("b");
              info.cdata (tl::to_string (QObject::tr ("Area")));
              info.start_element ("br");
              info.end_element ("br");
              info.cdata (tl::to_string (QObject::tr ("(square micron)")));
              info.end_element ("b");
              info.end_element ("td");
              info.end_element ("tr");

              for (std::map<unsigned int, db::coord_traits<db::Coord>::area_type>::iterator i = statinfo_area.begin (); i != statinfo_area.end(); ++i) {

                unsigned int l = i->first;

                info.start_element ("tr");
                info.start_element ("td");
                info.cdata (layer_names [l]);
                info.end_element ("td");
                info.start_element ("td");
                total_perimeter += statinfo_perimeter [l];
                info.cdata (tl::micron_to_string (statinfo_perimeter [l] * dbu_unidir));
                info.end_element ("td");
                info.start_element ("td");
                total_area += statinfo_area[l];
                info.cdata (tl::to_string (statinfo_area [l] * dbu_unidir * dbu_unidir));
                info.end_element ("td");
                info.end_element ("tr");

              }

              //  Only if more than one layer is involved, print summed values
              if (statinfo_area.size () != 1) {

                info.start_element ("tr");
                info.start_element ("td");
                info.cdata (tl::to_string (QObject::tr ("Total")));
                info.end_element ("td");
                info.start_element ("td");
                info.cdata (tl::micron_to_string (total_perimeter * dbu_unidir));
                info.end_element ("td");
                info.start_element ("td");
                info.cdata (tl::to_string (total_area * dbu_unidir * dbu_unidir));
                info.end_element ("td");
                info.end_element ("tr");

              }

              info.end_element ("table");

            }

            info.start_element ("h3");
            info.cdata (tl::to_string (QObject::tr ("Shapes:")));
            info.end_element ("h3");

            for (std::map<std::string, std::set<std::string> >::const_iterator s = shapes.begin (); s != shapes.end (); ++s) {

              info.start_element ("p");

              info.start_element ("b");
              info.cdata (tl::to_string (QObject::tr ("Cell ")));
              info.cdata (s->first);
              info.cdata (":");
              info.end_element ("b");

              for (std::set <std::string>::const_iterator l = s->second.begin (); l != s->second.end (); ++l) {
                info.start_element ("br");
                info.end_element ("br");
                info.cdata (*l);
              }

              info.end_element ("p");

            }

            if (incomplete) {
              info.start_element ("p");
              info.cdata ("...");
              info.end_element ("p");
            }

          }

        } else {

          bool incomplete = false;
          std::set<std::string> labels;

          for (Net::iterator net_shape = mp_nets [item_index]->begin (); net_shape != mp_nets [item_index]->end (); ++net_shape) {

            if (net_shape->shape ().is_text ()) {

              if (labels.size () >= max_labels) {
                incomplete = true;
                break;
              }

              std::string t (std::string (mp_nets [item_index]->cell_name (net_shape->cell_index ())) + "." + net_shape->shape ().text_string ());
              labels.insert (t);

            }

          }

          if (! labels.empty ()) {

            info.start_element ("h3");
            info.cdata (tl::to_string (QObject::tr ("Labels:")));
            info.end_element ("h3");

            info.start_element ("p");

            for (std::set <std::string>::const_iterator l = labels.begin (); l != labels.end (); ++l) {
              if (l != labels.begin ()) {
                info.start_element ("br");
                info.end_element ("br");
              }
              info.cdata (*l);
            }

            if (incomplete) {
              info.start_element ("br");
              info.end_element ("br");
              info.cdata ("...");
            }

            info.end_element ("p");

          }

          incomplete = false;
          std::set<std::string> cells;

          for (Net::iterator net_shape = mp_nets [item_index]->begin (); net_shape != mp_nets [item_index]->end (); ++net_shape) {

            if (cells.size () >= max_cells) {
              incomplete = true;
              break;
            }

            std::string t (mp_nets [item_index]->cell_name (net_shape->cell_index ()));
            cells.insert (t);
          }

          if (! cells.empty ()) {

            info.start_element ("h3");
            info.cdata (tl::to_string (QObject::tr ("Cells on net:")));
            info.end_element ("h3");

            info.start_element ("p");

            for (std::set <std::string>::const_iterator c = cells.begin (); c != cells.end (); ++c) {
              if (c != cells.begin ()) {
                info.start_element ("br");
                info.end_element ("br");
              }
              info.cdata (*c);
            }

            if (incomplete) {
              info.start_element ("br");
              info.end_element ("br");
              info.cdata ("...");
            }

            info.end_element ("p");

          }

        }

      }

    }

  }

  info.end_element ("body");
  info.end_element ("html");

  net_info_text->setHtml (tl::to_qstring (info_stream.str ()));

  //  determine and set the common net color

  if (selected_items.size () != 1) {
    net_color->set_color (QColor ());
    net_color->setEnabled (false);
  } else {

    QColor nc;

    int item_index = net_list->row (*selected_items.begin ());
    if (item_index >= 0 && item_index < int (mp_nets.size ())) {
      nc = mp_nets [item_index]->color ();
    }

    net_color->set_color (nc);
    net_color->setEnabled (true);

  }
}

void
NetTracerDialog::update_list ()
{
  QSize icon_size (12, 12);
  net_list->setIconSize (icon_size);

  QPixmap empty_pxmp (icon_size);
  empty_pxmp.fill (QColor (0, 0, 0, 0));

  QColor text_color = palette ().color (QPalette::Active, QPalette::Text);

  for (size_t i = 0; i < mp_nets.size (); ++i) {

    QListWidgetItem *item = 0;

    if (net_list->count () > int (i)) {
      item = net_list->item (i);
    } else {
      item = new QListWidgetItem (net_list);
      net_list->addItem (item);
    }

    item->setData (Qt::DisplayRole, tl::to_qstring (mp_nets [i]->name ()));

    if (mp_nets [i]->color ().isValid ()) {

      QPixmap pxmp (icon_size);
      QPainter pxpainter (&pxmp);
      pxpainter.setPen (QPen (text_color));
      pxpainter.setBrush (QBrush (mp_nets [i]->color ()));
      QRect r (0, 0, pxmp.width () - 1, pxmp.height () - 1);
      pxpainter.drawRect (r);

      item->setIcon (QIcon (pxmp));

    } else {

      item->setIcon (QIcon (empty_pxmp));

    }

  }

  while (net_list->count () > int (mp_nets.size ())) {
    delete net_list->item (int (mp_nets.size ()));
  }
}

void 
NetTracerDialog::trace_path_button_clicked ()
{
BEGIN_PROTECTED
  net_list->setCurrentItem (0);
  m_mouse_state = 2;
  view ()->message (tl::to_string (QObject::tr ("Click on the first point in the net")));
  widget ()->grab_mouse (this, false);
END_PROTECTED
}

void 
NetTracerDialog::trace_net_button_clicked ()
{
BEGIN_PROTECTED
  net_list->setCurrentItem (0);
  m_mouse_state = 1;
  view ()->message (tl::to_string (QObject::tr ("Click on a point in the net")));
  widget ()->grab_mouse (this, false);
END_PROTECTED
}

void
NetTracerDialog::sticky_mode_clicked ()
{
BEGIN_PROTECTED
  if (! sticky_cbx->isChecked ()) {
    release_mouse ();
  } else {
    trace_net_button_clicked ();
  }
END_PROTECTED
}

void 
NetTracerDialog::release_mouse ()
{
  add_pb->setChecked (false);
  add2_pb->setChecked (false);
  m_mouse_state = 0;
  view ()->message ();
  widget ()->ungrab_mouse (this);
}

void
NetTracerDialog::clear_all_button_clicked ()
{
BEGIN_PROTECTED
  release_mouse ();

  if (QMessageBox::question (this, QObject::tr ("Clear All Nets"),
                             QObject::tr ("Are you sure to delete all nets?\nThis operation cannot be undone."),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

    clear_markers ();
    clear_nets ();
    update_list ();
    item_selection_changed ();
    
  }
END_PROTECTED
}

void 
NetTracerDialog::delete_button_clicked ()
{
BEGIN_PROTECTED
  release_mouse ();

  std::vector<int> to_delete;

  QList<QListWidgetItem *> selected_items = net_list->selectedItems ();
  for (QList<QListWidgetItem *>::const_iterator item = selected_items.begin (); item != selected_items.end (); ++item) {
    int item_index = net_list->row (*item);
    if (item_index >= 0 && item_index < int (mp_nets.size ())) {
      to_delete.push_back (item_index);
    }
  }

  std::sort (to_delete.begin (), to_delete.end ());

  for (std::vector<int>::const_iterator d = to_delete.end (); d != to_delete.begin (); ) {
    --d;
    delete mp_nets [*d];
    mp_nets.erase (mp_nets.begin () + *d);
  }

  clear_markers ();
  update_list ();
  item_selection_changed ();
    
END_PROTECTED
}

void 
NetTracerDialog::layer_stack_clicked ()
{
BEGIN_PROTECTED

  release_mouse ();

  std::string tech_name;

  //  use actual technology name of the active cellview
  int cv_index = view ()->active_cellview_index ();
  lay::CellView cv = view ()->cellview (cv_index);
  if (cv.is_valid ()) {
    tech_name = cv->tech_name ();
  }

  if (! lay::Technologies::instance ()->has_technology (tech_name)) {
    throw std::runtime_error (tl::to_string (QObject::tr ("Invalid technology attached to layout: ")) + tech_name);
  }

  //  create a temporary copy
  lay::Technology tech = *lay::Technologies::instance ()->technology_by_name (tech_name);

  //  call the dialog and if successful, install the new technology
  lay::TechComponentSetupDialog dialog (this, &tech, net_tracer_component_name);
  if (dialog.exec ()) {
    *lay::Technologies::instance ()->technology_by_name (tech.name ()) = tech;
  }

END_PROTECTED
}

void 
NetTracerDialog::export_text_clicked ()
{
BEGIN_PROTECTED

  release_mouse ();

  int cv_index = view ()->active_cellview_index ();
  lay::CellView cv = view ()->cellview (cv_index);
  if (cv.is_valid ()) {

    QList<QListWidgetItem *> selected_items = net_list->selectedItems ();
    if (selected_items.size () == 0) {
      throw tl::Exception (tl::to_string (QObject::tr ("No net selected to export")));
    }

    if (mp_export_file_dialog->get_save (m_export_file_name)) {

      std::ofstream os (m_export_file_name.c_str ());
      tl::XMLWriter w (os);

      w.start_document ();

      w.start_element ("nets");

      for (QList<QListWidgetItem *>::const_iterator item = selected_items.begin (); item != selected_items.end (); ++item) {

        int item_index = net_list->row (*item);
        if (item_index >= 0 && item_index < int (mp_nets.size ())) {

          w.start_element ("net");

          const Net *net = mp_nets[item_index];

          w.start_element ("name");
          w.cdata (net->name ());
          w.end_element ("name");

          w.start_element ("top_cell");
          w.cdata (net->top_cell_name ());
          w.end_element ("top_cell");

          w.start_element ("layout");
          w.cdata (net->layout_filename ());
          w.end_element ("layout");

          w.start_element ("dbu");
          w.cdata (tl::to_string (net->dbu ()));
          w.end_element ("dbu");

          w.start_element ("complete");
          w.cdata (tl::to_string (! net->incomplete ()));
          w.end_element ("complete");

          w.start_element ("shapes");

          for (Net::iterator net_shape = net->begin (); net_shape != net->end (); ++net_shape) {

            w.start_element ("element");

            w.start_element ("layer");
            std::string l (mp_nets [item_index]->layer_for (net_shape->layer ()).to_string ());
            if (l.empty ()) {
              l = "<anonymous>";
            }
            w.cdata (l);
            w.end_element ("layer");

            w.start_element ("cell");
            w.cdata (net->cell_name (net_shape->cell_index ()));
            w.end_element ("cell");

            w.start_element ("trans");
            w.cdata (net_shape->trans ().to_string ());
            w.end_element ("trans");

            w.start_element ("shape");
            w.cdata (net_shape->shape ().to_string ());
            w.end_element ("shape");

            w.end_element ("element");

          }

          w.end_element ("shapes");

          w.end_element ("net");

        }

      }

      w.end_element ("nets");

      w.end_document ();

    }

  }

END_PROTECTED
}

void 
NetTracerDialog::export_clicked ()
{
BEGIN_PROTECTED

  release_mouse ();

  int cv_index = view ()->active_cellview_index ();
  lay::CellView cv = view ()->cellview (cv_index);
  if (cv.is_valid ()) {

    QList<QListWidgetItem *> selected_items = net_list->selectedItems ();
    if (selected_items.size () == 0) {
      throw tl::Exception (tl::to_string (QObject::tr ("No net selected to export")));
    }

    bool ok;
    QString cell_name = tl::to_qstring (m_export_cell_name);
    cell_name = QInputDialog::getText (this, QObject::tr ("Export Net"), QObject::tr ("Export net to cell named"), QLineEdit::Normal, cell_name, &ok);
    if (ok) {

      if (cell_name.isEmpty ()) {
        throw tl::Exception (tl::to_string (QObject::tr ("No cell was specified")));
      }

      //  Clear undo buffers if layout is created.
      view ()->manager ()->clear ();

      m_export_cell_name = tl::to_string (cell_name);
      std::pair<bool, db::cell_index_type> cbn = cv->layout ().cell_by_name (m_export_cell_name.c_str ());
      if (! cbn.first) {
        cbn.second = cv->layout ().add_cell (m_export_cell_name.c_str ());
      } 

      db::Cell &export_cell = cv->layout ().cell (cbn.second);

      for (QList<QListWidgetItem *>::const_iterator item = selected_items.begin (); item != selected_items.end (); ++item) {

        int item_index = net_list->row (*item);
        if (item_index >= 0 && item_index < int (mp_nets.size ())) {

          std::vector<unsigned int> new_layers = mp_nets[item_index]->export_net (cv->layout (), export_cell);

          //  Add a new entries in the layer list
          for (std::vector<unsigned int>::const_iterator l = new_layers.begin (); l != new_layers.end (); ++l) {
            lay::LayerProperties props;
            props.set_source (lay::ParsedLayerSource (cv->layout ().get_properties (*l), cv_index));
            view ()->init_layer_properties (props);
            view ()->insert_layer (view ()->end_layers (), props);
          }

        }

      }

      view ()->select_cell (export_cell.cell_index (), view ()->active_cellview_index ());

    }

  }

END_PROTECTED
}

void 
NetTracerDialog::configure_clicked ()
{
BEGIN_PROTECTED
  lay::ConfigurationDialog config_dialog (this, main_window (), "NetTracerPlugin");
  config_dialog.exec ();
END_PROTECTED
}

void  
NetTracerDialog::deactivated ()
{
  clear_markers ();
  release_mouse ();
}

void  
NetTracerDialog::activated ()
{
  // .. nothing yet ..
}

lay::ViewService * 
NetTracerDialog::view_service_interface ()
{
  return this;
}

void
NetTracerDialog::signal_observed ()
{
  if (active ()) {
    update_highlights ();
  }
}

void
NetTracerDialog::adjust_view ()
{
  int cv_index = view ()->active_cellview_index ();
  const lay::CellView &cv = view ()->cellview (cv_index);
  if (! cv.is_valid ()) {
    return;
  }

  if (m_window != NTFitNet && m_window != NTCenter && m_window != NTCenterSize) {
    return;
  }

  db::DBox bbox;

  QList<QListWidgetItem *> selected_items = net_list->selectedItems ();
  for (QList<QListWidgetItem *>::const_iterator item = selected_items.begin (); item != selected_items.end (); ++item) {

    int item_index = net_list->row (*item);

    if (item_index >= 0 && item_index < int (mp_nets.size ())) {

      std::map<unsigned int, std::vector<db::CplxTrans> > tv_by_layer = view ()->cv_transform_variants_by_layer (cv_index);

      std::map <db::LayerProperties, unsigned int, db::LPLogicalLessFunc> lm;
      for (db::Layout::layer_iterator l = cv->layout ().begin_layers (); l != cv->layout ().end_layers (); ++l) {
        lm.insert (std::make_pair (*(*l).second, (*l).first));
      }

      std::map <unsigned int, unsigned int> llmap;

      db::DBox cv_bbox;

      //  Create markers for the shapes 
      for (Net::iterator net_shape = mp_nets [item_index]->begin (); net_shape != mp_nets [item_index]->end (); ++net_shape) {

        //  Find the actual layer by looking up the layer properties ..
        std::map <unsigned int, unsigned int>::const_iterator ll = llmap.find (net_shape->layer ());
        if (ll == llmap.end ()) {
          std::map <db::LayerProperties, unsigned int, db::LPLogicalLessFunc>::const_iterator i = lm.find (mp_nets [item_index]->representative_layer_for (net_shape->layer ()));
          if (i != lm.end ()) {
            ll = llmap.insert (std::make_pair (net_shape->layer (), i->second)).first;
          }
        }

        unsigned int ly = 0;
        if (ll != llmap.end ()) {
          ly = ll->second;
        }

        std::map<unsigned int, std::vector<db::CplxTrans> >::const_iterator tv = tv_by_layer.find (ly);
        if (tv != tv_by_layer.end ()) {

          db::Box shape_box = net_shape->shape ().bbox ();
          for (std::vector<db::CplxTrans>::const_iterator t = tv->second.begin (); t != tv->second.end (); ++t) {
            cv_bbox += *t * db::CplxTrans(net_shape->trans ()) * shape_box;
          }

        }

      }

      bbox += cv_bbox * cv->layout ().dbu ();

    }

  }

  if (! bbox.empty ()) {

    if (m_window == NTFitNet) {

      view ()->zoom_box (bbox.enlarged (db::DPoint (m_window_dim, m_window_dim)));

    } else if (m_window == NTCenter) {

      view ()->pan_center ((bbox.p1 () + bbox.p2 ()) * 0.5);

    } else if (m_window == NTCenterSize) {

      double w = std::max (bbox.width (), m_window_dim);
      double h = std::max (bbox.height (), m_window_dim);
      db::DPoint center ((bbox.p1 () + bbox.p2 ()) * 0.5);
      db::DPoint d (w * 0.5, h * 0.5);
      view ()->zoom_box (db::DBox (center - d, center + d));

    }

  }

}

void
NetTracerDialog::update_highlights ()
{
  clear_markers ();

  int cv_index = view ()->active_cellview_index ();
  const lay::CellView &cv = view ()->cellview (cv_index);
  if (! cv.is_valid ()) {
    return;
  }

  size_t n_marker = 0;

  QList<QListWidgetItem *> selected_items = net_list->selectedItems ();
  for (QList<QListWidgetItem *>::const_iterator item = selected_items.begin (); item != selected_items.end (); ++item) {

    int item_index = net_list->row (*item);

    if (item_index >= 0 && item_index < int (mp_nets.size ())) {

      std::map<unsigned int, std::vector<db::CplxTrans> > tv_by_layer = view ()->cv_transform_variants_by_layer (cv_index);
      std::map<unsigned int, lay::LayerPropertiesConstIterator> layer_props;

      std::map <db::LayerProperties, unsigned int, db::LPLogicalLessFunc> lm;
      for (db::Layout::layer_iterator l = cv->layout ().begin_layers (); l != cv->layout ().end_layers (); ++l) {
        lm.insert (std::make_pair (*(*l).second, (*l).first));
      }

      std::map <unsigned int, unsigned int> llmap;

      QColor net_color = mp_nets [item_index]->color ();

      //  Create markers for the shapes 
      for (Net::iterator net_shape = mp_nets [item_index]->begin (); net_shape != mp_nets [item_index]->end () && n_marker < m_max_marker_count; ++net_shape) {

        //  Find the actual layer by looking up the layer properties ..
        std::map <unsigned int, unsigned int>::const_iterator ll = llmap.find (net_shape->layer ());
        if (ll == llmap.end ()) {
          std::map <db::LayerProperties, unsigned int, db::LPLogicalLessFunc>::const_iterator i = lm.find (mp_nets [item_index]->representative_layer_for (net_shape->layer ()));
          if (i != lm.end ()) {
            ll = llmap.insert (std::make_pair (net_shape->layer (), i->second)).first;
          }
        }

        unsigned int ly = 0;
        if (ll != llmap.end ()) {
          ly = ll->second;
        }

        std::map<unsigned int, std::vector<db::CplxTrans> >::const_iterator tv = tv_by_layer.find (ly);
        if (tv != tv_by_layer.end ()) {

          lay::LayerPropertiesConstIterator original;

          std::map<unsigned int, lay::LayerPropertiesConstIterator>::const_iterator lp_cache = layer_props.find (ly);
          if (lp_cache != layer_props.end ()) {
            original = lp_cache->second;
          } else {
            for (lay::LayerPropertiesConstIterator lp = view ()->begin_layers (); !lp.at_end (); ++lp) {
              if (!lp->has_children () && lp->cellview_index () == int (cv_index) && lp->layer_index () == int (ly)) {
                layer_props.insert (std::make_pair (ly, lp));
                original = lp;
                break;
              }
            }
          }

          mp_markers.push_back (new lay::ShapeMarker (view (), cv_index));
          mp_markers.back ()->set (net_shape->shape (), net_shape->trans (), tv->second);

          if (! original.at_end ()) {
            mp_markers.back ()->set_line_width (original->width (true));
            mp_markers.back ()->set_vertex_size (1);
            mp_markers.back ()->set_dither_pattern (original->dither_pattern (true));
            if (view ()->background_color ().green () < 128) {
              mp_markers.back ()->set_color (original->eff_fill_color_brighter (true, (m_marker_intensity * 255) / 100));
              mp_markers.back ()->set_frame_color (original->eff_frame_color_brighter (true, (m_marker_intensity * 255) / 100));
            } else {
              mp_markers.back ()->set_color (original->eff_fill_color_brighter (true, (-m_marker_intensity * 255) / 100));
              mp_markers.back ()->set_frame_color (original->eff_frame_color_brighter (true, (-m_marker_intensity * 255) / 100));
            }
          }

          if (net_color.isValid ()) {
            mp_markers.back ()->set_color (net_color);
            mp_markers.back ()->set_frame_color (net_color);
          } else if (m_marker_color.isValid ()) {
            mp_markers.back ()->set_color (m_marker_color);
            mp_markers.back ()->set_frame_color (m_marker_color);
          }

          if (m_marker_line_width >= 0) {
            mp_markers.back ()->set_line_width (m_marker_line_width);
          }

          if (m_marker_vertex_size >= 0) {
            mp_markers.back ()->set_vertex_size (m_marker_vertex_size);
          }

          if (m_marker_halo >= 0) {
            mp_markers.back ()->set_halo (m_marker_halo);
          }

          if (m_marker_dither_pattern >= 0) {
            mp_markers.back ()->set_dither_pattern (m_marker_dither_pattern);
          }

          ++n_marker;

        }

      }

    }

  }

}

void
NetTracerDialog::clear_markers ()
{
  for (std::vector <lay::ShapeMarker *>::iterator m = mp_markers.begin (); m != mp_markers.end (); ++m) {
    delete *m;
  }

  mp_markers.clear ();
}

// -----------------------------------------------------------------------------------
//  NetTracerPlugin definition and implementation

class NetTracerPluginDeclaration
  : public lay::PluginDeclaration,
    public lay::TechnologyComponentProvider
{
public:
  virtual void get_options (std::vector < std::pair<std::string, std::string> > &options) const 
  {
    options.push_back (std::pair<std::string, std::string> (cfg_nt_window_mode, "fit-net"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_window_dim, "1.0"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_max_shapes_highlighted, "10000"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_marker_color, lay::ColorConverter ().to_string (QColor ())));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_marker_cycle_colors_enabled, "false"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_marker_cycle_colors, "255,0,0 0,255,0 0,0,255 255,255,0 255,0,255 0,255,255 160,80,255 255,160,0"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_marker_line_width, "-1"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_marker_vertex_size, "-1"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_marker_halo, "-1"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_marker_dither_pattern, "-1"));
    options.push_back (std::pair<std::string, std::string> (cfg_nt_marker_intensity, "50"));
  }

  virtual std::vector<std::pair <std::string, lay::ConfigPage *> > config_pages (QWidget *parent) const 
  {
    std::vector<std::pair <std::string, lay::ConfigPage *> > pages;
    pages.push_back (std::make_pair (tl::to_string (QObject::tr ("Other Tools|Net Tracer")), new NetTracerConfigPage (parent)));
    return pages;
  }

  virtual void get_menu_entries (std::vector<lay::MenuEntry> &menu_entries) const
  {
    //  TODO: where should that go?
    lay::PluginDeclaration::get_menu_entries (menu_entries);
    menu_entries.push_back (lay::MenuEntry ("net_trace_group", "tools_menu.end"));
    menu_entries.push_back (lay::MenuEntry ("ext::net_trace", "net_trace", "tools_menu.end", tl::to_string (QObject::tr ("Trace Net"))));
  }
 
  virtual lay::Plugin *create_plugin (db::Manager *manager, lay::MainWindow *main, lay::LayoutView *view) const
  {
    return new NetTracerDialog (main, view);
  }

  virtual const lay::TechnologyComponentProvider *technology_component_provider () const 
  {
    return this;
  }

  virtual lay::TechnologyComponent *create_component () const
  {
    return new NetTracerTechnologyComponent ();
  }

  virtual tl::XMLElementBase *xml_element () const 
  {
    return new lay::TechnologyComponentXMLElement<NetTracerTechnologyComponent> (net_tracer_component_name, 
      tl::make_member ((NetTracerTechnologyComponent::const_iterator (NetTracerTechnologyComponent::*) () const) &NetTracerTechnologyComponent::begin, (NetTracerTechnologyComponent::const_iterator (NetTracerTechnologyComponent::*) () const) &NetTracerTechnologyComponent::end, &NetTracerTechnologyComponent::add, "connection") +
      tl::make_member ((NetTracerTechnologyComponent::const_symbol_iterator (NetTracerTechnologyComponent::*) () const) &NetTracerTechnologyComponent::begin_symbols, (NetTracerTechnologyComponent::const_symbol_iterator (NetTracerTechnologyComponent::*) () const) &NetTracerTechnologyComponent::end_symbols, &NetTracerTechnologyComponent::add_symbol, "symbols")
    );
  }
};

static tl::RegisteredClass<lay::PluginDeclaration> config_decl (new NetTracerPluginDeclaration (), 13000, "NetTracerPlugin");

}

