//
// Created by moose on 30/03/19.
//

#ifndef CITYSIM_CONSTANTS_H
#define CITYSIM_CONSTANTS_H

#include <cstdint>

#define BASE_Z_OFFSET 0.0f
#define Z_FAR 4500.0f
#define Z_NEAR 0.01f

#define VTX_TYPE float
#define IDX_TYPE unsigned short


#define VTX_SIZE (VTX_POS_SIZE + VTX_COLOR_SIZE /*+ VTX_NORM_SIZE*/)
#define VTX_STRIDE (VTX_SIZE * sizeof(VTX_TYPE))
#define VTX_BYTE_SIZE VTX_STRIDE

#define VTX_POS_TYPE float
#define VTX_POS_SIZE 3
#define VTX_POS_OFFSET 0
#define VTX_POS_BYTE_SIZE (VTX_POS_SIZE * sizeof(VTX_POS_TYPE))

#define VTX_NORM_TYPE float
#define VTX_NORM_SIZE 3
#define VTX_NORM_OFFSET VTX_POS_BYTE_SIZE
#define VTX_NORM_BYTE_SIZE (VTX_NORM_SIZE * sizeof(VTX_NORM_TYPE))

#define VTX_COLOR_TYPE float
#define VTX_COLOR_SIZE 4
#define VTX_COLOR_BYTE_SIZE (VTX_COLOR_SIZE * sizeof(VTX_COLOR_TYPE))
#define VTX_COLOR_OFFSET (VTX_POS_OFFSET + VTX_NORM_OFFSET)

#define VTX_IDX_SIZE 1
#define VTX_IDX_BYTE_SIZE sizeof(IDX_TYPE)
#define VTX_IDX_RESTART_ID (USHRT_MAX-1)

#endif //CITYSIM_CONSTANTS_H
