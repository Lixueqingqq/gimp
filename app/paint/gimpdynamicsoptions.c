/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995-1999 Spencer Kimball and Peter Mattis
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

#include "config.h"

#include <gegl.h>

#include "libgimpbase/gimpbase.h"
#include "libgimpmath/gimpmath.h"
#include "libgimpconfig/gimpconfig.h"

#include "paint-types.h"

#include "core/gimp.h"
#include "core/gimpimage.h"
#include "core/gimpgradient.h"
#include "core/gimppaintinfo.h"

//#include "gimppaintoptions.h"
#include "gimpdynamicsoptions.h"

#include "gimp-intl.h"


#define DEFAULT_DYNAMICS_EXPANDED      FALSE

#define DEFAULT_PRESSURE_OPACITY       TRUE
#define DEFAULT_PRESSURE_HARDNESS      FALSE
#define DEFAULT_PRESSURE_RATE          FALSE
#define DEFAULT_PRESSURE_SIZE          FALSE
#define DEFAULT_PRESSURE_INVERSE_SIZE  FALSE
#define DEFAULT_PRESSURE_ASPECT_RATIO  FALSE
#define DEFAULT_PRESSURE_COLOR         FALSE
#define DEFAULT_PRESSURE_ANGLE         FALSE
#define DEFAULT_PRESSURE_PRESCALE      1.0

#define DEFAULT_VELOCITY_OPACITY       FALSE
#define DEFAULT_VELOCITY_HARDNESS      FALSE
#define DEFAULT_VELOCITY_RATE          FALSE
#define DEFAULT_VELOCITY_SIZE          FALSE
#define DEFAULT_VELOCITY_INVERSE_SIZE  FALSE
#define DEFAULT_VELOCITY_ASPECT_RATIO  FALSE
#define DEFAULT_VELOCITY_COLOR         FALSE
#define DEFAULT_VELOCITY_ANGLE         FALSE
#define DEFAULT_VELOCITY_PRESCALE      1.0

#define DEFAULT_DIRECTION_OPACITY      FALSE
#define DEFAULT_DIRECTION_HARDNESS     FALSE
#define DEFAULT_DIRECTION_RATE         FALSE
#define DEFAULT_DIRECTION_SIZE         FALSE
#define DEFAULT_DIRECTION_INVERSE_SIZE FALSE
#define DEFAULT_DIRECTION_ASPECT_RATIO FALSE
#define DEFAULT_DIRECTION_COLOR        FALSE
#define DEFAULT_DIRECTION_ANGLE        FALSE
#define DEFAULT_DIRECTION_PRESCALE     1.0

#define DEFAULT_TILT_OPACITY           FALSE
#define DEFAULT_TILT_HARDNESS          FALSE
#define DEFAULT_TILT_RATE              FALSE
#define DEFAULT_TILT_SIZE              FALSE
#define DEFAULT_TILT_INVERSE_SIZE      FALSE
#define DEFAULT_TILT_ASPECT_RATIO      FALSE
#define DEFAULT_TILT_COLOR             FALSE
#define DEFAULT_TILT_ANGLE             FALSE
#define DEFAULT_TILT_PRESCALE          1.0

#define DEFAULT_RANDOM_OPACITY         FALSE
#define DEFAULT_RANDOM_HARDNESS        FALSE
#define DEFAULT_RANDOM_RATE            FALSE
#define DEFAULT_RANDOM_SIZE            FALSE
#define DEFAULT_RANDOM_INVERSE_SIZE    FALSE
#define DEFAULT_RANDOM_ASPECT_RATIO    FALSE
#define DEFAULT_RANDOM_COLOR           FALSE
#define DEFAULT_RANDOM_ANGLE           FALSE
#define DEFAULT_RANDOM_PRESCALE        1.0

#define DEFAULT_FADING_OPACITY         FALSE
#define DEFAULT_FADING_HARDNESS        FALSE
#define DEFAULT_FADING_RATE            FALSE
#define DEFAULT_FADING_SIZE            FALSE
#define DEFAULT_FADING_INVERSE_SIZE    FALSE
#define DEFAULT_FADING_ASPECT_RATIO    FALSE
#define DEFAULT_FADING_COLOR           FALSE
#define DEFAULT_FADING_ANGLE           FALSE
#define DEFAULT_FADING_PRESCALE        1.0

#define DEFAULT_FADE_LENGTH            100.0

enum
{
  PROP_0,

  PROP_DYNAMICS_INFO,
	
  PROP_DYNAMICS_EXPANDED,

  PROP_PRESSURE_OPACITY,
  PROP_PRESSURE_HARDNESS,
  PROP_PRESSURE_RATE,
  PROP_PRESSURE_SIZE,
  PROP_PRESSURE_INVERSE_SIZE,
  PROP_PRESSURE_ASPECT_RATIO,
  PROP_PRESSURE_COLOR,
  PROP_PRESSURE_ANGLE,
  PROP_PRESSURE_PRESCALE,

  PROP_VELOCITY_OPACITY,
  PROP_VELOCITY_HARDNESS,
  PROP_VELOCITY_RATE,
  PROP_VELOCITY_SIZE,
  PROP_VELOCITY_INVERSE_SIZE,
  PROP_VELOCITY_ASPECT_RATIO,
  PROP_VELOCITY_COLOR,
  PROP_VELOCITY_ANGLE,
  PROP_VELOCITY_PRESCALE,

  PROP_DIRECTION_OPACITY,
  PROP_DIRECTION_HARDNESS,
  PROP_DIRECTION_RATE,
  PROP_DIRECTION_SIZE,
  PROP_DIRECTION_INVERSE_SIZE,
  PROP_DIRECTION_ASPECT_RATIO,
  PROP_DIRECTION_COLOR,
  PROP_DIRECTION_ANGLE,
  PROP_DIRECTION_PRESCALE,

  PROP_TILT_OPACITY,
  PROP_TILT_HARDNESS,
  PROP_TILT_RATE,
  PROP_TILT_SIZE,
  PROP_TILT_INVERSE_SIZE,
  PROP_TILT_ASPECT_RATIO,
  PROP_TILT_COLOR,
  PROP_TILT_ANGLE,
  PROP_TILT_PRESCALE,

  PROP_RANDOM_OPACITY,
  PROP_RANDOM_HARDNESS,
  PROP_RANDOM_RATE,
  PROP_RANDOM_SIZE,
  PROP_RANDOM_INVERSE_SIZE,
  PROP_RANDOM_ASPECT_RATIO,
  PROP_RANDOM_COLOR,
  PROP_RANDOM_ANGLE,
  PROP_RANDOM_PRESCALE,

