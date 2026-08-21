// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VSHARED__SYMS_H_
#define VERILATED_VSHARED__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vshared.h"

// INCLUDE MODULE CLASSES
#include "Vshared___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vshared__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vshared* const __Vm_modelp;
    bool __Vm_activity = false;  ///< Used by trace routines to determine change occurred
    uint32_t __Vm_baseCode = 0;  ///< Used by trace routines when tracing multiple models
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vshared___024root              TOP;

    // CONSTRUCTORS
    Vshared__Syms(VerilatedContext* contextp, const char* namep, Vshared* modelp);
    ~Vshared__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
