/* cc-desktop-icons-panel.c
 *
 * Copyright 2023 Elliot <BlindRepublic@mailo.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <shell/cc-shell.h>
#include <shell/cc-object-storage.h>

#include "cc-desktop-icons-panel.h"
#include "cc-desktop-icons-resources.h"

struct _CcDesktopIconsPanel {
	CcPanel  parent_instance;

  GtkSwitch *enable_switch;
  GtkBox    *header_box;

  GtkSwitch *home_switch;
  GtkSwitch *trash_switch;
  GtkSwitch *mounts_switch;

  GtkComboBoxText *click_policy_combo;
  GtkComboBoxText *icon_size_combo;

  GSettings *settings;
};

CC_PANEL_REGISTER (CcDesktopIconsPanel, cc_desktop_icons_panel)

static void
cc_desktop_icons_panel_constructed (GObject *object)
{
  CcDesktopIconsPanel *panel = CC_DESKTOP_ICONS_PANEL (object);

  G_OBJECT_CLASS (cc_desktop_icons_panel_parent_class)->constructed (object);

  g_settings_bind (panel->settings,
                   "show",
                   panel->enable_switch,
                   "active",
                   G_SETTINGS_BIND_DEFAULT);

  g_object_bind_property (panel->enable_switch,
                          "active",
                          panel,
                          "sensitive",
                          G_BINDING_SYNC_CREATE);


  cc_shell_embed_widget_in_header (cc_panel_get_shell (CC_PANEL (panel)),
  									               GTK_WIDGET (panel->header_box), GTK_POS_RIGHT);
}

static void
cc_desktop_icons_panel_finalize (GObject *object)
{
	CcDesktopIconsPanel *panel = CC_DESKTOP_ICONS_PANEL (object);

  g_clear_object (&panel->settings);

	G_OBJECT_CLASS (cc_desktop_icons_panel_parent_class)->finalize (object);
}

static void
cc_desktop_icons_panel_class_init (CcDesktopIconsPanelClass *klass)
{
  GObjectClass *object_class   = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  CcPanelClass *panel_class    = CC_PANEL_CLASS (klass);

  object_class->constructed = cc_desktop_icons_panel_constructed;
  object_class->finalize    = cc_desktop_icons_panel_finalize;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/desktop-icons/cc-desktop-icons-panel.ui");

  gtk_widget_class_bind_template_child (widget_class, CcDesktopIconsPanel, enable_switch);
  gtk_widget_class_bind_template_child (widget_class, CcDesktopIconsPanel, header_box);
  gtk_widget_class_bind_template_child (widget_class, CcDesktopIconsPanel, home_switch);
  gtk_widget_class_bind_template_child (widget_class, CcDesktopIconsPanel, trash_switch);
  gtk_widget_class_bind_template_child (widget_class, CcDesktopIconsPanel, mounts_switch);
  gtk_widget_class_bind_template_child (widget_class, CcDesktopIconsPanel, click_policy_combo);
  gtk_widget_class_bind_template_child (widget_class, CcDesktopIconsPanel, icon_size_combo);
}

static void
cc_desktop_icons_panel_init (CcDesktopIconsPanel *panel)
{
  g_resources_register (cc_desktop_icons_get_resource ());

  panel->settings = g_settings_new ("org.buddiesofbudgie.budgie-desktop-view");

  gtk_widget_init_template (GTK_WIDGET (panel));

  g_settings_bind (panel->settings,
                   "show-home-folder",
                   panel->home_switch,
                   "active",
                   G_SETTINGS_BIND_DEFAULT);

  g_settings_bind (panel->settings,
                   "show-trash-folder",
                   panel->trash_switch,
                   "active",
                   G_SETTINGS_BIND_DEFAULT);

  g_settings_bind (panel->settings,
                   "show-active-mounts",
                   panel->mounts_switch,
                   "active",
                   G_SETTINGS_BIND_DEFAULT);

  g_settings_bind (panel->settings,
                   "click-policy",
                   panel->click_policy_combo,
                   "active-id",
                   G_SETTINGS_BIND_DEFAULT);

  g_settings_bind (panel->settings,
                   "icon-size",
                   panel->icon_size_combo,
                   "active-id",
                   G_SETTINGS_BIND_DEFAULT);
}
