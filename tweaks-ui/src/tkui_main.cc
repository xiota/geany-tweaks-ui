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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>

#include "ao_colortip.h"
#include "ao_markword.h"
#include "auxiliary.h"
#include "tkui_main.h"
#include "tkui_settings.h"

/* ********************
 * Globals
 */
GeanyPlugin *geany_plugin;
GeanyData *geany_data;

namespace {

GtkWindow *geany_window = nullptr;
GtkNotebook *geany_sidebar = nullptr;
GtkNotebook *geany_msgwin = nullptr;
GtkNotebook *geany_editor = nullptr;
GtkWidget *geany_hpane = nullptr;
GtkWidget *geany_menubar = nullptr;

GtkWidget *g_tweaks_menu = nullptr;

gulong g_handle_pane_position = 0;
gulong g_handle_reload_config = 0;

AoMarkWord *g_markword = nullptr;
AoColorTip *g_colortip = nullptr;

GeanyKeyGroup *gKeyGroup = nullptr;

TweakUISettings settings;

/* ********************
 * Geany Signal Callbacks
 */

bool on_editor_notify(GObject *object, GeanyEditor *editor, SCNotification *nt,
                      gpointer data) {
  ao_mark_editor_notify(g_markword, editor, nt);
  ao_color_tip_editor_notify(g_colortip, editor, nt);

  return false;
}

void on_document_activate(GObject *obj, GeanyDocument *doc, gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  settings.column_markers.show_idle();
}

void on_document_new(GObject *obj, GeanyDocument *doc, gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  ao_mark_document_new(g_markword, doc);
  ao_color_tip_document_new(g_colortip, doc);

  settings.column_markers.show_idle();
}

void on_document_open(GObject *obj, GeanyDocument *doc, gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  ao_mark_document_open(g_markword, doc);
  ao_color_tip_document_open(g_colortip, doc);

  settings.column_markers.show_idle();
}

void on_document_close(GObject *obj, GeanyDocument *doc, gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  ao_mark_document_close(g_markword, doc);
  ao_color_tip_document_close(g_colortip, doc);
}

void on_document_reload(GObject *obj, GeanyDocument *doc, gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  settings.column_markers.show_idle();
}

void on_startup_complete(GObject *obj, GeanyDocument *doc, gpointer data) {
  settings.column_markers.show_idle();
}

void on_project_signal(GObject *obj, GKeyFile *config, gpointer data) {
  settings.column_markers.show_idle();
}

/* ********************
 * Plugin Functions
 */

gboolean tkui_plugin_init(GeanyPlugin *plugin, gpointer data) {
  geany_plugin = plugin;
  geany_data = plugin->geany_data;

  // geany widgets for later use
  geany_window = GTK_WINDOW(geany->main_widgets->window);
  geany_sidebar = GTK_NOTEBOOK(geany->main_widgets->sidebar_notebook);
  geany_msgwin = GTK_NOTEBOOK(geany->main_widgets->message_window_notebook);
  geany_editor = GTK_NOTEBOOK(geany->main_widgets->notebook);
  geany_hpane = ui_lookup_widget(GTK_WIDGET(geany_window), "hpaned1");
  geany_menubar = ui_lookup_widget(GTK_WIDGET(geany_window), "hbox_menubar");

  settings.open();
  settings.load();

  // setup menu
  GtkWidget *item;

  g_tweaks_menu = gtk_menu_item_new_with_label("Tweaks-UI");
  ui_add_document_sensitive(g_tweaks_menu);

  GtkWidget *submenu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(g_tweaks_menu), submenu);

  item = gtk_menu_item_new_with_label("Edit Config File");
  g_signal_connect(item, "activate", G_CALLBACK(on_pref_edit_config), nullptr);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  item = gtk_menu_item_new_with_label("Reload Config File");
  g_signal_connect(item, "activate", G_CALLBACK(on_pref_reload_config),
                   nullptr);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  item = gtk_menu_item_new_with_label("Open Config Folder");
  g_signal_connect(item, "activate", G_CALLBACK(on_pref_open_config_folder),
                   nullptr);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  item = gtk_separator_menu_item_new();
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  item = gtk_menu_item_new_with_label("Preferences");
  g_signal_connect(item, "activate", G_CALLBACK(on_menu_preferences), nullptr);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  gtk_widget_show_all(g_tweaks_menu);

  gtk_menu_shell_append(GTK_MENU_SHELL(geany_data->main_widgets->tools_menu),
                        g_tweaks_menu);

  // Set keyboard shortcuts
  gKeyGroup = plugin_set_key_group(geany_plugin, "Tweaks-UI", TWEAKS_KEY_COUNT,
                                   (GeanyKeyGroupCallback)on_key_binding);

