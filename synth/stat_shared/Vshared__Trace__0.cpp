// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals

#include "verilated_fst_c.h"
#include "Vshared__Syms.h"


void Vshared___024root__trace_chg_0_sub_0(Vshared___024root* vlSelf, VerilatedFst::Buffer* bufp);

void Vshared___024root__trace_chg_0(void* voidSelf, VerilatedFst::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vshared___024root__trace_chg_0\n"); );
    // Body
    Vshared___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vshared___024root*>(voidSelf);
    Vshared__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    Vshared___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vshared___024root__trace_chg_0_sub_0(Vshared___024root* vlSelf, VerilatedFst::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vshared___024root__trace_chg_0_sub_0\n"); );
    Vshared__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 0);
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[1U]))) {
        bufp->chgIData(oldp+0,(vlSelfRef.shared__DOT__s1),32);
        bufp->chgIData(oldp+1,(vlSelfRef.shared__DOT__s2),32);
        bufp->chgIData(oldp+2,(vlSelfRef.shared__DOT__s3),32);
        bufp->chgIData(oldp+3,((vlSelfRef.shared__DOT__s3 
                                & vlSelfRef.shared__DOT__s2)),32);
    }
    bufp->chgIData(oldp+4,(vlSelfRef.a),32);
    bufp->chgIData(oldp+5,(vlSelfRef.b),32);
    bufp->chgIData(oldp+6,(vlSelfRef.c),32);
    bufp->chgIData(oldp+7,(vlSelfRef.d),32);
    bufp->chgBit(oldp+8,(vlSelfRef.clk));
    bufp->chgIData(oldp+9,(vlSelfRef.acc),32);
    bufp->chgIData(oldp+10,((vlSelfRef.shared__DOT__s2 
                             - vlSelfRef.b)),32);
    bufp->chgIData(oldp+11,((vlSelfRef.shared__DOT__s2 
                             - (vlSelfRef.b >> 1U))),32);
    bufp->chgIData(oldp+12,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 
                             - (vlSelfRef.b >> 2U))),32);
    bufp->chgIData(oldp+13,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 
                             - vlSelfRef.b)),32);
    bufp->chgIData(oldp+14,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 
                             - (vlSelfRef.b >> 1U))),32);
    bufp->chgIData(oldp+15,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5 
                             - (vlSelfRef.b >> 2U))),32);
    bufp->chgIData(oldp+16,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 
                             - (vlSelfRef.b >> 1U))),32);
    bufp->chgIData(oldp+17,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 
                             - (vlSelfRef.b >> 2U))),32);
    bufp->chgIData(oldp+18,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 
                             - vlSelfRef.b)),32);
    bufp->chgIData(oldp+19,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5 
                             - (vlSelfRef.b >> 1U))),32);
    bufp->chgIData(oldp+20,((vlSelfRef.shared__DOT__s2 
                             - (vlSelfRef.b >> 2U))),32);
    bufp->chgIData(oldp+21,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 
                             - vlSelfRef.b)),32);
    bufp->chgIData(oldp+22,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 
                             - (vlSelfRef.b >> 1U))),32);
    bufp->chgIData(oldp+23,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 
                             - (vlSelfRef.b >> 2U))),32);
    bufp->chgIData(oldp+24,((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5 
                             - vlSelfRef.b)),32);
}

void Vshared___024root__trace_cleanup(void* voidSelf, VerilatedFst* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vshared___024root__trace_cleanup\n"); );
    // Body
    Vshared___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vshared___024root*>(voidSelf);
    Vshared__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
}
