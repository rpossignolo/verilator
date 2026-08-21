// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VRECON__SYMS_H_
#define VERILATED_VRECON__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vrecon.h"

// INCLUDE MODULE CLASSES
#include "Vrecon___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vrecon__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vrecon* const __Vm_modelp;
    bool __Vm_activity = false;  ///< Used by trace routines to determine change occurred
    uint32_t __Vm_baseCode = 0;  ///< Used by trace routines when tracing multiple models
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vrecon___024root               TOP;

    // CONSTRUCTORS
    Vrecon__Syms(VerilatedContext* contextp, const char* namep, Vrecon* modelp);
    ~Vrecon__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
