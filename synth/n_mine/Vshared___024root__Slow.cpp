// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vshared.h for the primary calling header

#include "Vshared__pch.h"

void Vshared___024root___ctor_var_reset(Vshared___024root* vlSelf);

Vshared___024root::Vshared___024root(Vshared__Syms* symsp, const char* namep)
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vshared___024root___ctor_var_reset(this);
}

void Vshared___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vshared___024root::~Vshared___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
