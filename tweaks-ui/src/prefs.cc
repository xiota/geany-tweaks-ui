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
    settings.save_default();
  }

  g_key_file_load_from_file(
      kf, conf_fn.c_str(),
      GKeyFileFlags(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS),
      nullptr);

  settings.load(kf);

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
  settings.menubar_previous_state = gtk_widget_is_visible(geany_menubar);
  SET_KEY(boolean, "menubar_previous_state", menubar_previous_state);

  SET_KEY(boolean, "column_marker_enable", column_marker_enable);

  {
    std::vector<std::string> vec_str_columns;
    for (auto x : column_marker_columns) {
      vec_str_columns.push_back(std::to_string(x));
    }

    std::vector<std::string> vec_str_colors;
    for (auto x : column_marker_colors) {
      gchar cstr[16];
      std::string strtmp;
      strtmp = cstr_assign(g_strdup_printf("%x", x));
      cstr[0] = '#';
      cstr[1] = strtmp[4];
      cstr[2] = strtmp[5];
      cstr[3] = strtmp[2];
      cstr[4] = strtmp[3];
      cstr[5] = strtmp[0];
      cstr[6] = strtmp[1];
      cstr[7] = '\0';
      vec_str_colors.push_back(cstr);
    }

    std::string str_columns;
    for (auto x : vec_str_columns) {
      str_columns += x + ";";
    }

    std::string str_colors;
    for (auto x : vec_str_colors) {
      str_colors += x + ";";
    }

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

  GET_KEY_BOOLEAN(column_marker_enable, true);

  {
    column_marker_columns.clear();
    column_marker_colors.clear();

    std::string str_columns;
    std::string str_colors;

    GET_KEY_STRING(str_columns, "column_marker_columns", "60;72;80;88;96;");
    GET_KEY_STRING(str_colors, "column_marker_colors",
                   "#e5e5e5;#b0d0ff;#ffc0ff;#e5e5e5;#ffb0a0;");

    auto vec_str_columns = split_string(str_columns, ";");
    auto vec_str_colors = split_string(str_colors, ";");

    int len_a = vec_str_columns.size();
    int len_b = vec_str_colors.size();
    int tmp_count = len_a < len_b ? len_a : len_b;
    for (int i = 0; i < tmp_count; ++i) {
      if (!vec_str_columns[i].empty() && !vec_str_colors[i].empty()) {
        char *ptr = nullptr;
        char str[16];
        gulong color_val = 0;

        if (vec_str_colors[i][0] == '0' && vec_str_colors[i][1] == 'x') {
          color_val = strtoul(vec_str_colors[i].c_str(), &ptr, 0);
        } else if (vec_str_colors[i][0] == '#' &&
                   vec_str_colors[i].length() == 7) {
          // convert from RGB to BGR format
          str[0] = '0';
          str[1] = 'x';
          str[2] = vec_str_colors[i][5];
          str[3] = vec_str_colors[i][6];
          str[4] = vec_str_colors[i][3];
          str[5] = vec_str_colors[i][4];
          str[6] = vec_str_colors[i][1];
          str[7] = vec_str_colors[i][2];
          str[8] = '\0';
          color_val = strtoul(str, &ptr, 0);
        } else if (vec_str_colors[i][0] == '#' &&
                   vec_str_colors[i].length() == 4) {
          str[0] = '0';
          str[1] = 'x';
          str[2] = vec_str_colors[i][3];
          str[3] = vec_str_colors[i][3];
          str[4] = vec_str_colors[i][2];
          str[5] = vec_str_colors[i][2];
          str[6] = vec_str_colors[i][1];
          str[7] = vec_str_colors[i][1];
          str[8] = '\0';
          color_val = strtoul(str, &ptr, 0);
        } else {
          color_val = atoi(vec_str_colors[i].c_str());
        }
        column_marker_columns.push_back(atoi(vec_str_columns[i].c_str()));
        column_marker_colors.push_back(color_val);
      }
    }
  }
}

TweakSettings::TweakSettings() {
  // Colors are in BGR order
  ADD_COLUMN_MARKER(60, 0xe5e5e5);
  ADD_COLUMN_MARKER(72, 0xffd0b0);  // blue
  ADD_COLUMN_MARKER(80, 0xffc0ff);  // purple
  ADD_COLUMN_MARKER(88, 0xe5e5e5);
  ADD_COLUMN_MARKER(96, 0xa0b0ff);  // red
  ADD_COLUMN_MARKER(104, 0xe5e5e5);
  ADD_COLUMN_MARKER(112, 0xe5e5e5);
  ADD_COLUMN_MARKER(120, 0xe5e5e5);
  ADD_COLUMN_MARKER(128, 0xe5e5e5);
  ADD_COLUMN_MARKER(136, 0xe5e5e5);
  ADD_COLUMN_MARKER(144, 0xe5e5e5);
  ADD_COLUMN_MARKER(152, 0xe5e5e5);
  ADD_COLUMN_MARKER(160, 0xe5e5e5);
}
