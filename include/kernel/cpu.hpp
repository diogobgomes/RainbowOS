/**
 * @file cpu.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions of useful cpu things
 * @version 0.1
 * @date 2025-02-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>

namespace kernel::cpu
{


struct pushad_frame
{
    uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
}__attribute__((packed));

static_assert(sizeof(pushad_frame)==32);

} // namespace kernel::cpu
 