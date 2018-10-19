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

/* ---------------- General defines ---------------- */
#define PART_MAIN_BG "bg"
#define PART_COLON_1 "colon1"
#define PART_COLON_2 "colon2"

#define STATE_DEFAULT "default"
#define STATE_CUSTOM "custom"
#define STATE_BTN_PRESSED "pressed"

#define FONT_STYLE_THIN "BreezeSans:style=thin"
#define FONT_SIZE_THIN 12*4
#define FONT_STYLE_MEDIUM "BreezeSans:style=medium"
#define FONT_SIZE_MEDIUM 8*4
#define FONT_STYLE_BOLD "BreezeSans:style=bold"
#define FONT_SIZE_BOLD 14*4
#define FONT_SIZE_TITLE 5*4

/* ---------------- Setup defines ---------------- */
#define PART_SETUP_BOARD_BG "setup_board_bg"
#define PART_SETUP_RESET_BUTTON "setup_reset_button"
#define PART_SETUP_START_BUTTON "setup_start_button"
#define PART_SETUP_INDICATOR_HOUR "setup_indicator_hour"
#define PART_SETUP_INDICATOR_MIN "setup_indicator_min"
#define PART_SETUP_INDICATOR_SEC "setup_indicator_sec"

#define IMAGE_FPATH_BOARD_SETUP_HOUR_BG "../res/images/setup/board_setup_hour_bg.png"
#define IMAGE_FPATH_BOARD_SETUP_MIN_SEC_BG "../res/images/setup/board_setup_min_sec_bg.png"
#define IMAGE_FPATH_INDICATOR_SETTER "../res/images/setup/indicator_setter.png"
#define IMAGE_FPATH_BUTTON_START_PRESSED "../res/images/setup/button_start_pressed.png"
#define IMAGE_FPATH_BUTTON_START "../res/images/setup/button_start.png"
#define IMAGE_FPATH_BUTTON_RESET "../res/images/setup/button_reset.png"

#define STATE_PART_SELECTED "part_selected"

/* ---------------- Run defines ---------------- */
#define IMAGE_FPATH_BOARD_RUN_BG "../res/images/run/board_run_bg.png"
#define IMAGE_FPATH_BUTTON_SET_TIME "../res/images/run/button_set_time.png"
#define IMAGE_FPATH_BUTTON_STOP "../res/images/run/button_stop.png"
#define IMAGE_FPATH_BUTTON_STOP_PRESSED "../res/images/run/button_stop_pressed.png"
#define IMAGE_FPATH_INDICATOR_RUNNER_DOT "../res/images/run/indicator_runner_dot.png"
#define IMAGE_FPATH_INDICATOR_RUNNER_LINE_GLOW "../res/images/run/indicator_runner_line_glow.png"

#define PART_RUN_BOARD_RUN_BG "run_board_run_bg"
#define PART_RUN_SET_TIME_BUTTON "run_set_time_button"
#define PART_RUN_STOP_BUTTON "run_stop_button"
#define PART_RUN_PROGRESS "run_progress"
#define PART_RUN_DOT "run_dot"
#define PART_RUN_CLIPPER "run_clipper"

#define DOT_ANGLE_START -120
#define DOT_ANGLE_END 120
#define PART_CLIPPER_REL_2_Y (260.0/360.0)

/* ---------------- Part macros ---------------- */
#define TIME_PART(part_name, txt, rel_x, width, rel_y, height, f_size, f_name) \
	part {                                                                     \
		name: part_name;                                                       \
		type: TEXT;                                                            \
		description {                                                          \
			state: STATE_DEFAULT 0.0;                                          \
			rel1 {                                                             \
				relative: rel_x rel_y;                                         \
			}                                                                  \
			rel2 {                                                             \
				relative: rel_x+width rel_y+height;                            \
			}                                                                  \
			text {                                                             \
				text: txt;                                                     \
				size: f_size;                                                  \
				font: f_name;                                                  \
			}                                                                  \
		}                                                                      \
		description {                                                          \
			state: STATE_PART_SELECTED 0.0;                                    \
			inherit:STATE_DEFAULT 0.0;                                         \
			text {                                                             \
				size: FONT_SIZE_BOLD;                                          \
				font: FONT_STYLE_BOLD;                                         \
			}                                                                  \
		}                                                                      \
	}                                                                          \

#define PART_COLON(part_name, part_rel1, part_rel2, color_r, color_g, color_b)  \
	part {                                                                      \
			name: part_name;                                                    \
			type: TEXT;                                                         \
			description {                                                       \
				state: STATE_DEFAULT 0.0;                                       \
				rel1 {                                                          \
					relative: 1.0 0.0;                                          \
					to: part_rel1;                                              \
				}                                                               \
				rel2 {                                                          \
					relative: 0.0 1.0;                                          \
					to: part_rel2;                                              \
				}                                                               \
				color: color_r color_g color_b 255;                             \
				text {                                                          \
					text: ":";                                                  \
					size: FONT_SIZE_MEDIUM;                                     \
					font: FONT_STYLE_MEDIUM;                                    \
				}                                                               \
			}                                                                   \
		}                                                                       \

#define PART_INDICATOR(part_name, color_r, color_g, color_b, width, height) \
	part {                                                                  \
		name: part_name;                                                    \
		type: IMAGE;                                                        \
		description {                                                       \
			state: STATE_DEFAULT 0.0;                                       \
			rel1 {                                                          \
				relative: 0.5-(width/2.0/360.0) 0.0694;                     \
			}                                                               \
			rel2 {                                                          \
				relative: 0.5+(width/2.0/360.0) 0.0694+(height/360.0);      \
			}                                                               \
			image {                                                         \
				normal: IMAGE_FPATH_INDICATOR_SETTER;                       \
			}                                                               \
			color: color_r color_g color_b 255;                             \
			map {                                                           \
				on: 1;                                                      \
				smooth: 1;                                                  \
				perspective_on: 1;                                          \
				rotation {                                                  \
					center: PART_MAIN_BG;                                   \
					z: 0.0;                                                 \
				}                                                           \
			}                                                               \
		}                                                                   \
	}                                                                       \
