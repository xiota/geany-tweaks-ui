/*
 * Column Markers - Tweaks-UI Plugin for Geany
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

#include <string>
#include <vector>

#include "tkui_addons.h"

class TweakUiColumnMarkers {
 public:
  TweakUiColumnMarkers() = default;

  void show();
  void show_idle();
  void clear_columns();
  void add_column(int nColumn, int nColor);
  void add_column(std::string strColumn, std::string strColor);
  void set_columns(std::string strColumns, std::string strColors);
  void get_columns(std::string &strColumns, std::string &strColors);

  std::pair<std::string, std::string> get_columns();

 public:
  bool enable = false;

 private:
  static gboolean show_idle_callback(gpointer user_data);

 private:
  bool bHandleShowIdleInProgress = false;
  std::vector<int> vn_columns;
  std::vector<int> vn_colors;
};
