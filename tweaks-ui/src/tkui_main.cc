/*
 * Tweaks-UI Plugin for Geany
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>

#include "auxiliary.h"
#include "tkui_main.h"
#include "tkui_settings.h"

/* ********************
 * Globals
 */
GeanyPlugin *geany_plugin;
GeanyData *geany_data;

namespace {  // global variable

GtkWindow *geany_window = nullptr;
GtkNotebook *geany_sidebar = nullptr;
GtkNotebook *geany_msgwin = nullptr;
GtkNotebook *geany_editor = nullptr;
GtkWidget *geany_menubar = nullptr;

GtkWidget *g_tweaks_menu = nullptr;

// AoMarkWord *gMarkWord = nullptr;

GeanyKeyGroup *gKeyGroup = nullptr;

TweakUiSettings settings;

}  // namespace

namespace {
/* ********************
 * Geany Signal Callbacks
 */

bool tkui_signal_editor_notify(GObject *object, GeanyEditor *editor,
                               SCNotification *nt, gpointer data) {
  settings.colortip.editor_notify(editor, nt);
  settings.markword.editor_notify(editor, nt);

  return false;
}

void tkui_signal_document_activate(GObject *obj, GeanyDocument *doc,
                                   gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  settings.auto_read_only.document_signal();
  settings.column_markers.show_idle();
}

void tkui_signal_document_new(GObject *obj, GeanyDocument *doc, gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  settings.column_markers.show_idle();

  settings.colortip.document_new(doc);
  settings.markword.document_new(doc);
}

void tkui_signal_document_open(GObject *obj, GeanyDocument *doc,
                               gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  settings.auto_read_only.document_signal();
  settings.column_markers.show_idle();

  settings.colortip.document_open(doc);
  settings.markword.document_open(doc);
}

void tkui_signal_document_close(GObject *obj, GeanyDocument *doc,
                                gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  settings.colortip.document_close(doc);
  settings.markword.document_close(doc);
}

void tkui_signal_document_reload(GObject *obj, GeanyDocument *doc,
                                 gpointer data) {
  g_return_if_fail(doc != NULL && doc->is_valid);

  settings.auto_read_only.document_signal();
  settings.column_markers.show_idle();
}

void tkui_signal_startup_complete(GObject *obj, GeanyDocument *doc,
                                  gpointer data) {
  settings.hide_menubar.startup();
  settings.column_markers.show_idle();
}

void tkui_signal_project_signal(GObject *obj, GKeyFile *config, gpointer data) {
  settings.column_markers.show_idle();
}

}  // namespace

