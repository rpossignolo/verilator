// -*- mode: C++; c-file-style: "cc-mode" -*-
//*************************************************************************
// DESCRIPTION: Verilator: Create triggers for code scheduling
//
// Code available from: https://verilator.org
//
//*************************************************************************
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of either the GNU Lesser General Public License Version 3
// or the Perl Artistic License Version 2.0.
// SPDX-FileCopyrightText: 2003-2026 Wilson Snyder
// SPDX-License-Identifier: LGPL-3.0-only OR Artistic-2.0
//
//*************************************************************************
//
//
//
//*************************************************************************

#include "V3PchAstNoMT.h"  // VL_MT_DISABLED_CODE_UNIT

#include "V3Const.h"
#include "V3EmitCBase.h"
#include "V3EmitV.h"
#include "V3Order.h"
#include "V3Sched.h"
#include "V3SenExprBuilder.h"
#include "V3Stats.h"

VL_DEFINE_DEBUG_FUNCTIONS;

namespace V3Sched {

namespace {

AstVarScope* newArgument(AstCFunc* funcp, AstNodeDType* dtypep, const std::string& name,
                         VDirection direction) {
    FileLine* const flp = funcp->fileline();
    AstScope* const scopep = funcp->scopep();
    AstVar* const varp = new AstVar{flp, VVarType::BLOCKTEMP, name, dtypep};
    varp->funcLocal(true);
    varp->direction(direction);
    funcp->addArgsp(varp);
    AstVarScope* const vscp = new AstVarScope{flp, scopep, varp};
    scopep->addVarsp(vscp);
    return vscp;
}

AstVarScope* newLocal(AstCFunc* funcp, AstNodeDType* dtypep, const std::string& name) {
    FileLine* const flp = funcp->fileline();
    AstScope* const scopep = funcp->scopep();
    AstVar* const varp = new AstVar{flp, VVarType::BLOCKTEMP, name, dtypep};
    varp->funcLocal(true);
    varp->noReset(true);
    funcp->addVarsp(varp);
    AstVarScope* const vscp = new AstVarScope{flp, scopep, varp};
    scopep->addVarsp(vscp);
    return vscp;
}

}  // namespace

AstCFunc* TriggerKit::createDumpExtFunc() const {
    UASSERT(m_nPreWords, "Just call the regular dumping function if there are no pre triggers");

    AstNetlist* const netlistp = v3Global.rootp();
    FileLine* const flp = netlistp->topScopep()->fileline();
    AstNodeDType* const u32DTypep = netlistp->findUInt32DType();
    AstNodeDType* const strDtypep = netlistp->findStringDType();

    // Dumping function always slow
    const std::string name = "_dump_triggers__" + m_name + "_ext";
    AstCFunc* const funcp = util::makeSubFunction(netlistp, name, true);
    funcp->isStatic(true);
    funcp->ifdef("VL_DEBUG");

    // Add argument
    AstVarScope* const eVscp = newArgument(funcp, m_trigExtDTypep, "ext", VDirection::CONSTREF);
    AstVarScope* const tVscp = newArgument(funcp, strDtypep, "tag", VDirection::CONSTREF);

    // Creates read/write reference
    const auto rd = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::READ}; };
    const auto wr = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::WRITE}; };

    // This is a slow function, only for dumping, so we can just copy to locals

    // Copy the vec part, dump it
    {
        AstVarScope* const vVscp = newLocal(funcp, m_trigVecDTypep, "vec");
        AstVarScope* const iVscp = newLocal(funcp, u32DTypep, "i");
        funcp->addStmtsp(util::setVar(iVscp, 0));
        // Add loop
        AstLoop* const loopp = new AstLoop{flp};
        funcp->addStmtsp(loopp);
        // Loop body
        AstNodeExpr* const lhsp = new AstArraySel{flp, wr(vVscp), rd(iVscp)};
        AstNodeExpr* const rhsp = new AstArraySel{flp, rd(eVscp), rd(iVscp)};
        AstNodeExpr* const limp = new AstConst{flp, AstConst::WidthedValue{}, 32, m_nVecWords};
        loopp->addStmtsp(new AstAssign{flp, lhsp, rhsp});
        loopp->addStmtsp(util::incrementVar(iVscp));
        loopp->addStmtsp(new AstLoopTest{flp, loopp, new AstLt{flp, rd(iVscp), limp}});
        // Use the vec dumping function
        AstCCall* const callp = new AstCCall{flp, m_dumpp};
        callp->dtypeSetVoid();
        callp->addArgsp(rd(vVscp));
        callp->addArgsp(rd(tVscp));
        funcp->addStmtsp(callp->makeStmt());
    }

    // Copy the pre part, zero top bits, dump it
    {
        AstVarScope* const pVscp = newLocal(funcp, m_trigVecDTypep, "pre");
        AstVarScope* const jVscp = newLocal(funcp, u32DTypep, "j");
        funcp->addStmtsp(util::setVar(jVscp, 0));
        // Copy pre words
        {
            // Add loop
            AstLoop* const loopp = new AstLoop{flp};
            funcp->addStmtsp(loopp);
            // Loop body
            AstNodeExpr* const lhsp = new AstArraySel{flp, wr(pVscp), rd(jVscp)};
            AstNodeExpr* const rhsp = new AstArraySel{flp, rd(eVscp), rd(jVscp)};
            AstNodeExpr* const limp = new AstConst{flp, AstConst::WidthedValue{}, 32, m_nPreWords};
            loopp->addStmtsp(new AstAssign{flp, lhsp, rhsp});
            loopp->addStmtsp(util::incrementVar(jVscp));
            loopp->addStmtsp(new AstLoopTest{flp, loopp, new AstLt{flp, rd(jVscp), limp}});
        }
        // Zero the rest
        {
            // Add loop - copy
            AstLoop* const loopp = new AstLoop{flp};
            funcp->addStmtsp(loopp);
            // Loop body
            AstNodeExpr* const lhsp = new AstArraySel{flp, wr(pVscp), rd(jVscp)};
            AstNodeExpr* const rhsp = new AstConst{flp, AstConst::DTyped{}, m_wordDTypep};
            AstNodeExpr* const limp = new AstConst{flp, AstConst::WidthedValue{}, 32, m_nVecWords};
            loopp->addStmtsp(new AstAssign{flp, lhsp, rhsp});
            loopp->addStmtsp(util::incrementVar(jVscp));
            loopp->addStmtsp(new AstLoopTest{flp, loopp, new AstLt{flp, rd(jVscp), limp}});
        }
        // Use the vec dumping function
        AstCCall* const callp = new AstCCall{flp, m_dumpp};
        callp->dtypeSetVoid();
        callp->addArgsp(rd(pVscp));
        callp->addArgsp(
            new AstConcatN{flp, rd(tVscp), new AstConst{flp, AstConst::String{}, " pre"}});
        funcp->addStmtsp(callp->makeStmt());
    }

    return funcp;
}

