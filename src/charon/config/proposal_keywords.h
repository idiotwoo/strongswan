/* proposal keywords
 * Copyright (C) 2009 Andreas Steffen
 * Hochschule fuer Technik Rapperswil, Switzerland
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * RCSID $Id:$
 */

#ifndef _PROPOSAL_KEYWORDS_H_
#define _PROPOSAL_KEYWORDS_H_

typedef struct proposal_token proposal_token_t;

extern const proposal_token_t* in_word_set(register const char *str, register unsigned int len);

#endif /* _PROPOSAL_KEYWORDS_H_ */

