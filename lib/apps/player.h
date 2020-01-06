#pragma once



#define modeMP3		4
#define modeMP3sel	5

#define btn_pgup_ret		0x20
#define btn_pgdown_ret		0x21
#define btn_exit_ret		0x0B
#define btn_song_ret_1		0x30
#define btn_song_ret_2		0x31
#define btn_song_ret_3		0x32
#define btn_song_ret_4		0x33
#define btn_song_ret_5		0x34
#define btn_song_ret_6		0x35
#define btn_play_ret		0x40
#define btn_incvol_ret		0x50
#define btn_decvol_ret		0x51
#define btn_mute_ret		0x52
#define btn_prev_ret		0x53
#define btn_next_ret		0x54

#define ret_toList			0x01
#define ret_prev			0x02
#define ret_next			0x03

FRESULT mediaplayer();
FRESULT IndexFileNames();

uint8_t playerControl();