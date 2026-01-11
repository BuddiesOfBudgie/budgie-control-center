/*
 * Copyright (C) 2024 Budgie Desktop Developers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <config.h>
#include "cc-introduction-panel.h"
#include "cc-introduction-resources.h"
#include <gio/gdesktopappinfo.h>
#include <glib/gi18n.h>

struct _CcIntroductionPanel {
  CcPanel parent_instance;

  GtkWidget *flow_box;
  GtkWidget *quick_start_box;
  GPtrArray *desktop_apps;
};

CC_PANEL_REGISTER (CcIntroductionPanel, cc_introduction_panel)

typedef struct {
  const gchar *panel_id;
  const gchar *title;
  const gchar *icon_name;
} QuickStartItem;

static const QuickStartItem quick_start_items[] = {
  { "power", N_("Power"), "system-shutdown-symbolic" },
  { "sound", N_("Sound"), "audio-volume-high-symbolic" },
  { "wifi", N_("Wi-Fi"), "network-wireless-symbolic" },
  { "network", N_("Network"), "org.buddiesofbudgie.Settings-network-symbolic" },
};

static void
launch_desktop_app (GtkButton *button, GDesktopAppInfo *app_info)
{
  GError *error = NULL;

  if (!g_app_info_launch (G_APP_INFO (app_info), NULL, NULL, &error))
    {
      g_warning ("Failed to launch application: %s", error->message);
      g_error_free (error);
    }
}

static void
navigate_to_panel (GtkButton *button, CcIntroductionPanel *self)
{
  const gchar *panel_id;
  CcShell *shell;

  panel_id = g_object_get_data (G_OBJECT (button), "panel-id");
  shell = cc_panel_get_shell (CC_PANEL (self));

  if (shell && panel_id)
    {
      GError *error = NULL;
      if (!cc_shell_set_active_panel_from_id (shell, panel_id, NULL, &error))
        {
          g_warning ("Failed to navigate to panel '%s': %s", panel_id, error->message);
          g_clear_error (&error);
        }
    }
}

static GtkWidget *
create_quick_start_button (CcIntroductionPanel *self, const QuickStartItem *item)
{
  GtkWidget *button, *box, *image, *label;

  button = gtk_button_new ();
  gtk_widget_set_size_request (button, 120, 120);
  g_object_set_data (G_OBJECT (button), "panel-id", (gpointer) item->panel_id);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (box, GTK_ALIGN_CENTER);

  image = gtk_image_new_from_icon_name (item->icon_name, GTK_ICON_SIZE_DIALOG);
  gtk_image_set_pixel_size (GTK_IMAGE (image), 48);

  label = gtk_label_new (_(item->title));
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_label_set_max_width_chars (GTK_LABEL (label), 12);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER);

  gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (button), box);

  g_signal_connect (button, "clicked", G_CALLBACK (navigate_to_panel), self);

  return button;
}

static GtkWidget *
create_app_button (GDesktopAppInfo *app_info)
{
  GtkWidget *button, *box, *image, *label;
  GIcon *icon;
  const gchar *name;

  button = gtk_button_new ();
  gtk_widget_set_size_request (button, 120, 120);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (box, GTK_ALIGN_CENTER);

  icon = g_app_info_get_icon (G_APP_INFO (app_info));
  if (icon)
    {
      image = gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_DIALOG);
      gtk_image_set_pixel_size (GTK_IMAGE (image), 48);
    }
  else
    {
      image = gtk_image_new_from_icon_name ("application-x-executable", GTK_ICON_SIZE_DIALOG);
    }

  name = g_app_info_get_display_name (G_APP_INFO (app_info));
  label = gtk_label_new (name);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_label_set_max_width_chars (GTK_LABEL (label), 12);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER);

  gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (button), box);

  g_signal_connect (button, "clicked", G_CALLBACK (launch_desktop_app), app_info);

  return button;
}

static void
load_quick_start_buttons (CcIntroductionPanel *self)
{
  guint i;

  for (i = 0; i < G_N_ELEMENTS (quick_start_items); i++)
    {
      GtkWidget *button = create_quick_start_button (self, &quick_start_items[i]);
      gtk_flow_box_insert (GTK_FLOW_BOX (self->quick_start_box), button, -1);
      gtk_widget_show_all (button);
    }
}

static void
load_introduction_apps (CcIntroductionPanel *self)
{
  const gchar *intro_dir = BUDGIE_DATADIR "/budgie-control-center/introduction";
  g_autofree gchar *list_file = g_build_filename (intro_dir, "introduction.list", NULL);
  g_autofree gchar *template_file = g_build_filename (intro_dir, "introduction.template", NULL);
  g_autofree gchar *file_contents = NULL;
  g_auto(GStrv) lines = NULL;
  GError *error = NULL;
  gsize length;
  gint i;

  self->desktop_apps = g_ptr_array_new_with_free_func (g_object_unref);

  /* Try to read introduction.list first, fall back to introduction.template */
  if (!g_file_get_contents (list_file, &file_contents, &length, &error))
    {
      g_clear_error (&error);

      if (!g_file_get_contents (template_file, &file_contents, &length, &error))
        {
          g_warning ("Failed to read introduction files: %s", error->message);
          g_error_free (error);
          return;
        }
    }

  /* Parse the file line by line */
  lines = g_strsplit (file_contents, "\n", -1);

  for (i = 0; lines[i] != NULL; i++)
    {
      g_autofree gchar *trimmed = g_strstrip (g_strdup (lines[i]));
      GDesktopAppInfo *app_info;

      /* Skip empty lines and comments */
      if (!trimmed || *trimmed == '\0' || *trimmed == '#')
        continue;

      /* Create desktop app info from the path */
      app_info = g_desktop_app_info_new_from_filename (trimmed);

      if (app_info)
        {
          GtkWidget *button = create_app_button (app_info);
          gtk_flow_box_insert (GTK_FLOW_BOX (self->flow_box), button, -1);
          gtk_widget_show_all (button);

          g_ptr_array_add (self->desktop_apps, app_info);
        }
      else
        {
          g_warning ("Failed to load desktop file: %s", trimmed);
        }
    }
}

static const char *
cc_introduction_panel_get_help_uri (CcPanel *panel)
{
  return "help:gnome-help/introduction";
}

static void
cc_introduction_panel_finalize (GObject *object)
{
  CcIntroductionPanel *self = CC_INTRODUCTION_PANEL (object);

  g_clear_pointer (&self->desktop_apps, g_ptr_array_unref);

  G_OBJECT_CLASS (cc_introduction_panel_parent_class)->finalize (object);
}

static void
cc_introduction_panel_class_init (CcIntroductionPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  CcPanelClass *panel_class = CC_PANEL_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = cc_introduction_panel_finalize;
  panel_class->get_help_uri = cc_introduction_panel_get_help_uri;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/introduction/cc-introduction.ui");

  gtk_widget_class_bind_template_child (widget_class, CcIntroductionPanel, flow_box);
  gtk_widget_class_bind_template_child (widget_class, CcIntroductionPanel, quick_start_box);
}

static void
cc_introduction_panel_init (CcIntroductionPanel *self)
{
  g_resources_register (cc_introduction_get_resource ());
  gtk_widget_init_template (GTK_WIDGET (self));

  load_quick_start_buttons (self);
  load_introduction_apps (self);
}

GPtrArray *
cc_introduction_panel_get_desktop_apps (CcIntroductionPanel *self)
{
  g_return_val_if_fail (CC_IS_INTRODUCTION_PANEL (self), NULL);
  return self->desktop_apps;
}
