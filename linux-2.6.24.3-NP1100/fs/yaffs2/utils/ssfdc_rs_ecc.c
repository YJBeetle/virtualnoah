/* Reed-Solomon decoder
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */
#include "ssfdc_rs_ecc.h"

#include <string.h>

struct CONV_DATA {
	unsigned char  shift0;
	unsigned char  mask0;
	unsigned char  mask1;
	unsigned char  merge_shift;
} ConvData [8]= {
	{0x0, 0xff, 0x01, 0x8},  /* 0 */
	{0x1, 0x7f, 0x03, 0x7},  /* 1 */
	{0x2, 0x3f, 0x07, 0x6},  /* 2 */
	{0x3, 0x1f, 0x0f, 0x5},  /* 3 */
	{0x4, 0x0f, 0x1f, 0x4},  /* 4 */
	{0x5, 0x07, 0x3f, 0x3},  /* 5 */
	{0x6, 0x03, 0x7f, 0x2},  /* 6 */
	{0x7, 0x01, 0xff, 0x1},  /* 7 */
};

static void kfree(void *ptr)
{
//	return 0;
}

static int modnn(struct rs *rs,int x)
{
	while (x >= rs->nn) {
		x -= rs->nn;
		x = (x >> rs->mm) + (x & rs->nn);
	}
	return x;
}

void encode_rs(void *p,data_t *data, unsigned short *bb)
{
	struct rs *rs = (struct rs *)p;
	int i, j;
	data_t feedback;

	memset(bb,0,(rs->nroots)*sizeof(unsigned short));

	for(i=0;i<(rs->nn)-(rs->nroots)-(rs->pad);i++){

		feedback = (rs->index_of)[data[i] ^ bb[0]];
		if(feedback != ((rs->nn))){
				for(j=1;j<(rs->nroots);j++)
					bb[j] ^= (rs->alpha_to)[modnn(rs,feedback + (rs->genpoly)[(rs->nroots)-j])];
		}

		memmove(&bb[0],&bb[1],sizeof(unsigned short)*((rs->nroots)-1));
	
		if(feedback != ((rs->nn))) {
			bb[(rs->nroots)-1] = (rs->alpha_to)[modnn(rs,feedback + (rs->genpoly)[0])];
		}
		else
			bb[(rs->nroots)-1] = 0;
	}

}

void free_rs_int(void *p)
{
	struct rs *rs = (struct rs *)p;
  
	free(rs->alpha_to);
	free(rs->index_of);
	free(rs->genpoly);
	free(rs);
}

/*
 * init_rs_int - Initialize a Reed-Solomon codec
 * @symsize:	symbol size
 * @gfpoly:	Field generator polynomial coefficients
 * @fcr:	first root of RS code generator polynomial, index form
 * @prim:	primitive element to generate polynomial roots
 * @nroots:	RS code generator polynomial degree (number of roots)
 */
struct rs *init_rs_int(int symsize,int gfpoly,int fcr,int prim,int nroots,int pad)
{
 	struct rs *rs;
#if 0
	static unsigned int rs0[64]={0};
	static data_t alpha0[512]={0};
	static data_t index0[512]={0};
	static data_t genepoly0[9]={0};
#endif
	int i, j, sr,root,iprim;
		
	rs = ((void *)0);
	if(symsize < 0 || symsize > 8*sizeof(data_t)){
		goto done;
	}

	if(fcr < 0 || fcr >= (1<<symsize))
		goto done;
	if(prim <= 0 || prim >= (1<<symsize))
		goto done;
	if(nroots < 0 || nroots >= (1<<symsize))
		goto done;
	if(pad < 0 || pad >= ((1<<symsize) -1 - nroots))
		goto done;
		
	rs = (struct rs *)malloc(sizeof(struct rs));
//	rs = (struct rs *)rs0;
	if(rs == ((void *)0))
		goto done;
	rs->mm = symsize;
	rs->nn = (1<<symsize)-1;
	rs->pad = pad;
	rs->alpha_to = (data_t *)malloc(sizeof(data_t)*(rs->nn+1));
//	rs->alpha_to = alpha0;
	if(rs->alpha_to == ((void *)0)){
		kfree(rs);
		rs = ((void *)0);
		goto done;
	}
	rs->index_of = (data_t *)malloc(sizeof(data_t)*(rs->nn+1));
//	rs->index_of = index0;
	if(rs->index_of == ((void *)0)){
		kfree(rs->alpha_to);
		kfree(rs);
		rs = ((void *)0);
		goto done;
	}
	rs->index_of[0] = ((rs->nn));
	rs->alpha_to[((rs->nn))] = 0;
	sr = 1;
	for(i=0;i<rs->nn;i++){
		rs->index_of[sr] = i;
		rs->alpha_to[i] = sr;
		sr <<= 1;
		if(sr & (1<<symsize))
			sr ^= gfpoly;
		sr &= rs->nn;
	}
	if(sr != 1){
		kfree(rs->alpha_to);
		kfree(rs->index_of);
		kfree(rs);
		rs = ((void *)0);
		goto done;
	}
	rs->genpoly = (data_t *)malloc(sizeof(data_t)*(nroots+1));
//	rs->genpoly = genepoly0;
	if(rs->genpoly == ((void *)0)){
		kfree(rs->alpha_to);
		kfree(rs->index_of);
		kfree(rs);
		rs = ((void *)0);
		goto done;
	}
	rs->fcr = fcr;
	rs->prim = prim;
	rs->nroots = nroots;
	for(iprim=1;(iprim % prim) != 0;iprim += rs->nn)
		;
	rs->iprim = iprim / prim;
	rs->genpoly[0] = 1;
	for (i = 0,root=fcr*prim; i < nroots; i++,root += prim) {
		rs->genpoly[i+1] = 1;
		for (j = i; j > 0; j--){
			if (rs->genpoly[j] != 0)
				rs->genpoly[j] = rs->genpoly[j-1] ^ rs->alpha_to[modnn(rs,rs->index_of[rs->genpoly[j]] + root)];
			else
				rs->genpoly[j] = rs->genpoly[j-1];
		}
		rs->genpoly[0] = rs->alpha_to[modnn(rs,rs->index_of[rs->genpoly[0]] + root)];
	}
	for (i = 0; i <= nroots; i++)
		rs->genpoly[i] = rs->index_of[rs->genpoly[i]];
	done:;
	return rs;
}

