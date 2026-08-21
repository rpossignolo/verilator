// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vrecon.h for the primary calling header

#include "Vrecon__pch.h"

void Vrecon___024root___ctor_var_reset(Vrecon___024root* vlSelf);

Vrecon___024root::Vrecon___024root(Vrecon__Syms* symsp, const char* namep)
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vrecon___024root___ctor_var_reset(this);
}

void Vrecon___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vrecon___024root::~Vrecon___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