AstCFunc* TriggerKit::createAnySetFunc(AstUnpackArrayDType* const dtypep) const {
    AstNetlist* const netlistp = v3Global.rootp();
    FileLine* const flp = netlistp->topScopep()->fileline();
    AstNodeDType* const u32DTypep = netlistp->findUInt32DType();

    // Create function
    std::string name = "_trigger_anySet__" + m_name;
    name += dtypep == m_trigVecDTypep ? "" : "_ext";
    AstCFunc* const funcp = util::makeSubFunction(netlistp, name, m_slow);
    funcp->isStatic(true);
    funcp->rtnType("bool");

    // Add argument
    AstVarScope* const iVscp = newArgument(funcp, dtypep, "in", VDirection::CONSTREF);

    // Add loop counter variable
    AstVarScope* const nVscp = newLocal(funcp, u32DTypep, "n");

    // Creates read reference
    const auto rd = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::READ}; };

    // Function body
    AstLoop* const loopp = new AstLoop{flp};
    funcp->addStmtsp(util::setVar(nVscp, 0));
    funcp->addStmtsp(loopp);
    funcp->addStmtsp(new AstCReturn{flp, new AstConst{flp, AstConst::BitFalse{}}});

    // Loop body
    const uint32_t nWords = dtypep->elementsConst();
    AstNodeExpr* const condp = new AstArraySel{flp, rd(iVscp), rd(nVscp)};
    AstNodeStmt* const thenp = new AstCReturn{flp, new AstConst{flp, AstConst::BitTrue{}}};
    AstNodeExpr* const limp = new AstConst{flp, AstConst::WidthedValue{}, 32, nWords};
    loopp->addStmtsp(new AstIf{flp, condp, thenp});
    loopp->addStmtsp(util::incrementVar(nVscp));
    loopp->addStmtsp(new AstLoopTest{flp, loopp, new AstLt{flp, rd(nVscp), limp}});

    // Done
    return funcp;
}
AstCFunc* TriggerKit::createClearFunc() const {
    AstNetlist* const netlistp = v3Global.rootp();
    FileLine* const flp = netlistp->topScopep()->fileline();
    AstNodeDType* const u32DTypep = netlistp->findUInt32DType();

    // Create function
    AstCFunc* const funcp = util::makeSubFunction(netlistp, "_trigger_clear__" + m_name, m_slow);
    funcp->isStatic(true);

    // Add arguments
    AstVarScope* const oVscp = newArgument(funcp, m_trigVecDTypep, "out", VDirection::OUTPUT);

    // Add loop counter variable
    AstVarScope* const nVscp = newLocal(funcp, u32DTypep, "n");

    // Creates read/write reference
    const auto rd = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::READ}; };
    const auto wr = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::WRITE}; };

    // Function body
    AstLoop* const loopp = new AstLoop{flp};
    funcp->addStmtsp(util::setVar(nVscp, 0));
    funcp->addStmtsp(loopp);

    // Loop body
    AstNodeExpr* const lhsp = new AstArraySel{flp, wr(oVscp), rd(nVscp)};
    AstNodeExpr* const rhsp = new AstConst{flp, AstConst::DTyped{}, m_wordDTypep};
    AstNodeExpr* const limp = new AstConst{flp, AstConst::WidthedValue{}, 32, m_nVecWords};
    loopp->addStmtsp(new AstAssign{flp, lhsp, rhsp});
    loopp->addStmtsp(util::incrementVar(nVscp));
    loopp->addStmtsp(new AstLoopTest{flp, loopp, new AstLt{flp, rd(nVscp), limp}});

    // Done
    return funcp;
}
AstCFunc* TriggerKit::createOrIntoFunc(AstUnpackArrayDType* const oDtypep,
                                       AstUnpackArrayDType* const iDtypep) const {
    AstNetlist* const netlistp = v3Global.rootp();
    FileLine* const flp = netlistp->topScopep()->fileline();
    AstNodeDType* const u32DTypep = netlistp->findUInt32DType();

    // Create function
    std::string name = "_trigger_orInto__" + m_name;
    name += iDtypep == m_trigVecDTypep ? "_vec" : "_ext";
    name += oDtypep == m_trigVecDTypep ? "_vec" : "_ext";
    AstCFunc* const funcp = util::makeSubFunction(netlistp, name, m_slow);
    funcp->isStatic(true);

    // Add arguments
    AstVarScope* const oVscp = newArgument(funcp, oDtypep, "out", VDirection::INOUT);
    AstVarScope* const iVscp = newArgument(funcp, iDtypep, "in", VDirection::CONSTREF);

    // Add loop counter variable
    AstVarScope* const nVscp = newLocal(funcp, u32DTypep, "n");

    // Creates read/write reference
    const auto rd = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::READ}; };
    const auto wr = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::WRITE}; };

    // Function body
    AstLoop* const loopp = new AstLoop{flp};
    funcp->addStmtsp(util::setVar(nVscp, 0));
    funcp->addStmtsp(loopp);

    // Loop body
    AstNodeExpr* const lhsp = new AstArraySel{flp, wr(oVscp), rd(nVscp)};
    AstNodeExpr* const oWordp = new AstArraySel{flp, rd(oVscp), rd(nVscp)};
    AstNodeExpr* const iWordp = new AstArraySel{flp, rd(iVscp), rd(nVscp)};
    AstNodeExpr* const rhsp = new AstOr{flp, oWordp, iWordp};
    AstConst* const outputRangeLeftp = VN_AS(oDtypep->rangep()->leftp(), Const);
    AstConst* const inputRangeLeftp = VN_AS(iDtypep->rangep()->leftp(), Const);
    AstNodeExpr* const limp = outputRangeLeftp->num().toSInt() < inputRangeLeftp->num().toSInt()
                                  ? outputRangeLeftp->cloneTreePure(false)
                                  : inputRangeLeftp->cloneTreePure(false);
    loopp->addStmtsp(new AstAssign{flp, lhsp, rhsp});
    loopp->addStmtsp(util::incrementVar(nVscp));
    loopp->addStmtsp(new AstLoopTest{flp, loopp, new AstLte{flp, rd(nVscp), limp}});

    // Done
    return funcp;
}

AstNodeExpr* TriggerKit::newAnySetCall(AstVarScope* const vscp) const {
    FileLine* const flp = v3Global.rootp()->topScopep()->fileline();
    if (!m_nVecWords) return new AstConst{flp, AstConst::BitFalse{}};

    AstCFunc* funcp = nullptr;
    if (vscp->dtypep() == m_trigVecDTypep) {
        if (!m_anySetVecp) m_anySetVecp = createAnySetFunc(m_trigVecDTypep);
        funcp = m_anySetVecp;
    } else if (vscp->dtypep() == m_trigExtDTypep) {
        if (!m_anySetExtp) m_anySetExtp = createAnySetFunc(m_trigExtDTypep);
        funcp = m_anySetExtp;
    } else {
        vscp->v3fatalSrc("Bad trigger vector type");
    }
    AstCCall* const callp = new AstCCall{flp, funcp};
    callp->addArgsp(new AstVarRef{flp, vscp, VAccess::WRITE});
    callp->dtypeSetBit();
    return callp;
}
AstNodeStmt* TriggerKit::newClearCall(AstVarScope* const vscp) const {
    if (!m_nVecWords) return nullptr;
    UASSERT_OBJ(vscp->dtypep() == m_trigVecDTypep, vscp, "Bad trigger vector type");
    if (!m_clearp) m_clearp = createClearFunc();
    FileLine* const flp = v3Global.rootp()->topScopep()->fileline();
    AstCCall* const callp = new AstCCall{flp, m_clearp};
    callp->addArgsp(new AstVarRef{flp, vscp, VAccess::WRITE});
    callp->dtypeSetVoid();
    return callp->makeStmt();
}
AstNodeStmt* TriggerKit::newOrIntoCall(AstVarScope* const oVscp, AstVarScope* const iVscp) const {
    if (!m_nVecWords) return nullptr;
    UASSERT_OBJ(iVscp->dtypep() == m_trigVecDTypep || iVscp->dtypep() == m_trigExtDTypep, iVscp,
                "Bad input trigger vector type");
    UASSERT_OBJ(oVscp->dtypep() == m_trigVecDTypep || oVscp->dtypep() == m_trigExtDTypep, oVscp,
                "Bad output trigger vector type");
    const size_t mask
        = ((oVscp->dtypep() == m_trigExtDTypep) << 1) | (iVscp->dtypep() == m_trigExtDTypep);
    AstCFunc*& funcp = m_orIntoVecps[mask];
    if (!funcp) {
        funcp = createOrIntoFunc(VN_AS(oVscp->dtypep(), UnpackArrayDType),
                                 VN_AS(iVscp->dtypep(), UnpackArrayDType));
    }
    FileLine* const flp = v3Global.rootp()->topScopep()->fileline();
    AstCCall* const callp = new AstCCall{flp, funcp};
    callp->addArgsp(new AstVarRef{flp, oVscp, VAccess::WRITE});
    callp->addArgsp(new AstVarRef{flp, iVscp, VAccess::READ});
    callp->dtypeSetVoid();
    return callp->makeStmt();
}

AstNodeStmt* TriggerKit::newCompBaseCall() const {
    if (!m_nVecWords) return nullptr;
    FileLine* const flp = v3Global.rootp()->topScopep()->fileline();
    AstCCall* const callp = new AstCCall{flp, m_compVecp};
    callp->dtypeSetVoid();
    return callp->makeStmt();
}

AstNodeStmt* TriggerKit::newCompExtCall(AstVarScope* vscp) const {
    if (!m_nPreWords) return nullptr;
    FileLine* const flp = v3Global.rootp()->topScopep()->fileline();
    AstCCall* const callp = new AstCCall{flp, m_compExtp};
    callp->addArgsp(new AstVarRef{flp, vscp, VAccess::READ});
    callp->dtypeSetVoid();
    return callp->makeStmt();
}

AstNodeStmt* TriggerKit::newDumpCall(AstVarScope* const vscp, const std::string& tag,
                                     bool debugOnly) const {
    if (!m_nVecWords) return nullptr;
    AstCFunc* funcp = nullptr;
    if (vscp->dtypep() == m_trigVecDTypep) {
        funcp = m_dumpp;
    } else if (vscp->dtypep() == m_trigExtDTypep) {
        if (!m_dumpExtp) m_dumpExtp = createDumpExtFunc();
        funcp = m_dumpExtp;
    } else {
        vscp->v3fatalSrc("Bad trigger vector type");
    }
    FileLine* const flp = v3Global.rootp()->topScopep()->fileline();
    AstCCall* const callp = new AstCCall{flp, funcp};
    callp->addArgsp(new AstVarRef{flp, vscp, VAccess::READ});
    callp->addArgsp(new AstConst{flp, AstConst::String{}, tag});
    callp->dtypeSetVoid();
    AstCStmt* const cstmtp = new AstCStmt{flp};
    cstmtp->add("#ifdef VL_DEBUG\n");
    if (debugOnly) {
        cstmtp->add("if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {\n");
        cstmtp->add(callp->makeStmt());
        cstmtp->add("}\n");
    } else {
        cstmtp->add(callp->makeStmt());
    }
    cstmtp->add("#endif");
    return cstmtp;
}

AstVarScope* TriggerKit::newTrigVec(const std::string& name) const {
    if (!m_nVecWords) return nullptr;
    AstScope* const scopep = v3Global.rootp()->topScopep()->scopep();
    return scopep->createTemp("__V" + name + "Triggered", m_trigVecDTypep);
}

AstSenTree* TriggerKit::newTriggerSenTree(AstVarScope* const vscp,
                                          const std::vector<uint32_t>& indices) const {
    AstNetlist* const netlistp = v3Global.rootp();
    AstTopScope* const topScopep = netlistp->topScopep();
    FileLine* const flp = topScopep->fileline();

    AstSenTree* const senTreep = new AstSenTree{flp, nullptr};
    topScopep->addSenTreesp(senTreep);
    for (const uint32_t index : indices) {
        UASSERT(index <= (m_nVecWords + m_nPreWords) * WORD_SIZE, "Invalid trigger index");
        const uint32_t wordIndex = index / WORD_SIZE;
        const uint32_t bitIndex = index % WORD_SIZE;
        AstVarRef* const refp = new AstVarRef{flp, vscp, VAccess::READ};
        AstNodeExpr* const aselp = new AstArraySel{flp, refp, static_cast<int>(wordIndex)};
        // Use a mask & _ to extract the bit, V3Const can optimize this to combine terms
        AstConst* const maskp
            = new AstConst{flp, AstConst::WidthedValue{}, static_cast<int>(WORD_SIZE), 0};
        maskp->num().setBit(bitIndex, '1');
        AstNodeExpr* const termp = new AstAnd{flp, maskp, aselp};
        senTreep->addSensesp(new AstSenItem{flp, VEdgeType::ET_TRUE, termp});
    }
    return senTreep;
}

