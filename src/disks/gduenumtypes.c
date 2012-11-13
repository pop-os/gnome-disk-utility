
/* Generated data (by glib-mkenums) */

#include "gduenums.h"
#include "gduenumtypes.h"

/* enumerations from "gduenums.h" */
GType
gdu_volume_grid_element_type_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GEnumValue values[] = {
        { GDU_VOLUME_GRID_ELEMENT_TYPE_CONTAINER, "GDU_VOLUME_GRID_ELEMENT_TYPE_CONTAINER", "container" },
        { GDU_VOLUME_GRID_ELEMENT_TYPE_NO_MEDIA, "GDU_VOLUME_GRID_ELEMENT_TYPE_NO_MEDIA", "no-media" },
        { GDU_VOLUME_GRID_ELEMENT_TYPE_FREE_SPACE, "GDU_VOLUME_GRID_ELEMENT_TYPE_FREE_SPACE", "free-space" },
        { GDU_VOLUME_GRID_ELEMENT_TYPE_DEVICE, "GDU_VOLUME_GRID_ELEMENT_TYPE_DEVICE", "device" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_enum_register_static (g_intern_static_string ("GduVolumeGridElementType"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}

GType
gdu_power_state_flags_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GFlagsValue values[] = {
        { GDU_POWER_STATE_FLAGS_NONE, "GDU_POWER_STATE_FLAGS_NONE", "none" },
        { GDU_POWER_STATE_FLAGS_STANDBY, "GDU_POWER_STATE_FLAGS_STANDBY", "standby" },
        { GDU_POWER_STATE_FLAGS_CHECKING, "GDU_POWER_STATE_FLAGS_CHECKING", "checking" },
        { GDU_POWER_STATE_FLAGS_FAILED, "GDU_POWER_STATE_FLAGS_FAILED", "failed" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_flags_register_static (g_intern_static_string ("GduPowerStateFlags"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}


/* Generated data ends here */

