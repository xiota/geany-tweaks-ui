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

#include "tkui_auto_read_only.h"

void TweakUiAutoReadOnly::initialize(GtkWidget* mw) {
  if (mw) {
    main_window = mw;
    readonly_menu_item = GTK_CHECK_MENU_ITEM(
        ui_lookup_widget(GTK_WIDGET(main_window), "set_file_readonly1"));
  }
}

void TweakUiAutoReadOnly::document_signal() {
  if (!enable) {
    return;
  }

  GeanyDocument* doc = document_get_current();
  g_return_if_fail(DOC_VALID(doc));

  if (doc->real_path && euidaccess(doc->real_path, W_OK) != 0) {
    set_readonly();
  }
}

void TweakUiAutoReadOnly::set_readonly() {
  if (readonly_menu_item) {
    gtk_check_menu_item_set_active(readonly_menu_item, true);
  }
}

void TweakUiAutoReadOnly::toggle() {
  if (readonly_menu_item) {
    gtk_check_menu_item_set_active(
        readonly_menu_item,
        !gtk_check_menu_item_get_active(readonly_menu_item));
  }
}
