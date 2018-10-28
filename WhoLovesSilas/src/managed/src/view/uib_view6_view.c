
/*******************************************************************************
* This file was generated by UI Builder.
* This file will be auto-generated each and everytime you save your project.
* Do not hand edit this file.
********************************************************************************/
		#include "g_inc_uib.h"
#include "uib_views.h"
#include "uib_views_inc.h"
#include "uib_app_manager.h"


/* event handler declarations */
void connection_view6_prev6_onclicked(uib_view6_view_context*, Evas_Object*, void*);
void connection_view6_next6_onclicked(uib_view6_view_context*, Evas_Object*, void*);

uib_view_context* uib_view_view6_create(Evas_Object* parent, void* create_callback_param) {
	uib_view6_view_context* vc = calloc(1, sizeof(uib_view6_view_context));
	vc->parent = parent;
	vc->view_name = "view6";
	vc->indicator_state = ELM_WIN_INDICATOR_SHOW;
	vc->is_user_view = false;
	uib_app_manager_get_instance()->add_view_context((uib_view_context*)vc);
	if (!vc->grid3) {
		vc->grid3= elm_grid_add(parent);
		vc->root_container = vc->grid3;
	}
	uib_views_get_instance()->set_targeted_view((uib_view_context*)vc);

	//bind event handler
	evas_object_smart_callback_add(vc->prev6, "clicked", (Evas_Smart_Cb)connection_view6_prev6_onclicked, vc);

	evas_object_smart_callback_add(vc->next6, "clicked", (Evas_Smart_Cb)connection_view6_next6_onclicked, vc);



	evas_object_data_set(vc->root_container, KEY_VIEW_CONTEXT, vc);
	uib_views_create_callback(vc, evas_object_evas_get(vc->root_container), vc->root_container, create_callback_param);
	evas_object_event_callback_add(vc->root_container, EVAS_CALLBACK_DEL, (Evas_Object_Event_Cb)uib_views_destroy_callback, vc);

	return (uib_view_context*)vc;
}
void uib_view6_config_CIRCLE_360x360_portrait() {
	uib_app_manager_st* uib_app_manager = uib_app_manager_get_instance();
	uib_view6_view_context* vc = (uib_view6_view_context*)uib_app_manager->find_view_context("view6");
	if(!vc) {
		return;
	}
	if(vc->grid3) {
		elm_grid_clear(vc->grid3, EINA_FALSE);
		evas_object_size_hint_align_set(vc->grid3, -1.0, -1.0);
		if (!vc->prev6) {
			vc->prev6 = elm_button_add(vc->grid3);
		}
		if (vc->prev6) {
			evas_object_size_hint_align_set(vc->prev6, -1.0, -1.0);
			elm_object_style_set(vc->prev6,"bottom");
			evas_object_show(vc->prev6);
		}
		if (!vc->box3) {
			vc->box3 = elm_box_add(vc->grid3);
		}
		if(vc->box3) {
			elm_win_alpha_set(vc->box3, EINA_FALSE);
			elm_box_homogeneous_set(vc->box3, EINA_FALSE);
			elm_box_horizontal_set(vc->box3, EINA_FALSE);
			elm_box_padding_set(vc->box3,0,0);
			evas_object_size_hint_align_set(vc->box3, -1.0, -1.0);
			evas_object_size_hint_weight_set(vc->box3, 1.0, 1.0);
			if (!vc->label3) {
				vc->label3 = elm_label_add(vc->box3);
			}
			if(vc->label3) {
				evas_object_size_hint_align_set(vc->label3, 0.5, 0.5);
				elm_label_line_wrap_set(vc->label3, (Elm_Wrap_Type)ELM_WRAP_NONE);
				elm_label_wrap_width_set(vc->label3,0);
				elm_label_ellipsis_set(vc->label3, EINA_FALSE);
				evas_object_show(vc->label3);
			}
			elm_box_pack_end(vc->box3, vc->label3);
			evas_object_show(vc->box3);
		}
		if (!vc->next6) {
			vc->next6 = elm_button_add(vc->grid3);
		}
		if (vc->next6) {
			evas_object_size_hint_align_set(vc->next6, -1.0, -1.0);
			elm_object_style_set(vc->next6,"bottom");
			evas_object_show(vc->next6);
		}
		elm_grid_pack(vc->grid3, vc->prev6, 100, 100, 800, 80);
	}
}