  PROP_FADING_OPACITY,
  PROP_FADING_HARDNESS,
  PROP_FADING_RATE,
  PROP_FADING_SIZE,
  PROP_FADING_INVERSE_SIZE,
  PROP_FADING_ASPECT_RATIO,
  PROP_FADING_COLOR,
  PROP_FADING_ANGLE,
  PROP_FADING_PRESCALE,
};

static gdouble gimp_dynamics_options_get_dynamics_mix (gdouble       mix1,
                                                       gdouble       mix1_scale,
                                                       gdouble       mix2,
                                                       gdouble       mix2_scale,
                                                       gdouble       mix3,
                                                       gdouble       mix3_scale,
                                                       gdouble       mix4,
                                                       gdouble       mix4_scale,
                                                       gdouble       mix5,
                                                       gdouble       mix5_scale,
                                                       gdouble       mix6,
                                                       gdouble       mix6_scale);

static void    gimp_dynamics_options_finalize         (GObject      *object);


static void    gimp_dynamics_options_notify           (GObject      *object,
                                                       GParamSpec   *pspec);

static void    gimp_dynamics_options_set_property     (GObject      *object,
                                                       guint         property_id,
                                                       const GValue *value,
                                                       GParamSpec   *pspec);
static void    gimp_dynamics_options_get_property     (GObject      *object,
                                                       guint         property_id,
                                                       GValue       *value,
                                                       GParamSpec   *pspec);

G_DEFINE_TYPE_WITH_CODE (GimpDynamicsOptions, gimp_dynamics_options, GIMP_TYPE_DATA,
                         G_IMPLEMENT_INTERFACE (GIMP_TYPE_TAGGED,
                                                gimp_dynamics_editor_docked_iface_init))
/*
G_DEFINE_TYPE (GimpDynamicsOptions, gimp_dynamics_options,
               GIMP_TYPE_DATA)
*/

#define parent_class gimp_dynamics_options_parent_class


static void
gimp_dynamics_options_class_init (GimpDynamicsOptionsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize     = gimp_dynamics_options_finalize;
  object_class->set_property = gimp_dynamics_options_set_property;
  object_class->get_property = gimp_dynamics_options_get_property;
  object_class->notify       = gimp_dynamics_options_notify;

	
  g_object_class_install_property (object_class, PROP_DYNAMICS_INFO,
                                   g_param_spec_object ("dynamics-info",
                                                        NULL, NULL,
                                                        GIMP_TYPE_PAINT_INFO,
                                                        GIMP_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY));
	
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DYNAMICS_EXPANDED,
                                    "dynamics-expanded", NULL,
                                    DEFAULT_DYNAMICS_EXPANDED,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_RANDOM_ASPECT_RATIO,
                                    "random-aspect-ratio", NULL,
                                    DEFAULT_RANDOM_ASPECT_RATIO,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_FADING_ASPECT_RATIO,
                                    "fading-aspect-ratio", NULL,
                                    DEFAULT_FADING_ASPECT_RATIO,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_PRESSURE_OPACITY,
                                    "pressure-opacity", NULL,
                                    DEFAULT_PRESSURE_OPACITY,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_PRESSURE_HARDNESS,
                                    "pressure-hardness", NULL,
                                    DEFAULT_PRESSURE_HARDNESS,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_PRESSURE_RATE,
                                    "pressure-rate", NULL,
                                    DEFAULT_PRESSURE_RATE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_PRESSURE_SIZE,
                                    "pressure-size", NULL,
                                    DEFAULT_PRESSURE_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_PRESSURE_COLOR,
                                    "pressure-color", NULL,
                                    DEFAULT_PRESSURE_COLOR,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_PRESSURE_ANGLE,
                                    "pressure-angle", NULL,
                                    DEFAULT_PRESSURE_COLOR,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_PRESSURE_INVERSE_SIZE,
                                    "pressure-inverse-size", NULL,
                                    DEFAULT_PRESSURE_INVERSE_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_PRESSURE_ASPECT_RATIO,
                                    "pressure-aspect-ratio", NULL,
                                    DEFAULT_PRESSURE_ASPECT_RATIO,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_PRESSURE_PRESCALE,
                                   "pressure-prescale", NULL,
                                   0.0, 1.0, DEFAULT_PRESSURE_PRESCALE,
                                   GIMP_PARAM_STATIC_STRINGS);

  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VELOCITY_OPACITY,
                                    "velocity-opacity", NULL,
                                    DEFAULT_VELOCITY_OPACITY,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VELOCITY_HARDNESS,
                                    "velocity-hardness", NULL,
                                    DEFAULT_VELOCITY_HARDNESS,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VELOCITY_RATE,
                                    "velocity-rate", NULL,
                                    DEFAULT_VELOCITY_RATE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VELOCITY_SIZE,
                                    "velocity-size", NULL,
                                    DEFAULT_VELOCITY_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VELOCITY_COLOR,
                                    "velocity-color", NULL,
                                    DEFAULT_VELOCITY_COLOR,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VELOCITY_ANGLE,
                                    "velocity-angle", NULL,
                                    DEFAULT_VELOCITY_COLOR,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VELOCITY_INVERSE_SIZE,
                                    "velocity-inverse-size", NULL,
                                    DEFAULT_VELOCITY_INVERSE_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VELOCITY_ASPECT_RATIO,
                                    "velocity-aspect-ratio", NULL,
                                    DEFAULT_VELOCITY_ASPECT_RATIO,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_VELOCITY_PRESCALE,
                                   "velocity-prescale", NULL,
                                   0.0, 1.0, DEFAULT_VELOCITY_PRESCALE,
                                   GIMP_PARAM_STATIC_STRINGS);

  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DIRECTION_OPACITY,
                                    "direction-opacity", NULL,
                                    DEFAULT_DIRECTION_OPACITY,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DIRECTION_HARDNESS,
                                    "direction-hardness", NULL,
                                    DEFAULT_DIRECTION_HARDNESS,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DIRECTION_RATE,
                                    "direction-rate", NULL,
                                    DEFAULT_DIRECTION_RATE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DIRECTION_SIZE,
                                    "direction-size", NULL,
                                    DEFAULT_DIRECTION_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DIRECTION_COLOR,
                                    "direction-color", NULL,
                                    DEFAULT_DIRECTION_COLOR,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DIRECTION_ANGLE,
                                    "direction-angle", NULL,
                                    DEFAULT_DIRECTION_ANGLE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DIRECTION_INVERSE_SIZE,
                                    "direction-inverse-size", NULL,
                                    DEFAULT_DIRECTION_INVERSE_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DIRECTION_ASPECT_RATIO,
                                    "direction-aspect-ratio", NULL,
                                    DEFAULT_DIRECTION_ASPECT_RATIO,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_DIRECTION_PRESCALE,
                                   "direction-prescale", NULL,
                                   0.0, 1.0, DEFAULT_DIRECTION_PRESCALE,
                                   GIMP_PARAM_STATIC_STRINGS);

  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_TILT_OPACITY,
                                    "tilt-opacity", NULL,
                                    DEFAULT_TILT_OPACITY,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_TILT_HARDNESS,
                                    "tilt-hardness", NULL,
                                    DEFAULT_TILT_HARDNESS,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_TILT_RATE,
                                    "tilt-rate", NULL,
                                    DEFAULT_TILT_RATE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_TILT_SIZE,
                                    "tilt-size", NULL,
                                    DEFAULT_TILT_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_TILT_COLOR,
                                    "tilt-color", NULL,
                                    DEFAULT_TILT_COLOR,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_TILT_ANGLE,
                                    "tilt-angle", NULL,
                                    DEFAULT_TILT_ANGLE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_TILT_INVERSE_SIZE,
                                    "tilt-inverse-size", NULL,
                                    DEFAULT_TILT_INVERSE_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_TILT_ASPECT_RATIO,
                                    "tilt-aspect-ratio", NULL,
                                    DEFAULT_TILT_ASPECT_RATIO,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_TILT_PRESCALE,
                                   "tilt-prescale", NULL,
                                   0.0, 1.0, DEFAULT_TILT_PRESCALE,
                                   GIMP_PARAM_STATIC_STRINGS);

  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_RANDOM_OPACITY,
                                    "random-opacity", NULL,
                                    DEFAULT_RANDOM_OPACITY,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_RANDOM_HARDNESS,
                                    "random-hardness", NULL,
                                    DEFAULT_RANDOM_HARDNESS,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_RANDOM_RATE,
                                    "random-rate", NULL,
                                    DEFAULT_RANDOM_RATE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_RANDOM_SIZE,
                                    "random-size", NULL,
                                    DEFAULT_RANDOM_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_RANDOM_COLOR,
                                    "random-color", NULL,
                                    DEFAULT_RANDOM_COLOR,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_RANDOM_ANGLE,
                                    "random-angle", NULL,
                                    DEFAULT_RANDOM_ANGLE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_RANDOM_INVERSE_SIZE,
                                    "random-inverse-size", NULL,
                                    DEFAULT_RANDOM_INVERSE_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_RANDOM_PRESCALE,
                                   "random-prescale", NULL,
                                   0.0, 1.0, DEFAULT_RANDOM_PRESCALE,
                                   GIMP_PARAM_STATIC_STRINGS);

  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_FADING_OPACITY,
                                    "fading-opacity", NULL,
                                    DEFAULT_FADING_OPACITY,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_FADING_HARDNESS,
                                    "fading-hardness", NULL,
                                    DEFAULT_FADING_HARDNESS,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_FADING_RATE,
                                    "fading-rate", NULL,
                                    DEFAULT_FADING_RATE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_FADING_SIZE,
                                    "fading-size", NULL,
                                    DEFAULT_FADING_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_FADING_COLOR,
                                    "fading-color", NULL,
                                    DEFAULT_FADING_COLOR,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_FADING_ANGLE,
                                    "fading-angle", NULL,
                                    DEFAULT_FADING_ANGLE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_FADING_INVERSE_SIZE,
                                    "fading-inverse-size", NULL,
                                    DEFAULT_FADING_INVERSE_SIZE,
                                    GIMP_PARAM_STATIC_STRINGS);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_FADING_PRESCALE,
                                   "fading-prescale", NULL,
                                   0.0, 1.0, DEFAULT_FADING_PRESCALE,
                                   GIMP_PARAM_STATIC_STRINGS);
}

