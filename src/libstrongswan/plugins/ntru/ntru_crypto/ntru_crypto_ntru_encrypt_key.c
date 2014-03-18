/******************************************************************************
 * NTRU Cryptography Reference Source Code
 * Copyright (c) 2009-2013, by Security Innovation, Inc. All rights reserved. 
 *
 * ntru_crypto_ntru_encrypt_key.c is a component of ntru-crypto.
 *
 * Copyright (C) 2009-2013  Security Innovation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *****************************************************************************/
 
/******************************************************************************
 *
 * File: ntru_crypto_ntru_encrypt_key.c
 *
 * Contents: Routines for exporting and importing public and private keys
 *           for NTRUEncrypt.
 *
 *****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ntru_crypto_ntru_encrypt_key.h"


/* ntru_crypto_ntru_encrypt_key_parse
 *
 * Parses an NTRUEncrypt key blob.
 * If the blob is not corrupt, returns packing types for public and private
 * keys, a pointer to the parameter set, a pointer to the public key, and
 * a pointer to the private key if it exists.
 *
 * Returns TRUE if successful.
 * Returns FALSE if the blob is invalid.
 */

bool
ntru_crypto_ntru_encrypt_key_parse(
    bool                     pubkey_parse,      /*  in - if parsing pubkey
                                                         blob */
    uint16_t                 key_blob_len,      /*  in - no. octets in key
                                                         blob */
    uint8_t const           *key_blob,          /*  in - pointer to key blob */
    uint8_t                 *pubkey_pack_type,  /* out - addr for pubkey
                                                         packing type */
    uint8_t                 *privkey_pack_type, /* out - addr for privkey
                                                         packing type */
    ntru_param_set_t       **params,            /* out - addr for ptr to
                                                         parameter set */
    uint8_t const          **pubkey,            /* out - addr for ptr to
                                                         packed pubkey */
    uint8_t const          **privkey)           /* out - addr for ptr to
                                                         packed privkey */
{
    uint8_t tag;

    /* parse key blob based on tag */
    tag = key_blob[0];
    switch (tag) {
        case NTRU_PUBKEY_TAG:
            if (!pubkey_parse)
                return FALSE;
            break;
        case NTRU_PRIVKEY_DEFAULT_TAG:
        case NTRU_PRIVKEY_TRITS_TAG:
        case NTRU_PRIVKEY_INDICES_TAG:
            assert(privkey_pack_type);
            assert(privkey);
            if (pubkey_parse)
                return FALSE;
            break;
        default:
            return FALSE;
    }

    switch (tag) {
        case NTRU_PUBKEY_TAG:
        case NTRU_PRIVKEY_DEFAULT_TAG:
        case NTRU_PRIVKEY_TRITS_TAG:
        case NTRU_PRIVKEY_INDICES_TAG:

            /* Version 0:
             *  byte  0:   tag
             *  byte  1:   no. of octets in OID
             *  bytes 2-4: OID
             *  bytes 5- : packed pubkey
             *             [packed privkey]
             */

        {
            ntru_param_set_t *p = NULL;
            uint16_t pubkey_packed_len;

            /* check OID length and minimum blob length for tag and OID */

            if ((key_blob_len < 5) || (key_blob[1] != 3))
                return FALSE;

			/* get a pointer to the parameter set corresponding to the OID */
			p = ntru_param_set_get_by_oid(key_blob + 2);
			if (!p)
			{
				return FALSE;
			}

            /* check blob length and assign pointers to blob fields */

            pubkey_packed_len = (p->N * p->q_bits + 7) / 8;
            if (pubkey_parse) { /* public-key parsing */
                if (key_blob_len != 5 + pubkey_packed_len)
                    return FALSE;

                *pubkey = key_blob + 5;

            } else { /* private-key parsing */
                uint16_t privkey_packed_len;
                uint16_t privkey_packed_trits_len = (p->N + 4) / 5;
                uint16_t privkey_packed_indices_len;
                uint16_t dF;

                /* check packing type for product-form private keys */

                if (p->is_product_form &&
                        (tag == NTRU_PRIVKEY_TRITS_TAG))
                    return FALSE;

                /* set packed-key length for packed indices */

                if (p->is_product_form)
                    dF = (uint16_t)( (p->dF_r & 0xff) +            /* df1 */
                                    ((p->dF_r >>  8) & 0xff) +     /* df2 */
                                    ((p->dF_r >> 16) & 0xff));     /* df3 */
                else
                    dF = (uint16_t)p->dF_r;
                privkey_packed_indices_len = ((dF << 1) * p->N_bits + 7) >> 3;

                /* set private-key packing type if defaulted */

                if (tag == NTRU_PRIVKEY_DEFAULT_TAG) {
                    if (p->is_product_form ||
                            (privkey_packed_indices_len <=
                             privkey_packed_trits_len))
                        tag = NTRU_PRIVKEY_INDICES_TAG;
                    else
                        tag = NTRU_PRIVKEY_TRITS_TAG;
                }

                if (tag == NTRU_PRIVKEY_TRITS_TAG)
                    privkey_packed_len = privkey_packed_trits_len;
                else
                    privkey_packed_len = privkey_packed_indices_len;

                if (key_blob_len != 5 + pubkey_packed_len + privkey_packed_len)
                    return FALSE;

                *pubkey = key_blob + 5;
                *privkey = *pubkey + pubkey_packed_len;
                *privkey_pack_type = (tag == NTRU_PRIVKEY_TRITS_TAG) ?
                    NTRU_KEY_PACKED_TRITS :
                    NTRU_KEY_PACKED_INDICES;
            }

            /* return parameter set pointer */

            *pubkey_pack_type = NTRU_KEY_PACKED_COEFFICIENTS;
            *params = p;
        }
        default:
            break;  /* can't get here */
    }
    return TRUE;
}
