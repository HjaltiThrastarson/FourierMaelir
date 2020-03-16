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

#include "../../include/DSPLib.h"

/*
 * Optimized helper function for right shift operations with LEA.
 */    
static inline msp_status msp_shift_right_q15(const _q15 *src, _q15 *dst, uint16_t length, uint8_t shift);

/*
 * Perform element wise left or right shift of a single source vector.
 */
msp_status msp_shift_q15(const msp_shift_q15_params *params, const _q15 *src, _q15 *dst)
{
    int8_t shift;               // Shift count
    uint16_t length;            // Shift length
    
    /* Initialize the loop counter and shift variables. */
    length = params->length;
    shift = params->shift;

#ifndef MSP_DISABLE_DIAGNOSTICS
    /* Verify the shift parameter. */
    if ((shift > 15) || (shift < -15)) {
        return MSP_SHIFT_SIZE_ERROR;
    }
#endif //MSP_DISABLE_DIAGNOSTICS

    /* Shift src array left for a positive shift parameter. */
    if (shift > 0) {  
        /* Loop through all vector elements. */
        while (length--) {
            /* Shift src left by the shift parameter and store to dst. */
            *dst++ = *src++ << shift;
        }
    }
    /* Shift src array right for a negative shift parameter. */
    else {
        /* Use optimized helper function. */
        return msp_shift_right_q15(src, dst, length, -shift);
    }
    
    return MSP_SUCCESS;
}

#if defined(MSP_USE_LEA)

/* Shift factor lookup table. */
const uint16_t msp_shift_right_factor_q15[16] = {
    0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100, 
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001
};
    
static inline msp_status msp_shift_right_q15(const _q15 *src, _q15 *dst, uint16_t length, uint8_t shift)
{
    int16_t shiftValue;
    int16_t *shiftVector;
    msp_status status;
    MSP_LEA_MPYMATRIX_PARAMS *leaParams;
    
    /* If shift is zero and source and data are not the same array copy data. */
    if ((shift == 0) && (src != dst)) {
        msp_copy_q15_params copyParams;
        copyParams.length = length;
        return msp_copy_q15(&copyParams, src, dst);
    }
    
    /* Lookup the fractional shift value. */
    shiftValue = msp_shift_right_factor_q15[shift & 0xf];

#ifndef MSP_DISABLE_DIAGNOSTICS
    /* Check that the data arrays are aligned and in a valid memory segment. */
    if (!(MSP_LEA_VALID_ADDRESS(src, 4) && MSP_LEA_VALID_ADDRESS(dst, 4))) {
        return MSP_LEA_INVALID_ADDRESS;
    }

    /* Acquire lock for LEA module. */
    if (!msp_lea_acquireLock()) {
        return MSP_LEA_BUSY;
    }
#endif //MSP_DISABLE_DIAGNOSTICS

    /* Initialize LEA if it is not enabled. */
    if (!(LEAPMCTL & LEACMDEN)) {
        msp_lea_init();
    }
        
    /* Allocate MSP_LEA_MPYMATRIX_PARAMS structure. */
    leaParams = (MSP_LEA_MPYMATRIX_PARAMS *)msp_lea_allocMemory(sizeof(MSP_LEA_MPYMATRIX_PARAMS)/sizeof(uint32_t));
        
    /* Allocate offset vector of length two. */
    shiftVector = (int16_t *)msp_lea_allocMemory(2*sizeof(int16_t)/sizeof(uint32_t));
    shiftVector[0] = shiftValue;
    shiftVector[1] = shiftValue;

    /* Set MSP_LEA_MPYLONGMATRIX_PARAMS structure. */
    leaParams->input2 = MSP_LEA_CONVERT_ADDRESS(shiftVector);
    leaParams->output = MSP_LEA_CONVERT_ADDRESS(dst);
    leaParams->vectorSize = length;
    leaParams->input1Offset = 1;
    leaParams->input2Offset = 0;
    leaParams->outputOffset = 1;

    /* Load source arguments to LEA. */
    LEAPMS0 = MSP_LEA_CONVERT_ADDRESS(src);
    LEAPMS1 = MSP_LEA_CONVERT_ADDRESS(leaParams);

    /* Invoke the LEACMD__MPYMATRIX command with interrupts enabled. */
    LEAPMCB = LEACMD__MPYMATRIX | LEAITFLG1;

    /* Clear DSPLib flags and enter LPM0. */
    msp_lea_ifg = 0;
    msp_lea_enterLPM();

    /* Free MSP_LEA_MPYMATRIX_PARAMS structure and shift vector. */
    msp_lea_freeMemory(2*sizeof(int16_t)/sizeof(uint32_t));
    msp_lea_freeMemory(sizeof(MSP_LEA_MPYMATRIX_PARAMS)/sizeof(uint32_t));
    
    /* Set status flag. */
    status = MSP_SUCCESS;
        
#ifndef MSP_DISABLE_DIAGNOSTICS
    /* Check LEA interrupt flags for any errors. */
    if (msp_lea_ifg & LEACOVLIFG) {
        status = MSP_LEA_COMMAND_OVERFLOW;
    }
    else if (msp_lea_ifg & LEAOORIFG) {
        status = MSP_LEA_OUT_OF_RANGE;
    }
    else if (msp_lea_ifg & LEASDIIFG) {
        status = MSP_LEA_SCALAR_INCONSISTENCY;
    }
#endif

    /* Free lock for LEA module and return status. */
    msp_lea_freeLock();
    return status;
}

#else //MSP_USE_LEA
    
static inline msp_status msp_shift_right_q15(const _q15 *src, _q15 *dst, uint16_t length, uint8_t shift)
{    
    /* Loop through all vector elements. */
    while (length--) {
        /* Shift src right and store to dst. */
        *dst++ = *src++ >> shift;
    }

    return MSP_SUCCESS;
}

#endif //MSP_USE_LEA
