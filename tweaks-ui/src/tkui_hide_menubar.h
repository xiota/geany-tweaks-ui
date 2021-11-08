/*
 * Hide Menubar - Tweaks-UI Plugin for Geany
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

#include "tkui_addon.h"

class TweakUiHideMenubar {
 public:
  void initialize();
  bool hide();
  void show();
  void startup();
  void toggle();
  bool get_state();

  void set_keybinding(GeanyKeyGroup *group, gsize key_id);
  void set_menubar_widget(GtkWidget *widget);

 public:
  bool hide_on_start = false;
  bool restore_state = false;
  bool previous_state = true;

 private:
  GtkWidget *geany_menubar = nullptr;
  GeanyKeyBinding *keybinding = nullptr;
};
