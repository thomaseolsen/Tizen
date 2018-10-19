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

#if !defined(_DATA_H)
#define _DATA_H

typedef void (*Time_Elapse_Tick_Cb)(int start_time, int current_time);
typedef void (*Time_Elapse_End_Cb)(void);

void data_initialize(Time_Elapse_Tick_Cb time_elapse_tick_cb, Time_Elapse_End_Cb time_elapse_end_cb);
void data_finalize(void);
void data_set_time(unsigned hour, unsigned min, unsigned sec);
void data_run(void);
void data_stop(void);
void data_pause(void);

#endif