AstSenTree* TriggerKit::newExtraTriggerSenTree(AstVarScope* vscp, uint32_t index) const {
    UASSERT(index <= m_nExtraWords * WORD_SIZE, "Invalid external trigger index");
    return newTriggerSenTree(vscp, {index + m_nSenseWords * WORD_SIZE});
}

void TriggerKit::addExtraTriggerAssignment(AstVarScope* vscp, uint32_t index, bool clear) const {
    index += m_nSenseWords * WORD_SIZE;
    const uint32_t wordIndex = index / WORD_SIZE;
    const uint32_t bitIndex = index % WORD_SIZE;
    FileLine* const flp = vscp->fileline();
    // Set the trigger bit
    AstVarRef* const refp = new AstVarRef{flp, m_vscp, VAccess::WRITE};
    AstNodeExpr* const wordp = new AstArraySel{flp, refp, static_cast<int>(wordIndex)};
    AstNodeExpr* const trigLhsp = new AstSel{flp, wordp, static_cast<int>(bitIndex), 1};
    AstNodeExpr* const trigRhsp = new AstVarRef{flp, vscp, VAccess::READ};
    AstNode* const setp = new AstAssign{flp, trigLhsp, trigRhsp};
    if (clear) {
        // Clear the input variable
        setp->addNext(new AstAssign{flp, new AstVarRef{flp, vscp, VAccess::WRITE},
                                    new AstConst{flp, AstConst::BitFalse{}}});
    }
    if (AstNode* const nodep = m_compVecp->stmtsp()) {
        setp->addNext(setp, nodep->unlinkFrBackWithNext());
    }
    m_compVecp->addStmtsp(setp);
}

// Value-change detection for a single interface member VarScope written through a VIF.
// Creates: trigger[bit] = (vscp != prev); prev = vscp;
void TriggerKit::addValueChangeTriggerAssignment(AstNetlist* netlistp, AstCFunc* initFuncp,
                                                 AstVarScope* instVscp, uint32_t index) const {
    index += m_nSenseWords * WORD_SIZE;
    const uint32_t wordIndex = index / WORD_SIZE;
    const uint32_t bitIndex = index % WORD_SIZE;

    AstScope* const scopeTopp = netlistp->topScopep()->scopep();
    FileLine* const flp = netlistp->fileline();
    AstNodeDType* const dtypep = instVscp->dtypep()->skipRefp();

    // Reject types that do not support value-change comparison
    if (const AstBasicDType* const bdtypep = VN_CAST(dtypep, BasicDType)) {
        if (bdtypep->isEvent() || bdtypep->isString()
            || bdtypep->keyword() == VBasicDTypeKwd::CHANDLE) {
            instVscp->v3warn(E_UNSUPPORTED, "Unsupported: virtual interface trigger on "
                                                << dtypep->prettyDTypeNameQ() << " type");
            return;
        }
    } else if (!VN_IS(dtypep, PackArrayDType) && !VN_IS(dtypep, UnpackArrayDType)
               && !VN_IS(dtypep, NodeUOrStructDType) && !VN_IS(dtypep, ClassRefDType)) {
        instVscp->v3warn(E_UNSUPPORTED, "Unsupported: virtual interface trigger on "
                                            << dtypep->prettyDTypeNameQ() << " type");
        return;
    }

    const auto rdInst = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::READ}; };
    const auto wrPrev = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::WRITE}; };
    const auto rdPrev = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::READ}; };

    // Create prev variable
    const std::string prevName = "__Vtrigprevvif_" + m_name + "_"
                                 + instVscp->scopep()->nameDotless() + "__"
                                 + instVscp->varp()->name();
    AstVarScope* const prevVscp = scopeTopp->createTemp(prevName, instVscp->dtypep());

    // Initialize prev = inst
    if (VN_IS(dtypep, UnpackArrayDType)) {
        AstCMethodHard* const cmhp = new AstCMethodHard{
            flp, wrPrev(prevVscp), VCMethod::UNPACKED_ASSIGN, rdInst(instVscp)};
        cmhp->dtypeSetVoid();
        initFuncp->addStmtsp(cmhp->makeStmt());
    } else {
        initFuncp->addStmtsp(new AstAssign{flp, wrPrev(prevVscp), rdInst(instVscp)});
    }

    // Build comparison: inst != prev
    AstNodeExpr* neqp;
    if (VN_IS(dtypep, UnpackArrayDType)) {
        AstCMethodHard* const cmhp
            = new AstCMethodHard{flp, rdPrev(prevVscp), VCMethod::UNPACKED_NEQ, rdInst(instVscp)};
        cmhp->dtypeSetBit();
        neqp = cmhp;
    } else {
        neqp = new AstNeq{flp, rdInst(instVscp), rdPrev(prevVscp)};
    }

    // Build post-update: prev = inst
    AstNode* updp;
    if (VN_IS(dtypep, UnpackArrayDType)) {
        AstCMethodHard* const cmhp = new AstCMethodHard{
            flp, wrPrev(prevVscp), VCMethod::UNPACKED_ASSIGN, rdInst(instVscp)};
        cmhp->dtypeSetVoid();
        updp = cmhp->makeStmt();
    } else {
        updp = new AstAssign{flp, wrPrev(prevVscp), rdInst(instVscp)};
    }

    // Set trigger bit
    AstVarRef* const refp = new AstVarRef{flp, m_vscp, VAccess::WRITE};
    AstNodeExpr* const wordp = new AstArraySel{flp, refp, static_cast<int>(wordIndex)};
    AstNodeExpr* const trigLhsp = new AstSel{flp, wordp, static_cast<int>(bitIndex), 1};
    AstNode* const setp = new AstAssign{flp, trigLhsp, neqp};

    // Chain: set trigger bit -> update prev
    setp->addNext(updp);

    // Prepend before existing statements
    if (AstNode* const nodep = m_compVecp->stmtsp()) {
        setp->addNext(setp, nodep->unlinkFrBackWithNext());
    }
    m_compVecp->addStmtsp(setp);
}

TriggerKit::TriggerKit(const std::string& name, bool slow, uint32_t nSenseWords,
                       uint32_t nExtraWords, uint32_t nPreWords,
                       std::unordered_map<VNRef<const AstSenItem>, size_t> senItem2TrigIdx,
                       bool useAcc)
    : m_name{name}
    , m_slow{slow}
    , m_nSenseWords{nSenseWords}
    , m_nExtraWords{nExtraWords}
    , m_nPreWords{nPreWords}
    , m_senItem2TrigIdx{std::move(senItem2TrigIdx)} {
    // If no triggers, we don't need to generate anything
    if (!m_nVecWords) return;
    // Othewise construc the parts of the kit
    AstNetlist* const netlistp = v3Global.rootp();
    AstScope* const scopep = netlistp->topScopep()->scopep();
    FileLine* const flp = scopep->fileline();
    // Data type of a single trigger word
    m_wordDTypep = netlistp->findBitDType(WORD_SIZE, WORD_SIZE, VSigning::UNSIGNED);
    // Data type of trigger vector
    AstRange* const rp = new AstRange{flp, static_cast<int>(m_nVecWords - 1), 0};
    m_trigVecDTypep = new AstUnpackArrayDType{flp, m_wordDTypep, rp};
    netlistp->typeTablep()->addTypesp(m_trigVecDTypep);
    // Data type of extended trigger vector, which only differs if there are pre triggers
    if (m_nPreWords) {
        AstRange* const ep = new AstRange{flp, static_cast<int>(m_nVecWords + m_nPreWords - 1), 0};
        m_trigExtDTypep = new AstUnpackArrayDType{flp, m_wordDTypep, ep};
        netlistp->typeTablep()->addTypesp(m_trigExtDTypep);
        m_compExtp = util::makeSubFunction(netlistp, "_eval_triggers_ext__" + m_name, m_slow);
    } else {
        m_trigExtDTypep = m_trigVecDTypep;
    }
    // The AstVarScope representing the extended trigger vector
    m_vscp = scopep->createTemp("__V" + m_name + "Triggered", m_trigExtDTypep);
    m_vscp->varp()->isInternal(true);
    // The trigger computation function
    m_compVecp = util::makeSubFunction(netlistp, "_eval_triggers_vec__" + m_name, m_slow);
    // The debug dump function, always 'slow'
    m_dumpp = util::makeSubFunction(netlistp, "_dump_triggers__" + m_name, true);
    m_dumpp->isStatic(true);
    m_dumpp->ifdef("VL_DEBUG");
    if (useAcc) {
        m_vscAccp = scopep->createTemp("__V" + m_name + "TriggeredAcc", m_trigVecDTypep);
        m_vscAccp->varp()->isInternal(true);
    }
}

