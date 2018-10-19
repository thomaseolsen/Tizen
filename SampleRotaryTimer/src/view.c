/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "view.h"
#include "data.h"
#include "samplerotarytimer.h"
#include "view_defines.h"

#define TIMER_SETUP_EDJ "/edje/main.edj"
#define TIMER_RUN_EDJ "/edje/run.edj"

enum timer_selected_part_s {
	SEL_NONE = 0,
	SEL_HOUR,
	SEL_MIN,
	SEL_SEC,
};

typedef enum timer_selected_part_s timer_selected_part_t;

struct timer_val_s {
	int hour;
	int minute;
	int second;
};

typedef struct timer_val_s timer_val_t;

static struct view_info {
	Evas_Object *win;
	Evas_Object *layout_setup;
	Evas_Object *layout_run;
	timer_val_t displayed_time;
	timer_selected_part_t current_part;
} s_info = {
	.win = NULL,
	.layout_setup = NULL,
	.layout_run = NULL,
	.displayed_time = {
		.hour = 0,
		.minute = 0,
		.second = 0,
	},
	.current_part = SEL_NONE,
};

static void _layout_back_cb(void *data, Evas_Object *obj, void *event_info);
static void _win_delete_request_cb(void *data , Evas_Object *obj , void *event_info);
static Eina_Bool _rotary_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *rotary_info);
static void _selected_time_display_part_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _set_time_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _reset_timer_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _switch_view_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static const char *_create_resource_path(const char *file_name);
static void _init_rotary(void);
static void _set_time_value(int *part_value, bool inc, int max);
static void _set_selected_part_displayed_value(Evas_Object *current_layout, char *part, unsigned value);
static void _set_board_state(void);
static void _set_displayed_time_value(Evas_Object *layout);
static bool _check_string_equality(const char *s1, const char *s2);

/**
 * @brief Create Essential Object window and layout
 */
Eina_Bool view_create(void)
{
	/* Create window */
	s_info.win = view_create_win(PACKAGE);
	if (s_info.win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a window.");
		return EINA_FALSE;
	}

	s_info.layout_setup = view_create_layout_for_win(s_info.win, _create_resource_path(TIMER_SETUP_EDJ), GRP_SETUP);
	elm_layout_signal_callback_add(s_info.layout_setup, SIGNAL_PART_SELECTED, PART_HOUR, _selected_time_display_part_cb, NULL);
	elm_layout_signal_callback_add(s_info.layout_setup, SIGNAL_PART_SELECTED, PART_MINUTE, _selected_time_display_part_cb, NULL);
	elm_layout_signal_callback_add(s_info.layout_setup, SIGNAL_PART_SELECTED, PART_SECOND, _selected_time_display_part_cb, NULL);
	elm_layout_signal_callback_add(s_info.layout_setup, SIGNAL_RESET_TIMER, "", _reset_timer_cb, NULL);

	s_info.layout_run = view_create_layout_for_win(s_info.win, _create_resource_path(TIMER_RUN_EDJ), GRP_RUN);
	elm_layout_signal_callback_add(s_info.layout_run, SIGNAL_SET_TIMER, "", _set_time_cb, NULL);

	_init_rotary();

	evas_object_show(s_info.layout_setup);

	/* Show window after main view is set up */
	evas_object_show(s_info.win);

	return EINA_TRUE;
}

/**
 * @brief Make a basic window named package
 * @param[in] pkg_name Name of the window
 */
