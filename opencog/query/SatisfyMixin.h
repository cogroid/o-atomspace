/*
 * SatisfyMixin.h
 *
 * Copyright (C) 2009, 2014 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  January 2009
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

#ifndef _OPENCOG_SATISFY_MIXIN_H
#define _OPENCOG_SATISFY_MIXIN_H

#include "PatternMatchCallback.h"

namespace opencog {

class SatisfyMixin:
	public virtual PatternMatchCallback
{
	bool cartesian_product(const HandleSeq& virtuals,
	                       const PatternTermSeq& absents,
	                       const GroundingMap& var_gnds,
	                       const GroundingMap& term_gnds,
	                       // copies, NOT references!
	                       GroundingMapSeqSeq comp_var_gnds,
	                       GroundingMapSeqSeq comp_term_gnds);

	public:
		virtual bool satisfy(const PatternLinkPtr&);
};

}; // namespace opencog

#endif // _OPENCOG_SATISFY_MIXIN_H
