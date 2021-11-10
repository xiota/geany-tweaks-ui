# Tweaks-UI Plugin for Geany

This plugin provides (mainly) user-interface tweaks for Geany.

## Features

* Auto Read Only: Automatically switch to read-only mode for files without write permission.
* Column Markers: Show multiple column markers in the editor.
* Hide Menubar: Hide the menubar via keybinding.  Restore previous state or hide at startup.
* Sidebar Save Size: Save different sidebar sizes for maximized and normal window states.
* Sidebar Auto Size: Automatically size the sidebar to keep a specified number of columns visible in the editor on maximize/unmaximize.
* UnchangeDocument: Unsets the change state for new, empty documents.

### Ported from the Addons plugin

* ColorTip: Visualize color hex codes in a tooltip.  Open the color chooser tool.
* MarkWord: Highlight double clicked words throughout the document.

### Keybindings:

* Extra Copy and Paste
* Toggle menubar visibility
* Toggle document read-only mode

## Usage

Options with their descriptions are contained in the config file `~/.config/geany/plugins/tweaks/tweaks-ui.conf`.  For convenience, menu items under *Tools / Tweaks-UI* may be used to edit and reload the configuration. 

Keybindings are accessible under *Edit / Preferences / Keybindings / Tweaks-UI*.

## Installing

This plugin is not yet available in distro package repositories.  Experimental (aka, *buggy*) versions of this and some other plugins are available for Ubuntu via [PPA](https://launchpad.net/~xiota/+archive/ubuntu/geany-plugins).

## Problems

If you are having problems with geany-plugins, you may report problems at [geany/plugins](https://github.com/geany/plugins/).

If you are using packages from the [PPA](https://launchpad.net/~xiota/+archive/ubuntu/geany-plugins), please report problems at [xiota/tweaks-ui](https://github.com/xiota/tweaks-ui/).

## License

This plugin is licensed under the [GPLv3](COPYING) or later.