static void
gimp_dynamics_options_init (GimpDynamicsOptions *options)
{
  options->application_mode_save = DEFAULT_APPLICATION_MODE;
	
  options->pressure_options  = g_slice_new0 (GimpDynamicOptions);
  options->velocity_options  = g_slice_new0 (GimpDynamicOptions);
  options->direction_options = g_slice_new0 (GimpDynamicOptions);
  options->tilt_options      = g_slice_new0 (GimpDynamicOptions);
  options->random_options    = g_slice_new0 (GimpDynamicOptions);
  options->fading_options    = g_slice_new0 (GimpDynamicOptions);
}


static void
gimp_dynamics_options_finalize (GObject *object)
{
  GimpDynamicsOptions *options = GIMP_DYNAMICS_OPTIONS (object);


  if (options->dynamics_info)
    g_object_unref (options->dynamics_info);
    
  g_slice_free (GimpDynamicOptions,  options->pressure_options);
  g_slice_free (GimpDynamicOptions,  options->velocity_options);
  g_slice_free (GimpDynamicOptions,  options->direction_options);
  g_slice_free (GimpDynamicOptions,  options->tilt_options);
  g_slice_free (GimpDynamicOptions,  options->random_options);
  g_slice_free (GimpDynamicOptions,  options->fading_options);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gimp_dynamics_options_set_property (GObject      *object,
                                 guint         property_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GimpDynamicsOptions *options           = GIMP_DYNAMICS_OPTIONS (object);
  GimpDynamicOptions  *pressure_options  = options->pressure_options;
  GimpDynamicOptions  *velocity_options  = options->velocity_options;
  GimpDynamicOptions  *direction_options = options->direction_options;
  GimpDynamicOptions  *tilt_options      = options->tilt_options;
  GimpDynamicOptions  *random_options    = options->random_options;
  GimpDynamicOptions  *fading_options    = options->fading_options;

  switch (property_id)
    {

    case PROP_DYNAMICS_INFO:
      g_value_set_object (value, options->dynamics_info);
      break;

    case PROP_DYNAMICS_EXPANDED:
      options->dynamics_expanded = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_OPACITY:
      pressure_options->opacity = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_HARDNESS:
      pressure_options->hardness = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_RATE:
      pressure_options->rate = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_SIZE:
      pressure_options->size = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_INVERSE_SIZE:
      pressure_options->inverse_size = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_ASPECT_RATIO:
      pressure_options->aspect_ratio = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_COLOR:
      pressure_options->color = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_ANGLE:
      pressure_options->angle = g_value_get_boolean (value);
      break;

    case PROP_PRESSURE_PRESCALE:
      pressure_options->prescale = g_value_get_double (value);
      break;

    case PROP_VELOCITY_OPACITY:
      velocity_options->opacity = g_value_get_boolean (value);
      break;

    case PROP_VELOCITY_HARDNESS:
      velocity_options->hardness = g_value_get_boolean (value);
      break;

    case PROP_VELOCITY_RATE:
      velocity_options->rate = g_value_get_boolean (value);
      break;

    case PROP_VELOCITY_SIZE:
      velocity_options->size = g_value_get_boolean (value);
      break;

    case PROP_VELOCITY_INVERSE_SIZE:
      velocity_options->inverse_size = g_value_get_boolean (value);
      break;

    case PROP_VELOCITY_ASPECT_RATIO:
      velocity_options->aspect_ratio = g_value_get_boolean (value);
      break;

    case PROP_VELOCITY_COLOR:
      velocity_options->color = g_value_get_boolean (value);
      break;

    case PROP_VELOCITY_ANGLE:
      velocity_options->angle = g_value_get_boolean (value);
      break;

    case PROP_VELOCITY_PRESCALE:
      velocity_options->prescale = g_value_get_double (value);
      break;

    case PROP_DIRECTION_OPACITY:
      direction_options->opacity = g_value_get_boolean (value);
      break;

    case PROP_DIRECTION_HARDNESS:
      direction_options->hardness = g_value_get_boolean (value);
      break;

    case PROP_DIRECTION_RATE:
      direction_options->rate = g_value_get_boolean (value);
      break;

    case PROP_DIRECTION_SIZE:
      direction_options->size = g_value_get_boolean (value);
      break;

    case PROP_DIRECTION_INVERSE_SIZE:
      direction_options->inverse_size = g_value_get_boolean (value);
      break;

    case PROP_DIRECTION_ASPECT_RATIO:
      direction_options->aspect_ratio = g_value_get_boolean (value);
      break;

    case PROP_DIRECTION_COLOR:
      direction_options->color = g_value_get_boolean (value);
      break;

    case PROP_DIRECTION_ANGLE:
      direction_options->angle = g_value_get_boolean (value);
      break;

    case PROP_DIRECTION_PRESCALE:
      direction_options->prescale = g_value_get_double (value);
      break;

    case PROP_TILT_OPACITY:
      tilt_options->opacity = g_value_get_boolean (value);
      break;

    case PROP_TILT_HARDNESS:
      tilt_options->hardness = g_value_get_boolean (value);
      break;

    case PROP_TILT_RATE:
      tilt_options->rate = g_value_get_boolean (value);
      break;

    case PROP_TILT_SIZE:
      tilt_options->size = g_value_get_boolean (value);
      break;

    case PROP_TILT_INVERSE_SIZE:
      tilt_options->inverse_size = g_value_get_boolean (value);
      break;

    case PROP_TILT_ASPECT_RATIO:
      tilt_options->aspect_ratio = g_value_get_boolean (value);
      break;

    case PROP_TILT_COLOR:
      tilt_options->color = g_value_get_boolean (value);
      break;

    case PROP_TILT_ANGLE:
      tilt_options->angle = g_value_get_boolean (value);
      break;

    case PROP_TILT_PRESCALE:
      tilt_options->prescale = g_value_get_double (value);
      break;

    case PROP_RANDOM_OPACITY:
      random_options->opacity = g_value_get_boolean (value);
      break;

    case PROP_RANDOM_HARDNESS:
      random_options->hardness = g_value_get_boolean (value);
      break;

    case PROP_RANDOM_RATE:
      random_options->rate = g_value_get_boolean (value);
      break;

    case PROP_RANDOM_SIZE:
      random_options->size = g_value_get_boolean (value);
      break;

    case PROP_RANDOM_INVERSE_SIZE:
      random_options->inverse_size = g_value_get_boolean (value);
      break;

    case PROP_RANDOM_ASPECT_RATIO:
      random_options->aspect_ratio = g_value_get_boolean (value);
      break;

    case PROP_RANDOM_COLOR:
      random_options->color = g_value_get_boolean (value);
      break;

    case PROP_RANDOM_ANGLE:
      random_options->angle = g_value_get_boolean (value);
      break;

    case PROP_RANDOM_PRESCALE:
      random_options->prescale = g_value_get_double (value);
      break;
/*Fading*/
    case PROP_FADING_OPACITY:
      fading_options->opacity = g_value_get_boolean (value);
      break;

    case PROP_FADING_HARDNESS:
      fading_options->hardness = g_value_get_boolean (value);
      break;

    case PROP_FADING_RATE:
      fading_options->rate = g_value_get_boolean (value);
      break;

    case PROP_FADING_SIZE:
      fading_options->size = g_value_get_boolean (value);
      break;

    case PROP_FADING_INVERSE_SIZE:
      fading_options->inverse_size = g_value_get_boolean (value);
      break;

    case PROP_FADING_ASPECT_RATIO:
      fading_options->aspect_ratio = g_value_get_boolean (value);
      break;

    case PROP_FADING_COLOR:
      fading_options->color = g_value_get_boolean (value);
      break;

    case PROP_FADING_ANGLE:
      fading_options->angle = g_value_get_boolean (value);
      break;

    case PROP_FADING_PRESCALE:
      fading_options->prescale = g_value_get_double (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}



static void
gimp_dynamics_options_get_property (GObject    *object,
                                 guint       property_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GimpDynamicsOptions *options           = GIMP_DYNAMICS_OPTIONS (object);
  GimpDynamicOptions  *pressure_options  = options->pressure_options;
  GimpDynamicOptions  *velocity_options  = options->velocity_options;
  GimpDynamicOptions  *direction_options = options->direction_options;
  GimpDynamicOptions  *tilt_options      = options->tilt_options;
  GimpDynamicOptions  *random_options    = options->random_options;
  GimpDynamicOptions  *fading_options    = options->fading_options;

  switch (property_id)
    {
    case PROP_DYNAMICS_INFO:
      g_value_set_object (value, options->dynamics_info);
      break;

    case PROP_DYNAMICS_EXPANDED:
      g_value_set_boolean (value, options->dynamics_expanded);
      break;

    case PROP_PRESSURE_OPACITY:
      g_value_set_boolean (value, pressure_options->opacity);
      break;

    case PROP_PRESSURE_HARDNESS:
      g_value_set_boolean (value, pressure_options->hardness);
      break;

    case PROP_PRESSURE_RATE:
      g_value_set_boolean (value, pressure_options->rate);
      break;

    case PROP_PRESSURE_SIZE:
      g_value_set_boolean (value, pressure_options->size);
      break;

    case PROP_PRESSURE_INVERSE_SIZE:
      g_value_set_boolean (value, pressure_options->inverse_size);
      break;

    case PROP_PRESSURE_ASPECT_RATIO:
      g_value_set_boolean (value, pressure_options->aspect_ratio);
      break;

    case PROP_PRESSURE_COLOR:
      g_value_set_boolean (value, pressure_options->color);
      break;

    case PROP_PRESSURE_ANGLE:
      g_value_set_boolean (value, pressure_options->angle);
      break;

    case PROP_PRESSURE_PRESCALE:
      g_value_set_double (value, pressure_options->prescale);
      break;

    case PROP_VELOCITY_OPACITY:
      g_value_set_boolean (value, velocity_options->opacity);
      break;

    case PROP_VELOCITY_HARDNESS:
      g_value_set_boolean (value, velocity_options->hardness);
      break;

    case PROP_VELOCITY_RATE:
      g_value_set_boolean (value, velocity_options->rate);
      break;

    case PROP_VELOCITY_SIZE:
      g_value_set_boolean (value, velocity_options->size);
      break;

    case PROP_VELOCITY_INVERSE_SIZE:
      g_value_set_boolean (value, velocity_options->inverse_size);
      break;

    case PROP_VELOCITY_ASPECT_RATIO:
      g_value_set_boolean (value, velocity_options->aspect_ratio);
      break;

    case PROP_VELOCITY_COLOR:
      g_value_set_boolean (value, velocity_options->color);
      break;

    case PROP_VELOCITY_ANGLE:
      g_value_set_boolean (value, velocity_options->angle);
      break;

    case PROP_VELOCITY_PRESCALE:
      g_value_set_double (value, velocity_options->prescale);
      break;

    case PROP_DIRECTION_OPACITY:
      g_value_set_boolean (value, direction_options->opacity);
      break;

    case PROP_DIRECTION_HARDNESS:
      g_value_set_boolean (value, direction_options->hardness);
      break;

    case PROP_DIRECTION_RATE:
      g_value_set_boolean (value, direction_options->rate);
      break;

    case PROP_DIRECTION_SIZE:
      g_value_set_boolean (value, direction_options->size);
      break;

    case PROP_DIRECTION_INVERSE_SIZE:
      g_value_set_boolean (value, direction_options->inverse_size);
      break;

    case PROP_DIRECTION_ASPECT_RATIO:
      g_value_set_boolean (value, direction_options->aspect_ratio);
      break;

    case PROP_DIRECTION_COLOR:
      g_value_set_boolean (value, direction_options->color);
      break;

    case PROP_DIRECTION_ANGLE:
      g_value_set_boolean (value, direction_options->angle);
      break;

    case PROP_DIRECTION_PRESCALE:
      g_value_set_double (value, direction_options->prescale);
      break;


    case PROP_TILT_OPACITY:
      g_value_set_boolean (value, tilt_options->opacity);
      break;

    case PROP_TILT_HARDNESS:
      g_value_set_boolean (value, tilt_options->hardness);
      break;

    case PROP_TILT_RATE:
      g_value_set_boolean (value, tilt_options->rate);
      break;

    case PROP_TILT_SIZE:
      g_value_set_boolean (value, tilt_options->size);
      break;

    case PROP_TILT_INVERSE_SIZE:
      g_value_set_boolean (value, tilt_options->inverse_size);
      break;

    case PROP_TILT_ASPECT_RATIO:
      g_value_set_boolean (value, tilt_options->aspect_ratio);
      break;

    case PROP_TILT_COLOR:
      g_value_set_boolean (value, tilt_options->color);
      break;

    case PROP_TILT_ANGLE:
      g_value_set_boolean (value, tilt_options->angle);
      break;

    case PROP_TILT_PRESCALE:
      g_value_set_double (value, tilt_options->prescale);
      break;

    case PROP_RANDOM_OPACITY:
      g_value_set_boolean (value, random_options->opacity);
      break;

    case PROP_RANDOM_HARDNESS:
      g_value_set_boolean (value, random_options->hardness);
      break;

    case PROP_RANDOM_RATE:
      g_value_set_boolean (value, random_options->rate);
      break;

    case PROP_RANDOM_SIZE:
      g_value_set_boolean (value, random_options->size);
      break;

    case PROP_RANDOM_INVERSE_SIZE:
      g_value_set_boolean (value, random_options->inverse_size);
      break;

    case PROP_RANDOM_ASPECT_RATIO:
      g_value_set_boolean (value, random_options->aspect_ratio);
      break;

    case PROP_RANDOM_COLOR:
      g_value_set_boolean (value, random_options->color);
      break;

    case PROP_RANDOM_ANGLE:
      g_value_set_boolean (value, random_options->angle);
      break;

    case PROP_RANDOM_PRESCALE:
      g_value_set_double (value, random_options->prescale);
      break;

/*fading*/

    case PROP_FADING_OPACITY:
      g_value_set_boolean (value, fading_options->opacity);
      break;

    case PROP_FADING_HARDNESS:
      g_value_set_boolean (value, fading_options->hardness);
      break;

    case PROP_FADING_RATE:
      g_value_set_boolean (value, fading_options->rate);
      break;

    case PROP_FADING_SIZE:
      g_value_set_boolean (value, fading_options->size);
      break;

    case PROP_FADING_INVERSE_SIZE:
      g_value_set_boolean (value, fading_options->inverse_size);
      break;

    case PROP_FADING_ASPECT_RATIO:
      g_value_set_boolean (value, fading_options->aspect_ratio);
      break;

    case PROP_FADING_COLOR:
      g_value_set_boolean (value, fading_options->color);
      break;

    case PROP_FADING_ANGLE:
      g_value_set_boolean (value, fading_options->angle);
      break;

    case PROP_FADING_PRESCALE:
      g_value_set_double (value, fading_options->prescale);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}


static void
gimp_dynamics_options_notify (GObject    *object,
                              GParamSpec *pspec)
{
  GimpDynamicsOptions *options = GIMP_DYNAMICS_OPTIONS (object);

  if (pspec->param_id == PROP_USE_GRADIENT)
    {
      if (options->gradient_options->use_gradient)
        {
          options->application_mode_save = options->application_mode;
          options->application_mode      = GIMP_PAINT_INCREMENTAL;
        }
      else
        {
          options->application_mode = options->application_mode_save;
        }

      g_object_notify (object, "application-mode");
    }

  if (G_OBJECT_CLASS (parent_class)->notify)
    G_OBJECT_CLASS (parent_class)->notify (object, pspec);
}

GimpDynamicsOptions *
gimp_dynamics_options_new (GimpPaintInfo *dynamics_info)
{
  GimpDynamicsOptions *options;

  g_return_val_if_fail (GIMP_IS_DYNAMICS_INFO (dynamics_info), NULL);

  options = g_object_new (dynamics_info->dynamics_options_type,
                          "gimp",       dynamics_info->gimp,
                          "name",       GIMP_OBJECT (dynamics_info)->name,
                          "paint-info", dynamics_info,
                          NULL);

  return options;
}


/* Calculates dynamics mix to be used for same parameter
 * (velocity/pressure/direction/tilt/random) mix Needed in may places and tools.
 *
 * Added one parameter: fading, the 6th driving factor. 
 * (velocity/pressure/direction/tilt/random/fading) 
 */
static gdouble
gimp_dynamics_options_get_dynamics_mix (gdouble mix1,
                                     gdouble mix1_scale,
                                     gdouble mix2,
                                     gdouble mix2_scale,
                                     gdouble mix3,
                                     gdouble mix3_scale,
                                     gdouble mix4,
                                     gdouble mix4_scale,
                                     gdouble mix5,
                                     gdouble mix5_scale,
                                     gdouble mix6,
                                     gdouble mix6_scale)
{
  gdouble scale_sum = 0.0;
  gdouble result    = 1.0;

  if (mix1 > -1.0)
    {
      scale_sum += fabs (mix1_scale);
    }
  else mix1 = 0.0;

  if (mix2 > -1.0)
    {
      scale_sum += fabs (mix2_scale);
    }
  else mix2 = 0.0;

  if (mix3 > -1.0)
    {
      scale_sum += fabs (mix3_scale);
    }
  else mix3 = 0.0;

  if (mix4 > -1.0)
    {
      scale_sum += fabs (mix4_scale);
    }
  else mix4 = 0.0;

  if (mix5 > -1.0)
    {
      scale_sum += fabs (mix5_scale);
    }
  else mix5 = 0.0;

  if (mix6 > -1.0)
    {
      scale_sum += fabs (mix6_scale);
    }
  else mix6 = 0.0;

  if (scale_sum > 0.0)
    {
      result = (mix1 * mix1_scale) / scale_sum +
               (mix2 * mix2_scale) / scale_sum +
               (mix3 * mix3_scale) / scale_sum +
               (mix4 * mix4_scale) / scale_sum +
               (mix5 * mix5_scale) / scale_sum +
               (mix6 * mix6_scale) / scale_sum;
    }

  if (result < 0.0)
    result = 1.0 + result;

  return MAX (0.0, result);
}

gdouble
gimp_dynamics_options_get_dynamic_opacity (GimpDynamicsOptions *dynamics_options,
                                        const GimpCoords *coords,
                                        gdouble           pixel_dist)
{
  gdouble opacity = 1.0;

  g_return_val_if_fail (GIMP_IS_DYNAMICS_OPTIONS (dynamics_options), 1.0);
  g_return_val_if_fail (coords != NULL, 1.0);

  if (dynamics_options->pressure_options->opacity  ||
      dynamics_options->velocity_options->opacity  ||
      dynamics_options->direction_options->opacity ||
      dynamics_options->tilt_options->opacity      ||
      dynamics_options->random_options->opacity    ||
      dynamics_options->fading_options->opacity)
    {
      gdouble pressure  = -1.0;
      gdouble velocity  = -1.0;
      gdouble direction = -1.0;
      gdouble tilt      = -1.0;
      gdouble random    = -1.0;
      gdouble fading    = -1.0;


      if (dynamics_options->pressure_options->opacity)
        pressure = GIMP_PAINT_PRESSURE_SCALE * coords->pressure;

      if (dynamics_options->velocity_options->opacity)
        velocity = GIMP_PAINT_VELOCITY_SCALE * (1 - coords->velocity);
		
      if (dynamics_options->random_options->opacity)
        random = g_random_double_range (0.0, 1.0);

      if (dynamics_options->tilt_options->opacity)
        tilt = 1.0 - sqrt (SQR (coords->xtilt) + SQR (coords->ytilt));

      if (dynamics_options->direction_options->opacity)
        direction = coords->direction + 0.5; /* mixer does not mix negative angles right so we shift */

      if (dynamics_options->fading_options->opacity)
        {
          gdouble p;
          gdouble fade_out;

          fade_out = DEFAULT_FADE_LENGTH;
          p = pixel_dist / fade_out;
          fading = exp (- p * p * 5.541);
        }
/*
          printf("pixel_dist: %f", pixel_dist);
*/

	  opacity = gimp_dynamics_options_get_dynamics_mix (pressure,
                                                     dynamics_options->pressure_options->prescale,
                                                     velocity,
                                                     dynamics_options->velocity_options->prescale,
                                                     random,
                                                     dynamics_options->random_options->prescale,
                                                     tilt,
                                                     dynamics_options->tilt_options->prescale,
                                                     direction,
                                                     dynamics_options->direction_options->prescale,
                                                     fading,
                                                     dynamics_options->fading_options->prescale);
    }

  return opacity;
}

gdouble
gimp_dynamics_options_get_dynamic_size (GimpDynamicsOptions *dynamics_options,
                                     GimpPaintOptions *paint_options,
                                     const GimpCoords *coords,
                                     gboolean          use_dynamics,
                                     gdouble           pixel_dist)
{
  gdouble scale = 1.0;

  if (use_dynamics)
    {
      gdouble pressure  = -1.0;
      gdouble velocity  = -1.0;
      gdouble direction = -1.0;
      gdouble random    = -1.0;
      gdouble tilt      = -1.0;
      gdouble fading    = -1.0;
      
      if (dynamics_options->pressure_options->size)
        {
          pressure = coords->pressure;
        }
      else if (dynamics_options->pressure_options->inverse_size)
        {
          pressure = 1.0 - 0.9 * coords->pressure;
        }

      if (dynamics_options->velocity_options->size)
        {
          velocity = 1.0 - sqrt (coords->velocity);
        }
      else if (dynamics_options->velocity_options->inverse_size)
        {
          velocity = sqrt (coords->velocity);
        }

      if (dynamics_options->random_options->size)
        {
          random = 1.0 - g_random_double_range (0.0, 1.0);
        }
      else if (dynamics_options->random_options->inverse_size)
        {
          random = g_random_double_range (0.0, 1.0);
        }

      if (dynamics_options->tilt_options->size)
        {
          tilt = 1.0 - sqrt (SQR (coords->xtilt) + SQR (coords->ytilt));
        }
      else if (dynamics_options->tilt_options->inverse_size)
        {
          tilt = sqrt (SQR (coords->xtilt) + SQR (coords->ytilt));
        }

      if (dynamics_options->direction_options->size)
         direction = coords->direction + 0.5; /* mixer does not mix negative angles right so we shift */


      if (dynamics_options->fading_options->size || 
          dynamics_options->fading_options->inverse_size)
        {
          gdouble p;
          gdouble fade_out;

          fade_out = DEFAULT_FADE_LENGTH;
          p = pixel_dist / fade_out;
          fading = exp (- p * p * 5.541);
        }	

      scale = gimp_dynamics_options_get_dynamics_mix (pressure,
                                                   dynamics_options->pressure_options->prescale,
                                                   velocity,
                                                   dynamics_options->velocity_options->prescale,
                                                   random,
                                                   dynamics_options->random_options->prescale,
                                                   tilt,
                                                   dynamics_options->tilt_options->prescale,
                                                   direction,
                                                   dynamics_options->direction_options->prescale,
                                                   fading,
                                                   dynamics_options->fading_options->prescale);

      if (scale < 1 / 64.0)
        scale = 1 / 8.0;
      else
        scale = sqrt (scale);
    }

  scale *= paint_options->brush_scale;

  return scale;
}

gdouble
gimp_dynamics_options_get_dynamic_aspect_ratio 
                                         (GimpDynamicsOptions *dynamics_options,
                                          GimpPaintOptions    *paint_options,
                                          const GimpCoords    *coords,
                                          gdouble              pixel_dist)
{
  gdouble aspect_ratio = 1.0;

  g_return_val_if_fail (GIMP_IS_DYNAMICS_OPTIONS (dynamics_options), 1.0);
  g_return_val_if_fail (coords != NULL, 1.0);

  if (dynamics_options->pressure_options->aspect_ratio  ||
      dynamics_options->velocity_options->aspect_ratio  ||
      dynamics_options->direction_options->aspect_ratio ||
      dynamics_options->tilt_options->aspect_ratio      ||
      dynamics_options->random_options->aspect_ratio    ||
      dynamics_options->fading_options->aspect_ratio)
    {
      gdouble pressure  = -1.0;
      gdouble velocity  = -1.0;
      gdouble direction = -1.0;
      gdouble tilt      = -1.0;
      gdouble random    = -1.0;
      gdouble fading    = -1.0;


      if (dynamics_options->pressure_options->aspect_ratio)
        pressure = 2 * coords->pressure;

      if (dynamics_options->velocity_options->aspect_ratio)
        velocity = 2 * coords->velocity;

      if (dynamics_options->random_options->aspect_ratio)
        {
           random = g_random_double_range (0.0, 1.0);
           if (random <= 0.5)
             {
                random = 1 / (random / 0.5 * (2.0 - 1.0) + 1.0);
             }
           else
             {
                random = (random - 0.5) / (1.0 - 0.5) * (2.0 - 1.0) + 1.0;
             }
        }

     if (dynamics_options->tilt_options->aspect_ratio)
       {
          tilt = sqrt ((1 - fabs (coords->xtilt)) / (1 - fabs (coords->ytilt)));
       }

      if (dynamics_options->direction_options->aspect_ratio)
        {
           direction = fmod (1 + coords->direction, 0.5) / 0.25;

           if ((coords->direction > 0.0) && (coords->direction < 0.5))
             direction = 1 / direction;
        }

      if (dynamics_options->fading_options->aspect_ratio)
        {
          gdouble p;
          gdouble fade_out;

          fade_out = DEFAULT_FADE_LENGTH;
          p = pixel_dist / fade_out;
          fading = exp (- p * p * 5.541);
        }

      aspect_ratio = gimp_dynamics_options_get_dynamics_mix (pressure,
                                                          dynamics_options->pressure_options->prescale,
                                                          velocity,
                                                          dynamics_options->velocity_options->prescale,
                                                          random,
                                                          dynamics_options->random_options->prescale,
                                                          tilt,
                                                          dynamics_options->tilt_options->prescale,
                                                          direction,
                                                          dynamics_options->direction_options->prescale,
                                                          fading,
                                                          dynamics_options->fading_options->prescale);
    }

  return paint_options->brush_aspect_ratio * aspect_ratio;
}

gdouble
gimp_dynamics_options_get_dynamic_rate (GimpDynamicsOptions *dynamics_options,
                                     const GimpCoords *coords,
                                     gdouble           pixel_dist)
{
  gdouble rate = 1.0;

  g_return_val_if_fail (GIMP_IS_DYNAMICS_OPTIONS (dynamics_options), 1.0);
  g_return_val_if_fail (coords != NULL, 1.0);

  if (dynamics_options->pressure_options->rate  ||
      dynamics_options->velocity_options->rate  ||
      dynamics_options->direction_options->rate ||
      dynamics_options->tilt_options->rate ||
      dynamics_options->random_options->rate)
    {
      gdouble pressure  = -1.0;
      gdouble velocity  = -1.0;
      gdouble direction = -1.0;
      gdouble random    = -1.0;
      gdouble tilt      = -1.0;
      gdouble fading    = -1.0;

      if (dynamics_options->pressure_options->rate)
        pressure = GIMP_PAINT_PRESSURE_SCALE * coords->pressure;

      if (dynamics_options->velocity_options->rate)
        velocity = GIMP_PAINT_VELOCITY_SCALE * (1 - coords->velocity);

      if (dynamics_options->random_options->rate)
        random = g_random_double_range (0.0, 1.0);

     if (dynamics_options->tilt_options->rate)
        tilt = 1.0 - sqrt (SQR (coords->xtilt) + SQR (coords->ytilt));

      if (dynamics_options->direction_options->rate)
        direction = coords->direction + 0.5; /* mixer does not mix negative angles right so we shift */

      if (dynamics_options->fading_options->rate)
        {
          gdouble p;
          gdouble fade_out;

          fade_out = DEFAULT_FADE_LENGTH;
          p = pixel_dist / fade_out;
          fading = exp (- p * p * 5.541);
        }

      rate = gimp_dynamics_options_get_dynamics_mix (pressure,
                                                  dynamics_options->pressure_options->prescale,
                                                  velocity,
                                                  dynamics_options->velocity_options->prescale,
                                                  random,
                                                  dynamics_options->random_options->prescale,
                                                  tilt,
                                                  dynamics_options->tilt_options->prescale,
                                                  direction,
                                                  dynamics_options->direction_options->prescale,
                                                  fading,
                                                  dynamics_options->fading_options->prescale);
    }

  return rate;
}


gdouble
gimp_dynamics_options_get_dynamic_color (GimpDynamicsOptions *dynamics_options,
                                      const GimpCoords *coords,
                                      gdouble           pixel_dist)
{
  gdouble color = 1.0;

  g_return_val_if_fail (GIMP_IS_DYNAMICS_OPTIONS (dynamics_options), 1.0);
  g_return_val_if_fail (coords != NULL, 1.0);

  if (dynamics_options->pressure_options->color  ||
      dynamics_options->velocity_options->color  ||
      dynamics_options->direction_options->color ||
      dynamics_options->tilt_options->color      ||
      dynamics_options->random_options->color  ||
      dynamics_options->fading_options->color)
    {
      gdouble pressure  = -1.0;
      gdouble velocity  = -1.0;
      gdouble direction = -1.0;
      gdouble random    = -1.0;
      gdouble tilt      = -1.0;
      gdouble fading    = -1.0;

      if (dynamics_options->pressure_options->color)
        pressure = GIMP_PAINT_PRESSURE_SCALE * coords->pressure;

      if (dynamics_options->velocity_options->color)
        velocity = GIMP_PAINT_VELOCITY_SCALE * coords->velocity;

      if (dynamics_options->random_options->color)
        random = g_random_double_range (0.0, 1.0);

     if (dynamics_options->tilt_options->color)
        tilt = 1.0 - sqrt (SQR (coords->xtilt) + SQR (coords->ytilt));

      if (dynamics_options->direction_options->color)
        direction = coords->direction + 0.5; /* mixer does not mix negative angles right so we shift */

      if (dynamics_options->fading_options->color)
        {
          gdouble p;
          gdouble fade_out;

          fade_out = DEFAULT_FADE_LENGTH;
          p = pixel_dist / fade_out;
          fading = exp (- p * p * 5.541);
        }
		
      color = gimp_dynamics_options_get_dynamics_mix (pressure,
                                                   dynamics_options->pressure_options->prescale,
                                                   velocity,
                                                   dynamics_options->velocity_options->prescale,
                                                   random,
                                                   dynamics_options->random_options->prescale,
                                                   tilt,
                                                   dynamics_options->tilt_options->prescale,
                                                   direction,
                                                   dynamics_options->direction_options->prescale,
                                                   fading,
                                                   dynamics_options->fading_options->prescale);
    }

  return color;
}

gdouble
gimp_dynamics_options_get_dynamic_hardness (GimpDynamicsOptions *dynamics_options,
                                         const GimpCoords *coords,
                                         gdouble           pixel_dist)
{
  gdouble hardness = 1.0;

  g_return_val_if_fail (GIMP_IS_DYNAMICS_OPTIONS (dynamics_options), 1.0);
  g_return_val_if_fail (coords != NULL, 1.0);

  if (dynamics_options->pressure_options->hardness  ||
      dynamics_options->velocity_options->hardness  ||
      dynamics_options->direction_options->hardness ||
      dynamics_options->tilt_options->hardness      ||
      dynamics_options->random_options->hardness   ||
      dynamics_options->fading_options->hardness)
    {
      gdouble pressure  = -1.0;
      gdouble velocity  = -1.0;
      gdouble direction = -1.0;
      gdouble random    = -1.0;
      gdouble tilt      = -1.0;
      gdouble fading      = -1.0;

      if (dynamics_options->pressure_options->hardness)
        pressure = GIMP_PAINT_PRESSURE_SCALE * coords->pressure;

      if (dynamics_options->velocity_options->hardness)
        velocity = GIMP_PAINT_VELOCITY_SCALE * (1 - coords->velocity);

      if (dynamics_options->random_options->hardness)
        random = g_random_double_range (0.0, 1.0);

     if (dynamics_options->tilt_options->hardness)
        tilt = 1.0 - sqrt (SQR (coords->xtilt) + SQR (coords->ytilt));

      if (dynamics_options->direction_options->hardness)
        direction = coords->direction + 0.5; /* mixer does not mix negative angles right so we shift */

      if (dynamics_options->fading_options->hardness)
        {
          gdouble p;
          gdouble fade_out;

          fade_out = DEFAULT_FADE_LENGTH;
          p = pixel_dist / fade_out;
          fading = exp (- p * p * 5.541);
        }

      hardness = gimp_dynamics_options_get_dynamics_mix (pressure,
                                                      dynamics_options->pressure_options->prescale,
                                                      velocity,
                                                      dynamics_options->velocity_options->prescale,
                                                      random,
                                                      dynamics_options->random_options->prescale,
                                                      tilt,
                                                      dynamics_options->tilt_options->prescale,
                                                      direction,
                                                      dynamics_options->direction_options->prescale,
                                                      fading,
                                                      dynamics_options->fading_options->prescale);
    }

  return hardness;
}

gdouble
gimp_dynamics_options_get_dynamic_angle (GimpDynamicsOptions *dynamics_options,
                                      const GimpCoords *coords,
                                      gdouble           pixel_dist)
{
  gdouble angle = 1.0;

  g_return_val_if_fail (GIMP_IS_DYNAMICS_OPTIONS (dynamics_options), 1.0);
  g_return_val_if_fail (coords != NULL, 1.0);

  if (dynamics_options->pressure_options->angle  ||
      dynamics_options->velocity_options->angle  ||
      dynamics_options->direction_options->angle ||
      dynamics_options->tilt_options->angle      ||
      dynamics_options->random_options->angle   ||
      dynamics_options->fading_options->angle)
    {
      gdouble pressure  = -1.0;
      gdouble velocity  = -1.0;
      gdouble direction = -1.0;
      gdouble random    = -1.0;
      gdouble tilt      = -1.0;
      gdouble fading    = -1.0;
      if (dynamics_options->pressure_options->angle)
        pressure = GIMP_PAINT_PRESSURE_SCALE * coords->pressure;

      if (dynamics_options->velocity_options->angle)
        velocity = GIMP_PAINT_VELOCITY_SCALE * (1 - coords->velocity);

      if (dynamics_options->random_options->angle)
        random = g_random_double_range (0.0, 1.0);

     /* For tilt to make sense, it needs to be converted to an angle, not just vector */
     if (dynamics_options->tilt_options->angle)
       {
         gdouble tilt_x = coords->xtilt;
         gdouble tilt_y = coords->ytilt;

         if (tilt_x == 0.0)
         {
           if (tilt_y >= 0.0)
             tilt = 0.5;
           else if (tilt_y < 0.0)
             tilt = 0.0;
           else tilt = -1.0;
         }
       else
         {
           tilt = atan ((- 1.0 * tilt_y) /
                                 tilt_x) / (2 * G_PI);

           if (tilt_x > 0.0)
             tilt = tilt + 0.5;
         }

         tilt = tilt + 0.5; /* correct the angle, its wrong by 180 degrees */

         while (tilt > 1.0)
           tilt -= 1.0;

         while (tilt < 0.0)
           tilt += 1.0;
       }

      if (dynamics_options->direction_options->angle)
        direction = coords->direction + 0.5; /* mixer does not mix negative angles right so we shift */

      if (dynamics_options->fading_options->angle)
        {
          gdouble p;
          gdouble fade_out;

          fade_out = DEFAULT_FADE_LENGTH;
          p = pixel_dist / fade_out;
          fading = exp (- p * p * 5.541);
        }

      angle = gimp_dynamics_options_get_dynamics_mix (pressure,
                                                   dynamics_options->pressure_options->prescale,
                                                   velocity,
                                                   dynamics_options->velocity_options->prescale,
                                                   random,
                                                   dynamics_options->random_options->prescale,
                                                   tilt,
                                                   dynamics_options->tilt_options->prescale,
                                                   direction,
                                                   dynamics_options->direction_options->prescale,
                                                   fading,
                                                   dynamics_options->fading_options->prescale);
      angle = angle - 0.5;
    }

  return angle + dynamics_options->brush_angle;
}

