#include <config.h>

#ifdef CONFIG_JZ4740
#include <common.h>
#include <command.h>
#include <asm/jz4740.h>
#include <asm/io.h>               /* virt_to_phys() */
#define JZ4740_CPM_TEST



int cpm_value;
void mdelay(int dly)
{
	udelay(dly * 1000);
}

int myatoi(char *string)
{
    int res = 0;
    while (*string>='0' && *string <='9')
    {
	res *= 10;
	res += *string-'0';
	string++;
    }

    return res;
}

int myhatoi(char *string)
{
    int res = 0;
    
    if ( *string != '0' || *(string+1) != 'x' ) {
	    return 0;
    }

    string += 2;

    while ( *string )
    {
	res *= 16;
	if (*string>='0' && *string <='9')
		res += *string-'0';
	else if (*string>='a' && *string <='f')
		res += *string-'a' + 10;
	else 
		return res;

	string++;
    }

    return res;
}

int atoi( char * string )
{
    if ( *string == '0' && *(string+1) == 'x' ) {
	    return myhatoi(string);
    }
    else 
	    return myatoi(string);
}

void cpm_add_test(void)
{
	pll_add_test(cpm_value);
	//sdram_add_test(cpm_value);
	calc_clocks_add_test();
}

int do_jz_cpmtest_function(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ( !strncmp(argv[1], "add", 3 ) ) {
		if ( argc == 3 ) {
			cpm_value = atoi( argv[2] );
			printf("cpm value:%d\n",cpm_value);
			if((cpm_value >= 999) || (cpm_value <= 100)) {
				printf("param is wrong!\n");
			}
			cpm_value *= 1000000;
			cpm_add_test();
		}
	} else {
		printf("command is wrong!\n");
	}
	return 0;
}

U_BOOT_CMD(
	cpm,	4,	1,	do_jz_cpmtest_function,
	"cpm:\t - Usage: cpmtest [add][cpm_value]\n",
	NULL
	);

#endif /* CONFIG_JZ4740 */
