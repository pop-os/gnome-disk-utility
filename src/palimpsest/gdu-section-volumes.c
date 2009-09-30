/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* gdu-section-volumes.c
 *
 * Copyright (C) 2009 David Zeuthen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <config.h>
#include <string.h>
#include <glib/gi18n.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include <math.h>

#include <gdu-gtk/gdu-gtk.h>
#include "gdu-section-volumes.h"

struct _GduSectionVolumesPrivate
{
        GduPresentable *cur_volume;

        GtkWidget *grid;
        GtkWidget *details_table;
        GtkWidget *button_table;

        /* shared between all volume types */
        GduDetailsElement *usage_element;
        GduDetailsElement *capacity_element;
        GduDetailsElement *partition_element;
        GduDetailsElement *device_element;

        /* elements for the 'filesystem' usage */
        GduDetailsElement *fs_type_element;
        GduDetailsElement *fs_available_element;
        GduDetailsElement *fs_label_element;
        GduDetailsElement *fs_mount_point_element;

        GduButtonElement *fs_mount_button;
        GduButtonElement *fs_unmount_button;
        GduButtonElement *fs_check_button;
        GduButtonElement *format_button;
        GduButtonElement *partition_edit_button;
        GduButtonElement *partition_delete_button;
        GduButtonElement *partition_create_button;
};

G_DEFINE_TYPE (GduSectionVolumes, gdu_section_volumes, GDU_TYPE_SECTION)

/* ---------------------------------------------------------------------------------------------------- */

