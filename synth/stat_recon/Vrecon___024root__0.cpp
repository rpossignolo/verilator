// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vrecon.h for the primary calling header

#include "Vrecon__pch.h"

bool Vrecon___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 2> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___trigger_anySet__ico\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((2U > n));
    return (0U);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vrecon___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vrecon___024root___eval_phase__ico(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_phase__ico\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VicoExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__ico
        vlSelfRef.__VicoTriggered[0U] = (QData)((IData)(
                                                        (((((IData)(vlSelfRef.clk) 
                                                            != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0)) 
                                                           << 3U) 
                                                          | ((vlSelfRef.z 
                                                              != vlSelfRef.__Vtrigprevexpr___TOP__z__0) 
                                                             << 2U)) 
                                                         | (((vlSelfRef.y 
                                                              != vlSelfRef.__Vtrigprevexpr___TOP__y__0) 
                                                             << 1U) 
                                                            | (vlSelfRef.x 
                                                               != vlSelfRef.__Vtrigprevexpr___TOP__x__0)))));
        vlSelfRef.__Vtrigprevexpr___TOP__x__0 = vlSelfRef.x;
        vlSelfRef.__Vtrigprevexpr___TOP__y__0 = vlSelfRef.y;
        vlSelfRef.__Vtrigprevexpr___TOP__z__0 = vlSelfRef.z;
        vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
        if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VicoDidInit)))))) {
            vlSelfRef.__VicoDidInit = 1U;
            vlSelfRef.__VicoTriggered[0U] = (1ULL | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (2ULL | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (4ULL | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (8ULL | vlSelfRef.__VicoTriggered[0U]);
        }
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vrecon___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    __VicoExecute = Vrecon___024root___trigger_anySet__ico(vlSelfRef.__VicoTriggered);
    if (__VicoExecute) {
        {
            // Inlined CFunc: _eval_ico
            if ((7ULL & vlSelfRef.__VicoTriggered[0U])) {
                {
                    // Inlined CFunc: _ico_comb__TOP__0
                    IData/*31:0*/ __Vinline_0__eval_ico___Vinline_1__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i;
                    __Vinline_0__eval_ico___Vinline_1__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i = 0;
                    vlSelfRef.recon__DOT__base = ((vlSelfRef.x 
                                                   ^ vlSelfRef.y) 
                                                  & (vlSelfRef.y 
                                                     | vlSelfRef.z));
                    vlSelfRef.recon__DOT__base2 = (
                                                   (vlSelfRef.x 
                                                    + vlSelfRef.y) 
                                                   - 
                                                   (vlSelfRef.z 
                                                    << 1U));
                    __Vinline_0__eval_ico___Vinline_1__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i = 0U;
                    while (VL_GTS_III(32, 0x00000080U, __Vinline_0__eval_ico___Vinline_1__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i)) {
                        vlSelfRef.recon__DOT__taps[(0x0000007fU 
                                                    & __Vinline_0__eval_ico___Vinline_1__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i)] 
                            = ((vlSelfRef.recon__DOT__base 
                                ^ VL_SHIFTR_QQI(64,64,32, vlSelfRef.recon__DOT__base2, 
                                                VL_MODDIVS_III(32, __Vinline_0__eval_ico___Vinline_1__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i, (IData)(7U)))) 
                               ^ (((QData)((IData)(vlSelfRef.recon__DOT__base)) 
                                   << 0x00000020U) 
                                  | (QData)((IData)(
                                                    (vlSelfRef.recon__DOT__base2 
                                                     >> 0x20U)))));
                        __Vinline_0__eval_ico___Vinline_1__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i 
                            = ((IData)(1U) + __Vinline_0__eval_ico___Vinline_1__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i);
                    }
                }
            }
        }
    }
    return (__VicoExecute);
}

bool Vrecon___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vrecon___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vrecon___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vrecon___024root___eval_phase__act(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_phase__act\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__act
        vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                        ((IData)(vlSelfRef.clk) 
                                                         & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__1)))));
        vlSelfRef.__Vtrigprevexpr___TOP__clk__1 = vlSelfRef.clk;
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vrecon___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vrecon___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    return (0U);
}

void Vrecon___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vrecon___024root___eval_phase__nba(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_phase__nba\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vrecon___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        {
            // Inlined CFunc: _eval_nba
            if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
                {
                    // Inlined CFunc: _nba_sequent__TOP__0
                    vlSelfRef.o = (vlSelfRef.recon__DOT__taps[0U] 
                                   ^ vlSelfRef.recon__DOT__taps[127U]);
                }
            }
        }
        Vrecon___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vrecon___024root___eval(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VicoIterCount;
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VicoIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vrecon___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
            VL_FATAL_MT("recon.sv", 3, "", "DIDNOTCONVERGE: Input combinational region did not converge after '--converge-limit' of 10000 tries");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        vlSelfRef.__VicoPhaseResult = Vrecon___024root___eval_phase__ico(vlSelf);
    } while (vlSelfRef.__VicoPhaseResult);
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vrecon___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("recon.sv", 3, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vrecon___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("recon.sv", 3, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactPhaseResult = Vrecon___024root___eval_phase__act(vlSelf);
        } while (vlSelfRef.__VactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vrecon___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

#ifdef VL_DEBUG
void Vrecon___024root___eval_debug_assertions(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_debug_assertions\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");
    }
}
#endif  // VL_DEBUG