AstAssign* TriggerKit::createSenTrigVecAssignment(AstVarScope* const target,
                                                  std::vector<AstNodeExpr*>& trigps) {
    FileLine* const flp = target->fileline();
    AstAssign* trigStmtsp = nullptr;
    // Assign sense triggers vector one word at a time
    for (size_t i = 0; i < trigps.size(); i += WORD_SIZE) {
        // Concatenate all bits in this trigger word using a balanced
        for (uint32_t level = 0; level < WORD_SIZE_LOG2; ++level) {
            const uint32_t stride = 1 << level;
            for (uint32_t j = 0; j < WORD_SIZE; j += 2 * stride) {
                trigps[i + j] = new AstConcat{trigps[i + j]->fileline(), trigps[i + j + stride],
                                              trigps[i + j]};
                trigps[i + j + stride] = nullptr;
            }
        }

        // Set the whole word in the trigger vector
        const int wordIndex = static_cast<int>(i / WORD_SIZE);
        AstArraySel* const aselp
            = new AstArraySel{flp, new AstVarRef{flp, target, VAccess::WRITE}, wordIndex};
        trigStmtsp = AstNode::addNext(trigStmtsp, new AstAssign{flp, aselp, trigps[i]});
    }
    return trigStmtsp;
}

namespace {

// Whether a trigger must be recomputed every time: its value can change without any write the
// dirty marks see (events, class state, calls, external writers)
bool alwaysRecompute(const AstSenItem* senItemp) {
    switch (senItemp->edgeType()) {
    case VEdgeType::ET_TRUE:
    case VEdgeType::ET_EVENT:
    case VEdgeType::ET_INITIAL_NBA: return true;
    default: break;
    }
    const AstNodeExpr* const senp = senItemp->sensp();
    if (!senp) return true;
    if (senp->exists([](const AstNode* nodep) {
            return VN_IS(nodep, MemberSel) || VN_IS(nodep, CMethodHard) || VN_IS(nodep, CExpr)
                   || VN_IS(nodep, NodeCCall) || VN_IS(nodep, NodeFTaskRef)
                   || VN_IS(nodep, CAwait) || VN_IS(nodep, VarXRef);
        })) {
        return true;
    }
    bool hasRef = false;
    const bool external = senp->exists([&](const AstVarRef* refp) {
        hasRef = true;
        const AstVar* const varp = refp->varp();
        return varp->isPrimaryIO() || varp->isSigUserRWPublic() || varp->isWrittenByDpi()
               || varp->isVirtIface() || varp->sensIfacep();
    });
    return external || !hasRef;
}

// Splits one segment of triggers into groups: always-recomputed first, the rest in their original
// order, which keeps each sensitivity list in few words for the region dispatch code
void layoutGroups(const std::vector<const AstSenItem*>& items,
                  std::vector<const AstSenItem*>& out, std::vector<TriggerKit::DirtyGroup>& groups) {
    constexpr uint32_t WORD_SIZE = TriggerKit::WORD_SIZE;
    constexpr size_t GROUP_ITEMS = 4 * WORD_SIZE;
    std::vector<const AstSenItem*> always;
    std::vector<const AstSenItem*> rest;
    for (const AstSenItem* const itemp : items) {
        (alwaysRecompute(itemp) ? always : rest).push_back(itemp);
    }
    const auto closeGroup = [&](size_t firstItem, bool isAlways) {
        out.resize(vlstd::roundUpToMultipleOf<WORD_SIZE>(out.size()), nullptr);
        const uint32_t firstWord = firstItem / WORD_SIZE;
        const uint32_t nWords = out.size() / WORD_SIZE - firstWord;
        if (nWords) groups.push_back({firstWord, nWords, isAlways});
    };
    const size_t base = out.size();
    out.insert(out.end(), always.begin(), always.end());
    closeGroup(base, true);
    V3Stats::addStat("Scheduling, 'act' dirty trigger always-recomputed", always.size());
    for (size_t i = 0; i < rest.size(); i += GROUP_ITEMS) {
        const size_t groupStart = out.size();
        const size_t n = std::min(GROUP_ITEMS, rest.size() - i);
        out.insert(out.end(), rest.begin() + i, rest.begin() + i + n);
        closeGroup(groupStart, false);
    }
}

}  // namespace

