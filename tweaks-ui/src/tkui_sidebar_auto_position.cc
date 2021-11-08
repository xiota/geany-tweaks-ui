/*
 * Sidebar Auto Position - Tweaks-UI Plugin for Geany
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

#include "tkui_sidebar_auto_position.h"

void TweakUiSidebarAutoPosition::initialize(GtkWidget *_geany_window) {
  geany_window = _geany_window;
  if (geany_window) {
    geany_hpane = ui_lookup_widget(GTK_WIDGET(geany_window), "hpaned1");
    update_connection();
  }

  update_position();
}

void TweakUiSidebarAutoPosition::update_position() {
  GeanyDocument *doc = document_get_current();
  if (DOC_VALID(doc)) {
    std::string str_auto_maximized(columns_maximized, '0');
    std::string str_auto_normal(columns_normal, '0');

    int pos_origin = (int)scintilla_send_message(doc->editor->sci,
                                                 SCI_POINTXFROMPOSITION, 0, 1);
    int pos_maximized = (int)scintilla_send_message(
        doc->editor->sci, SCI_TEXTWIDTH, 0, (gulong)str_auto_maximized.c_str());
    int pos_normal = (int)scintilla_send_message(
        doc->editor->sci, SCI_TEXTWIDTH, 0, (gulong)str_auto_normal.c_str());

    position_maximized = pos_origin + pos_maximized;
    position_normal = pos_origin + pos_normal;
  }
}

bool TweakUiSidebarAutoPosition::getEnabled() const { return enabled; }

void TweakUiSidebarAutoPosition::setEnabled(bool const val) {
  enabled = val;
  update_connection();
}

void TweakUiSidebarAutoPosition::update_connection() {
  if (enabled && !ulHandlePanePosition) {
    ulHandlePanePosition = g_signal_connect(GTK_WIDGET(geany_hpane), "draw",
                                            G_CALLBACK(hpane_callback), this);
  }

  if (!enabled && ulHandlePanePosition) {
    g_clear_signal_handler(&ulHandlePanePosition, GTK_WIDGET(geany_hpane));
  }
}

gboolean TweakUiSidebarAutoPosition::hpane_callback(GtkWidget *hpane,
                                                    cairo_t *cr,
                                                    gpointer user_data) {
  TweakUiSidebarAutoPosition *self = (TweakUiSidebarAutoPosition *)user_data;
  if (!self->enabled) {
    self->update_connection();
    return false;
  }

  self->update_hpane();
  return false;
}

void TweakUiSidebarAutoPosition::update_hpane() {
  if (!geany_hpane || !geany_window) {
    enabled = false;
    update_connection();
    return;
  }

  if (position_maximized < 0 || position_normal < 0) {
    update_position();
  }

  GdkWindowState wstate =
      gdk_window_get_state(gtk_widget_get_window(geany_window));

  static bool window_maximized_previous = false;
  bool window_maximized_current =
      wstate & (GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN);

  if (window_maximized_current != window_maximized_previous) {
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
