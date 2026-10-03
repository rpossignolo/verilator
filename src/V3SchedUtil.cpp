// -*- mode: C++; c-file-style: "cc-mode" -*-
//*************************************************************************
// DESCRIPTION: Verilator: Utility functions used by code scheduling
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

#include <algorithm>

VL_DEFINE_DEBUG_FUNCTIONS;

namespace V3Sched {
namespace util {

AstCFunc* makeSubFunction(AstNetlist* netlistp, const string& name, bool slow) {
    AstScope* const scopeTopp = netlistp->topScopep()->scopep();
    AstCFunc* const funcp = new AstCFunc{netlistp->fileline(), name, scopeTopp, ""};
    funcp->dontCombine(true);
    funcp->isStatic(false);
    funcp->isLoose(true);
    funcp->slow(slow);
    funcp->isConst(false);
    funcp->declPrivate(true);
    scopeTopp->addBlocksp(funcp);
    return funcp;
}

AstCFunc* makeTopFunction(AstNetlist* netlistp, const string& name, bool slow) {
    AstCFunc* const funcp = makeSubFunction(netlistp, name, slow);
    funcp->entryPoint(true);
    funcp->keepIfEmpty(true);
    return funcp;
}

AstNodeStmt* setVar(AstVarScope* vscp, uint32_t val) {
    FileLine* const flp = vscp->fileline();
    AstVarRef* const refp = new AstVarRef{flp, vscp, VAccess::WRITE};
    AstConst* const valp = new AstConst{flp, AstConst::DTyped{}, vscp->dtypep()};
    valp->num().setLong(val);
    return new AstAssign{flp, refp, valp};
}

AstNodeStmt* incrementVar(AstVarScope* vscp) {
    FileLine* const flp = vscp->fileline();
    AstVarRef* const wrefp = new AstVarRef{flp, vscp, VAccess::WRITE};
    AstVarRef* const rrefp = new AstVarRef{flp, vscp, VAccess::READ};
    AstConst* const onep = new AstConst{flp, AstConst::DTyped{}, vscp->dtypep()};
    onep->num().setLong(1);
    return new AstAssign{flp, wrefp, new AstAdd{flp, rrefp, onep}};
}

AstNodeStmt* callVoidFunc(AstCFunc* funcp) {
    if (!funcp) return nullptr;
    AstCCall* const callp = new AstCCall{funcp->fileline(), funcp};
    callp->dtypeSetVoid();
    return callp->makeStmt();
}

AstNodeStmt* checkIterationLimit(AstNetlist* netlistp, const string& name, AstVarScope* counterp,
                                 AstNodeStmt* dumpCallp) {
    FileLine* const flp = netlistp->fileline();

    // If we exceeded the iteration limit, die
    const uint32_t limit = v3Global.opt.convergeLimit();
    AstVarRef* const counterRefp = new AstVarRef{flp, counterp, VAccess::READ};
    AstConst* const constp = new AstConst{flp, AstConst::DTyped{}, counterp->dtypep()};
    constp->num().setLong(limit);
    AstNodeExpr* const condp = new AstGt{flp, counterRefp, constp};
    AstIf* const ifp = new AstIf{flp, condp};
    ifp->branchPred(VBranchPred::BP_UNLIKELY);
    if (dumpCallp) ifp->addThensp(dumpCallp);
    AstCStmt* const stmtp = new AstCStmt{flp};
    ifp->addThensp(stmtp);
    const FileLine* const locp = netlistp->topModulep()->fileline();
    const std::string& file = VIdProtect::protect(locp->filename());
    const std::string& line = std::to_string(locp->lineno());
    stmtp->add("VL_FATAL_MT(\"" + V3OutFormatter::quoteNameControls(file) + "\", " + line
               + ", \"\", \"DIDNOTCONVERGE: " + name
               + " region did not converge after '--converge-limit' of " + std::to_string(limit)
               + " tries\");");
    return ifp;
}

static AstCFunc* splitCheckCreateNewSubFunc(AstCFunc* ofuncp) {
    static std::map<AstCFunc*, uint32_t> s_funcNums;  // What split number to attach to a function
    const uint32_t funcNum = s_funcNums[ofuncp]++;
    const std::string name = ofuncp->name() + "__" + cvtToStr(funcNum);
    AstScope* const scopep = ofuncp->scopep();
    AstCFunc* const subFuncp = new AstCFunc{ofuncp->fileline(), name, scopep};
    scopep->addBlocksp(subFuncp);
    subFuncp->dontCombine(true);
    subFuncp->isStatic(ofuncp->isStatic());
    subFuncp->isLoose(true);
    subFuncp->slow(ofuncp->slow());
    subFuncp->declPrivate(ofuncp->declPrivate());
    if (ofuncp->needProcess()) subFuncp->setNeedProcess();
    for (AstVar* argp = ofuncp->argsp(); argp; argp = VN_AS(argp->nextp(), Var)) {
        AstVar* const clonep = argp->cloneTree(false);
        subFuncp->addArgsp(clonep);
        AstVarScope* const vscp = new AstVarScope{clonep->fileline(), scopep, clonep};
        scopep->addVarsp(vscp);
        argp->user3p(vscp);
    }
    return subFuncp;
};

void splitCheckFinishSubFunc(AstCFunc* ofuncp, AstCFunc* subFuncp,
                             const std::unordered_map<const AstVar*, AstVarScope*>& argVscps) {
    FileLine* const flp = subFuncp->fileline();
    AstCCall* const callp = new AstCCall{subFuncp->fileline(), subFuncp};
    callp->dtypeSetVoid();
    // Pass arguments through to subfunction
    for (AstVar* argp = ofuncp->argsp(); argp; argp = VN_AS(argp->nextp(), Var)) {
        UASSERT_OBJ(argp->direction() == VDirection::CONSTREF, argp, "Unexpected direction");
        callp->addArgsp(new AstVarRef{flp, argVscps.at(argp), VAccess::READ});
    }

    bool containsAwait = false;
    subFuncp->foreach([&](AstNode* nodep) {
        // Record if it has a CAwait
        if (VN_IS(nodep, CAwait)) containsAwait = true;
        // Redirect references to arguments to the clone in the sub-function
        if (AstVarRef* const refp = VN_CAST(nodep, VarRef)) {
            if (AstVarScope* const vscp = VN_AS(refp->varp()->user3p(), VarScope)) {
                refp->varp(vscp->varp());
                refp->varScopep(vscp);
            }
        }
    });

    if (ofuncp->isCoroutine() && containsAwait) {  // Wrap call with co_await
        subFuncp->rtnType("VlCoroutine");
        ofuncp->addStmtsp(new AstCAwait{flp, callp});
    } else {
        ofuncp->addStmtsp(callp->makeStmt());
    }
}

// Compute, for each top level statement of 'ofuncp', whether a new sub-function may begin there.
// Sub-functions are emitted as separate C++ functions, so they cannot see each other's automatic
// storage. A function-local AstVar declared among the top level statements must therefore end up
// in the same sub-function as every reference to it, otherwise the emitted C++ refers to an
// undeclared identifier (and the AstVar can be deleted from under the still-live references).
static std::vector<bool> splitCheckBreakable(const AstCFunc* ofuncp) {
    // Gather the top level statements, and where each locally declared variable is declared
    std::vector<const AstNode*> stmtps;
    std::unordered_map<const AstVar*, size_t> declIdx;  // Local var -> index it is declared at
    for (const AstNode* nodep = ofuncp->stmtsp(); nodep; nodep = nodep->nextp()) {
        if (const AstVar* const varp = VN_CAST(nodep, Var)) declIdx.emplace(varp, stmtps.size());
        stmtps.push_back(nodep);
    }

    // Find the last statement referencing each locally declared variable
    std::vector<size_t> lastUse(stmtps.size());
    for (size_t i = 0; i < stmtps.size(); ++i) {
        lastUse[i] = i;  // A declaration is live at least where it is declared
        stmtps[i]->foreach([&](const AstNodeVarRef* refp) {
            const auto it = declIdx.find(refp->varp());  // 'end()' if not one of our locals
            if (it != declIdx.end()) lastUse[it->second] = std::max(lastUse[it->second], i);
        });
    }

    // A break before statement 'i' is allowed only if no local declared before 'i' is still live
    std::vector<bool> breakable(stmtps.size(), true);
    size_t liveEnd = 0;  // Last index any so far declared local is referenced at
    for (size_t i = 0; i < stmtps.size(); ++i) {
        breakable[i] = liveEnd < i;
        if (VN_IS(stmtps[i], Var)) liveEnd = std::max(liveEnd, lastUse[i]);
    }
    return breakable;
}

// Split large function according to --output-split-cfuncs
void splitCheck(AstCFunc* const ofuncp) {
    if (!ofuncp) return;
    UASSERT_OBJ(!ofuncp->varsp(), ofuncp, "Can't split function with local variables");
    if (!v3Global.opt.outputSplitCFuncs() || !ofuncp->stmtsp()) return;
    if (ofuncp->nodeCount() < v3Global.opt.outputSplitCFuncs()) return;

    // Statement boundaries that would separate a local declaration from a reference to it
    const std::vector<bool> breakable = splitCheckBreakable(ofuncp);

    // Need to find the AstVarScopes for the function arguments. They should be in the same Scope.
    std::unordered_map<const AstVar*, AstVarScope*> argVscps;
    for (AstVar* argp = ofuncp->argsp(); argp; argp = VN_AS(argp->nextp(), Var)) {
        UASSERT_OBJ(argVscps.size() < 2, argp, "There should be at most 2 arguments, or O(n^2)");
        bool found = false;
        for (AstVarScope *vscp = ofuncp->scopep()->varsp(), *nextp; vscp; vscp = nextp) {
            nextp = VN_AS(vscp->nextp(), VarScope);
            if (vscp->varp() != argp) continue;
            argVscps[argp] = vscp;
            found = true;
            break;
        }
        UASSERT_OBJ(found, argp, "Can't find VarScope for function argument");
    }

    // AstVar::user3p(): AstVarScope for function argument in clone
    const VNUser3InUse user3InUse;

    size_t size = 0;
    AstCFunc* subFuncp = nullptr;

    // Move statements one by one to the new sub-functions
    AstNode* stmtsp = ofuncp->stmtsp()->unlinkFrBackWithNext();
    for (size_t index = 0; AstNode* const itemp = stmtsp; ++index) {
        stmtsp = stmtsp->nextp();
        if (stmtsp) stmtsp->unlinkFrBackWithNext();
        const size_t itemSize = static_cast<size_t>(itemp->nodeCount());
        size += itemSize;

        if (size > static_cast<size_t>(v3Global.opt.outputSplitCFuncs()) && breakable[index]) {
            if (subFuncp) splitCheckFinishSubFunc(ofuncp, subFuncp, argVscps);
            subFuncp = nullptr;
            size = itemSize;
        }

        if (!subFuncp) subFuncp = splitCheckCreateNewSubFunc(ofuncp);
        subFuncp->addStmtsp(itemp);
    }
    if (subFuncp) splitCheckFinishSubFunc(ofuncp, subFuncp, argVscps);
}

// Build an AstIf conditional on the given SenTree being triggered
AstIf* createIfFromSenTree(AstSenTree* senTreep) {
    senTreep = VN_AS(V3Const::constifyExpensiveEdit(senTreep), SenTree);
    UASSERT_OBJ(senTreep->sensesp(), senTreep, "No sensitivity list during scheduling");
    // Convert the SenTree to a boolean expression that is true when triggered
    AstNodeExpr* senEqnp = nullptr;
    for (AstSenItem *senp = senTreep->sensesp(), *nextp; senp; senp = nextp) {
        nextp = VN_AS(senp->nextp(), SenItem);
        // They should all be ET_TRUE, as set up by V3Sched
        UASSERT_OBJ(senp->edgeType() == VEdgeType::ET_TRUE, senp, "Bad scheduling trigger type");
        AstNodeExpr* const senOnep = senp->sensp()->cloneTree(false);
        senEqnp = senEqnp ? new AstOr{senp->fileline(), senEqnp, senOnep} : senOnep;
    }
    // Create the if statement conditional on the triggers
    return new AstIf{senTreep->fileline(), senEqnp};
}

AstNodeStmt* addTriggerWordSkips(AstNodeStmt* stmtsp) {
    // A run is wrapped while its words stay few, so the outer test stays cheaper than the guards
    constexpr size_t MAX_WORDS = 16;
    constexpr size_t MAX_STMTS = 64;
    struct Word final {
        AstVarScope* m_vscp;
        uint32_t m_index;
        bool operator==(const Word& other) const {
            return m_vscp == other.m_vscp && m_index == other.m_index;
        }
    };
    // Adds the trigger words an AstIf condition reads, if it only tests trigger bits
    const auto addWords = [](const AstNode* stmtp, std::vector<Word>& words) {
        const AstIf* const ifp = VN_CAST(stmtp, If);
        if (!ifp) return false;
        bool pure = true;
        bool any = false;
        ifp->condp()->foreach([&](const AstNode* nodep) {
            if (!pure) return;
            if (const AstArraySel* const selp = VN_CAST(nodep, ArraySel)) {
                const AstVarRef* const refp = VN_CAST(selp->fromp(), VarRef);
                const AstConst* const idxp = VN_CAST(selp->bitp(), Const);
                if (!refp || !idxp) {
                    pure = false;
                    return;
                }
                any = true;
                const Word word{refp->varScopep(), idxp->toUInt()};
                if (std::find(words.begin(), words.end(), word) == words.end()) {
                    words.push_back(word);
                }
            } else if (!VN_IS(nodep, And) && !VN_IS(nodep, Or) && !VN_IS(nodep, Const)
                       && !VN_IS(nodep, VarRef)) {
                pure = false;
            }
        });
        return pure && any;
    };
    std::vector<AstNodeStmt*> stmts;
    for (AstNodeStmt *nodep = stmtsp, *nextp; nodep; nodep = nextp) {
        nextp = VN_AS(nodep->nextp(), NodeStmt);
        if (nextp) nextp->unlinkFrBackWithNext();
        stmts.push_back(nodep);
    }
    AstNodeStmt* resultp = nullptr;
    size_t i = 0;
    while (i < stmts.size()) {
        std::vector<Word> runWords;
        size_t j = i;
        while (j < stmts.size() && j - i < MAX_STMTS) {
            std::vector<Word> words = runWords;
            if (!addWords(stmts[j], words) || words.size() > MAX_WORDS) break;
            runWords = std::move(words);
            ++j;
        }
        if (j - i < 2) {
            resultp = AstNode::addNext(resultp, stmts[i]);
            ++i;
            continue;
        }
        FileLine* const flp = stmts[i]->fileline();
        AstNodeExpr* anyp = nullptr;
        for (const Word& word : runWords) {
            AstNodeExpr* const wordp
                = new AstArraySel{flp, new AstVarRef{flp, word.m_vscp, VAccess::READ},
                                  static_cast<int>(word.m_index)};
            anyp = anyp ? new AstOr{flp, anyp, wordp} : wordp;
        }
        AstNodeExpr* const condp = new AstNeq{
            flp, anyp, new AstConst{flp, AstConst::WidthedValue{}, anyp->width(), 0}};
        condp->dtypeSetBit();
        AstIf* const ifp = new AstIf{flp, condp};
        for (size_t k = i; k < j; ++k) ifp->addThensp(stmts[k]);
        resultp = AstNode::addNext<AstNodeStmt, AstNodeStmt>(resultp, ifp);
        i = j;
    }
    return resultp;
}

}  // namespace util
}  // namespace V3Sched