TriggerKit TriggerKit::create(AstNetlist* netlistp,  //
                              AstCFunc* const initFuncp,  //
                              SenExprBuilder& senExprBuilder,  //
                              const std::vector<const AstSenTree*>& preTreeps,  //
                              const std::vector<const AstSenTree*>& senTreeps,  //
                              const string& name,  //
                              const ExtraTriggers& extraTriggers,  //
                              bool slow,  //
                              bool useAcc) {
    // Need to gather all the unique SenItems under the given SenTrees

    // List of unique SenItems used by all 'senTreeps'
    std::vector<const AstSenItem*> senItemps;
    // Map from SenItem to trigger bit standing for that SenItem. There might
    // be duplicate SenItems, we map all of them to the same index.
    std::unordered_map<VNRef<const AstSenItem>, size_t> senItem2TrigIdx;

    // Process the 'pre' trees first, so they are at the begining of the vector
    for (const AstSenTree* const senTreep : preTreeps) {
        for (const AstSenItem *itemp = senTreep->sensesp(), *nextp; itemp; itemp = nextp) {
            nextp = VN_AS(itemp->nextp(), SenItem);
            UASSERT_OBJ(itemp->isClocked() || itemp->isHybrid(), itemp,
                        "Cannot create trigger expression for non-clocked sensitivity");
            const auto pair = senItem2TrigIdx.emplace(*itemp, senItemps.size());
            if (pair.second) senItemps.push_back(itemp);
        }
    }
    const uint32_t nPreSenItems = senItemps.size();
    V3Stats::addStat("Scheduling, '" + name + "' pre triggers", nPreSenItems);
    // Number of pre triggers, rounded up to a full word.
    const uint32_t nPreTriggers = vlstd::roundUpToMultipleOf<WORD_SIZE>(senItemps.size());
    // Pad 'senItemps' to nSenseTriggers with nullptr
    senItemps.resize(nPreTriggers);
    // Number of words for pre triggers
    const uint32_t nPreWords = nPreTriggers / WORD_SIZE;

    // Process the rest of the trees
    for (const AstSenTree* const senTreep : senTreeps) {
        for (const AstSenItem *itemp = senTreep->sensesp(), *nextp; itemp; itemp = nextp) {
            nextp = VN_AS(itemp->nextp(), SenItem);
            UASSERT_OBJ(itemp->isClocked() || itemp->isHybrid(), itemp,
                        "Cannot create trigger expression for non-clocked sensitivity");
            const auto pair = senItem2TrigIdx.emplace(*itemp, senItemps.size());
            if (pair.second) senItemps.push_back(itemp);
        }
    }
    const uint32_t nSenItems = senItemps.size() - nPreTriggers;
    V3Stats::addStat("Scheduling, '" + name + "' sense triggers", nSenItems + nPreSenItems);
    // Number of sense triggers, rounded up to a full word
    const uint32_t nSenseTriggers = vlstd::roundUpToMultipleOf<WORD_SIZE>(senItemps.size());
    // Pad 'senItemps' to nSenseTriggers with nullptr
    senItemps.resize(nSenseTriggers);
    // Number of words sense triggers (inclued pre)
    uint32_t nSenseWords = nSenseTriggers / WORD_SIZE;

    // Recompute groups of 'act' triggers only when their inputs were written
    const bool useDirty = v3Global.opt.schedDirtyTriggers() && name == "act" && !slow
                          && !v3Global.opt.mtasks();
    std::vector<TriggerKit::DirtyGroup> groups;
    uint32_t nPreWordsUsed = nPreWords;
    if (useDirty) {
        const std::vector<const AstSenItem*> preItems(senItemps.begin(),
                                                      senItemps.begin() + nPreSenItems);
        const std::vector<const AstSenItem*> senseItems(
            senItemps.begin() + nPreTriggers, senItemps.begin() + nPreTriggers + nSenItems);
        std::vector<const AstSenItem*> laid;
        layoutGroups(preItems, laid, groups);
        nPreWordsUsed = laid.size() / WORD_SIZE;
        layoutGroups(senseItems, laid, groups);
        senItemps = std::move(laid);
        nSenseWords = senItemps.size() / WORD_SIZE;
        senItem2TrigIdx.clear();
        for (size_t i = 0; i < senItemps.size(); ++i) {
            if (senItemps[i]) senItem2TrigIdx.emplace(*senItemps[i], i);
        }
        V3Stats::addStat("Scheduling, '" + name + "' dirty trigger groups", groups.size());
    }
    const uint32_t nSenseTriggersUsed = nSenseWords * WORD_SIZE;

    // Allocate space for the extra triggers
    V3Stats::addStat("Scheduling, '" + name + "' extra triggers", extraTriggers.size());
    // Number of extra triggers, rounded up to a full word.
    const uint32_t nExtraTriggers = vlstd::roundUpToMultipleOf<WORD_SIZE>(extraTriggers.size());
    const uint32_t nExtraWords = nExtraTriggers / WORD_SIZE;

    // We can now construct the trigger kit - this constructs all items that will be kept
    TriggerKit kit{name, slow, nSenseWords, nExtraWords, nPreWordsUsed, senItem2TrigIdx, useAcc};

    // If there are no triggers we are done
    if (!kit.m_nVecWords) return kit;

    FileLine* const flp = netlistp->topScopep()->fileline();

    // Creates read/write reference
    const auto rd = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::READ}; };
    const auto wr = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::WRITE}; };

    // Construct the comp and dump functions

    // Add arguments to the dump function. The trigger vector is passed into
    // the dumping function via reference so one dump function can dump all
    // different copies of the trigger vector. To do so, it also needs the tag
    // string at runtime, which is the second argument.
    AstVarScope* const dumpTrgp
        = newArgument(kit.m_dumpp, kit.m_trigVecDTypep, "triggers", VDirection::CONSTREF);
    AstVarScope* const dumpTagp
        = newArgument(kit.m_dumpp, netlistp->findStringDType(), "tag", VDirection::CONSTREF);

    // Add a print to the dumping function if there are no triggers pending
    {
        AstIf* const ifp = new AstIf{flp, new AstLogNot{flp, kit.newAnySetCall(dumpTrgp)}};
        kit.m_dumpp->addStmtsp(ifp);
        AstCStmt* const cstmtp = new AstCStmt{flp};
        ifp->addThensp(cstmtp);
        cstmtp->add("VL_DBG_MSGS(\"         No '\" + ");
        cstmtp->add(rd(dumpTagp));
        cstmtp->add(" + \"\' region triggers active\\n\");");
    }

    // Adds a debug dumping statement for this trigger
    const auto addDebug = [&](uint32_t index, const string& text) {
        const int wrdIndex = static_cast<int>(index / WORD_SIZE);
        const int bitIndex = static_cast<int>(index % WORD_SIZE);
        AstNodeExpr* const aselp = new AstArraySel{flp, rd(dumpTrgp), wrdIndex};
        AstNodeExpr* const condp = new AstSel{flp, aselp, bitIndex, 1};
        AstIf* const ifp = new AstIf{flp, condp};
        kit.m_dumpp->addStmtsp(ifp);
        AstCStmt* const cstmtp = new AstCStmt{flp};
        ifp->addThensp(cstmtp);
        cstmtp->add("VL_DBG_MSGS(\"         '\" + ");
        cstmtp->add(rd(dumpTagp));
        cstmtp->add(" + \"' region trigger index " + std::to_string(index) + " is active: " + text
                    + "\\n\");");
    };

    // Add sense trigger computation
    // List of trigger computation expressions
    std::vector<AstNodeExpr*> trigps;
    trigps.reserve(nSenseTriggersUsed);
    // Update statements each trigger built, as [begin, end) into the builder's results
    std::vector<std::array<size_t, 4>> updateRanges(senItemps.size(), {0, 0, 0, 0});
    // Statements to exectue at initialization time to fire initial triggers
    AstNodeStmt* initialTrigsp = nullptr;
    for (size_t i = 0; i < senItemps.size(); ++i) {
        const AstSenItem* const senItemp = senItemps[i];

        // If this is just paddign, use constant zero
        if (!senItemp) {
            trigps.emplace_back(new AstConst{flp, AstConst::BitFalse{}});
            continue;
        }

        // Create the trigger computation expression
        const size_t preBegin = senExprBuilder.preUpdateCount();
        const size_t postBegin = senExprBuilder.postUpdateCount();
        const auto& pair = senExprBuilder.build(senItemp);
        trigps.emplace_back(pair.first);
        updateRanges[i] = {preBegin, senExprBuilder.preUpdateCount(), postBegin,
                           senExprBuilder.postUpdateCount()};

        // Add initialization time trigger
        if (pair.second || v3Global.opt.xInitialEdge()) {
            const int wrdIndex = static_cast<int>(i / WORD_SIZE);
            const int bitIndex = static_cast<int>(i % WORD_SIZE);
            AstNodeExpr* const wordp = new AstArraySel{flp, wr(kit.m_vscp), wrdIndex};
            AstNodeExpr* const lhsp = new AstSel{flp, wordp, bitIndex, 1};
            AstNodeExpr* const rhsp = new AstConst{flp, AstConst::BitTrue{}};
            if (useAcc) {
                initFuncp->addStmtsp(new AstAssign{flp, lhsp, rhsp});
            } else {
                initialTrigsp = AstNode::addNext(initialTrigsp, new AstAssign{flp, lhsp, rhsp});
            }
        }

        // Add a debug statement for this trigger
        std::stringstream ss;
        ss << "@(";
        V3EmitV::verilogForTree(senItemp, ss);
        ss << ")";
        std::string desc = VString::quoteBackslash(ss.str());
        desc = VString::replaceSubstr(desc, "\n", "\\n");
        addDebug(i, desc);
    }
    UASSERT(trigps.size() == nSenseTriggersUsed, "Inconsistent number of trigger expressions");

    AstAssign* const trigStmtsp = createSenTrigVecAssignment(kit.m_vscp, trigps);

    // Add a print for each of the extra triggers
    for (unsigned i = 0; i < extraTriggers.size(); ++i) {
        addDebug(nSenseTriggersUsed + i,
                 "Internal '" + name + "' trigger - " + extraTriggers.m_descriptions.at(i));
    }

    // Construct the maps from old SenTrees to new SenTrees
    {
        std::vector<uint32_t> indices;
        indices.reserve(32);
        // Map regular SenTrees to the Sense triggers
        for (const AstSenTree* const senTreep : senTreeps) {
            indices.clear();
            for (const AstSenItem *itemp = senTreep->sensesp(), *nextp; itemp; itemp = nextp) {
                nextp = VN_AS(itemp->nextp(), SenItem);
                indices.push_back(senItem2TrigIdx.at(*itemp));
            }
            kit.m_mapVec[senTreep] = kit.newTriggerSenTree(kit.m_vscp, indices);
        }
        // Map Pre SenTrees to the Pre triggers
        for (const AstSenTree* const senTreep : preTreeps) {
            indices.clear();
            for (const AstSenItem *itemp = senTreep->sensesp(), *nextp; itemp; itemp = nextp) {
                nextp = VN_AS(itemp->nextp(), SenItem);
                indices.push_back(senItem2TrigIdx.at(*itemp) + kit.m_nVecWords * WORD_SIZE);
            }
            kit.m_mapPre[senTreep] = kit.newTriggerSenTree(kit.m_vscp, indices);
        }
    }

    // Get the SenExprBuilder results
    const SenExprBuilder::Results senResults = senExprBuilder.getResultsAndClearUpdates();

    // Add the SenExprBuilder init statements to the static initialization functino
    for (AstNodeStmt* const nodep : senResults.m_inits) initFuncp->addStmtsp(nodep);

    // Assemble the base trigger computation function
    AstScope* const scopep = netlistp->topScopep()->scopep();
    {
        AstCFunc* const fp = kit.m_compVecp;
        // Profiling push
        if (v3Global.opt.profExec()) {
            fp->addStmtsp(AstCStmt::profExecSectionPush(flp, "trigBase " + name));
        }
        // Trigger computation
        if (!useDirty) {
            for (AstNodeStmt* const nodep : senResults.m_preUpdates) fp->addStmtsp(nodep);
            fp->addStmtsp(trigStmtsp);
            for (AstNodeStmt* const nodep : senResults.m_postUpdates) fp->addStmtsp(nodep);
        } else {
            kit.addDirtyGroups(fp, initFuncp, groups, trigStmtsp, senItemps, updateRanges,
                               senResults.m_preUpdates, senResults.m_postUpdates);
        }
        // Add the initialization time triggers
        if (initialTrigsp) {
            AstVarScope* const initVscp = scopep->createTemp("__V" + name + "DidInit", 1);
            AstIf* const ifp = new AstIf{flp, new AstNot{flp, rd(initVscp)}};
            fp->addStmtsp(ifp);
            ifp->branchPred(VBranchPred::BP_UNLIKELY);
            ifp->addThensp(util::setVar(initVscp, 1));
            ifp->addThensp(initialTrigsp);
        }
        // Profiling pop
        if (v3Global.opt.profExec()) {
            fp->addStmtsp(AstCStmt::profExecSectionPop(flp, "trigBase " + name));
        }
        util::splitCheck(fp);
    };
    // If there are 'pre' triggers, compute them
    if (kit.m_nPreWords) {
        AstCFunc* const fp = kit.m_compExtp;
        // Add an argument to the function that takes the latched values
        AstVarScope* const latchedp
            = newArgument(fp, kit.m_trigVecDTypep, "latched", VDirection::CONSTREF);
        // Add loop counter variable - this can't be local because we call util::splitCheck
        AstVarScope* const nVscp = scopep->createTemp("__V" + name + "TrigPreLoopCounter", 32);
        nVscp->varp()->noReset(true);
        // Add a loop to compute the pre words
        AstLoop* const loopp = new AstLoop{flp};
        fp->addStmtsp(util::setVar(nVscp, 0));
        fp->addStmtsp(loopp);
        // Loop body
        AstNodeExpr* const offsetp = new AstConst{flp, kit.m_nVecWords};
        AstNodeExpr* const lIdxp = new AstAdd{flp, rd(nVscp), offsetp};
        AstNodeExpr* const lhsp = new AstArraySel{flp, wr(kit.m_vscp), lIdxp};
        AstNodeExpr* const aWordp = new AstArraySel{flp, rd(kit.m_vscp), rd(nVscp)};
        AstNodeExpr* const bWordp = new AstArraySel{flp, rd(latchedp), rd(nVscp)};
        AstNodeExpr* const rhsp = new AstAnd{flp, aWordp, new AstNot{flp, bWordp}};
        AstNodeExpr* const limp = new AstConst{flp, AstConst::WidthedValue{}, 32, nPreWords};
        loopp->addStmtsp(new AstAssign{flp, lhsp, rhsp});
        loopp->addStmtsp(util::incrementVar(nVscp));
        loopp->addStmtsp(new AstLoopTest{flp, loopp, new AstLt{flp, rd(nVscp), limp}});
        util::splitCheck(fp);
    }
    // Done with the trigger computation function, split as might be large

    // The debug code might leak signal names, so simply delete it when using --protect-ids
    if (v3Global.opt.protectIds()) kit.m_dumpp->stmtsp()->unlinkFrBackWithNext()->deleteTree();
    // Done with the trigger dump function, split as might be large
    util::splitCheck(kit.m_dumpp);

    return kit;
}

//  Find all CAwaits, clear SenTrees inside them, generate before-trigger functions (functions that
//  shall be called before awaiting for a VCMethod::SCHED_TRIGGER) and add thier calls before
//  proper CAwaits
class AwaitBeforeTrigVisitor final : public VNVisitor {
    const VNUser1InUse m_user1InUse;