namespace {

bool g_handle_reload_config = false;

gboolean reload_config(gpointer user_data) {
  settings.load();

  settings.hide_menubar.startup();
  settings.column_markers.show_idle();

  settings.sidebar_save_position.initialize(geany->main_widgets);
  settings.sidebar_auto_position.initialize(geany->main_widgets);
  settings.auto_read_only.initialize(GTK_WIDGET(geany_window));

  g_handle_reload_config = 0;
  return FALSE;
}

void tkui_pref_reload_config(GtkWidget *self, GtkWidget *dialog) {
  if (!g_handle_reload_config) {
    g_handle_reload_config = true;
    g_idle_add(reload_config, nullptr);
  }
}

void tkui_pref_save_config(GtkWidget *self, GtkWidget *dialog) {
  settings.save();
}

void tkui_pref_reset_config(GtkWidget *self, GtkWidget *dialog) {
  settings.reset();
}

void tkui_pref_open_config_folder(GtkWidget *self, GtkWidget *dialog) {
  std::string conf_dn = cstr_assign(
      g_build_filename(geany_data->app->configdir, "plugins", nullptr));

  std::string command = R"(xdg-open ")" + conf_dn + R"(")";
  (void)!system(command.c_str());
}

void tkui_pref_edit_config(GtkWidget *self, GtkWidget *dialog) {
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

void tkui_menu_preferences(GtkWidget *self, GtkWidget *dialog) {
  plugin_show_configure(geany_plugin);
}

/* ********************
 * Pane Position Callbacks
 */

bool tkui_key_binding(int key_id) {
  switch (key_id) {
    case TKUI_KEY_TOGGLE_MENUBAR_VISIBILITY:
      settings.hide_menubar.toggle();
      break;
    case TKUI_KEY_COPY:
      keybindings_send_command(GEANY_KEY_GROUP_CLIPBOARD,
                               GEANY_KEYS_CLIPBOARD_COPY);
      break;
    case TKUI_KEY_PASTE_1:
    case TKUI_KEY_PASTE_2:
      keybindings_send_command(GEANY_KEY_GROUP_CLIPBOARD,
                               GEANY_KEYS_CLIPBOARD_PASTE);
      break;
    case TKUI_KEY_TOGGLE_READONLY:
      settings.auto_read_only.toggle();
      break;
    default:
      return false;
  }
  return true;
}

}  // namespace

namespace {

gboolean tkui_plugin_init(GeanyPlugin *plugin, gpointer data) {
  geany_plugin = plugin;
  geany_data = plugin->geany_data;

  // geany widgets for later use
  geany_window = GTK_WINDOW(geany->main_widgets->window);
  geany_sidebar = GTK_NOTEBOOK(geany->main_widgets->sidebar_notebook);
  geany_msgwin = GTK_NOTEBOOK(geany->main_widgets->message_window_notebook);
  geany_editor = GTK_NOTEBOOK(geany->main_widgets->notebook);
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
  g_signal_connect(item, "activate", G_CALLBACK(tkui_pref_edit_config),
                   nullptr);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  item = gtk_menu_item_new_with_label("Reload Config File");
  g_signal_connect(item, "activate", G_CALLBACK(tkui_pref_reload_config),
                   nullptr);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  item = gtk_menu_item_new_with_label("Open Config Folder");
  g_signal_connect(item, "activate", G_CALLBACK(tkui_pref_open_config_folder),
                   nullptr);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  item = gtk_separator_menu_item_new();
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  item = gtk_menu_item_new_with_label("Preferences");
  g_signal_connect(item, "activate", G_CALLBACK(tkui_menu_preferences),
                   nullptr);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

  gtk_widget_show_all(g_tweaks_menu);

  gtk_menu_shell_append(GTK_MENU_SHELL(geany_data->main_widgets->tools_menu),
                        g_tweaks_menu);

  // Set keyboard shortcuts
  gKeyGroup = plugin_set_key_group(geany_plugin, "Tweaks-UI", TKUI_KEY_COUNT,
                                   (GeanyKeyGroupCallback)tkui_key_binding);

  keybindings_set_item(gKeyGroup, TKUI_KEY_COPY, nullptr, 0, GdkModifierType(0),
                       "tweaks_ui_copy", _("Edit/Copy"), nullptr);

  keybindings_set_item(gKeyGroup, TKUI_KEY_PASTE_1, nullptr, 0,
                       GdkModifierType(0), "tweaks_ui_paste_1",
                       _("Edit/Paste (1)"), nullptr);

  keybindings_set_item(gKeyGroup, TKUI_KEY_PASTE_2, nullptr, 0,
                       GdkModifierType(0), "tweaks_ui_paste_2",
                       _("Edit/Paste (2)"), nullptr);

  keybindings_set_item(gKeyGroup, TKUI_KEY_TOGGLE_MENUBAR_VISIBILITY, nullptr,
                       0, GdkModifierType(0),
                       "tweaks_ui_toggle_visibility_menubar_",
                       _("Toggle visibility of the menubar."), nullptr);

  keybindings_set_item(gKeyGroup, TKUI_KEY_TOGGLE_READONLY, nullptr, 0,
                       GdkModifierType(0), "tweaks_ui_toggle_readonly",
                       _("Toggle document read-only state"), nullptr);

  // initialize hide menubar
  settings.hide_menubar.set_menubar_widget(geany_menubar);
  settings.hide_menubar.set_keybinding(gKeyGroup,
                                       TKUI_KEY_TOGGLE_MENUBAR_VISIBILITY);

  if (g_handle_reload_config == 0) {
    g_handle_reload_config = 1;
    g_idle_add(reload_config, nullptr);
  }

  return true;
}

void tkui_plugin_cleanup(GeanyPlugin *plugin, gpointer data) {
  gtk_widget_destroy(g_tweaks_menu);

  settings.save_session();
}

GtkWidget *tkui_plugin_configure(GeanyPlugin *plugin, GtkDialog *dialog,
                                 gpointer pdata) {
  GtkWidget *box, *btn;
  char *tooltip;

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

  tooltip = g_strdup("Save the active settings to the config file.");
  btn = gtk_button_new_with_label("Save Config");
  g_signal_connect(btn, "clicked", G_CALLBACK(tkui_pref_save_config), dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  tooltip = g_strdup(
      "Reload settings from the config file.  May be used "
      "to apply preferences after editing without restarting Geany.");
  btn = gtk_button_new_with_label("Reload Config");
  g_signal_connect(btn, "clicked", G_CALLBACK(tkui_pref_reload_config), dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  tooltip = g_strdup(
      "Delete the current config file and restore the default "
      "file with explanatory comments.");
  btn = gtk_button_new_with_label("Reset Config");
  g_signal_connect(btn, "clicked", G_CALLBACK(tkui_pref_reset_config), dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  tooltip = g_strdup("Open the config file in Geany for editing.");
  btn = gtk_button_new_with_label("Edit Config");
  g_signal_connect(btn, "clicked", G_CALLBACK(tkui_pref_edit_config), dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  tooltip = g_strdup(
      "Open the config folder in the default file manager.  The config folder "
      "contains the stylesheets, which may be edited.");
  btn = gtk_button_new_with_label("Open Config Folder");
  g_signal_connect(btn, "clicked", G_CALLBACK(tkui_pref_open_config_folder),
                   dialog);
  gtk_box_pack_start(GTK_BOX(box), btn, false, false, 3);
  gtk_widget_set_tooltip_text(btn, tooltip);
  GFREE(tooltip);

  return box;
}

PluginCallback tkui_plugin_callbacks[] = {
    {"document-activate", (GCallback)&tkui_signal_document_activate, true,
     nullptr},
    {"document-close", (GCallback)&tkui_signal_document_close, true, nullptr},
    {"document-new", (GCallback)&tkui_signal_document_new, true, nullptr},
    {"document-open", (GCallback)&tkui_signal_document_open, true, nullptr},
    {"document-reload", (GCallback)&tkui_signal_document_reload, true, nullptr},
    {"editor-notify", (GCallback)&tkui_signal_editor_notify, true, nullptr},
    {"geany-startup-complete", (GCallback)&tkui_signal_startup_complete, true,
     nullptr},
    {"project-close", (GCallback)&tkui_signal_project_signal, true, nullptr},
    {"project-open", (GCallback)&tkui_signal_project_signal, true, nullptr},
    {"project-save", (GCallback)&tkui_signal_project_signal, true, nullptr},
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
