/*
 * opencog/atoms/reduct/NumericFunctionLink.cc
 *
 * Copyright (C) 2015, 2018, 2021 Linas Vepstas
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <limits>

#include <opencog/atoms/atom_types/atom_types.h>
#include <opencog/atoms/atom_types/NameServer.h>
#include <opencog/atoms/core/DefineLink.h>
#include <opencog/atoms/core/NumberNode.h>
#include "NumericFunctionLink.h"

using namespace opencog;

NumericFunctionLink::NumericFunctionLink(const HandleSeq&& oset, Type t)
    : FunctionLink(std::move(oset), t)
{
	init();
}

void NumericFunctionLink::init(void)
{
	Type tscope = get_type();
	if (NUMERIC_FUNCTION_LINK == tscope)
		throw InvalidParamException(TRACE_INFO,
			"NumericOutLinks are private and cannot be instantiated.");

	if (not nameserver().isA(tscope, NUMERIC_FUNCTION_LINK))
		throw InvalidParamException(TRACE_INFO, "Expecting an NumericFunctionLink");
}

// ===========================================================

/// Generic utility -- execute the argument, and return the result
/// of the execution.
ValuePtr NumericFunctionLink::get_value(AtomSpace* as, bool silent, ValuePtr vptr)
{
	if (DEFINED_SCHEMA_NODE == vptr->get_type())
	{
		vptr = DefineLink::get_definition(HandleCast(vptr));
	}
	while (vptr->is_atom())
	{
		Handle h(HandleCast(vptr));
		if (not h->is_executable()) break;

		ValuePtr red(h->execute(as, silent));

		// It would probably be better to throw a silent exception, here?
		if (nullptr == red) return vptr;
		if (*red == *vptr) return vptr;
		vptr = red;

		// The executable function might be a GetLink, which returns
		// a SetLink of results. If the SetLink is wrapping only one
		// atom, then unwrap it and return that value. If it contains
		// more than one atom, we don't know what to do.
		if (SET_LINK == vptr->get_type())
		{
			Handle setl(HandleCast(vptr));
			if (1 == setl->get_arity())
				vptr = setl->getOutgoingAtom(0);
		}
	}
	return vptr;
}

// ===========================================================

/// Generic utility -- convert the argument to a vector of doubles,
/// if possible.  Reutnr nullptr if not possible.
const std::vector<double>*
NumericFunctionLink::get_vector(AtomSpace* as, bool silent,
                           ValuePtr vptr, Type& t)
{
	t = vptr->get_type();

	bool is_fv = nameserver().isA(t, FLOAT_VALUE);
	bool is_nu = (NUMBER_NODE == t);

	if (not is_fv and not is_nu) return nullptr;

	if (is_nu)
		return & NumberNodeCast(vptr)->value();
	if (is_fv)
		return & FloatValueCast(vptr)->value();

	return nullptr; // not reached
}

// ============================================================

/// Generic utility -- execute the Handle, and, if that returned
/// a vector of doubles, then apply the function to them.
/// If there wasn't a numeric vectors, return a null pointer.
/// In this last case, the result of reduction is returned
/// in `reduction`
ValuePtr
NumericFunctionLink::apply_func(AtomSpace* as, bool silent,
                           const Handle& arg,
                           double (*fun)(double),
                           ValuePtr& vx)
{
	// get_value() causes execution.
	vx = get_value(as, silent, arg);

	// get_vector gets numeric values, if possible.
	Type vxtype;
	const std::vector<double>* xvec = get_vector(as, silent, vx, vxtype);

	// No numeric values available. Sorry!
	if (nullptr == xvec or 0 == xvec->size())
		return nullptr;

	std::vector<double> funvec;
	size_t sz = xvec->size();
	for (size_t i=0; i<sz; i++)
		funvec.push_back(fun(xvec->operator[](i)));

	if (NUMBER_NODE == vxtype)
		return createNumberNode(funvec);

	return createFloatValue(funvec);
}

// ============================================================

/// Generic utility -- execute the HandleSeq, and, if that returned
/// vectors of doubles, then apply the function to them.
/// If there weren't any vectors, return a null pointer.
/// In this last case, the result of reduction is returned
/// in `reduction`
ValuePtr
NumericFunctionLink::apply_func(AtomSpace* as, bool silent,
                           const HandleSeq& args,
                           double (*fun)(double, double),
                           ValueSeq& reduction)
{
	// get_value() causes execution.
	ValuePtr vx(get_value(as, silent, args[0]));
	ValuePtr vy(get_value(as, silent, args[1]));

	// get_vector gets numeric values, if possible.
	Type vxtype;
	const std::vector<double>* xvec = get_vector(as, silent, vx, vxtype);

	Type vytype;
	const std::vector<double>* yvec = get_vector(as, silent, vy, vytype);

	// No numeric values available. Sorry!
	if (nullptr == xvec or nullptr == yvec or
	    0 == xvec->size() or 0 == yvec->size())
	{
		reduction.push_back(vx);
		reduction.push_back(vy);
		return nullptr;
	}

	std::vector<double> funvec;
	if (1 == xvec->size())
	{
		double x = xvec->back();
		for (double y : *yvec)
			funvec.push_back(fun(x, y));
	}
	else if (1 == yvec->size())
	{
		double y = yvec->back();
		for (double x : *xvec)
			funvec.push_back(fun(x, y));
	}
	else
	{
		size_t sz = std::min(xvec->size(), yvec->size());
		for (size_t i=0; i<sz; i++)
			funvec.push_back(fun(xvec->operator[](i), yvec->operator[](i)));
	}

	if (NUMBER_NODE == vxtype and NUMBER_NODE == vytype)
		return createNumberNode(funvec);

	return createFloatValue(funvec);
}

// ===========================================================