    /**
     * AstCAwait::user1()       ->  bool.           True if node has been visited
     * AstSenTree::user1p()     ->  AstCFunc*.      Function that has to be called before awaiting
     *                                              for CAwait pointing to this SenTree
     * AstCFunc::user1p()       ->  AstVarScope*    Function's local temporary extended trigger
     *                                              vector variable scope
     */

    // Netlist - needed for using util::makeSubFunction()
    AstNetlist* const m_netlistp;
    // Trigger kit - for accessing trigger vectors and mapping senItems to thier indexes
    const TriggerKit& m_trigKit;
    // Expression builder - for building expressions from SenItems
    SenExprBuilder& m_senExprBuilder;
    // Generator of unique names for before-trigger function
    V3UniqueNames m_beforeTriggerFuncUniqueName;

    // Vector containing every generated CFuncs and related SenTree
    std::vector<std::pair<AstCFunc*, AstSenTree*>> m_generatedFuncs;
    // Vector containing SenTrees and coresponding scheduler
    std::vector<std::pair<AstSenTree*, AstNodeExpr*>> m_senTreeToSched;
    // Map containing vectors of SenItems that share the same prevValue variable
    std::unordered_map<VNRef<AstNode>, std::vector<AstSenItem*>> m_senExprToSenItem;

    // Returns node which is used for grouping SenItems in `m_senExprToSenItem`
    static AstNode* getSenHashNode(const AstSenItem* const nodep) {
        if (AstVarRef* const varRefp = VN_CAST(nodep->sensp(), VarRef)) return varRefp;
        return nodep->sensp();
    }

    // Populates `m_senExprToSenItem` with every group of SenItems that share the same prevValue
    // variable. Groups that contain only one type of an edge are omitted.
    void fillSenExprToSenItem() {
        for (auto senTreeSched : m_senTreeToSched) {
            AstSenTree* const senTreep = senTreeSched.first;

            for (AstSenItem* senItemp = senTreep->sensesp(); senItemp;
                 senItemp = VN_AS(senItemp->nextp(), SenItem)) {
                const VEdgeType edge = senItemp->edgeType();
                if (edge.anEdge() || edge == VEdgeType::ET_CHANGED
                    || edge == VEdgeType::ET_HYBRID) {
                    m_senExprToSenItem[*getSenHashNode(senItemp)].push_back(senItemp);
                }
            }
        }

        std::vector<VNRef<AstNode>> toRemove;
        for (const auto& senExprToSenTree : m_senExprToSenItem) {
            std::vector<AstSenItem*> senItemps = senExprToSenTree.second;
            toRemove.push_back(senExprToSenTree.first);
            for (size_t i = 1; i < senItemps.size(); ++i) {
                if (senItemps[i]->edgeType() != senItemps[i - 1]->edgeType()) {
                    toRemove.pop_back();
                    break;
                }
            }
        }
        for (const VNRef<AstNode> it : toRemove) m_senExprToSenItem.erase(it);
    }

    // For set of bits indexes (of sensitivity vector) return map from those indexes to set
    // of schedulers sensitive to these indexes. Indices are split into word index and bit
    // masking this index within given word
    std::map<size_t, std::map<size_t, std::vector<AstNodeExpr*>>>
    getUsedTriggersToTrees(const std::set<size_t>& usedTriggers) {
        std::map<size_t, std::map<size_t, std::vector<AstNodeExpr*>>> usedTrigsToUsingTrees;
        for (auto senTreeSched : m_senTreeToSched) {
            const AstSenTree* const senTreep = senTreeSched.first;
            AstNodeExpr* const shedp = senTreeSched.second;

            // Find all common SenItem indexes for `senTreep` and `usedTriggers`
            for (AstSenItem* senItemp = senTreep->sensesp(); senItemp;
                 senItemp = VN_AS(senItemp->nextp(), SenItem)) {
                const size_t idx = m_trigKit.senItem2TrigIdx(senItemp);
                if (usedTriggers.find(idx) != usedTriggers.end()) {
                    usedTrigsToUsingTrees[idx / TriggerKit::WORD_SIZE]
                                         [size_t{1} << (idx % TriggerKit::WORD_SIZE)]
                                             .push_back(shedp);
                }
            }
        }
        if (VL_UNLIKELY(v3Global.opt.debugCheck())) {
            for (const auto& triggersToTrees : usedTrigsToUsingTrees) {
                for (const auto& bitsToTrees : triggersToTrees.second) {
                    const std::set<const AstNodeExpr*> exprps{bitsToTrees.second.begin(),
                                                              bitsToTrees.second.end()};
                    UASSERT(bitsToTrees.second.size() == exprps.size(),
                            "There is a SenTree with two SenItems indicating to the same bit");
                }
            }
        }
        return usedTrigsToUsingTrees;
    }

    // Returns a CCall to a before-trigger function for a given SenTree,
    // Constructs such a function if it doesn't exist yet
    AstCCall* getBeforeTriggerStmt(AstSenTree* const senTreep) {
        FileLine* const flp = senTreep->fileline();
        if (!senTreep->user1p()) {
            AstCFunc* const funcp = util::makeSubFunction(
                m_netlistp, m_beforeTriggerFuncUniqueName.get(senTreep), false);
            senTreep->user1p(funcp);

            // Create a local temporary extended vector
            AstVarScope* const vscAccp = m_trigKit.vscAccp();
            AstVarScope* const tmpp = vscAccp->scopep()->createTempLike("__VTmp", vscAccp);
            AstVar* const tmpVarp = tmpp->varp()->unlinkFrBack();
            funcp->user1p(tmpp);
            funcp->addVarsp(tmpVarp);
            // This function can be called multiple times, and accesses model state, which
            // violates the assumption made in V3Life that there is no such function.
            funcp->noLife(true);
            tmpVarp->funcLocal(true);
            tmpVarp->noReset(true);

            AstVar* const argp = new AstVar{flp, VVarType::BLOCKTEMP, "__VeventDescription",
                                            senTreep->findBasicDType(VBasicDTypeKwd::CHARPTR)};
            argp->funcLocal(true);
            argp->direction(VDirection::INPUT);
            funcp->addArgsp(argp);
            // Scope is created in the constructor after iterate finishes

            m_generatedFuncs.emplace_back(funcp, senTreep);
        }
        AstCCall* const callp = new AstCCall{flp, VN_AS(senTreep->user1p(), CFunc)};
        callp->dtypeSetVoid();
        return callp;
    }

    void visit(AstCAwait* const nodep) override {
        if (nodep->user1SetOnce()) return;

        // Check whether it is a CAwait for a VCMethod::SCHED_TRIGGER
        if (const AstCMethodHard* const cMethodHardp = VN_CAST(nodep->exprp(), CMethodHard)) {
            if (cMethodHardp->method() == VCMethod::SCHED_TRIGGER) {
                AstCCall* const beforeTrigp = getBeforeTriggerStmt(nodep->sentreep());

                // Add eventDescription argument value to a CCall - it is used for --runtime-debug
                AstNode* const pinp = cMethodHardp->pinsp()->nextp()->nextp();
                UASSERT_OBJ(pinp, cMethodHardp, "No event description");
                beforeTrigp->addArgsp(VN_AS(pinp, NodeExpr)->cloneTree(false));

                // Call the before-trigger function before the CAwait
                nodep->addHereThisAsNext(beforeTrigp->makeStmt());
                m_senTreeToSched.emplace_back(nodep->sentreep(), cMethodHardp->fromp());
            }
        }
        nodep->clearSentreep();  // Clear as these sentrees will get deleted later
        iterate(nodep);
    }

