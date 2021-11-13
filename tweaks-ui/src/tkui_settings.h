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

#include "tkui_auto_read_only.h"
#include "tkui_colortip.h"
#include "tkui_column_markers.h"
#include "tkui_detect_filetype.h"
#include "tkui_hide_menubar.h"
#include "tkui_main.h"
#include "tkui_markword.h"
#include "tkui_sidebar_auto_position.h"
#include "tkui_unchange_document.h"
#include "tkui_window_geometry.h"

#define TKUI_KF_GROUP "tweaks"

class TweakUiSettings {
 public:
  void open();
  void close();
  void load();
  void save(bool bSession = false);
  void save_session();
  void reset();

  std::string get_config_file() const { return config_file; }
  std::string get_session_file() const { return session_file; }

 public:
  TweakUiAutoReadOnly auto_read_only;
  TweakUiColumnMarkers column_markers;
  TweakUiHideMenubar hide_menubar;
  TweakUiSidebarAutoPosition sidebar_auto_position;
  TweakUiUnchangeDocument unchange_document;
  TweakUiWindowGeometry window_geometry;

  TweakUiDetectFileType detect_filetype_on_reload;

  TweakUiColorTip colortip;
  TweakUiMarkWord markword;

 private:
  bool kf_has_key(std::string const &key);

  void kf_set_boolean(std::string const &key, bool const &val);
  bool kf_get_boolean(std::string const &key, bool const &def);

  void kf_set_double(std::string const &key, double const &val);
  double kf_get_double(std::string const &key, double const &def,
                       double const &min);

  void kf_set_integer(std::string const &key, int const &val);
  int kf_get_integer(std::string const &key, int const &def, int const &min);

  void kf_set_string(std::string const &key, std::string const &val);
  std::string kf_get_string(std::string const &key, std::string const &def);

  void kf_set_comment(std::string const &key, std::string const &val);

 private:
  bool bSaveInProgress = false;
  GKeyFile *keyfile = nullptr;
  std::string config_file;
  std::string session_file;
};

// Macros to make loading settings easier

#define GET_KEY(T, key) \
  g_key_file_get_##T(keyfile, TKUI_KF_GROUP, (key), nullptr)

#define SET_KEY(T, key, _val) \
  g_key_file_set_##T(keyfile, TKUI_KF_GROUP, (key), (_val))