Evas_Object *view_create_win(const char *pkg_name)
{
	Evas_Object *win = NULL;

	/*
	 * Window
	 * Create and initialize elm_win.
	 * elm_win is mandatory to manipulate window
	 */
	win = elm_win_util_standard_add(pkg_name, pkg_name);
	elm_win_conformant_set(win, EINA_TRUE);
	elm_win_autodel_set(win, EINA_TRUE);

	/* Rotation setting */
	if (elm_win_wm_rotation_supported_get(win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(win, "delete,request", _win_delete_request_cb, NULL);

	return win;
}

/**
 * @brief Internal function which creates a layout object and adds it to the main window.
 * @param[in] win The main window.
 * @param[in] file_path Path to the layout file.
 * @param[in] group_name The group name to be set to the created layout.
 * @return The newly created layout object or NULL if the function fails.
 */
Evas_Object *view_create_layout_for_win(Evas_Object *win, const char *file_path, const char *group_name)
{
	Evas_Object *layout = NULL;

	if (win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] parent is NULL.", __FILE__, __LINE__);
		return NULL;
	}

	layout = elm_layout_add(win);
	if (layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] layout is NULL.", __FILE__, __LINE__);
		return NULL;
	}

	elm_layout_file_set(layout, file_path, group_name);
	elm_layout_signal_callback_add(layout, SIGNAL_VIEW_CHANGE, "", _switch_view_cb, NULL);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, layout);
	eext_object_event_callback_add(layout, EEXT_CALLBACK_BACK, _layout_back_cb, NULL);

	return layout;
}

/**
 * @brief Destroys window and frees its resources.
 */
void view_destroy(void)
{
	if (s_info.win == NULL)
		return;

	evas_object_del(s_info.win);
}

/**
 * @brief Register rotary event callback function.
 * @param[in] obj object that will receive rotary event
 * @param[in] rotary_cb function will be operated when rotary event happens
 * @param[in] user_data Data passed to the 'rotary_cb' function
 */
void view_set_rotary_event_callback(Evas_Object *obj, Eext_Rotary_Event_Cb rotary_cb, void *user_data)
{
	if (obj == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] parent is NULL.", __FILE__, __LINE__);
		return;
	}

	eext_rotary_object_event_activated_set(obj, EINA_TRUE);
	eext_rotary_object_event_callback_add(obj, rotary_cb, user_data);
}

/**
 * @brief Updates the displayed time.
 * @param[in] start_value The initial time value in seconds.
 * @param[in] current_value The current time value in seconds.
 */
void view_update_time_display(int start_value, int current_value)
{
	Edje_Message_Int_Set *msg = calloc(1, sizeof(Edje_Message_Int_Set) + 1 * sizeof(int));
	if (!msg) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] msg == NULL", __FILE__, __LINE__);
		return;
	}

	s_info.displayed_time.hour = current_value / 3600;
	s_info.displayed_time.minute = (current_value - (s_info.displayed_time.hour * 3600)) / 60;
	s_info.displayed_time.second = current_value - (s_info.displayed_time.hour * 3600) - (s_info.displayed_time.minute * 60);

	_set_displayed_time_value(s_info.layout_run);

	msg->count = 2;
	msg->val[0] = current_value;
	msg->val[1] = start_value;

	edje_object_message_send(elm_layout_edje_get(s_info.layout_run), EDJE_MESSAGE_INT_SET, MSG_ID_DOT_ANGLE, msg);
	free(msg);
}

/**
 * @brief Finishes the time count down and switches the view to the setup view.
 */
void view_end_elapsing(void)
{
	_switch_view_cb(NULL, s_info.layout_run, NULL, NULL);
}

/**
 * @brief Callback function to be invoked on hardware back button press.
 * @param[in] data Custom data passed to the view_create_layout_for_conformant() function.
 * @param[in] obj Object to which the handler is attached.
 * @param[in] event_info The information on the event occurred.
 */
static void _layout_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	elm_win_lower(s_info.win);
}

/**
 * @brief Callback function to be invoked on main window close.
 * @param[in] data Custom data passed to the callback attachment function.
 * @param[in] obj Object to which the handler is attached.
 * @param[in] event_info The information on the event occurred.
 */
static void _win_delete_request_cb(void *data , Evas_Object *obj , void *event_info)
{
	ui_app_exit();
}

/**
 * @brief The callback function responsible for handling device's ring rotation event.
 * @param[in] data Custom data passed to the callback attachment function.
 * @param[in] obj Object to which the handler is attached.
 * @param[in] rotary_info The information on the event occurred.
 */
