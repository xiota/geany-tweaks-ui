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

#pragma once

#include "tkui_column_markers.h"
#include "tkui_hide_menubar.h"
#include "tkui_main.h"

#define TKUI_KF_GROUP "tweaks"

class TweakUISettings {
 public:
  ~TweakUISettings();

  void open();
  void close();
  void load();
  void save();
  void save_session();
  void reset();

  std::string get_config_file() const { return config_file; }
  std::string get_session_file() const { return session_file; }

 public:
  gboolean sidebar_save_size_enabled = true;
  gboolean sidebar_save_size_update = true;
  int sidebar_save_size_normal = 0;
  int sidebar_save_size_maximized = 0;

  gboolean sidebar_auto_size_enabled = false;
  int sidebar_auto_size_normal = 76;
  int sidebar_auto_size_maximized = 100;

  bool markword_enable = false;
  bool markword_deselect_single_click = false;
  bool colortip_enable = false;
  bool colortip_chooser_double_click = false;

  tkuiColumnMarkers column_markers;
  tkuiHideMenubar hide_menubar;

 private:
  bool kf_has_key(std::string const &key);

  void kf_set_boolean(std::string const &key, bool const &val);
  bool kf_get_boolean(std::string const &key, bool const &def);

  void kf_set_integer(std::string const &key, int const &val);
  int kf_get_integer(std::string const &key, int const &def, int const &min);

  void kf_set_string(std::string const &key, std::string const &val);
  std::string kf_get_string(std::string const &key, std::string const &def);

 private:
  GKeyFile *keyfile = nullptr;
  GKeyFile *session = nullptr;
  std::string config_file;
  std::string session_file;
};

// Macros to make loading settings easier

#define GET_KEY(T, key) \
  g_key_file_get_##T(keyfile, TKUI_KF_GROUP, (key), nullptr)

#define SET_KEY(T, key, _val) \
  g_key_file_set_##T(keyfile, TKUI_KF_GROUP, (key), (_val))
