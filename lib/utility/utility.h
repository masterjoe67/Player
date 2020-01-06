#pragma once
#include "ili9341_text.h"
#include "ili9341_graph.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"
#include "tm_stm32f4_delay.h"
#include "xpt2046.h"

#define SEARCH_TIMEOUT 10000

bool searchUSB();

const char *GetFilenameExt(const char *filename);

int map(int x, int in_min, int in_max, int out_min, int out_max);
