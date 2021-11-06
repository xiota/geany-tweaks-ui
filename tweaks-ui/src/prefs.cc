/*
 * Tweaks-UI Plugin for Geany
 * Copyright 2021 xiota
 *
 * This program is free software; you can redistgribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "auxiliary.h"
#include "prefs.h"

// Global Variables
TweakSettings settings;

// Functions

void TweakSettings::open() {
  std::string conf_fn =
      cstr_assign(g_build_filename(geany_data->app->configdir, "plugins",
                                   "tweaks", "tweaks-ui.conf", nullptr));
  std::string conf_dn = g_path_get_dirname(conf_fn.c_str());
  g_mkdir_with_parents(conf_dn.c_str(), 0755);

  GKeyFile *kf = g_key_file_new();

  // if file does not exist, create it
  if (!g_file_test(conf_fn.c_str(), G_FILE_TEST_EXISTS)) {
    save_default();
  }

  g_key_file_load_from_file(
      kf, conf_fn.c_str(),
      GKeyFileFlags(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS),
      nullptr);

  load(kf);

  GKEY_FILE_FREE(kf);
}

void TweakSettings::save_default() {
  std::string conf_fn =
      cstr_assign(g_build_filename(geany_data->app->configdir, "plugins",
                                   "tweaks", "tweaks-ui.conf", nullptr));
  std::string conf_dn = cstr_assign(g_path_get_dirname(conf_fn.c_str()));
  g_mkdir_with_parents(conf_dn.c_str(), 0755);

  // delete if file exists
  GFile *file = g_file_new_for_path(conf_fn.c_str());
  if (!g_file_trash(file, nullptr, nullptr)) {
    g_file_delete(file, nullptr, nullptr);
  }

  // copy default config
  std::string contents = file_get_contents(TWEAKS_CONFIG);
  if (!contents.empty()) {
    file_set_contents(conf_fn, contents);
  }

  msgwin_status_add(TWEAKS_CONFIG);

  g_object_unref(file);
}

void TweakSettings::save() {
  GKeyFile *kf = g_key_file_new();
  std::string fn =
      cstr_assign(g_build_filename(geany_data->app->configdir, "plugins",
                                   "tweaks", "tweaks-ui.conf", nullptr));

  // Load old contents in case user changed file outside of GUI
  g_key_file_load_from_file(
      kf, fn.c_str(),
      GKeyFileFlags(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS),
      nullptr);

  SET_KEY(boolean, "sidebar_save_size_enabled", sidebar_save_size_enabled);
  SET_KEY(boolean, "sidebar_save_size_update", sidebar_save_size_update);
  SET_KEY(integer, "sidebar_save_size_normal", sidebar_save_size_normal);
  SET_KEY(integer, "sidebar_save_size_maximized", sidebar_save_size_maximized);

  SET_KEY(boolean, "sidebar_auto_size_enabled", sidebar_auto_size_enabled);
  SET_KEY(integer, "sidebar_auto_size_normal", sidebar_auto_size_normal);
  SET_KEY(integer, "sidebar_auto_size_maximized", sidebar_auto_size_maximized);

  SET_KEY(boolean, "menubar_hide_on_start", menubar_hide_on_start);
  SET_KEY(boolean, "menubar_restore_state", menubar_restore_state);

  GtkWidget *geany_menubar =
      ui_lookup_widget(GTK_WIDGET(geany->main_widgets->window), "hbox_menubar");
  menubar_previous_state = gtk_widget_is_visible(geany_menubar);
  SET_KEY(boolean, "menubar_previous_state", menubar_previous_state);

  SET_KEY(boolean, "column_marker_enable", column_markers.enable);

  {
    std::string str_columns;
    std::string str_colors;
    column_markers.get_columns(str_columns, str_colors);
    SET_KEY_STRING("column_marker_columns", str_columns);
    SET_KEY_STRING("column_marker_colors", str_colors);
  }

  // Store back on disk
  std::string contents = cstr_assign(g_key_file_to_data(kf, nullptr, nullptr));
  if (!contents.empty()) {
    file_set_contents(fn, contents);
  }

  GKEY_FILE_FREE(kf);
}

void TweakSettings::load(GKeyFile *kf) {
  if (!g_key_file_has_group(kf, "tweaks")) {
    return;
  }

  GET_KEY_BOOLEAN(sidebar_save_size_enabled, true);
  GET_KEY_BOOLEAN(sidebar_save_size_update, true);
  GET_KEY_INTEGER(sidebar_save_size_normal, 0, 0);
  GET_KEY_INTEGER(sidebar_save_size_maximized, 0, 0);

  GET_KEY_BOOLEAN(sidebar_auto_size_enabled, false);
  GET_KEY_INTEGER(sidebar_auto_size_normal, 76, 0);
  GET_KEY_INTEGER(sidebar_auto_size_maximized, 100, 0);

  GET_KEY_BOOLEAN(menubar_hide_on_start, false);
  GET_KEY_BOOLEAN(menubar_restore_state, false);
  GET_KEY_BOOLEAN(menubar_previous_state, true);

  GET_KEY_BOOLEAN(column_markers.enable, true);

  {
    std::string str_columns;
    std::string str_colors;

    GET_KEY_STRING(str_columns, "column_marker_columns", "60;72;80;88;96;");
    GET_KEY_STRING(str_colors, "column_marker_colors",
                   "#e5e5e5;#b0d0ff;#ffc0ff;#e5e5e5;#ffb0a0;");

    column_markers.set_columns(str_columns, str_colors);
  }
}
