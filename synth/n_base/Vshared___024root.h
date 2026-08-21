// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vshared.h for the primary calling header

#ifndef VERILATED_VSHARED___024ROOT_H_
#define VERILATED_VSHARED___024ROOT_H_  // guard

#include "verilated.h"


class Vshared__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vshared___024root final {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __VicoDidInit;
    CData/*0:0*/ __VicoPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__1;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    VL_IN(a,31,0);
    VL_IN(b,31,0);
    VL_IN(c,31,0);
    VL_IN(d,31,0);
    VL_OUT(acc,31,0);
    IData/*31:0*/ shared__DOT__s1;
    IData/*31:0*/ shared__DOT__s2;
    IData/*31:0*/ shared__DOT__s3;
    IData/*31:0*/ __VdfgRegularize_h6e95ff9d_0_2;
    IData/*31:0*/ __VdfgRegularize_h6e95ff9d_0_3;
    IData/*31:0*/ __VdfgRegularize_h6e95ff9d_0_4;
    IData/*31:0*/ __VdfgRegularize_h6e95ff9d_0_5;
    IData/*31:0*/ __Vtrigprevexpr___TOP__a__0;
    IData/*31:0*/ __Vtrigprevexpr___TOP__b__0;
    IData/*31:0*/ __Vtrigprevexpr___TOP__c__0;
    IData/*31:0*/ __Vtrigprevexpr___TOP__d__0;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 2> __VicoTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlUnpacked<CData/*0:0*/, 2> __Vm_traceActivity;

    // INTERNAL VARIABLES
    Vshared__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vshared___024root(Vshared__Syms* symsp, const char* namep);
    ~Vshared___024root();
    VL_UNCOPYABLE(Vshared___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
