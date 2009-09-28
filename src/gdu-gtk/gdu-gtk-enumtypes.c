
/* Generated data (by glib-mkenums) */

#include <gdu-gtk/gdu-gtk.h>

/* enumerations from "gdu-gtk-enums.h" */
GType
gdu_pool_tree_model_column_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GEnumValue values[] = {
        { GDU_POOL_TREE_MODEL_COLUMN_ICON, "GDU_POOL_TREE_MODEL_COLUMN_ICON", "icon" },
        { GDU_POOL_TREE_MODEL_COLUMN_NAME, "GDU_POOL_TREE_MODEL_COLUMN_NAME", "name" },
        { GDU_POOL_TREE_MODEL_COLUMN_VPD_NAME, "GDU_POOL_TREE_MODEL_COLUMN_VPD_NAME", "vpd-name" },
        { GDU_POOL_TREE_MODEL_COLUMN_DESCRIPTION, "GDU_POOL_TREE_MODEL_COLUMN_DESCRIPTION", "description" },
        { GDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE, "GDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE", "presentable" },
        { GDU_POOL_TREE_MODEL_COLUMN_VISIBLE, "GDU_POOL_TREE_MODEL_COLUMN_VISIBLE", "visible" },
        { GDU_POOL_TREE_MODEL_COLUMN_TOGGLED, "GDU_POOL_TREE_MODEL_COLUMN_TOGGLED", "toggled" },
        { GDU_POOL_TREE_MODEL_COLUMN_CAN_BE_TOGGLED, "GDU_POOL_TREE_MODEL_COLUMN_CAN_BE_TOGGLED", "can-be-toggled" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_enum_register_static (g_intern_static_string ("GduPoolTreeModelColumn"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}

GType
gdu_pool_tree_view_flags_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GFlagsValue values[] = {
        { GDU_POOL_TREE_VIEW_FLAGS_NONE, "GDU_POOL_TREE_VIEW_FLAGS_NONE", "none" },
        { GDU_POOL_TREE_VIEW_FLAGS_SHOW_TOGGLE, "GDU_POOL_TREE_VIEW_FLAGS_SHOW_TOGGLE", "show-toggle" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_flags_register_static (g_intern_static_string ("GduPoolTreeViewFlags"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}

GType
gdu_pool_tree_model_flags_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GFlagsValue values[] = {
        { GDU_POOL_TREE_MODEL_FLAGS_NONE, "GDU_POOL_TREE_MODEL_FLAGS_NONE", "none" },
        { GDU_POOL_TREE_MODEL_FLAGS_NO_VOLUMES, "GDU_POOL_TREE_MODEL_FLAGS_NO_VOLUMES", "no-volumes" },
        { GDU_POOL_TREE_MODEL_FLAGS_NO_UNALLOCATABLE_DRIVES, "GDU_POOL_TREE_MODEL_FLAGS_NO_UNALLOCATABLE_DRIVES", "no-unallocatable-drives" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_flags_register_static (g_intern_static_string ("GduPoolTreeModelFlags"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}


/* Generated data ends here */