static void
gdu_section_volumes_finalize (GObject *object)
{
        GduSectionVolumes *section = GDU_SECTION_VOLUMES (object);

        if (section->priv->cur_volume != NULL)
                g_object_unref (section->priv->cur_volume);

        if (G_OBJECT_CLASS (gdu_section_volumes_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (gdu_section_volumes_parent_class)->finalize (object);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
unmount_op_callback (GduDevice *device,
                     GError    *error,
                     gpointer   user_data)
{
        GduShell *shell = GDU_SHELL (user_data);

        /* TODO: handle busy mounts using GtkMountOperation */

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = gdu_error_dialog_for_volume (GTK_WINDOW (gdu_shell_get_toplevel (shell)),
                                                      device,
                                                      _("Error unmounting volume"),
                                                      error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        }
        g_object_unref (shell);
}

static void
on_unmount_button_clicked (GduButtonElement *button_element,
                           gpointer          user_data)
{
        GduSectionVolumes *section = GDU_SECTION_VOLUMES (user_data);

        GduPresentable *v;
        GduDevice *d;

        v = NULL;
        d = NULL;

        v = gdu_volume_grid_get_selected (GDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = gdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        gdu_device_op_filesystem_unmount (d,
                                          unmount_op_callback,
                                          g_object_ref (gdu_section_get_shell (GDU_SECTION (section))));

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mount_op_callback (GduDevice *device,
                   gchar     *mount_point,
                   GError    *error,
                   gpointer   user_data)
{
        GduShell *shell = GDU_SHELL (user_data);

        if (error != NULL) {
                gdu_shell_raise_error (shell,
                                       NULL,
                                       error,
                                       _("Error mounting device"));
                g_error_free (error);
        } else {
                g_free (mount_point);
        }
        g_object_unref (shell);
}

static void
on_mount_button_clicked (GduButtonElement *button_element,
                         gpointer          user_data)
{
        GduSectionVolumes *section = GDU_SECTION_VOLUMES (user_data);
        GduPresentable *v;
        GduDevice *d;

        v = NULL;
        d = NULL;

        v = gdu_volume_grid_get_selected (GDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = gdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        gdu_device_op_filesystem_mount (d,
                                        NULL,
                                        mount_op_callback,
                                        g_object_ref (gdu_section_get_shell (GDU_SECTION (section))));

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
gdu_section_volumes_update (GduSection *_section)
{
        GduSectionVolumes *section = GDU_SECTION_VOLUMES (_section);
        GduPresentable *v;
        GduDevice *d;
        gchar *s;
        gchar *s2;
        const gchar *usage;
        gboolean show_fs_mount_button;
        gboolean show_fs_unmount_button;
        gboolean show_fs_check_button;
        gboolean show_format_button;
        gboolean show_partition_edit_button;
        gboolean show_partition_delete_button;
        gboolean show_partition_create_button;

        v = NULL;
        d = NULL;
        usage = "";
        show_fs_mount_button = FALSE;
        show_fs_unmount_button = FALSE;
        show_fs_check_button = FALSE;
        show_format_button = FALSE;
        show_partition_edit_button = FALSE;
        show_partition_delete_button = FALSE;
        show_partition_create_button = FALSE;

        v = gdu_volume_grid_get_selected (GDU_VOLUME_GRID (section->priv->grid));

        if (v != NULL) {
                d = gdu_presentable_get_device (v);
                if (d != NULL) {
                        usage = gdu_device_id_get_usage (d);
                }
        }

        /* ---------------------------------------------------------------------------------------------------- */
        /* rebuild table if the selected volume has changed */

        if (section->priv->cur_volume != v) {
                GPtrArray *elements;

                if (section->priv->cur_volume != NULL)
                        g_object_unref (section->priv->cur_volume);
                section->priv->cur_volume = v != NULL ? g_object_ref (v) : NULL;

                section->priv->usage_element = NULL;
                section->priv->capacity_element = NULL;
                section->priv->partition_element = NULL;
                section->priv->device_element = NULL;
                section->priv->fs_type_element = NULL;
                section->priv->fs_label_element = NULL;
                section->priv->fs_available_element = NULL;
                section->priv->fs_mount_point_element = NULL;

                elements = g_ptr_array_new_with_free_func (g_object_unref);

                section->priv->usage_element = gdu_details_element_new (_("Usage:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->usage_element);

                section->priv->device_element = gdu_details_element_new (_("Device:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->device_element);

                section->priv->partition_element = gdu_details_element_new (_("Partition:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->partition_element);

                section->priv->capacity_element = gdu_details_element_new (_("Capacity:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->capacity_element);

                if (g_strcmp0 (usage, "filesystem") == 0) {
                        section->priv->fs_type_element = gdu_details_element_new (_("Type:"), NULL, NULL);
                        g_ptr_array_add (elements, section->priv->fs_type_element);

                        section->priv->fs_available_element = gdu_details_element_new (_("Available:"), NULL, NULL);
                        g_ptr_array_add (elements, section->priv->fs_available_element);

                        section->priv->fs_label_element = gdu_details_element_new (_("Label:"), NULL, NULL);
                        g_ptr_array_add (elements, section->priv->fs_label_element);

                        section->priv->fs_mount_point_element = gdu_details_element_new (_("Mount Point:"), NULL, NULL);
                        g_ptr_array_add (elements, section->priv->fs_mount_point_element);
                }

                gdu_details_table_set_elements (GDU_DETAILS_TABLE (section->priv->details_table), elements);
                g_ptr_array_unref (elements);
        }

        /* ---------------------------------------------------------------------------------------------------- */
        /* reset all elements */

        if (section->priv->usage_element != NULL)
                gdu_details_element_set_text (section->priv->usage_element, "–");
        if (section->priv->capacity_element != NULL) {
                if (v != NULL) {
                        s = gdu_util_get_size_for_display (gdu_presentable_get_size (v), FALSE, TRUE);
                        gdu_details_element_set_text (section->priv->capacity_element, s);
                        g_free (s);
                } else {
                        gdu_details_element_set_text (section->priv->capacity_element, "–");
                }
        }
        if (section->priv->partition_element != NULL) {
                if (d != NULL && gdu_device_is_partition (d)) {
                        const gchar * const *partition_flags;
                        guint n;
                        GString *str;

                        str = g_string_new (NULL);
                        partition_flags = (const gchar * const *) gdu_device_partition_get_flags (d);
                        for (n = 0; partition_flags != NULL && partition_flags[n] != NULL; n++) {
                                const gchar *flag = partition_flags[n];

                                if (str->len > 0)
                                        g_string_append (str, ", ");

                                if (g_strcmp0 (flag, "boot") == 0) {
                                        /* Translators: This is for the 'boot' MBR/APM partition flag */
                                        g_string_append (str, _("Bootable"));
                                } else if (g_strcmp0 (flag, "required") == 0) {
                                        /* Translators: This is for the 'required' GPT partition flag */
                                        g_string_append (str, _("Required"));
                                } else if (g_strcmp0 (flag, "allocated") == 0) {
                                        /* Translators: This is for the 'allocated' APM partition flag */
                                        g_string_append (str, _("Allocated"));
                                } else if (g_strcmp0 (flag, "allow_read") == 0) {
                                        /* Translators: This is for the 'allow_read' APM partition flag */
                                        g_string_append (str, _("Allow Read"));
                                } else if (g_strcmp0 (flag, "allow_write") == 0) {
                                        /* Translators: This is for the 'allow_write' APM partition flag */
                                        g_string_append (str, _("Allow Write"));
                                } else if (g_strcmp0 (flag, "boot_code_is_pic") == 0) {
                                        /* Translators: This is for the 'boot_code_is_pic' APM partition flag */
                                        g_string_append (str, _("Boot Code PIC"));
                                } else {
                                        g_string_append (str, flag);
                                }
                        }

                        s = gdu_util_get_desc_for_part_type (gdu_device_partition_get_scheme (d),
                                                             gdu_device_partition_get_type (d));
                        if (str->len > 0) {
                                /* Translators: First %s is the partition type, second %s is a comma
                                 *              separated list of partition flags
                                 */
                                s2 = g_strdup_printf (C_("Partition Type", "%s (%s)"), s, str->str);
                                gdu_details_element_set_text (section->priv->partition_element, s2);
                                g_free (s2);
                        } else {
                                gdu_details_element_set_text (section->priv->partition_element, s);
                        }
                        g_free (s);
                        g_string_free (str, TRUE);

                        show_partition_edit_button = TRUE;
                        show_partition_delete_button = TRUE;
                } else {
                        gdu_details_element_set_text (section->priv->partition_element, "–");
                }
        }
        if (section->priv->device_element != NULL) {
                if (d != NULL) {
                        gdu_details_element_set_text (section->priv->device_element,
                                                      gdu_device_get_device_file (d));
                } else {
                        gdu_details_element_set_text (section->priv->device_element, "–");
                }
        }
        if (section->priv->fs_type_element != NULL)
                gdu_details_element_set_text (section->priv->fs_type_element, "–");
        if (section->priv->fs_available_element != NULL)
                gdu_details_element_set_text (section->priv->fs_available_element, "–");
        if (section->priv->fs_label_element != NULL)
                gdu_details_element_set_text (section->priv->fs_label_element, "–");
        if (section->priv->fs_mount_point_element != NULL)
                gdu_details_element_set_text (section->priv->fs_mount_point_element, "–");

        if (v == NULL)
                goto out;

        /* ---------------------------------------------------------------------------------------------------- */
        /* populate according to usage */

        if (g_strcmp0 (usage, "filesystem") == 0) {
                gdu_details_element_set_text (section->priv->usage_element, _("Filesystem"));
                s = gdu_util_get_fstype_for_display (gdu_device_id_get_type (d),
                                                     gdu_device_id_get_version (d),
                                                     TRUE);
                gdu_details_element_set_text (section->priv->fs_type_element, s);
                g_free (s);
                gdu_details_element_set_text (section->priv->fs_label_element,
                                              gdu_device_id_get_label (d));

                /* TODO: figure out amount of free space */
                gdu_details_element_set_text (section->priv->fs_available_element, "–");


                if (gdu_device_is_mounted (d)) {
                        const gchar* const *mount_paths;

                        /* For now we ignore if the device is mounted in multiple places */
                        mount_paths = (const gchar* const *) gdu_device_get_mount_paths (d);
                        s = g_strdup_printf ("<a title=\"%s\" href=\"file://%s\">%s</a>",
                                              /* Translators: this the mount point hyperlink tooltip */
                                              _("View files on the volume"),
                                             mount_paths[0],
                                             mount_paths[0]);
                        /* Translators: this the the text for the mount point
                         * item - %s is the mount point, e.g. '/media/disk'
                         */
                        s2 = g_strdup_printf (_("Mounted at %s"), s);
                        gdu_details_element_set_text (section->priv->fs_mount_point_element, s2);
                        g_free (s);
                        g_free (s2);

                        show_fs_unmount_button = TRUE;
                } else {
                        gdu_details_element_set_text (section->priv->fs_mount_point_element, _("Not Mounted"));
                        show_fs_mount_button = TRUE;
                }

                show_fs_check_button = TRUE;

                show_format_button = TRUE;
        } else if (g_strcmp0 (usage, "") == 0 &&
                   d != NULL && gdu_device_is_partition (d) &&
                   g_strcmp0 (gdu_device_partition_get_scheme (d), "mbr") == 0 &&
                   (g_strcmp0 (gdu_device_partition_get_type (d), "0x05") == 0 ||
                    g_strcmp0 (gdu_device_partition_get_type (d), "0x0f") == 0 ||
                    g_strcmp0 (gdu_device_partition_get_type (d), "0x85") == 0)) {
                gdu_details_element_set_text (section->priv->usage_element, _("Container for Logical Partitions"));

                show_format_button = TRUE;
        } else if (GDU_IS_VOLUME_HOLE (v)) {
                GduDevice *drive_device;
                gdu_details_element_set_text (section->priv->usage_element, _("Unallocated Space"));
                drive_device = gdu_presentable_get_device (gdu_section_get_presentable (GDU_SECTION (section)));
                gdu_details_element_set_text (section->priv->device_element,
                                              gdu_device_get_device_file (drive_device));
                g_object_unref (drive_device);

                show_partition_create_button = TRUE;
        }

        gdu_button_element_set_visible (section->priv->fs_mount_button, show_fs_mount_button);
        gdu_button_element_set_visible (section->priv->fs_unmount_button, show_fs_unmount_button);
        gdu_button_element_set_visible (section->priv->fs_check_button, show_fs_check_button);
        gdu_button_element_set_visible (section->priv->format_button, show_format_button);
        gdu_button_element_set_visible (section->priv->partition_edit_button, show_partition_edit_button);
        gdu_button_element_set_visible (section->priv->partition_delete_button, show_partition_delete_button);
        gdu_button_element_set_visible (section->priv->partition_create_button, show_partition_create_button);

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
on_grid_changed (GduVolumeGrid *grid,
                 gpointer       user_data)
{
        GduSectionVolumes *section = GDU_SECTION_VOLUMES (user_data);

        gdu_section_volumes_update (GDU_SECTION (section));
}

/* ---------------------------------------------------------------------------------------------------- */

static void
gdu_section_volumes_constructed (GObject *object)
{
        GduSectionVolumes *section = GDU_SECTION_VOLUMES (object);
        GPtrArray *button_elements;
        GduButtonElement *button_element;
        GtkWidget *grid;
        GtkWidget *align;
        GtkWidget *label;
        GtkWidget *vbox2;
        GtkWidget *table;
        gchar *s;

        gtk_box_set_spacing (GTK_BOX (section), 12);

        /*------------------------------------- */

        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        s = g_strconcat ("<b>", _("_Volumes"), "</b>", NULL);
        gtk_label_set_markup (GTK_LABEL (label), s);
        gtk_label_set_use_underline (GTK_LABEL (label), TRUE);
        g_free (s);
        gtk_box_pack_start (GTK_BOX (section), label, FALSE, FALSE, 0);

        align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
        gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 12, 0);
        gtk_box_pack_start (GTK_BOX (section), align, FALSE, FALSE, 0);

        vbox2 = gtk_vbox_new (FALSE, 6);
        gtk_container_add (GTK_CONTAINER (align), vbox2);

        grid = gdu_volume_grid_new (GDU_DRIVE (gdu_section_get_presentable (GDU_SECTION (section))));
        gtk_label_set_mnemonic_widget (GTK_LABEL (label), grid);
        section->priv->grid = grid;
        gtk_box_pack_start (GTK_BOX (vbox2),
                            grid,
                            FALSE,
                            FALSE,
                            0);
        g_signal_connect (grid,
                          "changed",
                          G_CALLBACK (on_grid_changed),
                          section);

        table = gdu_details_table_new (2, NULL);
        section->priv->details_table = table;
        gtk_box_pack_start (GTK_BOX (vbox2), table, FALSE, FALSE, 0);

        align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
        gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 12, 0);
        gtk_box_pack_start (GTK_BOX (vbox2), align, FALSE, FALSE, 0);

        table = gdu_button_table_new (2, NULL);
        section->priv->button_table = table;
        gtk_container_add (GTK_CONTAINER (align), table);
        button_elements = g_ptr_array_new_with_free_func (g_object_unref);

        button_element = gdu_button_element_new ("gdu-mount",
                                                 _("_Mount Volume"),
                                                 _("Mount the volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_mount_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->fs_mount_button = button_element;

        button_element = gdu_button_element_new ("gdu-unmount",
                                                 _("_Unmount Volume"),
                                                 _("Unmount the volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_unmount_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->fs_unmount_button = button_element;

        button_element = gdu_button_element_new ("gdu-check-disk",
                                                 _("_Check Filesystem"),
                                                 _("Check the filesystem for errors"));
#if 0
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_fsck_button_clicked),
                          section);
#endif
        g_ptr_array_add (button_elements, button_element);
        section->priv->fs_check_button = button_element;

        button_element = gdu_button_element_new ("nautilus-gdu",
                                                 _("Fo_rmat Volume"),
                                                 _("Format the volume"));
#if 0
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_format_button_clicked),
                          section);
#endif
        g_ptr_array_add (button_elements, button_element);
        section->priv->format_button = button_element;

        button_element = gdu_button_element_new (GTK_STOCK_EDIT,
                                                 _("Ed_it Partition"),
                                                 _("Change partition type and flags"));
#if 0
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_partition_edit_button_clicked),
                          section);
#endif
        g_ptr_array_add (button_elements, button_element);
        section->priv->partition_edit_button = button_element;

        button_element = gdu_button_element_new (GTK_STOCK_DELETE,
                                                 _("D_elete Partition"),
                                                 _("Delete the partition"));
#if 0
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_partition_delete_button_clicked),
                          section);
#endif
        g_ptr_array_add (button_elements, button_element);
        section->priv->partition_delete_button = button_element;

        button_element = gdu_button_element_new (GTK_STOCK_ADD,
                                                 _("_Create Partition"),
                                                 _("Create a new partition"));
#if 0
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_partition_create_button_clicked),
                          section);
#endif
        g_ptr_array_add (button_elements, button_element);
        section->priv->partition_create_button = button_element;

        gdu_button_table_set_elements (GDU_BUTTON_TABLE (section->priv->button_table), button_elements);
        g_ptr_array_unref (button_elements);

        /* -------------------------------------------------------------------------------- */

        gtk_widget_show_all (GTK_WIDGET (section));

        if (G_OBJECT_CLASS (gdu_section_volumes_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (gdu_section_volumes_parent_class)->constructed (object);
}

static void
gdu_section_volumes_class_init (GduSectionVolumesClass *klass)
{
        GObjectClass *gobject_class;
        GduSectionClass *section_class;

        gobject_class = G_OBJECT_CLASS (klass);
        section_class = GDU_SECTION_CLASS (klass);

        gobject_class->finalize    = gdu_section_volumes_finalize;
        gobject_class->constructed = gdu_section_volumes_constructed;
        section_class->update      = gdu_section_volumes_update;

        g_type_class_add_private (klass, sizeof (GduSectionVolumesPrivate));
}

static void
gdu_section_volumes_init (GduSectionVolumes *section)
{
        section->priv = G_TYPE_INSTANCE_GET_PRIVATE (section, GDU_TYPE_SECTION_VOLUMES, GduSectionVolumesPrivate);
}

GtkWidget *
gdu_section_volumes_new (GduShell       *shell,
                         GduPresentable *presentable)
{
        return GTK_WIDGET (g_object_new (GDU_TYPE_SECTION_VOLUMES,
                                         "shell", shell,
                                         "presentable", presentable,
                                         NULL));
}