    void visit(AstNode* const nodep) override { iterateChildren(nodep); }

public:
    AwaitBeforeTrigVisitor(AstNetlist* netlistp, SenExprBuilder& senExprBuilder,
                           const TriggerKit& trigKit)
        : m_netlistp{netlistp}
        , m_trigKit{trigKit}
        , m_senExprBuilder{senExprBuilder}
        , m_beforeTriggerFuncUniqueName{"__VbeforeTrig"} {
        iterate(netlistp);

        fillSenExprToSenItem();

        std::vector<AstNodeExpr*> trigps;
        std::set<size_t> usedTriggers;
        // In each of before-trigger functions check if anything was triggered and mark as ready
        // triggered schedulers
        for (const auto& funcToUsedTriggers : m_generatedFuncs) {
            AstCFunc* const funcp = funcToUsedTriggers.first;
            AstVarScope* const vscp = VN_AS(funcp->user1p(), VarScope);
            FileLine* const flp = funcp->fileline();

            // Generate trigger evaluation
            {
                AstSenTree* const senTreep = funcToUsedTriggers.second;
                // Puts `exprp` at `pos` and makes sure that trigps.size() is multiple of
                // TriggerKit::WORD_SIZE
                const auto emplaceAt
                    = [flp, &trigps, &usedTriggers](AstNodeExpr* const exprp, const size_t pos) {
                          const size_t targetSize
                              = vlstd::roundUpToMultipleOf<TriggerKit::WORD_SIZE>(pos + 1);
                          if (trigps.capacity() < targetSize) trigps.reserve(targetSize * 2);
                          while (trigps.size() < targetSize) {
                              trigps.push_back(new AstConst{flp, AstConst::BitFalse{}});
                          }
                          trigps[pos]->deleteTree();
                          trigps[pos] = exprp;
                          usedTriggers.insert(pos);
                      };

                // Find all trigger indexes of SenItems inside `senTreep`
                // and add them to `trigps` and `usedTriggers`
                for (const AstSenItem* itemp = senTreep->sensesp(); itemp;
                     itemp = VN_AS(itemp->nextp(), SenItem)) {
                    const size_t idx = m_trigKit.senItem2TrigIdx(itemp);
                    emplaceAt(m_senExprBuilder.build(itemp).first, idx);
                    auto iter = m_senExprToSenItem.find(*getSenHashNode(itemp));
                    if (iter != m_senExprToSenItem.end()) {
                        for (AstSenItem* const additionalItemp : iter->second) {
                            const size_t idx = m_trigKit.senItem2TrigIdx(additionalItemp);
                            emplaceAt(m_senExprBuilder.build(additionalItemp).first, idx);
                        }
                    }
                }

                // Fill the function with neccessary statements
                const SenExprBuilder::Results results
                    = m_senExprBuilder.getResultsAndClearUpdates();
                for (AstNodeStmt* const stmtsp : results.m_inits) funcp->addStmtsp(stmtsp);
                for (AstNodeStmt* const stmtsp : results.m_preUpdates) funcp->addStmtsp(stmtsp);
                funcp->addStmtsp(TriggerKit::createSenTrigVecAssignment(vscp, trigps));
                trigps.clear();
                for (AstNodeStmt* const stmtsp : results.m_postUpdates) funcp->addStmtsp(stmtsp);
            }

            const std::map<size_t, std::map<size_t, std::vector<AstNodeExpr*>>>
                usedTrigsToUsingTrees = getUsedTriggersToTrees(usedTriggers);
            usedTriggers.clear();

            // Helper returning expression getting array index `idx` from `scocep` with access
            // `access`
            const auto getIdx = [flp](AstVarScope* const scocep, VAccess access, size_t idx) {
                return new AstArraySel{flp, new AstVarRef{flp, scocep, access},
                                       new AstConst{flp, AstConst::Unsized64{}, idx}};
            };

            // Get eventDescription argument
            AstVarScope* const argpVscp = new AstVarScope{flp, funcp->scopep(), funcp->argsp()};
            funcp->scopep()->addVarsp(argpVscp);

            // Mark as ready triggered schedulers
            for (const auto& triggersToTrees : usedTrigsToUsingTrees) {
                const size_t word = triggersToTrees.first;

                for (const auto& bitsToTrees : triggersToTrees.second) {
                    const size_t bit = bitsToTrees.first;
                    const auto& schedulers = bitsToTrees.second;

                    // Check if given bit is fired - single bits are checked since
                    // usually there is only a few of them (only one most of the times as we await
                    // only for one event)
                    AstConst* const maskConstp = new AstConst{flp, AstConst::Unsized64{}, bit};
                    AstAnd* const condp
                        = new AstAnd{flp, getIdx(vscp, VAccess::READ, word), maskConstp};
                    AstIf* const ifp = new AstIf{flp, condp};

                    // Call ready() on each scheduler sensitive to `condp`
                    for (AstNodeExpr* const schedp : schedulers) {
                        AstCMethodHard* const callp = new AstCMethodHard{
                            flp, schedp->cloneTree(false), VCMethod::SCHED_READY};
                        callp->dtypeSetVoid();
                        callp->addPinsp(new AstVarRef{flp, argpVscp, VAccess::READ});
                        ifp->addThensp(callp->makeStmt());
                    }
                    funcp->addStmtsp(ifp);
                }
            }

            AstVarScope* const vscAccp = m_trigKit.vscAccp();
            // Add touched values to accumulator
            for (const auto& triggersToTrees : usedTrigsToUsingTrees) {
                const size_t word = triggersToTrees.first;
                funcp->addStmtsp(new AstAssign{flp, getIdx(vscAccp, VAccess::WRITE, word),
                                               new AstOr{flp, getIdx(vscAccp, VAccess::READ, word),
                                                         getIdx(vscp, VAccess::READ, word)}});
            }
        }
    }
    ~AwaitBeforeTrigVisitor() override = default;
};

void beforeTrigVisitor(AstNetlist* netlistp, SenExprBuilder& senExprBuilder,
                       const TriggerKit& trigKit) {
    AwaitBeforeTrigVisitor{netlistp, senExprBuilder, trigKit};
}


void TriggerKit::addDirtyGroups(AstCFunc* fp, AstCFunc* initFuncp,
                                const std::vector<DirtyGroup>& groups, AstAssign* wordStmtsp,
                                const std::vector<const AstSenItem*>& senItemps,
                                const std::vector<std::array<size_t, 4>>& updateRanges,
                                const std::vector<AstNodeStmt*>& preUpdates,
                                const std::vector<AstNodeStmt*>& postUpdates) {
    AstNetlist* const netlistp = v3Global.rootp();
    AstScope* const scopep = netlistp->topScopep()->scopep();
    FileLine* const flp = scopep->fileline();
    const auto wr = [flp](AstVarScope* vp) { return new AstVarRef{flp, vp, VAccess::WRITE}; };

    AstNodeDType* const flagDTypep = netlistp->findBitDType(8, 8, VSigning::UNSIGNED);
    AstRange* const rp = new AstRange{flp, static_cast<int>(groups.size() - 1), 0};
    AstUnpackArrayDType* const dirtyDTypep = new AstUnpackArrayDType{flp, flagDTypep, rp};
    netlistp->typeTablep()->addTypesp(dirtyDTypep);
    m_dirtyVscp = scopep->createTemp("__V" + m_name + "TrigDirty", dirtyDTypep);
    m_dirtyVscp->varp()->isInternal(true);
    m_dirtyVscp->varp()->noReset(true);
    const auto flagp = [&](size_t g, VAccess access) -> AstNodeExpr* {
        return new AstArraySel{flp, new AstVarRef{flp, m_dirtyVscp, access},
                               static_cast<int>(g)};
    };
    const auto setFlag = [&](size_t g, uint32_t value) {
        return new AstAssign{flp, flagp(g, VAccess::WRITE),
                             new AstConst{flp, AstConst::WidthedValue{}, 8, value}};
    };
    // Everything is dirty before the first evaluation
    for (size_t g = 0; g < groups.size(); ++g) initFuncp->addStmtsp(setFlag(g, 1));

    // Word assignments, by word index
    std::vector<AstAssign*> words;
    for (AstAssign *nodep = wordStmtsp, *nextp; nodep; nodep = nextp) {
        nextp = VN_AS(nodep->nextp(), Assign);
        if (nextp) nextp->unlinkFrBackWithNext();
        words.push_back(nodep);
    }
    std::vector<size_t> groupOfWord(words.size(), 0);
    uint32_t nextWord = 0;
    for (size_t g = 0; g < groups.size(); ++g) {
        UASSERT(groups[g].m_firstWord == nextWord && groups[g].m_nWords > 0,
                "Trigger groups must tile the trigger words in order");
        nextWord += groups[g].m_nWords;
        for (uint32_t w = 0; w < groups[g].m_nWords; ++w) {
            groupOfWord.at(groups[g].m_firstWord + w) = g;
        }
    }
    UASSERT(nextWord == words.size(), "Trigger groups must cover every trigger word");
    // Update statements of the group whose trigger built them
    std::vector<AstNodeStmt*> groupPre(groups.size(), nullptr);
    std::vector<AstNodeStmt*> groupPost(groups.size(), nullptr);
    for (size_t i = 0; i < updateRanges.size(); ++i) {
        const size_t g = groupOfWord.at(i / WORD_SIZE);
        const auto& range = updateRanges[i];
        for (size_t j = range[0]; j < range[1]; ++j) {
            groupPre[g] = AstNode::addNext(groupPre[g], preUpdates[j]);
        }
        for (size_t j = range[2]; j < range[3]; ++j) {
            groupPost[g] = AstNode::addNext(groupPost[g], postUpdates[j]);
        }
    }
    // Same order as without groups, all pre updates, all words, then all post updates, since
    // groups over the same variables share 'prev' and 'curr' values
    const auto addGuarded = [&](size_t g, AstNode* thensp, AstNode* elsesp) {
        if (!thensp && !elsesp) return;
        if (groups[g].m_always) {
            if (thensp) fp->addStmtsp(thensp);
            return;
        }
        AstIf* const ifp = new AstIf{flp, flagp(g, VAccess::READ)};
        if (thensp) ifp->addThensp(thensp);
        if (elsesp) ifp->addElsesp(elsesp);
        fp->addStmtsp(ifp);
    };
    for (size_t g = 0; g < groups.size(); ++g) addGuarded(g, groupPre[g], nullptr);
    for (size_t g = 0; g < groups.size(); ++g) {
        AstNode* thensp = nullptr;
        AstNode* elsesp = nullptr;
        for (uint32_t w = 0; w < groups[g].m_nWords; ++w) {
            const int index = static_cast<int>(groups[g].m_firstWord + w);
            thensp = AstNode::addNext(thensp, words[index]);
            AstNodeExpr* const lhsp = new AstArraySel{flp, wr(m_vscp), index};
            AstConst* const zerop = new AstConst{flp, AstConst::WidthedValue{}, WORD_SIZE, 0};
            elsesp = AstNode::addNext(elsesp, new AstAssign{flp, lhsp, zerop});
        }
        addGuarded(g, thensp, elsesp);
    }
    for (size_t g = 0; g < groups.size(); ++g) {
        AstNode* const thensp = groups[g].m_always
                                    ? groupPost[g]
                                    : AstNode::addNext(groupPost[g], setFlag(g, 0));
        addGuarded(g, thensp, nullptr);
    }

    // Which groups each variable feeds
    for (size_t i = 0; i < senItemps.size(); ++i) {
        if (!senItemps[i]) continue;
        const uint32_t g = static_cast<uint32_t>(groupOfWord.at(i / WORD_SIZE));
        if (groups[g].m_always) continue;
        senItemps[i]->sensp()->foreach([&](const AstVarRef* refp) {
            std::vector<uint32_t>& gs = m_dirtyGroups[refp->varScopep()];
            if (gs.empty() || gs.back() != g) gs.push_back(g);
        });
    }
    for (auto& pair : m_dirtyGroups) {
        std::sort(pair.second.begin(), pair.second.end());
        pair.second.erase(std::unique(pair.second.begin(), pair.second.end()), pair.second.end());
    }
}