  keybindings_set_item(
      gKeyGroup, TWEAKS_KEY_SWITCH_FOCUS_EDITOR_SIDEBAR_MSGWIN, nullptr, 0,
      GdkModifierType(0), "tweaks_ui_switch_focus_editor_sidebar_msgwin",
      _("Switch focus among editor, sidebar, and message window."), nullptr);

  keybindings_set_item(gKeyGroup, TWEAKS_KEY_TOGGLE_VISIBILITY_MENUBAR, nullptr,
                       0, GdkModifierType(0),
                       "tweaks_ui_toggle_visibility_menubar_",
                       _("Toggle visibility of the menubar."), nullptr);

  keybindings_set_item(gKeyGroup, TWEAKS_KEY_COPY, nullptr, 0,
                       GdkModifierType(0), "tweaks_ui_copy", _("Edit/Copy"),
                       nullptr);

  keybindings_set_item(gKeyGroup, TWEAKS_KEY_PASTE_1, nullptr, 0,
                       GdkModifierType(0), "tweaks_ui_paste_1",
                       _("Edit/Paste (1)"), nullptr);

  keybindings_set_item(gKeyGroup, TWEAKS_KEY_PASTE_2, nullptr, 0,
                       GdkModifierType(0), "tweaks_ui_paste_2",
                       _("Edit/Paste (2)"), nullptr);

  if (g_handle_reload_config == 0) {
    g_handle_reload_config = 1;
    g_idle_add(reload_config, nullptr);
  }

  // individual features
  g_markword = ao_mark_word_new(true, true);
  g_colortip = ao_color_tip_new(true, false);

  return true;
}

void tkui_plugin_cleanup(GeanyPlugin *plugin, gpointer data) {
  gtk_widget_destroy(g_tweaks_menu);

  // pane_position_update(false);

  g_object_unref(g_markword);
  g_object_unref(g_colortip);

  settings.save();
}

