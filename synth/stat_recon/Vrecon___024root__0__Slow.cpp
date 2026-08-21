// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vrecon.h for the primary calling header

#include "Vrecon__pch.h"

VL_ATTR_COLD void Vrecon___024root___eval_static(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_static\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__x__0 = vlSelfRef.x;
    vlSelfRef.__Vtrigprevexpr___TOP__y__0 = vlSelfRef.y;
    vlSelfRef.__Vtrigprevexpr___TOP__z__0 = vlSelfRef.z;
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__clk__1 = vlSelfRef.clk;
}

VL_ATTR_COLD void Vrecon___024root___eval_initial(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_initial\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vrecon___024root___eval_final(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_final\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vrecon___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vrecon___024root___eval_phase__stl(Vrecon___024root* vlSelf);

VL_ATTR_COLD void Vrecon___024root___eval_settle(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_settle\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vrecon___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("recon.sv", 3, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vrecon___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD bool Vrecon___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vrecon___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vrecon___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vrecon___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD bool Vrecon___024root___eval_phase__stl(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___eval_phase__stl\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__stl
        vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                          & vlSelfRef.__VstlTriggered[0U]) 
                                         | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vrecon___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vrecon___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        {
            // Inlined CFunc: _eval_stl
            if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
                {
                    // Inlined CFunc: _ico_comb__TOP__0
                    IData/*31:0*/ __Vinline_0__eval_stl___Vinline_0__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i;
                    __Vinline_0__eval_stl___Vinline_0__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i = 0;
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
                    __Vinline_0__eval_stl___Vinline_0__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i = 0U;
                    while (VL_GTS_III(32, 0x00000080U, __Vinline_0__eval_stl___Vinline_0__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i)) {
                        vlSelfRef.recon__DOT__taps[(0x0000007fU 
                                                    & __Vinline_0__eval_stl___Vinline_0__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i)] 
                            = ((vlSelfRef.recon__DOT__base 
                                ^ VL_SHIFTR_QQI(64,64,32, vlSelfRef.recon__DOT__base2, 
                                                VL_MODDIVS_III(32, __Vinline_0__eval_stl___Vinline_0__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i, (IData)(7U)))) 
                               ^ (((QData)((IData)(vlSelfRef.recon__DOT__base)) 
                                   << 0x00000020U) 
                                  | (QData)((IData)(
                                                    (vlSelfRef.recon__DOT__base2 
                                                     >> 0x20U)))));
                        __Vinline_0__eval_stl___Vinline_0__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i 
                            = ((IData)(1U) + __Vinline_0__eval_stl___Vinline_0__ico_comb__TOP__0_recon__DOT__unnamedblk1__DOT__i);
                    }
                }
            }
        }
    }
    return (__VstlExecute);
}

bool Vrecon___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 2> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vrecon___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vrecon___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @( x)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @( y)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @( z)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @( clk)\n");
    }
    if ((1U & (IData)(triggers[1U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 64 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vrecon___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vrecon___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vrecon___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vrecon___024root___ctor_var_reset(Vrecon___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrecon___024root___ctor_var_reset\n"); );
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->x = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 9409450202036847209ull);
    vlSelf->y = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 11123243248953317070ull);
    vlSelf->z = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 9847161165854524911ull);
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->o = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 13210364986222294696ull);
    vlSelf->recon__DOT__base = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 3485024051879290962ull);
    vlSelf->recon__DOT__base2 = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 11878695862098359546ull);
    for (int __Vi0 = 0; __Vi0 < 128; ++__Vi0) {
        vlSelf->recon__DOT__taps[__Vi0] = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 9709575699706740279ull);
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__x__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__y__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__z__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = 0;
    vlSelf->__VicoDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__clk__1 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
