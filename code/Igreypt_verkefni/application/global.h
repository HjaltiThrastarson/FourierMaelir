/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*
 * global.h
 *
 *  Created on: May 20, 2015
 *      Author: a0866890
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_
#define VECTOR_SIZE 256
#define LOG2_SIZE       8
#define SQRT_SIZE       16
#define SCALE_FACTOR    0x00026000
#define MAXIMUM_ARRAY_SIZE              256
#define FFT_SAMPLING_FREQUENCY          20000
#define FIR_SAMPLING_FREQUENCY          20000
#include "audio_collect.h"
#include "audio_playback.h"
#include "grlib.h"
#include "application.h"

#define __SYSTEM_FREQUENCY_MHZ__        8000000

extern Audio_configParams gAudioConfig;
extern Playback_configParams gPlaybackConfig;
extern Graphics_Context g_sContext;
extern int16_t FFT_data[VECTOR_SIZE];
extern bool keepAppRunning;
extern void * leaRamStartAddress;


#endif /* GLOBAL_H_ */
