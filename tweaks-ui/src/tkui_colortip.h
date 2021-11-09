/*
 * ColorTip - Tweaks-UI Plugin for Geany
 * Copyright 2021 xiota
 *
 * AoColorTip - part of Addons, a Geany plugin
 * Copyright 2017 LarsGit223
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

#include "tkui_addons.h"

class TweakUiColorTip {
 public:
  void initialize();
  void document_new(GeanyDocument *doc);
  void document_open(GeanyDocument *doc);
  void document_close(GeanyDocument *doc);
  void editor_notify(GeanyEditor *editor, SCNotification *nt);
  void setSize(std::string strSize = "");

 public:
  bool color_tooltip = false;
  bool color_chooser = false;
  std::string color_tooltip_size{"small"};

 private:
  void connect_document_button_press_signal_handler(GeanyDocument *doc);

  static int contains_color_value(char *string, int position, int maxdist);
  static int get_color_value_at_current_doc_position();
  static gboolean on_editor_button_press_event(GtkWidget *widget,
                                               GdkEventButton *event,
                                               TweakUiColorTip *self);

 private:
  std::string colortip_template{"    "};
};