void TriggerKit::addDirtyMarks(AstNetlist* netlistp) const {
    if (!m_dirtyVscp) return;
    FileLine* const flp = m_dirtyVscp->fileline();
    size_t nSites = 0;
    size_t nMarks = 0;
    // AstNodeVarRef::user1() -> bool: write already marked by its own value check
    const VNUser1InUse user1InUse;
    const auto groupsWritten = [&](const AstNode* nodep, std::vector<uint32_t>& gs) {
        nodep->foreach([&](const AstNodeVarRef* refp) {
            if (!refp->varScopep() || !refp->access().isWriteOrRW() || refp->user1()) return;
            const auto it = m_dirtyGroups.find(refp->varScopep());
            if (it != m_dirtyGroups.end()) gs.insert(gs.end(), it->second.begin(), it->second.end());
        });
    };
    const auto uniq = [](std::vector<uint32_t>& gs) {
        std::sort(gs.begin(), gs.end());
        gs.erase(std::unique(gs.begin(), gs.end()), gs.end());
    };
    const auto newMarks = [&](const std::vector<uint32_t>& gs) {
        AstNode* marksp = nullptr;
        for (const uint32_t g : gs) {
            AstNodeExpr* const lhsp = new AstArraySel{
                flp, new AstVarRef{flp, m_dirtyVscp, VAccess::WRITE}, static_cast<int>(g)};
            marksp = AstNode::addNext(
                marksp, new AstAssign{flp, lhsp, new AstConst{flp, AstConst::WidthedValue{}, 8, 1}});
        }
        nMarks += gs.size();
        ++nSites;
        return marksp;
    };

    // Collected first, as inserting statements while the tree is being walked is not allowed
    std::vector<AstCFunc*> funcps;
    netlistp->foreach([&](AstCFunc* funcp) { funcps.push_back(funcp); });

    // Coroutine code only runs inside a scheduler resume, or when a coroutine is started, so
    // what any coroutine writes is marked right after each of those
    // Fork bodies count too: they only become coroutine functions after scheduling
    std::vector<uint32_t> coroGroups;
    for (const AstCFunc* const funcp : funcps) {
        if (funcp->isCoroutine() || funcp->exists([](const AstCAwait*) { return true; })) {
            groupsWritten(funcp, coroGroups);
        } else {
            funcp->foreach([&](const AstFork* forkp) { groupsWritten(forkp, coroGroups); });
        }
    }
    uniq(coroGroups);
    if (!coroGroups.empty()) {
        std::vector<AstNode*> sitesp;
        for (AstCFunc* const funcp : funcps) {
            funcp->foreach([&](AstNode* nodep) {
                bool resumes = false;
                if (const AstCMethodHard* const methodp = VN_CAST(nodep, CMethodHard)) {
                    resumes = methodp->method() == VCMethod::SCHED_RESUME
                              || methodp->method() == VCMethod::SCHED_RESUME_ZERO_DELAY
                              || methodp->method() == VCMethod::FORK_DONE;
                } else if (const AstNodeCCall* const callp = VN_CAST(nodep, NodeCCall)) {
                    resumes = callp->funcp()->isCoroutine() && !VN_IS(callp->backp(), CAwait);
                }
                if (!resumes) return;
                AstNode* stmtp = nodep;
                while (stmtp && !VN_IS(stmtp, NodeStmt)) stmtp = stmtp->abovep();
                UASSERT_OBJ(stmtp, nodep, "Resumption outside of a statement");
                sitesp.push_back(stmtp);
            });
        }
        std::sort(sitesp.begin(), sitesp.end());
        sitesp.erase(std::unique(sitesp.begin(), sitesp.end()), sitesp.end());
        for (AstNode* const stmtp : sitesp) stmtp->addNextHere(newMarks(coroGroups));
    }

    // Functions that (transitively) compute the triggers
    std::unordered_set<const AstCFunc*> computes{m_compVecp};
    for (bool changed = true; changed;) {
        changed = false;
        for (const AstCFunc* const funcp : funcps) {
            if (computes.count(funcp)) continue;
            if (funcp->exists([&](const AstNodeCCall* callp) {
                    return computes.count(callp->funcp()) > 0;
                })) {
                computes.insert(funcp);
                changed = true;
            }
        }
    }

    // Packed writes mark only when the written value changes: latches and combinational
    // logic rewrite equal values every time they run, which would keep their groups always dirty
    std::map<const AstNodeDType*, AstVarScope*> oldTemps;
    AstScope* const topScopep = netlistp->topScopep()->scopep();
    for (AstCFunc* const funcp : funcps) {
        if (funcp == m_compVecp || funcp->isCoroutine() || computes.count(funcp)) continue;
        std::vector<AstNodeAssign*> assignps;
        // The written variable under element, word and bit selects with side-effect free indices
        const auto baseRef = [](AstNodeExpr* lhsp) -> AstVarRef* {
            while (true) {
                if (AstArraySel* const selp = VN_CAST(lhsp, ArraySel)) {
                    lhsp = selp->fromp();
                } else if (AstWordSel* const selp = VN_CAST(lhsp, WordSel)) {
                    lhsp = selp->fromp();
                } else if (AstSel* const selp = VN_CAST(lhsp, Sel)) {
                    lhsp = selp->fromp();
                } else {
                    return VN_CAST(lhsp, VarRef);
                }
            }
        };
        funcp->foreach([&](AstNodeAssign* assignp) {
            if (!VN_IS(assignp, Assign) && !VN_IS(assignp, AssignW)) return;
            const AstVarRef* const refp = baseRef(assignp->lhsp());
            if (!refp || !m_dirtyGroups.count(refp->varScopep())) return;
            // The written part is re-read after the write, so its indices must not change by it
            if (!assignp->lhsp()->isPure() || !assignp->rhsp()->isPure()) return;
            size_t nRefs = 0;
            assignp->lhsp()->foreach([&](const AstNodeVarRef* vrefp) {
                if (vrefp->varScopep() == refp->varScopep()) ++nRefs;
            });
            if (nRefs != 1) return;
            // Wide values too: a wide copy and compare is far cheaper than recomputing the groups
            const AstNodeDType* const dtypep = assignp->lhsp()->dtypep()->skipRefp();
            if (!dtypep->isIntegralOrPacked() || VN_IS(dtypep, UnpackArrayDType)) return;
            assignps.push_back(assignp);
        });
        for (AstNodeAssign* const assignp : assignps) {
            AstVarRef* const refp = baseRef(assignp->lhsp());
            AstVarScope* const vscp = refp->varScopep();
            AstNodeDType* const dtypep = assignp->lhsp()->dtypep();
            // The written part, read back before and after the write
            const auto readLhs = [&]() {
                AstNodeExpr* const rdp = assignp->lhsp()->cloneTreePure(false);
                rdp->foreach([](AstNodeVarRef* vrefp) { vrefp->access(VAccess::READ); });
                return rdp;
            };
            AstVarScope*& oldp = oldTemps[dtypep];
            if (!oldp) {
                oldp = topScopep->createTemp("__V" + m_name + "TrigDirtyOld"
                                                 + std::to_string(oldTemps.size()),
                                             dtypep);
                oldp->varp()->isInternal(true);
                oldp->varp()->noReset(true);
            }
            FileLine* const aflp = assignp->fileline();
            assignp->addHereThisAsNext(
                new AstAssign{aflp, new AstVarRef{aflp, oldp, VAccess::WRITE}, readLhs()});
            AstNodeExpr* const condp
                = new AstNeq{aflp, new AstVarRef{aflp, oldp, VAccess::READ}, readLhs()};
            condp->dtypeSetBit();
            assignp->addNextHere(new AstIf{aflp, condp, newMarks(m_dirtyGroups.at(vscp))});
            refp->user1(true);
        }
    }

    // Other functions mark what they write on entry, and again after any trigger computation
    // they reach, since a computation in between clears the flags
    for (AstCFunc* const funcp : funcps) {
        if (funcp == m_compVecp || funcp->isCoroutine()) continue;
        std::vector<uint32_t> gs;
        groupsWritten(funcp, gs);
        if (gs.empty()) continue;
        uniq(gs);
        if (computes.count(funcp)) {
            std::vector<AstNode*> sitesp;
            funcp->foreach([&](AstNodeCCall* callp) {
                if (!computes.count(callp->funcp())) return;
                AstNode* stmtp = callp;
                while (stmtp && !VN_IS(stmtp, NodeStmt)) stmtp = stmtp->abovep();
                UASSERT_OBJ(stmtp, callp, "Call outside of a statement");
                sitesp.push_back(stmtp);
            });
            std::sort(sitesp.begin(), sitesp.end());
            sitesp.erase(std::unique(sitesp.begin(), sitesp.end()), sitesp.end());
            for (AstNode* const stmtp : sitesp) stmtp->addNextHere(newMarks(gs));
        }
        AstNode* const marksp = newMarks(gs);
        if (AstNode* const stmtsp = funcp->stmtsp()) {
            stmtsp->addHereThisAsNext(marksp);
        } else {
            funcp->addStmtsp(marksp);
        }
    }
    V3Stats::addStat("Scheduling, '" + m_name + "' dirty trigger marking sites", nSites);
    V3Stats::addStat("Scheduling, '" + m_name + "' dirty trigger marks", nMarks);
}

}  // namespace V3Sched
