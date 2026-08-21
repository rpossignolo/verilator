// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vrecon.h for the primary calling header

#ifndef VERILATED_VRECON___024ROOT_H_
#define VERILATED_VRECON___024ROOT_H_  // guard

#include "verilated.h"


class Vrecon__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vrecon___024root final {
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
    IData/*31:0*/ __VactIterCount;
    VL_IN64(x,63,0);
    VL_IN64(y,63,0);
    VL_IN64(z,63,0);
    VL_OUT64(o,63,0);
    QData/*63:0*/ recon__DOT__base;
    QData/*63:0*/ recon__DOT__base2;
    QData/*63:0*/ __Vtrigprevexpr___TOP__x__0;
    QData/*63:0*/ __Vtrigprevexpr___TOP__y__0;
    QData/*63:0*/ __Vtrigprevexpr___TOP__z__0;
    VlUnpacked<QData/*63:0*/, 128> recon__DOT__taps;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 2> __VicoTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vrecon__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vrecon___024root(Vrecon__Syms* symsp, const char* namep);
    ~Vrecon___024root();
    VL_UNCOPYABLE(Vrecon___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