GtkWidget *tkui_plugin_configure(GeanyPlugin *plugin, GtkDialog *dialog,
                                 gpointer pdata) {
  GtkWidget *box, *btn;
  char *tooltip;

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

  tooltip = g_strdup("Save the active settings to the config file.");
  btn = gtk_button_new_with_label("Save Config");
  g_signal_connect(btn, "clicked", G_CALLBACK(on_pref_save_config), dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  tooltip = g_strdup(
      "Reload settings from the config file.  May be used "
      "to apply preferences after editing without restarting Geany.");
  btn = gtk_button_new_with_label("Reload Config");
  g_signal_connect(btn, "clicked", G_CALLBACK(on_pref_reload_config), dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  tooltip = g_strdup(
      "Delete the current config file and restore the default "
      "file with explanatory comments.");
  btn = gtk_button_new_with_label("Reset Config");
  g_signal_connect(btn, "clicked", G_CALLBACK(on_pref_reset_config), dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  tooltip = g_strdup("Open the config file in Geany for editing.");
  btn = gtk_button_new_with_label("Edit Config");
  g_signal_connect(btn, "clicked", G_CALLBACK(on_pref_edit_config), dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  tooltip = g_strdup(
      "Open the config folder in the default file manager.  The config folder "
      "contains the stylesheets, which may be edited.");
  btn = gtk_button_new_with_label("Open Config Folder");
  g_signal_connect(btn, "clicked", G_CALLBACK(on_pref_open_config_folder),
                   dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  return box;
}

PluginCallback tkui_plugin_callbacks[] = {
    {"document-activate", (GCallback)&on_document_activate, true, nullptr},
    {"document-close", (GCallback)&on_document_close, true, nullptr},
    {"document-new", (GCallback)&on_document_new, true, nullptr},
    {"document-open", (GCallback)&on_document_open, true, nullptr},
    {"document-reload", (GCallback)&on_document_reload, true, nullptr},
    {"editor-notify", (GCallback)&on_editor_notify, true, nullptr},
    {"geany-startup-complete", (GCallback)&on_startup_complete, true, nullptr},
    {"project-close", (GCallback)&on_project_signal, true, nullptr},
    {"project-open", (GCallback)&on_project_signal, true, nullptr},
    {"project-save", (GCallback)&on_project_signal, true, nullptr},
    {nullptr, nullptr, false, nullptr}};
}  // namespace

G_MODULE_EXPORT
void geany_load_module(GeanyPlugin *plugin) {
  // translation
  main_locale_init(LOCALEDIR, GETTEXT_PACKAGE);

  // plugin metadata
  plugin->info->name = "Tweaks-UI";
  plugin->info->description = _("User-interface tweaks for Geany.");
  plugin->info->version = VERSION;
  plugin->info->author = "xiota";

  // plugin functions
  plugin->funcs->init = tkui_plugin_init;
  plugin->funcs->cleanup = tkui_plugin_cleanup;
  plugin->funcs->help = nullptr;
  plugin->funcs->configure = tkui_plugin_configure;
  plugin->funcs->callbacks = tkui_plugin_callbacks;

  // register
  GEANY_PLUGIN_REGISTER(plugin, 226);
}

/* ********************
 * Preferences Callbacks
 */

gboolean reload_config(gpointer user_data) {
  settings.load();

  if (settings.menubar_hide_on_start) {
    hide_menubar();
  } else if (settings.menubar_restore_state &&
             !settings.menubar_previous_state) {
    hide_menubar();
  } else {
    gtk_widget_show(geany_menubar);
  }

  // pane_position_update(settings.sidebar_save_size_enabled ||
  //                      settings.sidebar_auto_size_enabled);

  settings.column_markers.show_idle();

  g_handle_reload_config = 0;
  return FALSE;
}

void on_pref_reload_config(GtkWidget *self, GtkWidget *dialog) {
  if (g_handle_reload_config == 0) {
    g_handle_reload_config = 1;
    g_idle_add(reload_config, nullptr);
  }
}

void on_pref_save_config(GtkWidget *self, GtkWidget *dialog) {
  settings.save();
}

void on_pref_reset_config(GtkWidget *self, GtkWidget *dialog) {
  settings.reset();
}

void on_pref_open_config_folder(GtkWidget *self, GtkWidget *dialog) {
  std::string conf_dn = cstr_assign(
      g_build_filename(geany_data->app->configdir, "plugins", nullptr));

  std::string command = R"(xdg-open ")" + conf_dn + R"(")";
  (void)!system(command.c_str());
}

void on_pref_edit_config(GtkWidget *self, GtkWidget *dialog) {
  const std::string &filename = settings.get_config_file();
  if (filename.empty()) {
    return;
  }

  GeanyDocument *doc =
      document_open_file(filename.c_str(), false, nullptr, nullptr);
  document_reload_force(doc, nullptr);

  if (dialog != nullptr) {
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }
}

void on_menu_preferences(GtkWidget *self, GtkWidget *dialog) {
  plugin_show_configure(geany_plugin);
}

/* ********************
 * Pane Position Callbacks
 */

/*
void pane_position_update(gboolean enable) {
  if (enable && !g_handle_pane_position) {
    g_handle_pane_position = g_signal_connect(
        GTK_WIDGET(geany_hpane), "draw", G_CALLBACK(on_draw_pane), nullptr);
  }

  if (!enable && g_handle_pane_position) {
    g_clear_signal_handler(&g_handle_pane_position, GTK_WIDGET(geany_hpane));
  }
}
*/
/*
gboolean on_draw_pane(GtkWidget *self, cairo_t *cr, gpointer user_data) {
  if (!settings.sidebar_save_size_enabled &&
      !settings.sidebar_auto_size_enabled) {
    //pane_position_update(false);
    return false;
  }

  int pos_auto_normal = 0;
  int pos_auto_maximized = 0;

  if (settings.sidebar_auto_size_enabled) {
    GeanyDocument *doc = document_get_current();
    if (doc != nullptr) {
      std::string str_auto_normal(settings.sidebar_auto_size_normal, '0');
      std::string str_auto_maximized(settings.sidebar_auto_size_maximized, '0');

      int pos_origin = (int)scintilla_send_message(
          doc->editor->sci, SCI_POINTXFROMPOSITION, 0, 1);
      int pos_normal = (int)scintilla_send_message(
          doc->editor->sci, SCI_TEXTWIDTH, 0, (gulong)str_auto_normal.c_str());
      int pos_maximized =
          (int)scintilla_send_message(doc->editor->sci, SCI_TEXTWIDTH, 0,
                                      (gulong)str_auto_maximized.c_str());
      pos_auto_normal = pos_origin + pos_normal;
      pos_auto_maximized = pos_origin + pos_maximized;
    }
  }

  static gboolean window_maximized_previous = false;
  const gboolean window_maximized_current =
      gtk_window_is_maximized(geany_window);

  if (window_maximized_current == window_maximized_previous) {
    // save current sidebar divider position
    if (settings.sidebar_save_size_update) {
      if (window_maximized_current) {
        settings.sidebar_save_size_maximized =
            gtk_paned_get_position(GTK_PANED(self));
      } else {
        settings.sidebar_save_size_normal =
            gtk_paned_get_position(GTK_PANED(self));
      }
    }
  } else if (settings.sidebar_auto_size_enabled) {
    if (window_maximized_current) {
      if (pos_auto_maximized > 100) {
        gtk_paned_set_position(GTK_PANED(self), pos_auto_maximized);
      }
    } else {
      if (pos_auto_normal > 100) {
        gtk_paned_set_position(GTK_PANED(self), pos_auto_normal);
      }
    }
    window_maximized_previous = window_maximized_current;
  } else if (settings.sidebar_save_size_enabled) {
    if (window_maximized_current) {
      if (settings.sidebar_save_size_maximized) {
        gtk_paned_set_position(GTK_PANED(self),
                               settings.sidebar_save_size_maximized);
      }
    } else {
      if (settings.sidebar_save_size_normal) {
        gtk_paned_set_position(GTK_PANED(self),
                               settings.sidebar_save_size_normal);
      }
    }
    window_maximized_previous = window_maximized_current;
  }

  return false;
}
*/

/* ********************
 * Keybinding Functions and Callbacks
 */

void on_switch_focus_editor_sidebar_msgwin() {
  GeanyDocument *doc = document_get_current();
  if (doc != nullptr) {
    gint cur_page = gtk_notebook_get_current_page(geany_sidebar);
    GtkWidget *page = gtk_notebook_get_nth_page(geany_sidebar, cur_page);
    // page = find_focus_widget(page);

    if (gtk_widget_has_focus(GTK_WIDGET(doc->editor->sci)) &&
        gtk_widget_is_visible(GTK_WIDGET(geany_sidebar))) {
      g_signal_emit_by_name(G_OBJECT(geany_sidebar), "grab-focus", nullptr);
      keybindings_send_command(GEANY_KEY_GROUP_FOCUS, GEANY_KEYS_FOCUS_SIDEBAR);
    } else if (gtk_widget_has_focus(page) &&
               gtk_widget_is_visible(GTK_WIDGET(geany_msgwin))) {
      g_signal_emit_by_name(G_OBJECT(geany_sidebar), "grab-notify", nullptr);
      keybindings_send_command(GEANY_KEY_GROUP_FOCUS,
                               GEANY_KEYS_FOCUS_MESSAGE_WINDOW);
    } else {
      g_signal_emit_by_name(G_OBJECT(geany_sidebar), "grab-notify", nullptr);
      keybindings_send_command(GEANY_KEY_GROUP_FOCUS, GEANY_KEYS_FOCUS_EDITOR);
    }
  }
}

bool hide_menubar() {
  if (gtk_widget_is_visible(geany_menubar)) {
    GeanyKeyBinding *kb =
        keybindings_get_item(gKeyGroup, TWEAKS_KEY_TOGGLE_VISIBILITY_MENUBAR);
    if (kb->key != 0) {
      gtk_widget_hide(geany_menubar);
      gchar *val = gtk_accelerator_name(kb->key, kb->mods);
      msgwin_status_add(_("Menubar has been hidden.  To reshow it, use: %s"),
                        val);
      g_free(val);
      return true;
    } else {
      msgwin_status_add(
          _("Menubar will not be hidden until after a keybinding to reshow "
            " it has been set."));
      return false;
    }
  }
  return false;
}
void on_toggle_visibility_menubar() {
  if (!hide_menubar()) {
    gtk_widget_show(geany_menubar);
  }
}

bool on_key_binding(int key_id) {
  switch (key_id) {
    case TWEAKS_KEY_SWITCH_FOCUS_EDITOR_SIDEBAR_MSGWIN:
      on_switch_focus_editor_sidebar_msgwin();
      break;
    case TWEAKS_KEY_TOGGLE_VISIBILITY_MENUBAR:
      on_toggle_visibility_menubar();
      break;
    case TWEAKS_KEY_COPY:
      keybindings_send_command(GEANY_KEY_GROUP_CLIPBOARD,
                               GEANY_KEYS_CLIPBOARD_COPY);
      break;
    case TWEAKS_KEY_PASTE_1:
    case TWEAKS_KEY_PASTE_2:
      keybindings_send_command(GEANY_KEY_GROUP_CLIPBOARD,
                               GEANY_KEYS_CLIPBOARD_PASTE);
      break;
    default:
      return false;
  }
  return true;
}

/*
GtkWidget *find_focus_widget(GtkWidget *widget) {
  GtkWidget *focus = nullptr;

  // optimized simple case
  if (GTK_IS_BIN(widget)) {
    focus = find_focus_widget(gtk_bin_get_child(GTK_BIN(widget)));
  } else if (GTK_IS_CONTAINER(widget)) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
    GList *node;

    for (node = children; node && !focus; node = node->next)
      focus = find_focus_widget(GTK_WIDGET(node->data));
    g_list_free(children);
  }

  / * Some containers handled above might not have children and be what we want
   * to focus (e.g. GtkTreeView), so focus that if possible and we don't have
   * anything better * /
  if (!focus && gtk_widget_get_can_focus(widget)) {
    focus = widget;
  }
  return focus;
}
*/

/* ********************
 * Idle Callbacks
 */
