// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vshared__pch.h"
#include "verilated_fst_c.h"

//============================================================
// Constructors

Vshared::Vshared(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vshared__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , a{vlSymsp->TOP.a}
    , b{vlSymsp->TOP.b}
    , c{vlSymsp->TOP.c}
    , d{vlSymsp->TOP.d}
    , acc{vlSymsp->TOP.acc}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
    contextp()->traceBaseModelCbAdd(
        [this](VerilatedTraceBaseC* tfp, int levels, int options) { traceBaseModel(tfp, levels, options); });
}

Vshared::Vshared(const char* _vcname__)
    : Vshared(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vshared::~Vshared() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vshared___024root___eval_debug_assertions(Vshared___024root* vlSelf);
#endif  // VL_DEBUG
void Vshared___024root___eval_static(Vshared___024root* vlSelf);
void Vshared___024root___eval_initial(Vshared___024root* vlSelf);
void Vshared___024root___eval_settle(Vshared___024root* vlSelf);
void Vshared___024root___eval(Vshared___024root* vlSelf);

void Vshared::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vshared::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vshared___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vshared___024root___eval_static(&(vlSymsp->TOP));
        Vshared___024root___eval_initial(&(vlSymsp->TOP));
        Vshared___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vshared___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vshared::eventsPending() { return false; }

uint64_t Vshared::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vshared::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vshared___024root___eval_final(Vshared___024root* vlSelf);

VL_ATTR_COLD void Vshared::final() {
    contextp()->executingFinal(true);
    Vshared___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vshared::hierName() const { return vlSymsp->name(); }
const char* Vshared::modelName() const { return "Vshared"; }
unsigned Vshared::threads() const { return 1; }
void Vshared::prepareClone() const { contextp()->prepareClone(); }
void Vshared::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vshared::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false}};
};

//============================================================
// Trace configuration

void Vshared___024root__trace_decl_types(VerilatedFst* tracep);

void Vshared___024root__trace_init_top(Vshared___024root* vlSelf, VerilatedFst* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedFst* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vshared___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vshared___024root*>(voidSelf);
    Vshared__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(vlSymsp->name(), VerilatedTracePrefixType::SCOPE_MODULE);
    Vshared___024root__trace_decl_types(tracep);
    Vshared___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vshared___024root__trace_register(Vshared___024root* vlSelf, VerilatedFst* tracep);

VL_ATTR_COLD void Vshared::traceBaseModel(VerilatedTraceBaseC* tfp, int levels, int options) {
    (void)levels; (void)options;
    VerilatedFstC* const stfp = dynamic_cast<VerilatedFstC*>(tfp);
    if (VL_UNLIKELY(!stfp)) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vshared::trace()' called on non-VerilatedFstC object;"
            " use --trace-fst with VerilatedFst object, and --trace-vcd with VerilatedVcd object");
    }
    stfp->spTrace()->addModel(this);
    stfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP), name(), false, 26);
    Vshared___024root__trace_register(&(vlSymsp->TOP), stfp->spTrace());
}
