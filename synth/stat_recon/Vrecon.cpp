// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vrecon__pch.h"
#include "verilated_fst_c.h"

//============================================================
// Constructors

Vrecon::Vrecon(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vrecon__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , x{vlSymsp->TOP.x}
    , y{vlSymsp->TOP.y}
    , z{vlSymsp->TOP.z}
    , o{vlSymsp->TOP.o}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
    contextp()->traceBaseModelCbAdd(
        [this](VerilatedTraceBaseC* tfp, int levels, int options) { traceBaseModel(tfp, levels, options); });
}

Vrecon::Vrecon(const char* _vcname__)
    : Vrecon(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vrecon::~Vrecon() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vrecon___024root___eval_debug_assertions(Vrecon___024root* vlSelf);
#endif  // VL_DEBUG
void Vrecon___024root___eval_static(Vrecon___024root* vlSelf);
void Vrecon___024root___eval_initial(Vrecon___024root* vlSelf);
void Vrecon___024root___eval_settle(Vrecon___024root* vlSelf);
void Vrecon___024root___eval(Vrecon___024root* vlSelf);

void Vrecon::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vrecon::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vrecon___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vrecon___024root___eval_static(&(vlSymsp->TOP));
        Vrecon___024root___eval_initial(&(vlSymsp->TOP));
        Vrecon___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vrecon___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vrecon::eventsPending() { return false; }

uint64_t Vrecon::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vrecon::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vrecon___024root___eval_final(Vrecon___024root* vlSelf);

VL_ATTR_COLD void Vrecon::final() {
    contextp()->executingFinal(true);
    Vrecon___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vrecon::hierName() const { return vlSymsp->name(); }
const char* Vrecon::modelName() const { return "Vrecon"; }
unsigned Vrecon::threads() const { return 1; }
void Vrecon::prepareClone() const { contextp()->prepareClone(); }
void Vrecon::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vrecon::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false}};
};

//============================================================
// Trace configuration

void Vrecon___024root__trace_decl_types(VerilatedFst* tracep);

void Vrecon___024root__trace_init_top(Vrecon___024root* vlSelf, VerilatedFst* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedFst* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vrecon___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vrecon___024root*>(voidSelf);
    Vrecon__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(vlSymsp->name(), VerilatedTracePrefixType::SCOPE_MODULE);
    Vrecon___024root__trace_decl_types(tracep);
    Vrecon___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vrecon___024root__trace_register(Vrecon___024root* vlSelf, VerilatedFst* tracep);

VL_ATTR_COLD void Vrecon::traceBaseModel(VerilatedTraceBaseC* tfp, int levels, int options) {
    (void)levels; (void)options;
    VerilatedFstC* const stfp = dynamic_cast<VerilatedFstC*>(tfp);
    if (VL_UNLIKELY(!stfp)) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vrecon::trace()' called on non-VerilatedFstC object;"
            " use --trace-fst with VerilatedFst object, and --trace-vcd with VerilatedVcd object");
    }
    stfp->spTrace()->addModel(this);
    stfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP), name(), false, 14);
    Vrecon___024root__trace_register(&(vlSymsp->TOP), stfp->spTrace());
}
