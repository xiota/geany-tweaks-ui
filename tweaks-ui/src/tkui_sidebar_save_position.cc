/*
 * Sidebar Save Position - Tweaks-UI Plugin for Geany
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

#include "tkui_sidebar_save_position.h"

void TweakUiSidebarSavePosition::initialize(GtkWidget *_geany_window) {
  geany_window = _geany_window;
  if (geany_window) {
    geany_hpane = ui_lookup_widget(GTK_WIDGET(geany_window), "hpaned1");
    update_connection();
  }
};

bool TweakUiSidebarSavePosition::getEnabled() const { return enabled; }

void TweakUiSidebarSavePosition::setEnabled(bool const val) {
  enabled = val;
  update_connection();
}

void TweakUiSidebarSavePosition::update_connection() {
  if (enabled && !ulHandlePanePosition) {
    ulHandlePanePosition = g_signal_connect(GTK_WIDGET(geany_hpane), "draw",
                                            G_CALLBACK(hpane_callback), this);
  }

  if (!enabled && ulHandlePanePosition) {
    g_clear_signal_handler(&ulHandlePanePosition, GTK_WIDGET(geany_hpane));
  }
}

gboolean TweakUiSidebarSavePosition::hpane_callback(GtkWidget *hpane,
                                                    cairo_t *cr,
                                                    gpointer user_data) {
  TweakUiSidebarSavePosition *self = (TweakUiSidebarSavePosition *)user_data;
  if (!self->enabled) {
    self->update_connection();
    return false;
  }

  self->update();
  return false;
}

void TweakUiSidebarSavePosition::update() {
  if (!geany_hpane || !geany_window) {
    enabled = false;
    update_connection();
    return;
  }

  GdkWindowState wstate =
      gdk_window_get_state(gtk_widget_get_window(geany_window));

  static bool window_maximized_previous = false;
  bool window_maximized_current =
      wstate & (GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN);

  if (window_maximized_current == window_maximized_previous) {
    // save current sidebar divider position
    if (position_update) {
      if (window_maximized_current) {
        position_maximized = gtk_paned_get_position(GTK_PANED(geany_hpane));
      } else {
        position_normal = gtk_paned_get_position(GTK_PANED(geany_hpane));
      }
    }
  } else {
    if (window_maximized_current) {
      if (position_maximized >= 0) {
        gtk_paned_set_position(GTK_PANED(geany_hpane), position_maximized);
      }
    } else {
      if (position_normal >= 0) {
        gtk_paned_set_position(GTK_PANED(geany_hpane), position_normal);
      }
    }
    window_maximized_previous = window_maximized_current;
  }
}
