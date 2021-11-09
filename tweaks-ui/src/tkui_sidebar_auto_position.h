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

#pragma once

#include "tkui_addons.h"

class TweakUiSidebarAutoPosition {
 public:
  void initialize(GtkWidget *_geany_window);
  bool getEnabled() const;
  void setEnabled(bool const val);

 public:
  int position_maximized = -1;
  int position_normal = -1;
  int columns_maximized = -1;
  int columns_normal = -1;

 private:
  void update_connection();
  void update_position();
  void update_hpane();
  static gboolean hpane_callback(GtkWidget *hpane, cairo_t *cr,
                                 gpointer user_data);

 private:
  bool enabled = false;
  gulong ulHandlePanePosition = false;
  GtkWidget *geany_window = nullptr;
  GtkWidget *geany_hpane = nullptr;
};
