// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include <functional>

#include "Fixpoint.h"
#include "PredefContainers.h"
#include "PointsTo.h"

namespace llvm { namespace ptr {

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    VARIABLE<const llvm::Value *>,
		    VARIABLE<const llvm::Value *>
		    > const& E) {
    struct local {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval) {
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    PointsToSet const& R = S[rval];
	    std::size_t const old_size = L.size();
	    std::copy(R.begin(),R.end(),std::inserter(L,L.end()));
	    return old_size != L.size();
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument(),
		E.getArgument2().getArgument());
}

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    VARIABLE<const llvm::Value *>,
		    REFERENCE<
			VARIABLE<const llvm::Value *> >
		    > const& E) {
    struct local
    {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval) {
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    std::size_t const old_size = L.size();
	    L.insert(rval);
	    return old_size != L.size();
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument(),
		E.getArgument2().getArgument().getArgument());
}

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    VARIABLE<const llvm::Value *>,
		    DEREFERENCE< VARIABLE<const llvm::Value *> >
		    > const& E)
{
    struct local
    {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval) {
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    PointsToSet& R = S[rval];
	    std::size_t const old_size = L.size();
	    for (PointsToSet::const_iterator i = R.begin(); i!=R.end(); ++i) {
		PointsToSet& X = S[*i];
		std::copy(X.begin(),X.end(),std::inserter(L,L.end()));
	    }
	    return old_size != L.size();
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument(),
		E.getArgument2().getArgument().getArgument());
}

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    DEREFERENCE< VARIABLE<const llvm::Value *> >,
		    VARIABLE<const llvm::Value *>
		    > const& E)
{
    struct local
    {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval) {
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    PointsToSet& R = S[rval];
	    bool change = false;
	    for (PointsToSet::const_iterator i = L.begin(); i!=L.end(); ++i) {
		PointsToSet& X = S[*i];
		std::size_t const old_size = X.size();
		std::copy(R.begin(),R.end(),std::inserter(X,X.end()));
		change = change || X.size() != old_size;
	    }
	    return change;
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument().getArgument(),
		E.getArgument2().getArgument());
}

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    DEREFERENCE<
			VARIABLE<const llvm::Value *> >,
		    REFERENCE<
			VARIABLE<const llvm::Value *> >
		    > const &E)
{
    struct local
    {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval
			     )
	{
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    bool change = false;
	    for (PointsToSet::const_iterator i = L.begin(); i!=L.end(); ++i) {
		PointsToSet& X = S[*i];
		std::size_t const old_size = X.size();
		X.insert(rval);
		change = change || X.size() != old_size;
	    }
	    return change;
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument().getArgument(),
		E.getArgument2().getArgument().getArgument());
}

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    DEREFERENCE<
			VARIABLE<const llvm::Value *> >,
		    DEREFERENCE<
			VARIABLE<const llvm::Value *> >
		    > const& E)
{
    struct local {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval) {
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    bool change = false;
	    for (PointsToSet::const_iterator i = L.begin(); i!=L.end(); ++i)
		if (getRuleFunction(
			(ruleVar(*i) = *ruleVar(rval)).getSort())
			(S))
		    change = true;
	    return change;
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument().getArgument(),
		E.getArgument2().getArgument().getArgument());
}

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    VARIABLE<const llvm::Value *>,
		    ALLOC<const llvm::Value *>
		    > const &E)
{
    struct local
    {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval) {
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    std::size_t const old_size = L.size();
	    L.insert(rval);
	    return old_size != L.size();
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument(),
		E.getArgument2().getArgument());
}

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    VARIABLE<const llvm::Value *>,
		    NULLPTR<const llvm::Value *>
		    > const &E)
{
    struct local
    {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval) {
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    std::size_t const old_size = L.size();
	    L.insert(rval);
	    return old_size != L.size();
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument(),
		E.getArgument2().getArgument());
}

RuleFunction::Type getRuleFunction(ASSIGNMENT<
		    DEREFERENCE<
			VARIABLE<const llvm::Value *> >,
		    NULLPTR<const llvm::Value *>
		    > const &E)
{
    struct local
    {
	static bool function(PointsToSets<ANDERSEN>::Type &S,
			     const llvm::Value *lval,
			     const llvm::Value *rval) {
	    typedef PointsToSets<ANDERSEN>::Type::PointsToSet PointsToSet;
	    PointsToSet& L = S[lval];
	    bool change = false;
	    for (PointsToSet::const_iterator i = L.begin(); i!=L.end(); ++i) {
		PointsToSet& X = S[*i];
		std::size_t const old_size = X.size();
		L.insert(rval);
		change = change || X.size() != old_size;
	    }
	    return change;
	}
    };
    using std::bind;
    using std::placeholders::_1;
    return bind(&local::function,_1,
		E.getArgument1().getArgument().getArgument(),
		E.getArgument2().getArgument());
}

RuleFunction::Type getRuleFunction(DEALLOC<const llvm::Value *>) {
    return typename RuleFunction::Type(&RuleFunction::identity);
}

void getRulesOfCommand(RuleCode const& RC, Rules &R)
{
    switch (RC.getType())
    {
	case RCT_VAR_ASGN_ALLOC:
	    R.insert(ruleVar(RC.getLvalue())=ruleAllocSite(RC.getRvalue()));
	    break;
	case RCT_VAR_ASGN_NULL:
	    R.insert(ruleVar(RC.getLvalue()) = ruleNull(RC.getRvalue()));
	    break;
	case RCT_VAR_ASGN_VAR:
	    R.insert(ruleVar(RC.getLvalue()) = ruleVar(RC.getRvalue()));
	    break;
	case RCT_VAR_ASGN_REF_VAR:
	    R.insert(ruleVar(RC.getLvalue()) = &ruleVar(RC.getRvalue()));
	    break;
	case RCT_VAR_ASGN_DREF_VAR:
	    R.insert(ruleVar(RC.getLvalue()) = *ruleVar(RC.getRvalue()));
	    break;
	case RCT_DREF_VAR_ASGN_NULL:
	    R.insert(*ruleVar(RC.getLvalue()) = ruleNull(RC.getRvalue()));
	    break;
	case RCT_DREF_VAR_ASGN_VAR:
	    R.insert(*ruleVar(RC.getLvalue()) = ruleVar(RC.getRvalue()));
	    break;
	case RCT_DREF_VAR_ASGN_REF_VAR:
	    R.insert(*ruleVar(RC.getLvalue()) = &ruleVar(RC.getRvalue()));
	    break;
	case RCT_DREF_VAR_ASGN_DREF_VAR:
	    R.insert(*ruleVar(RC.getLvalue()) = *ruleVar(RC.getRvalue()));
	    break;
	case RCT_DEALLOC:
	    R.insert(ruleDeallocSite(RC.getValue()));
	    break;
	default:
	    break;
    }
}

PointsToSets<ANDERSEN>::Type &computePointsToSets(const ProgramStructure &P,
		PointsToSets<ANDERSEN>::Type &S, ANDERSEN) {
  return detail::pruneByType<ANDERSEN>(fixpoint<ANDERSEN>(P, S));
}

}}