static Eina_Bool _rotary_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *rotary_info)
{
	bool inc = false;
	int max = 59;

	inc = (rotary_info->direction == EEXT_ROTARY_DIRECTION_COUNTER_CLOCKWISE);

	if (s_info.displayed_time.hour == 24)
		max = 0;

	switch (s_info.current_part) {
	case SEL_HOUR:
		_set_time_value(&s_info.displayed_time.hour, inc, 24);

		if (s_info.displayed_time.hour == 24) {
			s_info.displayed_time.minute = 0;
			s_info.displayed_time.second = 0;
			_set_displayed_time_value(s_info.layout_setup);
		} else {
			_set_selected_part_displayed_value(s_info.layout_setup, PART_HOUR, s_info.displayed_time.hour);
		}
		break;
	case SEL_MIN:
		_set_time_value(&s_info.displayed_time.minute, inc, max);
		_set_selected_part_displayed_value(s_info.layout_setup, PART_MINUTE, s_info.displayed_time.minute);
		break;
	case SEL_SEC:
		_set_time_value(&s_info.displayed_time.second, inc, max);
		_set_selected_part_displayed_value(s_info.layout_setup, PART_SECOND, s_info.displayed_time.second);
		break;
	case SEL_NONE:
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] No part selected.", __FILE__, __LINE__);
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] Unused part type.", __FILE__, __LINE__);
	}

	_set_board_state();
	data_set_time(s_info.displayed_time.hour, s_info.displayed_time.minute, s_info.displayed_time.second);

	return ECORE_CALLBACK_PASS_ON;
}

/**
 * @brief Internal callback function responsible for reacting to timer's time component selection.
 * This callback function is invoked when SIGNAL_PART_SELECTED signal is received.
 * @param[in] data The user data passed to the elm_layout_signal_callback_add() function.
 * @param[in] obj The object emitting the signal.
 * @param[in] emission The emitted signal name.
 * @oaram[in] source The part name emitting the signal.
 */
static void _selected_time_display_part_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	if (_check_string_equality(source, PART_HOUR))
		s_info.current_part = SEL_HOUR;
	else if (_check_string_equality(source, PART_MINUTE))
		s_info.current_part = SEL_MIN;
	else
		s_info.current_part = SEL_SEC;

	_set_board_state();
}

/**
 * @brief Callback invoked when the 'Set time button is clicked'
 * @param[in] data User data
 * @param[in] obj The layout object
 * @param[in] emission The emitted signal
 * @param[in] source The emission source
 */
static void _set_time_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	data_stop();
}

/**
 * @brief Internal callback function responsible for resetting the timer's settings.
 * This callback function is invoked when SIGNAL_RESET_TIMER signal is received.
 * @param[in] data The user data passed to the elm_layout_signal_callback_add() function.
 * @param[in] obj The object emitting the signal.
 * @param[in] emission The emitted signal name.
 * @oaram[in] source The part name emitting the signal.
 */
static void _reset_timer_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	s_info.displayed_time.hour = 0;
	s_info.displayed_time.minute = 0;
	s_info.displayed_time.second = 0;

	_set_displayed_time_value(s_info.layout_setup);

	data_set_time(0, 0, 0);
}

/**
 * @brief Internal callback function responsible for switching between 'setup' and 'run' view.
 * This callback function is invoked when SIGNAL_VIEW_CHANGE signal is received.
 * @param[in] data The user data passed to the elm_layout_signal_callback_add() function.
 * @param[in] obj The object emitting the signal.
 * @param[in] emission The emitted signal name.
 * @oaram[in] source The part name emitting the signal.
 */
static void _switch_view_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	Evas_Object *prev = NULL;
	Evas_Object *next = NULL;

	if (obj == s_info.layout_setup) {
		prev = s_info.layout_setup;
		next = s_info.layout_run;

		if (s_info.displayed_time.hour == 0 && s_info.displayed_time.minute == 0 && s_info.displayed_time.second == 0)
			return;

		_set_displayed_time_value(s_info.layout_run);

		data_run();

	} else {
		prev = s_info.layout_run;
		next = s_info.layout_setup;

		_set_displayed_time_value(s_info.layout_setup);
		_set_board_state();

		if (s_info.displayed_time.hour > 0 || s_info.displayed_time.minute > 0 || s_info.displayed_time.second > 0)
			data_pause();
	}

	evas_object_hide(prev);
	evas_object_show(next);
}

/**
 * @brief Creates path to the given resource file by concatenation of the basic resource path and the given file_name.
 * @param[in] file_name File name or path relative to the resource directory.
 * @return The absolute path to the resource with given file_name is returned.
 */
