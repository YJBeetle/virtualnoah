/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */


#include <config.h>

#if defined(CONFIG_JZ4730) || defined(CONFIG_JZ4740) || defined(CONFIG_JZ5730) || defined(CONFIG_JZ4750)

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include <asm/io.h>
#if defined(CONFIG_JZ4730)
#include <asm/jz4730.h>
#endif
#if defined(CONFIG_JZ4740)
#include <asm/jz4740.h>
#endif
#if defined(CONFIG_JZ5730)
#include <asm/jz5730.h>
#endif
#if defined(CONFIG_JZ4750)
#include <asm/jz4750.h>
#endif


/* I2C protocol */
#define I2C_READ	1
#define I2C_WRITE	0

#define TIMEOUT         1000

#define ETIMEDOUT 1


static inline void my_udelay(int n )
{
	int i;
	i=n*100;
	while ( i-- )
		;
}


/*
 * I2C bus protocol basic routines
 */
static int i2c_put_data(unsigned char data)
{
	unsigned int timeout = TIMEOUT*10;

	__i2c_write(data);
	__i2c_set_drf();
	while (__i2c_check_drf() != 0);
	while (!__i2c_transmit_ended());
	while (!__i2c_received_ack() && timeout)
		timeout--;

	if (timeout)
		return 0;
	else
		return -ETIMEDOUT;
}

static int i2c_get_data(unsigned char *data, int ack)
{
	int timeout = TIMEOUT*10;

	if (!ack)
		__i2c_send_nack();
	else
		__i2c_send_ack();

	while (__i2c_check_drf() == 0 && timeout)
		timeout--;

	if (timeout) {
		if (!ack)
			__i2c_send_stop();
		*data = __i2c_read();
		__i2c_clear_drf();
		return 0;
	} else
		return -ETIMEDOUT;
}

/*
 * I2C interface
 */
void i2c_open(void)
{
	__i2c_set_clk(CFG_EXTAL, 10000); /* default 10 KHz */
	__i2c_enable();
}

void i2c_close(void)
{
	my_udelay(300); /* wait for STOP goes over. */
	__i2c_disable();
}

void i2c_setclk(unsigned int i2cclk)
{
	__i2c_set_clk(CFG_EXTAL, i2cclk);
}

int i2c_lseek(unsigned char device, unsigned char offset)
{
	__i2c_send_nack();	/* Master does not send ACK, slave sends it */
	__i2c_send_start();
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_err;
	if (i2c_put_data(offset) < 0)
		goto address_err;
	return 0;
 device_err:
	printf("No I2C device (0x%02x) installed.\n", device);
	__i2c_send_stop();
	return -1;
 address_err:
	printf("No I2C device (0x%02x) response.\n", device);
	__i2c_send_stop();
	return -1;
}

int i2c_read(unsigned char device, unsigned char *buf,
	       unsigned char address, int count)
{
	int cnt = count;
	int timeout = 5;

L_try_again:

	if (timeout < 0)
		goto L_timeout;

	__i2c_send_nack();	/* Master does not send ACK, slave sends it */
	__i2c_send_start();
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_werr;
	if (i2c_put_data(address) < 0)
		goto address_err;

	__i2c_send_start();
	if (i2c_put_data( (device << 1) | I2C_READ ) < 0)
		goto device_rerr;
	__i2c_send_ack();	/* Master sends ACK for continue reading */
	while (cnt) {
		if (cnt == 1) {
			if (i2c_get_data(buf, 0) < 0)
				break;
		} else {
			if (i2c_get_data(buf, 1) < 0)
				break;
		}
		cnt--;
		buf++;
	}

	__i2c_send_stop();
	return count - cnt;
 device_rerr:
 device_werr:
 address_err:
	timeout --;
	__i2c_send_stop();
	goto L_try_again;

L_timeout:
	__i2c_send_stop();
	printf("Read I2C device 0x%2x failed.\n", device);
	return -1;
}

int i2c_write(unsigned char device, unsigned char *buf,
		unsigned char address, int count)
{
	int cnt = count;
	int cnt_in_pg;
	int timeout = 5;
	unsigned char *tmpbuf;
	unsigned char tmpaddr;

	__i2c_send_nack();	/* Master does not send ACK, slave sends it */

 W_try_again:
	if (timeout < 0)
		goto W_timeout;

	cnt = count;
	tmpbuf = (unsigned char *)buf;
	tmpaddr = address;

 start_write_page:
	cnt_in_pg = 0;
	__i2c_send_start();
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_err;
	if (i2c_put_data(tmpaddr) < 0)
		goto address_err;
	while (cnt) {
		if (++cnt_in_pg > 8) {
			__i2c_send_stop();
			my_udelay(1000);
			tmpaddr += 8;
			goto start_write_page;
		}
		if (i2c_put_data(*tmpbuf) < 0)
			break;
		cnt--;
		tmpbuf++;
	}
	__i2c_send_stop();
	return count - cnt;
 device_err:
 address_err:
	timeout--;
	__i2c_send_stop();
	goto W_try_again;

 W_timeout:
	printf("Write I2C device 0x%2x failed.\n", device);
	__i2c_send_stop();
	return -1;
}

#endif /* CONFIG_JZ4730 || CONFIG_JZ4740 || CONFIG_JZ5730 */


