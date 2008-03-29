/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* gdu-page-volume-unrecognized.h
 *
 * Copyright (C) 2008 David Zeuthen
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

#ifndef GDU_PAGE_VOLUME_UNRECOGNIZED_H
#define GDU_PAGE_VOLUME_UNRECOGNIZED_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include "gdu-shell.h"

#define GDU_TYPE_PAGE_VOLUME_UNRECOGNIZED             (gdu_page_volume_unrecognized_get_type ())
#define GDU_PAGE_VOLUME_UNRECOGNIZED(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDU_TYPE_PAGE_VOLUME_UNRECOGNIZED, GduPageVolumeUnrecognized))
#define GDU_PAGE_VOLUME_UNRECOGNIZED_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), GDU_PAGE_VOLUME_UNRECOGNIZED,  GduPageVolumeUnrecognizedClass))
#define GDU_IS_PAGE_VOLUME_UNRECOGNIZED(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDU_TYPE_PAGE_VOLUME_UNRECOGNIZED))
#define GDU_IS_PAGE_VOLUME_UNRECOGNIZED_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), GDU_TYPE_PAGE_VOLUME_UNRECOGNIZED))
#define GDU_PAGE_VOLUME_UNRECOGNIZED_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), GDU_TYPE_PAGE_VOLUME_UNRECOGNIZED, GduPageVolumeUnrecognizedClass))

typedef struct _GduPageVolumeUnrecognizedClass       GduPageVolumeUnrecognizedClass;
typedef struct _GduPageVolumeUnrecognized            GduPageVolumeUnrecognized;

struct _GduPageVolumeUnrecognizedPrivate;
typedef struct _GduPageVolumeUnrecognizedPrivate     GduPageVolumeUnrecognizedPrivate;

struct _GduPageVolumeUnrecognized
{
        GObject parent;

        /* private */
        GduPageVolumeUnrecognizedPrivate *priv;
};

struct _GduPageVolumeUnrecognizedClass
{
        GObjectClass parent_class;
};

GType                      gdu_page_volume_unrecognized_get_type (void) G_GNUC_CONST;
GduPageVolumeUnrecognized *gdu_page_volume_unrecognized_new      (GduShell *shell);

#endif /* GDU_PAGE_VOLUME_UNRECOGNIZED_H */