static const char *_create_resource_path(const char *file_name)
{
	static char res_path_buff[PATH_MAX] = {0,};
	char *res_path = NULL;

	res_path = app_get_resource_path();
	if (res_path == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] failed to get resource path.", __FILE__, __LINE__);
		return NULL;
	}

	snprintf(res_path_buff, PATH_MAX, "%s%s", res_path, file_name);
	free(res_path);

	return &res_path_buff[0];
}

/**
 * @brief Initialization of the device's ring rotation feature by rotary callback function attachment.
 */
static void _init_rotary(void)
{
	view_set_rotary_event_callback(s_info.layout_setup, _rotary_cb, NULL);
}

/**
 * @brief Internal function increments/decrements the value of timer's time component.
 * @param[in] part_value The value displayed and to be altered.
 * @param[in] inc Time increment direction flag.
 * @param[in] max The maximum settable value.
 */
static void _set_time_value(int *part_value, bool inc, int max)
{
	if (inc && *part_value == max)
		return;
	else if (!inc && *part_value == 0)
		return;

	if (inc == true)
		(*part_value)++;
	else
		(*part_value)--;
}

/**
 * @brief Internal function displays the timer's time component value in relevant part.
 * @param[in] current_layout The currently visible layout object.
 * @param[in] part The name of the part to be updated.
 * @param[in] value The value to be displayed in the part.
 */
static void _set_selected_part_displayed_value(Evas_Object *current_layout, char *part, unsigned value)
{
	Eina_Stringshare *txt = NULL;

	if (!s_info.layout_setup) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.layout == NULL", __FILE__, __LINE__);
		return;
	}

	txt = eina_stringshare_printf("%.2d", value);
	elm_layout_text_set(current_layout, part, txt);
	eina_stringshare_del(txt);
}

/**
 * @brief Internal function sends a message to the EDJE script to set the valid position
 * for timer's time indicators as well as the rotation of the setup board.
 */
static void _set_board_state(void)
{
	int msg_id = MSG_ID_SET_HOUR;
	Edje_Message_Int_Set *msg = NULL;

	if (!s_info.layout_setup) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.layout == NULL", __FILE__, __LINE__);
		return;
	}

	switch (s_info.current_part) {
	case SEL_HOUR:
		msg_id = MSG_ID_SET_HOUR;
		break;
	case SEL_MIN:
		msg_id = MSG_ID_SET_MINUTE;
		break;
	case SEL_SEC:
		msg_id = MSG_ID_SET_SECOND;
		break;
	default:
		msg_id = MSG_ID_SET_HOUR;
		break;
	}

	msg = calloc(1, sizeof(Edje_Message_Int_Set) + (sizeof(int) * 2));
	if (!msg) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] msg == NULL", __FILE__, __LINE__);
		return;
	}

	msg->count = 3;
	msg->val[0] = s_info.displayed_time.hour;
	msg->val[1] = s_info.displayed_time.minute;
	msg->val[2] = s_info.displayed_time.second;

	edje_object_message_send(elm_layout_edje_get(s_info.layout_setup), EDJE_MESSAGE_INT_SET, msg_id, msg);
	free(msg);
}

/**
 * @brief Internal function responsible for time components value display.
 * @param[in] layout The layout object where the values are displayed.
 */
static void _set_displayed_time_value(Evas_Object *layout)
{
	_set_selected_part_displayed_value(layout, PART_HOUR, s_info.displayed_time.hour);
	_set_selected_part_displayed_value(layout, PART_MINUTE, s_info.displayed_time.minute);
	_set_selected_part_displayed_value(layout, PART_SECOND, s_info.displayed_time.second);
}

/**
 * @brief Internal function which checks whether two strings are equal.
 * @param[in] s1 The first string to compare with.
 * @param[in] s2 The second string being compared.
 * @return The function returns 'true' if both strings are equal,
 * otherwise 'false' is returned.
 */
static bool _check_string_equality(const char *s1, const char *s2)
{
	int len1 = strlen(s1);

	return (strncmp(s1, s2, len1) == 0 && len1 == strlen(s2));
}
