/*
 * Settings - Tweaks-UI Plugin for Geany
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

#include "auxiliary.h"
#include "tkui_settings.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void TweakUiSettings::open() {
  keyfile = g_key_file_new();

  config_file =
      cstr_assign(g_build_filename(geany_data->app->configdir, "plugins",
                                   "tweaks", "tweaks-ui.conf", nullptr));
  std::string conf_dn = g_path_get_dirname(config_file.c_str());
  g_mkdir_with_parents(conf_dn.c_str(), 0755);

  // if file does not exist, create it
  if (!g_file_test(config_file.c_str(), G_FILE_TEST_EXISTS)) {
    file_set_contents(config_file, "[tweaks]");
    load();
    save();
    save();
  }
}

void TweakUiSettings::close() {
  if (keyfile) {
    g_key_file_free(keyfile);
    keyfile = nullptr;
  }
}

void TweakUiSettings::reset() {
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

  // file_set_contents(config_file, "[tweaks]");

  TweakUiSettings new_settings;
  new_settings.open();
  new_settings.close();
}

void TweakUiSettings::save(bool bSession) {
  if (!keyfile) {
    return;
  }

  bSaveInProgress = true;

  // Load old contents in case user changed file outside of GUI
  g_key_file_load_from_file(keyfile, config_file.c_str(),
                            GKeyFileFlags(G_KEY_FILE_KEEP_COMMENTS), nullptr);

  // AutoReadOnly
  if (!bSession) {
    kf_set_comment("auto_read_only", "\n " + auto_read_only.desc_enable);
    kf_set_boolean("auto_read_only", auto_read_only.enable);
  }

  // HideMenubar
  if (!bSession) {
    kf_set_comment("menubar_hide_on_start",
                   "\n " + hide_menubar.desc_hide_on_start);
    kf_set_boolean("menubar_hide_on_start", hide_menubar.hide_on_start);
    kf_set_comment("menubar_restore_state",
                   "   " + hide_menubar.desc_restore_state);
    kf_set_boolean("menubar_restore_state", hide_menubar.restore_state);
  }
  if (hide_menubar.restore_state) {
    kf_set_boolean("menubar_previous_state", hide_menubar.get_state());
  }

  // unchange document
  if (!bSession) {
    kf_set_comment("unchange_document_enable",
                   "\n " + unchange_document.desc_enable);
    kf_set_boolean("unchange_document_enable", unchange_document.enable);
  }

  // Window Geometry
  if (!bSession) {
    kf_set_comment("geometry_enable", "\n " + window_geometry.description);
    kf_set_boolean("geometry_enable", window_geometry.getEnabled());
    kf_set_comment("geometry_update",
                   "   " + window_geometry.desc_geometry_update);
    kf_set_boolean("geometry_update", window_geometry.geometry_update);
    kf_set_comment("geometry_sidebar",
                   "   " + window_geometry.desc_sidebar_enable);
    kf_set_boolean("geometry_sidebar", window_geometry.sidebar_enable);
    kf_set_comment("geometry_msgwin",
                   "   " + window_geometry.desc_msgwin_enable);
    kf_set_boolean("geometry_msgwin", window_geometry.msgwin_enable);
  }
  kf_set_integer("geometry_xpos", window_geometry.xpos);
  kf_set_integer("geometry_ypos", window_geometry.ypos);
  kf_set_integer("geometry_width", window_geometry.width);
  kf_set_integer("geometry_height", window_geometry.height);

  kf_set_integer("geometry_xpos_maximized", window_geometry.xpos_maximized);
  kf_set_integer("geometry_ypos_maximized", window_geometry.ypos_maximized);
  kf_set_integer("geometry_width_maximized", window_geometry.width_maximized);
  kf_set_integer("geometry_height_maximized", window_geometry.height_maximized);

  kf_set_integer("geometry_sidebar_maximized",
                 window_geometry.sidebar_maximized);
  kf_set_integer("geometry_sidebar_normal", window_geometry.sidebar_normal);
  kf_set_integer("geometry_msgwin_maximized", window_geometry.msgwin_maximized);
  kf_set_integer("geometry_msgwin_normal", window_geometry.msgwin_normal);

  // sidebar auto size
  if (!bSession) {
    kf_set_comment("sidebar_auto_size_enabled",
                   "\n " + sidebar_auto_position.description);
    kf_set_boolean("sidebar_auto_size_enabled",
                   sidebar_auto_position.getEnabled());
  }
  kf_set_integer("sidebar_auto_size_normal",
                 sidebar_auto_position.columns_normal);
  kf_set_integer("sidebar_auto_size_maximized",
                 sidebar_auto_position.columns_maximized);

  // ColorTip
  if (!bSession) {
    kf_set_comment("colortip_tooltip", "\n " + colortip.desc_color_tooltip);
    kf_set_boolean("colortip_tooltip", colortip.color_tooltip);

    kf_set_comment("colortip_tooltip_size",
                   "   " + colortip.desc_color_tooltip_size);
    kf_set_string("colortip_tooltip_size", colortip.color_tooltip_size);

    kf_set_comment("colortip_chooser", "   " + colortip.desc_color_chooser);
    kf_set_boolean("colortip_chooser", colortip.color_chooser);
  }

  // MarkWord
  if (!bSession) {
    kf_set_comment("markword_enable", "\n " + markword.desc_enable);
    kf_set_boolean("markword_enable", markword.enable);
    kf_set_comment("markword_single_click_deselect",
                   "   " + markword.desc_single_click_deselect);
    kf_set_boolean("markword_single_click_deselect",
                   markword.single_click_deselect);
  }

  // ColumnMarker
  if (!bSession) {
    kf_set_comment("column_marker_enable", "\n " + column_markers.desc_enable);
    kf_set_boolean("column_marker_enable", column_markers.enable);

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

void TweakUiSettings::save_session() {
  if (!keyfile) {
    return;
  }

  save(true);
}

void TweakUiSettings::load() {
  if (!keyfile) {
    return;
  }

  g_key_file_load_from_file(keyfile, config_file.c_str(),
                            GKeyFileFlags(G_KEY_FILE_KEEP_COMMENTS), nullptr);

  if (!g_key_file_has_group(keyfile, TKUI_KF_GROUP)) {
    return;
  }

  auto_read_only.enable = kf_get_boolean("auto_read_only", false);

  hide_menubar.hide_on_start = kf_get_boolean("menubar_hide_on_start", false);
  hide_menubar.restore_state = kf_get_boolean("menubar_restore_state", false);
  hide_menubar.previous_state = kf_get_boolean("menubar_previous_state", true);

  unchange_document.enable = kf_get_boolean("unchange_document_enable", false);

  {  // Window Geometry
    bool enabled = kf_get_boolean("geometry_enable", false);
    window_geometry.setEnabled(enabled);

    window_geometry.geometry_update = kf_get_boolean("geometry_update", false);
    window_geometry.sidebar_enable = kf_get_boolean("geometry_sidebar", false);
    window_geometry.msgwin_enable = kf_get_boolean("geometry_msgwin", false);

    window_geometry.xpos = kf_get_integer("geometry_xpos", -1, INT_MIN);
    window_geometry.ypos = kf_get_integer("geometry_ypos", -1, INT_MIN);
    window_geometry.width = kf_get_integer("geometry_width", -1, 0);
    window_geometry.height = kf_get_integer("geometry_height", -1, 0);

    window_geometry.xpos_maximized = kf_get_integer("geometry_xpos_maximized", -1, INT_MIN);
    window_geometry.ypos_maximized = kf_get_integer("geometry_ypos_maximized", -1, INT_MIN);
    window_geometry.width_maximized = kf_get_integer("geometry_width_maximized", -1, 0);
    window_geometry.height_maximized = kf_get_integer("geometry_height_maximized", -1, 0);

    window_geometry.sidebar_maximized =
        kf_get_integer("geometry_sidebar_maximized", -1, 0);
    window_geometry.sidebar_normal =
        kf_get_integer("geometry_sidebar_normal", -1, 0);
    window_geometry.msgwin_maximized =
        kf_get_integer("geometry_msgwin_maximized", -1, 0);
    window_geometry.msgwin_normal =
        kf_get_integer("geometry_msgwin_normal", -1, 0);
  }

  {  // sidebar auto size
    bool enabled = kf_get_boolean("sidebar_auto_size_enabled", false);
    sidebar_auto_position.setEnabled(enabled);

    sidebar_auto_position.columns_normal =
        kf_get_integer("sidebar_auto_size_normal", 76, 0);
    sidebar_auto_position.columns_maximized =
        kf_get_integer("sidebar_auto_size_maximized", 100, 0);
  }

  // prevent auto size and window geometry conflict
  if (sidebar_auto_position.getEnabled()) {
    window_geometry.sidebar_enable = false;
    window_geometry.sidebar_maximized = -1;
    window_geometry.sidebar_normal = -1;
  }

  colortip.color_tooltip = kf_get_boolean("colortip_tooltip", false);
  colortip.setSize(kf_get_string("colortip_tooltip_size", "small"));
  colortip.color_chooser = kf_get_boolean("colortip_chooser", false);

  markword.enable = kf_get_boolean("markword_enable", false);
  markword.single_click_deselect =
      kf_get_boolean("markword_single_click_deselect", true);

  {
    column_markers.enable = kf_get_boolean("column_marker_enable", false);

    std::string str_columns;
    std::string str_colors;

    str_columns =
        kf_get_string("column_marker_columns",
                      "60;72;80;88;96;104;112;120;128;136;144;152;160;");
    str_colors =
        kf_get_string("column_marker_colors",
                      "#e5e5e5;#b0d0ff;#ffc0ff;#e5e5e5;#ffb0a0;#e5e5e5;#e5e5e5;"
                      "#e5e5e5;#e5e5e5;#e5e5e5;#e5e5e5;#e5e5e5;#e5e5e5;");

    column_markers.set_columns(str_columns, str_colors);
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Keyfile operations
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool TweakUiSettings::kf_has_key(std::string const &key) {
  return g_key_file_has_key(keyfile, TKUI_KF_GROUP, key.c_str(), nullptr);
}

void TweakUiSettings::kf_set_boolean(std::string const &key, bool const &val) {
  g_key_file_set_boolean(keyfile, TKUI_KF_GROUP, key.c_str(), val);
}

bool TweakUiSettings::kf_get_boolean(std::string const &key, bool const &def) {
  if (kf_has_key(key)) {
    return g_key_file_get_boolean(keyfile, TKUI_KF_GROUP, key.c_str(), nullptr);
  } else {
    return def;
  }
}

void TweakUiSettings::kf_set_integer(std::string const &key, int const &val) {
  g_key_file_set_integer(keyfile, TKUI_KF_GROUP, key.c_str(), val);
}

int TweakUiSettings::kf_get_integer(std::string const &key, int const &def,
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

void TweakUiSettings::kf_set_string(std::string const &key,
                                    std::string const &val) {
  g_key_file_set_string(keyfile, TKUI_KF_GROUP, key.c_str(), val.c_str());
}

std::string TweakUiSettings::kf_get_string(std::string const &key,
                                           std::string const &def) {
  if (kf_has_key(key)) {
    char *val =
        g_key_file_get_string(keyfile, TKUI_KF_GROUP, key.c_str(), nullptr);
    return cstr_assign(val);
  } else {
    return def;
  }
}

void TweakUiSettings::kf_set_comment(std::string const &key,
                                     std::string const &val) {
  if (key.empty()) {
    g_key_file_set_comment(keyfile, TKUI_KF_GROUP, nullptr, val.c_str(),
                           nullptr);
  } else {
    g_key_file_set_comment(keyfile, TKUI_KF_GROUP, key.c_str(),
                           (" " + val).c_str(), nullptr);
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
