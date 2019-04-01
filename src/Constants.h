//
// Created by moose on 30/03/19.
//

#ifndef CITYSIM_CONSTANTS_H
#define CITYSIM_CONSTANTS_H

#include <cstdint>

#define VTX_TYPE float
#define IDX_TYPE unsigned short

#define VTX_POS_SIZE 3
#define VTX_COLOR_SIZE 4
#define VTX_SIZE (VTX_POS_SIZE + VTX_COLOR_SIZE)
#define VTX_STRIDE (VTX_SIZE * sizeof(VTX_TYPE))
#define VTX_BYTE_SIZE VTX_STRIDE
#define VTX_IDX_SIZE 1
#define VTX_IDX_BYTE_SIZE sizeof(IDX_TYPE)
#define VTX_IDX_RESTART_ID (USHRT_MAX)

#endif //CITYSIM_CONSTANTS_H
