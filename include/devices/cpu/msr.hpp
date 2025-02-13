/**
 * @file msr.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Functions for the MSR (model specific register)
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>

namespace kernel::cpu
{
    static const int IA32_APIC_BASE_MSR =       0x1b;
    static const int IA32_APIC_BASE_MSR_BPS =   0x100; // Processor is bootstrap
    static const int IA32_APIC_BASE_MSR_ENABLE =0x800;

    /**
     * @brief Check if MSR is there (using CPUID)
     * 
     * @return true 
     * @return false 
     */
    bool checkMSR();

    //TODO Maybe change low and high to only one, combine in function?
    /**
     * @brief Get the MSR
     * 
     * @param msr MSR to get
     * @param low Low part
     * @param high High part
     */
    void getMSR(uint32_t msr, uint32_t *low, uint32_t *high);

    /**
     * @brief Set the MSR
     * 
     * @param msr MSR to set
     * @param low Low part
     * @param high High part
     */
    void setMSR(uint32_t msr, uint32_t low, uint32_t high);
} // namespace kernel::cpu
