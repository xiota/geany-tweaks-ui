/*
 * Tweaks-UI Plugin for Geany
 * Copyright 2021 xiota
 *
 * This program is free software; you can redistribute it and/or modify
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

#ifndef XITWEAKS_PREFS_H
#define XITWEAKS_PREFS_H

#include "plugin.h"
#include "tkui_column_markers.h"

class TweakSettings {
 public:
  TweakSettings() = default;
  ~TweakSettings() { save(); }

  void open();
  void load(GKeyFile *kf);
  void save();
  void save_default();

 public:
  gboolean sidebar_save_size_enabled = true;
  gboolean sidebar_save_size_update = true;
  int sidebar_save_size_normal = 0;
  int sidebar_save_size_maximized = 0;

  gboolean sidebar_auto_size_enabled = false;
  int sidebar_auto_size_normal = 76;
  int sidebar_auto_size_maximized = 100;

  gboolean menubar_hide_on_start = false;
  gboolean menubar_restore_state = false;
  gboolean menubar_previous_state = true;

  tkuiColumnMarkers column_markers;
};

// Macros to make loading settings easier
#define PLUGIN_GROUP "tweaks"

#define HAS_KEY(key) g_key_file_has_key(kf, PLUGIN_GROUP, (key), nullptr)
#define GET_KEY(T, key) g_key_file_get_##T(kf, PLUGIN_GROUP, (key), nullptr)
#define SET_KEY(T, key, _val) \
  g_key_file_set_##T(kf, PLUGIN_GROUP, (key), (_val))

#define SET_KEY_STRING(key, _val) \
  g_key_file_set_string(kf, PLUGIN_GROUP, (key), (_val.c_str()))

#define GET_KEY_STRING(var, key, def)   \
  do {                                  \
    if (HAS_KEY(key)) {                 \
      char *val = GET_KEY(string, key); \
      if (val) {                        \
        (var) = cstr_assign(val);       \
      } else {                          \
        (var) = std::string(def);       \
      }                                 \
    }                                   \
  } while (0)

#define SET_KEY_VALUE(key, _val) \
  g_key_file_set_value(kf, PLUGIN_GROUP, (key), (_val.c_str()))

#define GET_KEY_VALUE(var, key, def)   \
  do {                                 \
    if (HAS_KEY(#key)) {               \
      char *val = GET_KEY(value, key); \
      if (val) {                       \
        (var) = cstr_assign(val);      \
      } else {                         \
        (var) = std::string(def);      \
      }                                \
    }                                  \
  } while (0)

#define GET_KEY_BOOLEAN(key, def)   \
  do {                              \
    if (HAS_KEY(#key)) {            \
      key = GET_KEY(boolean, #key); \
    } else {                        \
      key = (def);                  \
    }                               \
  } while (0)

#define GET_KEY_INTEGER(key, def, min)  \
  do {                                  \
    if (HAS_KEY(#key)) {                \
      int val = GET_KEY(integer, #key); \
      if (val) {                        \
        if (val < (min)) {              \
          key = (min);                  \
        } else {                        \
          key = val;                    \
        }                               \
      } else {                          \
        key = (def);                    \
      }                                 \
    }                                   \
  } while (0)

#define GET_KEY_DOUBLE(key, def, min)  \
  do {                                 \
    if (HAS_KEY(#key)) {               \
      int val = GET_KEY(double, #key); \
      if (val) {                       \
        if (val < (min)) {             \
          key = (min);                 \
        } else {                       \
          key = val;                   \
        }                              \
      } else {                         \
        key = (def);                   \
      }                                \
    }                                  \
  } while (0)

#define GKEY_FILE_FREE(_z_) \
  do {                      \
    g_key_file_free(_z_);   \
    _z_ = nullptr;          \
  } while (0)

#define ADD_COLUMN_MARKER(col, bgr)       \
  do {                                    \
    column_marker_columns.push_back(col); \
    column_marker_colors.push_back(bgr);  \
  } while (0)

#endif  // XITWEAKS_PREFS_H
