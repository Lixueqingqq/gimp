/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GIMP_IMAGE_PRIVATE_H__
#define __GIMP_IMAGE_PRIVATE_H__


typedef struct _GimpImagePrivate GimpImagePrivate;

struct _GimpImagePrivate
{
  gint               ID;                    /*  provides a unique ID         */

  GimpPlugInProcedure *load_proc;           /*  procedure used for loading   */
  GimpPlugInProcedure *save_proc;           /*  last save procedure used     */

  gchar             *display_name;          /*  display basename             */
  gint               width;                 /*  width in pixels              */
  gint               height;                /*  height in pixels             */
  gdouble            xresolution;           /*  image x-res, in dpi          */
  gdouble            yresolution;           /*  image y-res, in dpi          */
  GimpUnit           resolution_unit;       /*  resolution unit              */
  GimpImageBaseType  base_type;             /*  base gimp_image type         */

  guchar            *colormap;              /*  colormap (for indexed)       */
  gint               n_colors;              /*  # of colors (for indexed)    */

  gint               dirty;                 /*  dirty flag -- # of ops       */
  guint              dirty_time;            /*  time when image became dirty */
  gint               export_dirty;          /*  'dirty' but for export       */

  gint               undo_freeze_count;     /*  counts the _freeze's         */

  gint               instance_count;        /*  number of instances          */
  gint               disp_count;            /*  number of displays           */

  GimpTattoo         tattoo_state;          /*  the last used tattoo         */

  GimpProjection    *projection;            /*  projection layers & channels */
  GeglNode          *graph;                 /*  GEGL projection graph        */
};

#define GIMP_IMAGE_GET_PRIVATE(image) \
        G_TYPE_INSTANCE_GET_PRIVATE (image, \
                                     GIMP_TYPE_IMAGE, \
                                     GimpImagePrivate)


#endif  /* __GIMP_IMAGE_PRIVATE_H__ */