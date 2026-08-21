// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals

#include "verilated_fst_c.h"
#include "Vrecon__Syms.h"


void Vrecon___024root__trace_chg_0_sub_0(Vrecon___024root* vlSelf, VerilatedFst::Buffer* bufp);

void Vrecon___024root__trace_chg_0(void* voidSelf, VerilatedFst::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root__trace_chg_0\n"); );
    // Body
    Vrecon___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vrecon___024root*>(voidSelf);
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    Vrecon___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vrecon___024root__trace_chg_0_sub_0(Vrecon___024root* vlSelf, VerilatedFst::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root__trace_chg_0_sub_0\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 0);
    bufp->chgQData(oldp+0,(vlSelfRef.x),64);
    bufp->chgQData(oldp+2,(vlSelfRef.y),64);
    bufp->chgQData(oldp+4,(vlSelfRef.z),64);
    bufp->chgBit(oldp+6,(vlSelfRef.clk));
    bufp->chgQData(oldp+7,(vlSelfRef.o),64);
    bufp->chgQData(oldp+9,(vlSelfRef.recon__DOT__base),64);
    bufp->chgQData(oldp+11,(vlSelfRef.recon__DOT__base2),64);
}

void Vrecon___024root__trace_cleanup(void* voidSelf, VerilatedFst* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root__trace_cleanup\n"); );
    // Locals
    VlUnpacked<CData/*0:0*/, 1> __Vm_traceActivity;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        __Vm_traceActivity[__Vi0] = 0;
    }
    // Body
    Vrecon___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vrecon___024root*>(voidSelf);
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    vlSymsp->__Vm_activity = false;
    __Vm_traceActivity[0U] = 0U;
}
