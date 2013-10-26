#ifndef __SSFDC_RS_ECC_H__
#define __SSFDC_RS_ECC_H__

/* Stuff common to all the general-purpose Reed-Solomon codecs
 * Copyright 2004 Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */

/* Reed-Solomon codec control block */
typedef unsigned char data_t;

struct rs {
  int mm;              /* Bits per symbol */
  int nn;              /* Symbols per block (= (1<<mm)-1) */
  data_t *alpha_to;     /* log lookup table */
  data_t *index_of;     /* Antilog lookup table */
  data_t *genpoly;      /* Generator polynomial */
  int nroots;     /* Number of generator roots = number of parity symbols */
  int fcr;        /* First consecutive root, index form */
  int prim;       /* Primitive element, index form */
  int iprim;      /* prim-th root of 1, index form */
  int pad;        /* Padding bytes in shortened block */
};

void encode_rs(void *p,data_t *data, unsigned short *bb);
struct rs *init_rs_int(int symsize,int gfpoly,int fcr,int prim,int nroots,int pad);
void free_rs_int(void *p);

#endif /* __SSFDC_RS_ECC_H__ */
