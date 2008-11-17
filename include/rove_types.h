/**
 * This file is part of rove.
 * rove is copyright 2007, 2008 william light <visinin@gmail.com>
 *
 * rove is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rove is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rove.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ROVE_TYPES_H
#define _ROVE_TYPES_H

#include <stdint.h>
#include <lo/lo.h>
#include <jack/jack.h>
#include <sndfile.h>
#include <pthread.h>

/**
 * rove_file
 */

typedef enum {
	FILE_STATE_ACTIVE,
	FILE_STATE_INACTIVE,
	
	FILE_STATE_ACTIVATE,
	FILE_STATE_DEACTIVATE,
	FILE_STATE_RESEEK
} rove_file_state_t;

typedef enum {
	FILE_PLAY_DIRECTION_FORWARD,
	FILE_PLAY_DIRECTION_REVERSE
} rove_file_play_direction_t;

typedef struct rove_group {
	int idx;
	struct rove_file *active_loop;
} rove_group_t;

typedef struct rove_file {
	rove_file_play_direction_t play_direction;
	rove_file_state_t state;
	
	double volume;
	
	sf_count_t length;
	sf_count_t play_offset;
	sf_count_t loop_offset;
	
	sf_count_t channels;
	sf_count_t file_length;
	sf_count_t sample_rate;
	float *file_data;
	
	sf_count_t new_offset;
	
	uint8_t y;
	uint8_t row_span;
	
	uint8_t monome_pos;             /* position on the monome is represented as a 1-byte unsigned char*/
	uint8_t monome_pos_old;         /* the upper 4 bits (>> 4) are the X position */
	uint8_t force_monome_update;    /* and the lower 4 bits (& 0x0F) are the Y position */
	
	rove_group_t *group;
} rove_file_t;

/**
 * rove_list
 */

typedef enum {
	HEAD,
	TAIL
} rove_list_global_location_t;

typedef enum {
	BEFORE,
	AFTER
} rove_list_local_location_t;

typedef struct rove_list_member {
	void *data;
	
	struct rove_list_member *prev;
	struct rove_list_member *next;
} rove_list_member_t;

typedef struct rove_list {
	rove_list_member_t *head;
	rove_list_member_t *tail;
} rove_list_t;

/**
 * rove_monome
 */

struct rove_state;
typedef void *(*rove_monome_callback_function_t)(struct rove_state *, const uint8_t x, const uint8_t y, const uint8_t mod_keys, void *arg);

typedef struct {
	rove_monome_callback_function_t cb;
	void *arg;
} rove_monome_callback_t;

typedef struct rove_monome {
	lo_server_thread *st;
	lo_address *outgoing;
	
	rove_monome_callback_t *callbacks;
} rove_monome_t;

/**
 * rove_pattern
 */

typedef enum {
	CMD_GROUP_DEACTIVATE,
	CMD_LOOP_SEEK
} rove_pattern_cmd_t;

typedef enum {
	PATTERN_STATUS_RECORDING,
	PATTERN_STATUS_ACTIVE,
	PATTERN_STATUS_ACTIVATE,
	PATTERN_STATUS_INACTIVE
} rove_pattern_status_t;

typedef struct rove_pattern_step {
	jack_nframes_t delay;
	rove_pattern_cmd_t cmd;
	rove_file_t *file;
	jack_nframes_t arg;
} rove_pattern_step_t;

typedef struct rove_pattern {
	uint8_t idx;

	rove_list_t *steps;
	rove_list_member_t *current_step;
	
	jack_nframes_t delay_frames;
	rove_pattern_status_t status;
} rove_pattern_t;

/**
 * rove
 */

typedef struct rove_state {
	rove_monome_t *monome;
	jack_client_t *client;
	
	uint8_t group_count;
	rove_group_t *groups;
	
	rove_list_t *files;
	rove_list_t *patterns;
	rove_list_member_t *pattern_rec;
	uint8_t pattern_lengths[2];

	uint8_t staged_loops;
	rove_list_t *active;
	rove_list_t *staging;
	
	pthread_mutex_t monome_mutex;
	pthread_cond_t monome_display_notification;

	uint8_t active_loops;

	double bpm;
	double beat_multiplier;

	jack_nframes_t snap_delay;
	jack_nframes_t frames;
} rove_state_t;

#endif