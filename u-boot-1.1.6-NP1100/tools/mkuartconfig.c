/*
 *  auto config Jz uart Baudrate 
 *  ver1.0 jbyu@ingenic.cn
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include <../include/config.h>

/*#define DEBUG_UART_CONFIG*/
#ifdef DEBUG_UART_CONFIG
static unsigned int counts = 0;
#define ucprintf(fmt...) printf(fmt)
#else
#define ucprintf(fmt...)
#endif

#define CONFIG_HEAD "/* Automatically generated - do not edit */ \n\n#ifndef __JZ_SERIAL_H__\n#define __JZ_SERIAL_H__\n\n"
#define CONFIG_TAIL "\n#endif //__JZ_SERIAL_H__\n\n"

# define do_div(n,base) ({					\
	unsigned int __base = (base);				\
	unsigned int __rem;						\
	__rem = ((long long)(n)) % __base;			\
	(n) = ((long long)(n)) / __base;				\
	__rem;							\
 })

static char *cmdname;
static unsigned short quot1[3] = {0};
static unsigned int uartclk;
static unsigned int uartdiv = 0;
static int write_file(int fd, char* data, int len)
{
	if (write(fd, data, strlen(data)) != strlen(data)) {
		fprintf (stderr, "%s: Write error on %s: %s\n",
                 cmdname, "jz_serial.h", strerror(errno));
		return (-1);
	}
    
    return (0);
}

static unsigned short *get_divisor(unsigned int baud)
{
	int err, sum, i, j;
	int a[12], b[12];
	unsigned short div, umr, uacr;
	unsigned short umr_best, div_best, uacr_best;
	long long t0, t1, t2, t3;
    int fd;
    char *divBest;
    char *umrBest;
    char *uacrBest;    

    if ((fd = open("../include/asm-mips/jz_serial.h", O_RDWR | O_CREAT, 0666)) < 0) {
		fprintf (stderr, "%s: Can't open %s: %s\n",
                 cmdname,  "jz_serial.h", strerror(errno));
		return quot1;
	}

    if (write_file(fd, CONFIG_HEAD, strlen(CONFIG_HEAD)) < 0)
        return quot1;
    
	sum = 0;
	umr_best = div_best = uacr_best = 0;
	div = 1;

    if ((uartclk % (16 * baud)) == 0) {
		quot1[0] = uartclk / (16 * baud);
		quot1[1] = 16;
		quot1[2] = 0;
        ucprintf("Normal baudrate mode setting.\n");
		return quot1;
	}
    ucprintf("Advance baudrate mode setting.\n");
	while (1) {
		umr = uartclk / (baud * div);
  		if (umr > 32) {
			div++;
			continue;
		}
		if (umr < 4) {
			break;
		}
		for (i = 0; i < 12; i++) {
			a[i] = umr;
			b[i] = 0;
			sum = 0;
			for (j = 0; j <= i; j++) {
				sum += a[j];
			}

            /* the precision could be 1/2^(36) due to the value of t0 */
			t0 = 0x1000000000LL;
			t1 = (i + 1) * t0;
			t2 = (sum * div) * t0;
			t3 = div * t0;
			do_div(t1, baud);
			do_div(t2, uartclk);
			do_div(t3, (2 * uartclk));
			err = t1 - t2 - t3;

			if (err > 0) {
				a[i] += 1;
				b[i] = 1;
			}
		}

		uacr = 0;
		for (i = 0; i < 12; i++) {
			if (b[i] == 1) {
				uacr |= 1 << i;
			}
		}

        if (div_best == 0){
			div_best = div;
			umr_best = umr;
			uacr_best = uacr;            
        }
        
        /* the best value of umr should be near 16, and the value of uacr should better be smaller */
		if (abs(umr - 16) < abs(umr_best - 16) || (abs(umr - 16) == abs(umr_best - 16) && uacr_best > uacr)) {
			div_best = div;
			umr_best = umr;
			uacr_best = uacr;
            ucprintf("find best Advance setting %d\n", ++counts);
		}
		div++;
	}
    
    quot1[0] = div_best;
	quot1[1] = umr_best;
	quot1[2] = uacr_best;
    
    ucprintf("div_best.%d, umr_best.%d, uacr_best.%d\n", quot1[0], quot1[1], quot1[2]);
    
    asprintf(&divBest, "#define DIV_BEST %d\n", div_best);
    asprintf(&umrBest, "#define UMR_BEST %d\n", umr_best);
    asprintf(&uacrBest, "#define UACR_BEST %d\n", uacr_best);

    if (uartdiv)
    {
        char *uart_div = "#define UART_DIV 1\n";
        if (write_file(fd, uart_div, strlen(uart_div)) < 0)
        {
            goto out;
        }
    }

    if (write_file(fd, divBest, strlen(divBest)) < 0)
    {
		goto out;
	}

    if (write_file(fd, umrBest, strlen(umrBest)) < 0)
    {
		goto out;
	}

    if (write_file(fd, uacrBest, strlen(uacrBest)) < 0)
    {
		goto out;
	}

    if (write_file(fd, CONFIG_TAIL, strlen(CONFIG_TAIL)) < 0)
    {
		goto out;
	}
    
    fsync(fd);
    
out:    
    free(divBest);
    free(umrBest);
    free(uacrBest);
    
    close(fd);
    
    return (quot1);
}

int main(int argc, char* argv[]){
    unsigned int baud = 0;
    
    cmdname = argv[0];
    
#if defined(CONFIG_FPGA)
	uartclk = CFG_EXTAL / CFG_DIV;
#elif defined(CONFIG_JZ4750) || defined(CONFIG_JZ4750D) || defined(CONFIG_JZ4760)
	if (CFG_EXTAL > 16000000) {
        uartdiv = 1;
		uartclk = CFG_EXTAL / 2;
	} else {
        uartclk = CFG_EXTAL;
	}
#else /* CONFIG_JZ4740 or CONFIG_JZ4730 */
    uartclk = CFG_EXTAL;
#endif
    
    baud = CONFIG_BAUDRATE;
    ucprintf("clk %d, Baudrate %d\n", uartclk, baud);
    get_divisor(baud);
    
    return (0);
}
