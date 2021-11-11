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

class TweakUiAutoReadOnly {
 public:
  void initialize();
  void check_read_only(GeanyDocument* doc);
  void set_read_only();
  void toggle();

  static void document_signal(GObject* obj, GeanyDocument* doc,
                              TweakUiAutoReadOnly* self);

 public:
  std::string desc_enable =
      _("AutoReadOnly: Automatically sets read-only mode when the user does "
        "not have write permissions.  Permissions are checked on document "
        "open, reload, and activation.");
  bool enable = false;

 private:
  GtkWidget* main_window = nullptr;
  GtkCheckMenuItem* read_only_menu_item = nullptr;
};
