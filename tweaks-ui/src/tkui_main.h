/*
 * Tweaks-UI Plugin for Geany
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

#pragma once

#include <geanyplugin.h>

extern GeanyPlugin *geany_plugin;
extern GeanyData *geany_data;

extern GtkWidget *find_focus_widget(GtkWidget *widget);
extern GeanyKeyGroup *keybindings_get_core_group(guint id);

enum TweakShortcuts {
  TWEAKS_KEY_SWITCH_FOCUS_EDITOR_SIDEBAR_MSGWIN,
  TWEAKS_KEY_TOGGLE_VISIBILITY_MENUBAR,
  TWEAKS_KEY_COPY,
  TWEAKS_KEY_PASTE_1,
  TWEAKS_KEY_PASTE_2,

  TWEAKS_KEY_COUNT,
};

// Pane Position Callbacks
// void pane_position_update(gboolean enable);
// gboolean tkui_draw_pane(GtkWidget *self, cairo_t *cr, gpointer user_data);

// Preferences Callbacks
gboolean reload_config(gpointer user_data);
void tkui_pref_reload_config(GtkWidget *self = nullptr,
                           GtkWidget *dialog = nullptr);
void tkui_pref_save_config(GtkWidget *self, GtkWidget *dialog);
void tkui_pref_reset_config(GtkWidget *self, GtkWidget *dialog);
void tkui_pref_open_config_folder(GtkWidget *self, GtkWidget *dialog);
void tkui_pref_edit_config(GtkWidget *self, GtkWidget *dialog);
void tkui_menu_preferences(GtkWidget *self, GtkWidget *dialog);

// Keybinding Functions and Callbacks
void tkui_switch_focus_editor_sidebar_msgwin();
bool hide_menubar();
void tkui_toggle_visibility_menubar();
bool tkui_key_binding(int key_id);

// Other functions
gboolean show_column_markers(gpointer user_data = nullptr);

#define GEANY_PSC(sig, cb)                                                  \
  plugin_signal_connect(geany_plugin, nullptr, (sig), TRUE, G_CALLBACK(cb), \
                        nullptr)

#define GFREE(_z_) \
  do {             \
    g_free(_z_);   \
    _z_ = nullptr; \
  } while (0)

#define GSTRING_FREE(_z_)     \
  do {                        \
    g_string_free(_z_, TRUE); \
    _z_ = nullptr;            \
  } while (0)

#define GERROR_FREE(_z_) \
  do {                   \
    g_error_free(_z_);   \
    _z_ = nullptr;       \
  } while (0)

#ifndef G_SOURCE_FUNC
#define G_SOURCE_FUNC(f) ((GSourceFunc)(void (*)(void))(f))
#endif  // G_SOURCE_FUNC

#ifndef g_clear_signal_handler
#include "gobject/gsignal.h"
// g_clear_signal_handler was added in glib 2.62
#define g_clear_signal_handler(handler, instance)      \
  do {                                                 \
    if (handler != nullptr && *handler != 0) {         \
      g_signal_handler_disconnect(instance, *handler); \
      *handler = 0;                                    \
    }                                                  \
  } while (0)
#endif  // g_clear_signal_handler
