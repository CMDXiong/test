
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


#include "layTechnologySelector.h"
#include "layPlugin.h"
#include "layConfig.h"
#include "layMainWindow.h"
#include "layTechnology.h"
#include "layConfig.h"
#include "tlDeferredExecution.h"

namespace lay
{

std::string tech_string_from_name (const std::string &tn)
{
  if (tn.empty ()) {
    return tl::to_string (QObject::tr ("(Default)"));
  } else {
    return tn;
  }
}

const std::string cfg_initial_technology ("initial-technology");

class KLAYOUT_DLL TechnologySelector
  : public PluginDeclaration
{
public:
  TechnologySelector () 
    : PluginDeclaration (),
      m_technologies_changed_observer (this, &TechnologySelector::technologies_changed),
      m_technology_changed_observer (this, &TechnologySelector::technology_changed),
      m_current_view_observer (this, &TechnologySelector::current_view_changed),
      m_active_cellview_observer (this, &TechnologySelector::active_cellview_changed),
      m_active_cellview_technology_changed_observer (this, &TechnologySelector::active_cellview_technology_changed)
  {
    mp_mw = 0;
    m_current_technology_updated = false;
  }

  void initialize (lay::MainWindow *main_window)
  {
    //  don't initialize in the -z case (no gui)
    if (! main_window) {
      return;
    }

    mp_mw = main_window;

    update_menu ();

    main_window->add_current_view_observer (m_current_view_observer);
    lay::Technologies::instance ()->add_technology_changed_observer (m_technology_changed_observer);
    lay::Technologies::instance ()->add_technologies_changed_observer (m_technologies_changed_observer);
    current_view_changed ();
  }

  void uninitialize (lay::MainWindow *main_window)
  {
    mp_mw = 0;
    m_tech_actions.clear ();
    m_current_view_observer.detach_all ();
    m_technologies_changed_observer.detach_all ();
    m_technology_changed_observer.detach_all ();
    m_active_cellview_observer.detach_all ();
    m_active_cellview_technology_changed_observer.detach_all ();
  }

  void get_options (std::vector < std::pair<std::string, std::string> > &options) const 
  {
    options.push_back (std::pair<std::string, std::string> (cfg_initial_technology, ""));
  }

  void get_menu_entries (std::vector<lay::MenuEntry> &menu_entries) const
  {
    lay::PluginDeclaration::get_menu_entries (menu_entries);
    menu_entries.push_back (lay::MenuEntry ("technology_selector:apply_technology", "technology_selector:tech_selector_group", "@toolbar.end", tl::to_string (QObject::tr ("Technology<:techs.png>{Select technology (click to apply)}"))));
  }

private:
  tl::ObserverDelegate<TechnologySelector> m_technologies_changed_observer;
  tl::ObserverDelegate<TechnologySelector> m_technology_changed_observer;
  tl::ObserverDelegate<TechnologySelector> m_current_view_observer;
  tl::ObserverDelegate<TechnologySelector> m_active_cellview_observer;
  tl::ObserverDelegate<TechnologySelector> m_active_cellview_technology_changed_observer;
  lay::MainWindow *mp_mw;
  tl::stable_vector <lay::Action> m_tech_actions;
  std::string m_current_technology;
  std::string m_active_technology;
  bool m_current_technology_updated;

  void current_view_changed ()
  {
    if (! mp_mw) {
      return;
    }

    m_active_cellview_observer.detach_all ();
    if (mp_mw->current_view ()) {
      mp_mw->current_view ()->add_active_cellview_observer (m_active_cellview_observer);
    }
    active_cellview_changed ();
  }

  void active_cellview_changed ()
  {
    if (! mp_mw) {
      return;
    }

    if (mp_mw->current_view () && mp_mw->current_view ()->active_cellview_index () >= 0 &&  mp_mw->current_view ()->active_cellview_index () <= int (mp_mw->current_view ()->cellviews ())) {
      m_active_cellview_technology_changed_observer.detach_all ();
      mp_mw->current_view ()->active_cellview ()->add_technology_changed_observer (m_active_cellview_technology_changed_observer);
    }
    active_cellview_technology_changed ();
  }

  void active_cellview_technology_changed ()
  {
    if (! mp_mw) {
      return;
    }

    std::string active_tech;
    if (mp_mw->current_view () && mp_mw->current_view ()->active_cellview_index () >= 0 &&  mp_mw->current_view ()->active_cellview_index () <= int (mp_mw->current_view ()->cellviews ())) {
      active_tech = mp_mw->current_view ()->active_cellview ()->tech_name ();
    }

    if (m_active_technology != active_tech) {
      m_active_technology = active_tech;
      mp_mw->tech_message (tech_string_from_name (active_tech));
      //  need to do this since macros may be bound to the new technology
      mp_mw->update_menu_with_macros ();
    }

#if 0 
    //  Hint with this implementation, the current technology follows the current layout.
    //  Although that's a nice way to display the current technology, it's pretty confusing 
    mp_mw->config_set (cfg_initial_technology, active_tech);
    mp_mw->config_finalize ();
#endif
  }

  void technologies_changed ()
  {
    //  delay actual update of menu so we can compress multiple events
    update_menu ();
  }

  void technology_changed ()
  {
    //  delay actual update of menu so we can compress multiple events
    update_menu ();
  }

  bool configure (const std::string &name, const std::string &value)
  {
    if (name == cfg_initial_technology) {

      if (value != m_current_technology) {
        m_current_technology = value;
        m_current_technology_updated = true;
      }
      
    }
    return false;
  }

  void config_finalize ()
  {
    if (! mp_mw) {
      return;
    }

    if (m_current_technology_updated) {
      update_current_technology ();
      m_current_technology_updated = false;
    }
  }

  bool menu_activated (const std::string &symbol) const
  {
    if (symbol == "technology_selector:apply_technology") {
      if (mp_mw->current_view () && mp_mw->current_view ()->active_cellview ().is_valid ()) {
        mp_mw->current_view ()->active_cellview ()->apply_technology (m_current_technology);
      }
      return true;
    } else {
      return lay::PluginDeclaration::menu_activated (symbol);
    }
  }

  void update_current_technology ()
  {
    if (! mp_mw) {
      return;
    }

    std::string title = tech_string_from_name (m_current_technology);

    std::vector<std::string> menu_entries = mp_mw->menu ().group ("tech_selector_group");
    for (std::vector<std::string>::const_iterator m = menu_entries.begin (); m != menu_entries.end (); ++m) {
      lay::Action action = mp_mw->menu ().action (*m);
      action.set_title (title);
    }
    
    size_t it = 0;
    for (lay::Technologies::const_iterator t = lay::Technologies::instance ()->begin (); t != lay::Technologies::instance ()->end () && it < m_tech_actions.size (); ++t, ++it) {
      m_tech_actions[it].set_checked (t->name () == m_current_technology);
    }
  }

  void update_menu ()
  {
    if (! mp_mw) {
      return;
    }

    if (mp_mw->current_view () && mp_mw->current_view ()->active_cellview ().is_valid ()) {
      m_current_technology = mp_mw->current_view ()->active_cellview ()->tech_name ();
    }

    std::string title = tech_string_from_name (m_current_technology);

    size_t ntech = 0;
    for (lay::Technologies::const_iterator t = lay::Technologies::instance ()->begin (); t != lay::Technologies::instance ()->end (); ++t) {
      ++ntech;
    }

    std::vector<std::string> tech_group = mp_mw->menu ().group ("tech_selector_group");
    for (std::vector<std::string>::const_iterator t = tech_group.begin (); t != tech_group.end (); ++t) {
      lay::Action action = mp_mw->menu ().action (*t);
      action.set_title (title);
      action.set_visible (ntech > 1);
      std::vector<std::string> items = mp_mw->menu ().items (*t);        
      for (std::vector<std::string>::const_iterator i = items.begin (); i != items.end (); ++i) {
        mp_mw->menu ().delete_item (*i);
      }
    }
      
    m_tech_actions.clear ();

    int it = 0;
    for (lay::Technologies::const_iterator t = lay::Technologies::instance ()->begin (); t != lay::Technologies::instance ()->end (); ++t, ++it) {

      std::string title = tech_string_from_name (t->name ());

      m_tech_actions.push_back (mp_mw->create_config_action ("", cfg_initial_technology, t->name ()));
      m_tech_actions.back ().set_title (title); // setting the title here avoids interpretation of '(...)' etc.
      m_tech_actions.back ().set_checkable (true);
      m_tech_actions.back ().set_checked (t->name () == m_current_technology);
      for (std::vector<std::string>::const_iterator t = tech_group.begin (); t != tech_group.end (); ++t) {
        mp_mw->menu ().insert_item (*t + ".end", "technology_" + tl::to_string (it), m_tech_actions.back ());
      }

    }
  }
};

static tl::RegisteredClass<lay::PluginDeclaration> config_decl (new TechnologySelector (), 9000, "TechnologySelector");

}


