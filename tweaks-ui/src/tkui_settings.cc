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
#include "tkui_settings.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TweakUISettings::~TweakUISettings() { close(); }

void TweakUISettings::open() {
  keyfile = g_key_file_new();

  config_file =
      cstr_assign(g_build_filename(geany_data->app->configdir, "plugins",
                                   "tweaks", "tweaks-ui.conf", nullptr));
  std::string conf_dn = g_path_get_dirname(config_file.c_str());
  g_mkdir_with_parents(conf_dn.c_str(), 0755);

  // if file does not exist, create it
  if (!g_file_test(config_file.c_str(), G_FILE_TEST_EXISTS)) {
    reset();
  }
}

void TweakUISettings::close() {
  if (keyfile) {
    g_key_file_free(keyfile);
    keyfile = nullptr;
  }
}

void TweakUISettings::reset() {
  if (config_file.empty()) {
    open();
  }

  {  // delete if file exists
    GFile *file = g_file_new_for_path(config_file.c_str());
    if (!g_file_trash(file, nullptr, nullptr)) {
      g_file_delete(file, nullptr, nullptr);
    }
    g_object_unref(file);
  }

  // copy default config
  std::string contents = file_get_contents(TWEAKS_CONFIG);
  if (!contents.empty()) {
    file_set_contents(config_file, contents);
  }
}

void TweakUISettings::save() {
  if (!keyfile) {
    return;
  }

  // Load old contents in case user changed file outside of GUI
  g_key_file_load_from_file(
      keyfile, config_file.c_str(),
      GKeyFileFlags(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS),
      nullptr);

  kf_set_boolean("sidebar_save_size_enabled", sidebar_save_size_enabled);
  kf_set_boolean("sidebar_save_size_update", sidebar_save_size_update);
  kf_set_integer("sidebar_save_size_normal", sidebar_save_size_normal);
  kf_set_integer("sidebar_save_size_maximized", sidebar_save_size_maximized);

  kf_set_boolean("sidebar_auto_size_enabled", sidebar_auto_size_enabled);
  kf_set_integer("sidebar_auto_size_normal", sidebar_auto_size_normal);
  kf_set_integer("sidebar_auto_size_maximized", sidebar_auto_size_maximized);

  kf_set_boolean("menubar_hide_on_start", menubar_hide_on_start);
  kf_set_boolean("menubar_restore_state", menubar_restore_state);

  GtkWidget *geany_menubar =
      ui_lookup_widget(GTK_WIDGET(geany->main_widgets->window), "hbox_menubar");
  menubar_previous_state = gtk_widget_is_visible(geany_menubar);
  kf_set_boolean("menubar_previous_state", menubar_previous_state);

  kf_set_boolean("column_marker_enable", column_markers.enable);

  {
    std::string str_columns;
    std::string str_colors;
    column_markers.get_columns(str_columns, str_colors);
    kf_set_string("column_marker_columns", str_columns);
    kf_set_string("column_marker_colors", str_colors);
  }

  // Store back on disk
  std::string contents =
      cstr_assign(g_key_file_to_data(keyfile, nullptr, nullptr));
  if (!contents.empty()) {
    file_set_contents(config_file, contents);
  }
}

void TweakUISettings::load() {
  if (!keyfile || !g_key_file_has_group(keyfile, TKUI_KF_GROUP)) {
    return;
  }

  g_key_file_load_from_file(
      keyfile, config_file.c_str(),
      GKeyFileFlags(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS),
      nullptr);

  sidebar_save_size_enabled = kf_get_boolean("sidebar_save_size_enabled", true);
  sidebar_save_size_update = kf_get_boolean("sidebar_save_size_update", true);
  sidebar_save_size_normal = kf_get_integer("sidebar_save_size_normal", 0, 0);
  sidebar_save_size_maximized =
      kf_get_integer("sidebar_save_size_maximized", 0, 0);

  sidebar_auto_size_enabled =
      kf_get_boolean("sidebar_auto_size_enabled", false);
  sidebar_auto_size_normal = kf_get_integer("sidebar_auto_size_normal", 76, 0);
  sidebar_auto_size_maximized =
      kf_get_integer("sidebar_auto_size_maximized", 100, 0);

  menubar_hide_on_start = kf_get_boolean("menubar_hide_on_start", false);
  menubar_restore_state = kf_get_boolean("menubar_restore_state", false);
  menubar_previous_state = kf_get_boolean("menubar_previous_state", true);

  column_markers.enable = kf_get_boolean("column_markers.enable", true);

  {
    std::string str_columns;
    std::string str_colors;

    str_columns = kf_get_string("column_marker_columns", "60;72;80;88;96;");
    str_colors = kf_get_string("column_marker_colors",
                               "#e5e5e5;#b0d0ff;#ffc0ff;#e5e5e5;#ffb0a0;");

    column_markers.set_columns(str_columns, str_colors);
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Keyfile operations
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool TweakUISettings::kf_has_key(std::string const &key) {
  return g_key_file_has_key(keyfile, TKUI_KF_GROUP, key.c_str(), nullptr);
}

void TweakUISettings::kf_set_boolean(std::string const &key, bool const &val) {
  g_key_file_set_boolean(keyfile, TKUI_KF_GROUP, key.c_str(), val);
}

bool TweakUISettings::kf_get_boolean(std::string const &key, bool const &def) {
  if (kf_has_key(key)) {
    return g_key_file_get_boolean(keyfile, TKUI_KF_GROUP, key.c_str(), nullptr);
  } else {
    return def;
  }
}

void TweakUISettings::kf_set_integer(std::string const &key, int const &val) {
  g_key_file_set_integer(keyfile, TKUI_KF_GROUP, key.c_str(), val);
}

int TweakUISettings::kf_get_integer(std::string const &key, int const &def,
                                    int const &min) {
  if (kf_has_key(key)) {
    int val =
        g_key_file_get_integer(keyfile, TKUI_KF_GROUP, key.c_str(), nullptr);

    if (val < min) {
      return min;
    } else {
      return val;
    }
  } else {
    return def;
  }
}

void TweakUISettings::kf_set_string(std::string const &key,
                                    std::string const &val) {
  g_key_file_set_string(keyfile, TKUI_KF_GROUP, key.c_str(), val.c_str());
}

std::string TweakUISettings::kf_get_string(std::string const &key,
                                           std::string const &def) {
  if (kf_has_key(key)) {
    char *val =
        g_key_file_get_string(keyfile, TKUI_KF_GROUP, key.c_str(), nullptr);
    return cstr_assign(val);
  } else {
    return def;
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
