/*
 * opencog/atoms/core/MapLink.h
 *
 * Copyright (C) 2015,2016 Linas Vepstas
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

#ifndef _OPENCOG_EXTRACT_LINK_H
#define _OPENCOG_EXTRACT_LINK_H

#include <opencog/atoms/core/FunctionLink.h>
#include <opencog/atoms/core/ScopeLink.h>
#include <opencog/atoms/core/Quotation.h>

namespace opencog
{
/** \addtogroup grp_atomspace
 *  @{
 */

/// The MapLink is a FunctionLink that undoes beta-reduction; it
/// can be used to "extract" the values that occupy certain variable
/// locations in a formula.  It is the "opposite" of PutLink, in that
/// PutLink substitutes values for variables; whereas this link holds
/// a template pattern, which can be compared to an input, and values 
/// are extracted for the variable locations.
///
class MapLink : public FunctionLink
{
protected:
	ScopeLinkPtr _pattern;
	const Variables* _mvars;
	const HandleSet* _varset;

	// Globby terms are terms that contain a GlobNode
	HandleSet _globby_terms;     // Smallest term that has a glob.

	bool _is_impl;
	Handle _rewrite;

	void init(void);

	MapLink(Type, const Handle&);

	bool extract(const Handle&, const Handle&, GroundingMap&,
	             Quotation quotation=Quotation()) const;

	Handle rewrite_one(const Handle&, AtomSpace*) const;

public:
	MapLink(const HandleSeq&&, Type=MAP_LINK);
	MapLink(const Handle& pattern, const Handle& term);
	MapLink(const MapLink&) = delete;
	MapLink operator=(const MapLink&) = delete;

	// Align the pattern and the term side-by-side, and extract the
	// values that match up with the variables.  If the term is not of
	// the same type as the pattern, return the undefined handle.
	virtual ValuePtr execute(AtomSpace*, bool);

	static Handle factory(const Handle&);
};

LINK_PTR_DECL(MapLink);
#define createMapLink CREATE_DECL(MapLink)

/** @}*/
}

#endif // _OPENCOG_EXTRACT_LINK_H
