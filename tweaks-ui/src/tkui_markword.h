/*
 * MarkWord - Tweaks-UI Plugin for Geany
 * Copyright 2021 xiota
 *
 * AoMarkWord - part of Addons, a Geany plugin
 * Copyright 2009-2011 Enrico Tröger <enrico(dot)troeger(at)uvena(dot)de>
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

#define DOUBLE_CLICK_DELAY 50

class TweakUiMarkWord {
 public:
  void initialize();
  void document_new(GeanyDocument *doc);
  void document_open(GeanyDocument *doc);
  void document_close(GeanyDocument *doc);
  void editor_notify(GeanyEditor *editor, SCNotification *nt);

 public:
  bool enable = false;
  bool single_click_deselect = true;

 private:
  void connect_document_button_press_signal_handler(GeanyDocument *doc);

  static void clear_marker();
  static gboolean mark_word(TweakUiMarkWord *self);
  static gboolean on_editor_button_press_event(GtkWidget *widget,
                                               GdkEventButton *event,
                                               TweakUiMarkWord *self);

 private:
  gulong double_click_timer_id = 0;
};
