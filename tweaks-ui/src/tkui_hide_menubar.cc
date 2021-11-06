/*
 * Hide Menubar - Tweaks-UI Plugin for Geany
 * Copyright 2021 xiota
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <string>

#include "auxiliary.h"
#include "tkui_hide_menubar.h"

bool tkuiHideMenubar::hide() {
  if (gtk_widget_is_visible(geany_menubar)) {
    if (keybinding && keybinding->key != 0) {
      gtk_widget_hide(geany_menubar);
      std::string val =
          cstr_assign(gtk_accelerator_name(keybinding->key, keybinding->mods));
      msgwin_status_add(_("Menubar has been hidden.  To reshow it, use: %s"),
                        val.c_str());
      return true;
    } else {
      msgwin_status_add(
          _("Menubar will not be hidden until after a keybinding "
            "to reshow it has been set."));
      return false;
    }
  }
  return false;
}

void tkuiHideMenubar::show() { gtk_widget_show(geany_menubar); }

void tkuiHideMenubar::startup() {
  if (hide_on_start) {
    hide();
  } else if (restore_state && !previous_state) {
    hide();
  } else {
    show();
  }
}

void tkuiHideMenubar::toggle() {
  if (!hide()) {
    show();
  }
}

bool tkuiHideMenubar::get_state() {
  return gtk_widget_is_visible(geany_menubar);
}

gboolean tkuiHideMenubar::toggle_idle_callback(gpointer user_data) {
  tkuiHideMenubar *self = (tkuiHideMenubar *)user_data;

  self->toggle();
  self->bToggleIdleInProgress = false;
  return false;
}

void tkuiHideMenubar::toggle_idle() {
  if (!bToggleIdleInProgress) {
    bToggleIdleInProgress = true;
    g_idle_add(toggle_idle_callback, this);
  }
}

void tkuiHideMenubar::set_menubar_widget(GtkWidget *widget) {
  geany_menubar = widget;
}

void tkuiHideMenubar::set_keybinding(GeanyKeyGroup *group, gsize key_id) {
  keybinding = keybindings_get_item(group, key_id);
}
