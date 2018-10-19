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

#include <Elementary.h>
#include <device/haptic.h>
#include "data.h"
#include "samplerotarytimer.h"

#define VIBRATION_TIME 2000
#define VIBRATION_INTENSITY 100

struct data_callbacks_s {
	Time_Elapse_Tick_Cb time_elapse_tick_cb;
	Time_Elapse_End_Cb time_elapse_end_cb;
};

static struct data_info {
	int time;
	int start_time;
	Ecore_Timer *second_timer;
	struct data_callbacks_s callbacks;
	bool elapsing_end;
	haptic_device_h haptic_device;
} s_info = {
	.time = 0,
	.start_time = 0,
	.second_timer = NULL,
	.elapsing_end = false,
	.haptic_device = NULL,
	.callbacks = {
		.time_elapse_tick_cb = NULL,
		.time_elapse_end_cb = NULL,
	},
};

static Eina_Bool _second_timer_update_cb(void *data);
static void _haptic_device_start_vibration(void);

/**
 * @brief Initialization function for data module.
 * Within this function haptic device is opened and data callback functions are set.
 * @param[in] time_elapse_tick_cb The callback function invoked with 1 second interval
 * when the timer is in run mode and preset time elapses.
 * @param[in] time_elapse_end_cb The callback function invoked when the preset time elapsed.
 */
void data_initialize(Time_Elapse_Tick_Cb time_elapse_tick_cb, Time_Elapse_End_Cb time_elapse_end_cb)
{
	int error = DEVICE_ERROR_NONE;

	s_info.callbacks.time_elapse_tick_cb = time_elapse_tick_cb;
	s_info.callbacks.time_elapse_end_cb = time_elapse_end_cb;

	error = device_haptic_open(0, &s_info.haptic_device);
	if (error != DEVICE_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d]  Haptic device error == %s", __FILE__, __LINE__, get_error_message(error));
}

/**
 * @brief Finalization function for data module.
 * Within this function the haptic device is closed.
 */
void data_finalize(void)
{
	int error = DEVICE_ERROR_NONE;

	error = device_haptic_close(s_info.haptic_device);
	if (error != DEVICE_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d]  Haptic device error == %s", __FILE__, __LINE__, get_error_message(error));
}

/**
 * @brief Sets the timer's initial time.
 * @param[in] hour The hour component of initial time.
 * @param[in] min The minute component of initial time.
 * @param[in] sec The second component of initial time.
 */
void data_set_time(unsigned hour, unsigned min, unsigned sec)
{
	if (s_info.second_timer) {
		ecore_timer_del(s_info.second_timer);
		s_info.second_timer = NULL;
	}

	s_info.time = (hour * 3600) + (min * 60) + sec;
	s_info.start_time = s_info.time;
}

/**
 * @brief Starts the time elapsing.
 */
void data_run(void)
{
	if (s_info.second_timer) {
		ecore_timer_thaw(s_info.second_timer);
	} else {
		if (s_info.callbacks.time_elapse_tick_cb)
			s_info.callbacks.time_elapse_tick_cb(s_info.start_time, s_info.time);

		s_info.second_timer = ecore_timer_add(1.0, _second_timer_update_cb, NULL);
	}
}

/**
 * @brief pause the timer
 */
void data_pause(void)
{
	ecore_timer_freeze(s_info.second_timer);
}

/**
 * @brief Stops the time elapsing.
 */
void data_stop(void)
{
	s_info.elapsing_end = false;

	if (s_info.callbacks.time_elapse_tick_cb)
		s_info.callbacks.time_elapse_tick_cb(s_info.start_time, s_info.start_time);

	if (s_info.callbacks.time_elapse_end_cb)
		s_info.callbacks.time_elapse_end_cb();

	s_info.time = s_info.start_time;

	if (s_info.second_timer) {
		ecore_timer_del(s_info.second_timer);
		s_info.second_timer = NULL;
	}
}

/**
 * @brief Internal callback function invoked with one second interval.
 * It is responsible for remaining time count down.
 * @param[in] data The user data passed to the ecore_timer_add() function.
 * @return Return ECORE_CALLBACK_RENEW to continue time measurement,
 * otherwise ECORE_CALLBACK_CANCEL shall be returned.
 */
static Eina_Bool _second_timer_update_cb(void *data)
{
	s_info.time--;

	if (s_info.elapsing_end) {
		data_stop();
		return ECORE_CALLBACK_CANCEL;
	}

	if (s_info.callbacks.time_elapse_tick_cb)
		s_info.callbacks.time_elapse_tick_cb(s_info.start_time, s_info.time);

	if (s_info.time == 0) {
		s_info.elapsing_end = true;
		_haptic_device_start_vibration();
	}

	return ECORE_CALLBACK_RENEW;
}

/**
 * @brief Internal function which causes the haptic to vibrate.
 * This function is called when the preset time had just elapsed.
 */
static void _haptic_device_start_vibration(void)
{
	int error = DEVICE_ERROR_NONE;

	error = device_haptic_vibrate(s_info.haptic_device, VIBRATION_TIME, VIBRATION_INTENSITY, NULL);
	if (error != DEVICE_ERROR_NONE)
		dlog_print(DLOG_INFO, LOG_TAG, "[%s:%d]  Haptic device error == %s", __FILE__, __LINE__, get_error_message(error));
}
