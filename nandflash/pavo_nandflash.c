#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "types.h"
#include "nand_flash.h"

char *NAND_Flash = NULL;

char *U_boot = NULL;
uint32_t U_boot_start, U_boot_end,U_boot_add_ecc;
char *Serial = NULL;
uint32_t Serial_start, Serial_end,Serial_add_ecc;
char *Kernel = NULL;
uint32_t Kernel_start, Kernel_end,Kernel_add_ecc;
char *ROOT_FS = NULL;
uint32_t ROOT_FS_start, ROOT_FS_end, ROOT_FS_oob,ROOT_FS_add_ecc;
char *Settings = NULL;
uint32_t Settings_start, Settings_end, Settings_oob,Settings_add_ecc;
char *Program_FS = NULL;
uint32_t Program_FS_start, Program_FS_end, Program_FS_oob,Program_FS_add_ecc;
char *Data_FS = NULL;
uint32_t Data_FS_start, Data_FS_end, Data_FS_oob,Data_FS_add_ecc;
char *User_FS = NULL;
uint32_t User_FS_start, User_FS_end, User_FS_oob,User_FS_add_ecc;
char *User_Disk = NULL;
uint32_t User_Disk_start, User_Disk_end, User_Disk_oob,User_Disk_add_ecc;

static int read_configure(int argc, char **argv)
{
  int i;
  for(i=1;i<argc;i++)
  {
    if(strcmp(argv[i],"--NAND_Flash")==0)
    {
      if(++i<argc)NAND_Flash=argv[i];
    }
    else if(strcmp(argv[i],"--U_boot")==0)
    {
      if(++i<argc)U_boot=argv[i];
      if(++i<argc)U_boot_start=strtol(argv[i],NULL,0);
      if(++i<argc)U_boot_end=strtol(argv[i],NULL,0);
      if(++i<argc)U_boot_add_ecc=strtol(argv[i],NULL,0);
    }
    else if(strcmp(argv[i],"--Serial")==0)
    {
      if(++i<argc)Serial=argv[i];
      if(++i<argc)Serial_start=strtol(argv[i],NULL,0);
      if(++i<argc)Serial_end=strtol(argv[i],NULL,0);
      if(++i<argc)Serial_add_ecc=strtol(argv[i],NULL,0);
    }
    else if(strcmp(argv[i],"--Kernel")==0)
    {
      if(++i<argc)Kernel=argv[i];
      if(++i<argc)Kernel_start=strtol(argv[i],NULL,0);
      if(++i<argc)Kernel_end=strtol(argv[i],NULL,0);
      if(++i<argc)Kernel_add_ecc=strtol(argv[i],NULL,0);
    }
    else if(strcmp(argv[i],"--ROOT_FS")==0)
    {
      if(++i<argc)ROOT_FS=argv[i];
      if(++i<argc)ROOT_FS_start=strtol(argv[i],NULL,0);
      if(++i<argc)ROOT_FS_end=strtol(argv[i],NULL,0);
      if(++i<argc)ROOT_FS_oob=strtol(argv[i],NULL,0);
      if(++i<argc)ROOT_FS_add_ecc=strtol(argv[i],NULL,0);
    }
    else if(strcmp(argv[i],"--Settings")==0)
    {
      if(++i<argc)Settings=argv[i];
      if(++i<argc)Settings_start=strtol(argv[i],NULL,0);
      if(++i<argc)Settings_end=strtol(argv[i],NULL,0);
      if(++i<argc)Settings_oob=strtol(argv[i],NULL,0);
      if(++i<argc)Settings_add_ecc=strtol(argv[i],NULL,0);
    }
    else if(strcmp(argv[i],"--Program_FS")==0)
    {
      if(++i<argc)Program_FS=argv[i];
      if(++i<argc)Program_FS_start=strtol(argv[i],NULL,0);
      if(++i<argc)Program_FS_end=strtol(argv[i],NULL,0);
      if(++i<argc)Program_FS_oob=strtol(argv[i],NULL,0);
      if(++i<argc)Program_FS_add_ecc=strtol(argv[i],NULL,0);
    }
    else if(strcmp(argv[i],"--Data_FS")==0)
    {
      if(++i<argc)Data_FS=argv[i];
      if(++i<argc)Data_FS_start=strtol(argv[i],NULL,0);
      if(++i<argc)Data_FS_end=strtol(argv[i],NULL,0);
      if(++i<argc)Data_FS_oob=strtol(argv[i],NULL,0);
      if(++i<argc)Data_FS_add_ecc=strtol(argv[i],NULL,0);
    }
    else if(strcmp(argv[i],"--User_FS")==0)
    {
      if(++i<argc)User_FS=argv[i];
      if(++i<argc)User_FS_start=strtol(argv[i],NULL,0);
      if(++i<argc)User_FS_end=strtol(argv[i],NULL,0);
      if(++i<argc)User_FS_oob=strtol(argv[i],NULL,0);
      if(++i<argc)User_FS_add_ecc=strtol(argv[i],NULL,0);
    }
    else if(strcmp(argv[i],"--User_Disk")==0)
    {
      if(++i<argc)User_Disk=argv[i];
      if(++i<argc)User_Disk_start=strtol(argv[i],NULL,0);
      if(++i<argc)User_Disk_end=strtol(argv[i],NULL,0);
      if(++i<argc)User_Disk_oob=strtol(argv[i],NULL,0);
      if(++i<argc)User_Disk_add_ecc=strtol(argv[i],NULL,0);
    }
  }
  return (0);
}

static int valid_conf()
{
    printf("\t正在检查参数\n");

    int fd;

    if (NAND_Flash)
    {
	printf("\tNAND_Flash = \t%s\n",NAND_Flash);
    }
    else
    {
        fprintf(stderr,"NAND_Flash没有设置\n");
        return (-1);
    }

    
    if (U_boot)
    {
        printf("\tU_boot = \t%s\n",U_boot);
        printf("\tU_boot_start = \t0x%X\n",U_boot_start);
        printf("\tU_boot_end = \t0x%X\n",U_boot_end);
        printf("\tU_boot_add_ecc = \t%d\n",U_boot_add_ecc);
        fd = open(U_boot, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开U_boot镜像：[ %s ]\n", U_boot);
            return (-1);
        }
        if (U_boot_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"U_boot镜像开始地址在0x%X，这不是页面大小的倍数\n", U_boot_start);
            return (-1);
        }
        if (U_boot_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"U_boot镜像结束地址在0x%X，这不是页面大小的倍数\n", U_boot_end);
            return (-1);
        }
        close(fd);
    }

    if (Serial)
    {
        printf("\tSerial = \t%s\n",Serial);
        printf("\tSerial_start = \t0x%X\n",Serial_start);
        printf("\tSerial_end = \t0x%X\n",Serial_end);
        printf("\tSerial_add_ecc = \t%d\n",Serial_add_ecc);
        fd = open(Serial, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开Serial镜像：[ %s ]\n", Serial);
            return (-1);
        }
        if (Serial_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"Serial镜像开始地址在0x%X，这不是页面大小的倍数\n", Serial_start);
            return (-1);
        }
        if (Serial_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"Serial镜像结束地址在0x%X，这不是页面大小的倍数\n", Serial_end);
            return (-1);
        }
        close(fd);
    }

    if (Kernel)
    {
        printf("\tKernel = \t%s\n",Kernel);
        printf("\tKernel_start = \t0x%X\n",Kernel_start);
        printf("\tKernel_end = \t0x%X\n",Kernel_end);
        printf("\tKernel_add_ecc = \t%d\n",Kernel_add_ecc);
        fd = open(Kernel, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开Kernel镜像：[ %s ]\n", Kernel);
            return (-1);
        }
        if (Kernel_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(Kernel镜像开始地址在0x%X，这不是页面大小的倍数\n", Kernel_start);
            return (-1);
        }
        if (Kernel_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"Kernel镜像结束地址在0x%X，这不是页面大小的倍数\n", Kernel_end);
            return (-1);
        }
        close(fd);
    }

    if (ROOT_FS)
    {
        printf("\tROOT_FS = \t%s\n",ROOT_FS);
        printf("\tROOT_FS_start = \t0x%X\n",ROOT_FS_start);
        printf("\tROOT_FS_end = \t0x%X\n",ROOT_FS_end);
        printf("\tROOT_FS_obb = \t%d\n",ROOT_FS_oob);
        printf("\tROOT_FS_add_ecc = \t%d\n",ROOT_FS_add_ecc);
        fd = open(ROOT_FS, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开ROOT_FS镜像：[ %s ]\n", ROOT_FS);
            return (-1);
        }
        if (ROOT_FS_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(ROOT_FS镜像开始地址在0x%X，这不是页面大小的倍数\n", ROOT_FS_start);
            return (-1);
        }
        if (ROOT_FS_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(ROOT_FS镜像结束地址在0x%X，这不是页面大小的倍数\n", ROOT_FS_end);
            return (-1);
        }
        close(fd);
    }

    if (Settings)
    {
        printf("\tSettings = \t%s\n",Settings);
        printf("\tSettings_start = \t0x%X\n",Settings_start);
        printf("\tSettings_end = \t0x%X\n",Settings_end);
        printf("\tSettings_obb = \t%d\n",Settings_oob);
        printf("\tSettings_add_ecc = \t%d\n",Settings_add_ecc);
        fd = open(Settings, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开Settings镜像：[ %s ]\n", Settings);
            return (-1);
        }
        if (Settings_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(Settings镜像开始地址在0x%X，这不是页面大小的倍数\n", Settings_start);
            return (-1);
        }
        if (Settings_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(Settings镜像结束地址在0x%X，这不是页面大小的倍数\n", Settings_end);
            return (-1);
        }
        close(fd);
    }

    if (Program_FS)
    {
        printf("\tProgram_FS = \t%s\n",Program_FS);
        printf("\tProgram_FS_start = \t0x%X\n",Program_FS_start);
        printf("\tProgram_FS_end = \t0x%X\n",Program_FS_end);
        printf("\tProgram_FS_obb = \t%d\n",Program_FS_oob);
        printf("\tProgram_FS_add_ecc = \t%d\n",Program_FS_add_ecc);
        fd = open(Program_FS, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开Program_FS镜像：[ %s ]\n", Program_FS);
            return (-1);
        }
        if (Program_FS_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(Program_FS镜像开始地址在0x%X，这不是页面大小的倍数\n", Program_FS_start);
            return (-1);
        }
        if (Program_FS_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(Program_FS镜像结束地址在0x%X，这不是页面大小的倍数\n", Program_FS_end);
            return (-1);
        }
        close(fd);
    }

    if (Data_FS)
    {
        printf("\tData_FS = \t%s\n",Data_FS);
        printf("\tData_FS_start = \t0x%X\n",Data_FS_start);
        printf("\tData_FS_end = \t0x%X\n",Data_FS_end);
        printf("\tData_FS_obb = \t%d\n",Data_FS_oob);
        printf("\tData_FS_add_ecc = \t%d\n",Data_FS_add_ecc);
        fd = open(Data_FS, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开Data_FS镜像：[ %s ]\n", Data_FS);
            return (-1);
        }
        if (Data_FS_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(Data_FS镜像开始地址在0x%X，这不是页面大小的倍数\n", Data_FS_start);
            return (-1);
        }
        if (Data_FS_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(Data_FS镜像结束地址在0x%X，这不是页面大小的倍数\n", Data_FS_end);
            return (-1);
        }
        close(fd);
    }

    if (User_FS)
    {
        printf("\tUser_FS = \t%s\n",User_FS);
        printf("\tUser_FS_start = \t0x%X\n",User_FS_start);
        printf("\tUser_FS_end = \t0x%X\n",User_FS_end);
        printf("\tUser_FS_obb = \t%d\n",User_FS_oob);
        printf("\tUser_FS_add_ecc = \t%d\n",User_FS_add_ecc);
        fd = open(User_FS, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开User_FS镜像：[ %s ]\n", User_FS);
            return (-1);
        }
        if (User_FS_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(User_FS镜像开始地址在0x%X，这不是页面大小的倍数\n", User_FS_start);
            return (-1);
        }
        if (User_FS_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(User_FS镜像结束地址在0x%X，这不是页面大小的倍数\n", User_FS_end);
            return (-1);
        }
        close(fd);
    }

    if (User_Disk)
    {
        printf("\tUser_Disk = \t%s\n",User_Disk);
        printf("\tUser_Disk_start = \t0x%X\n",User_Disk_start);
        printf("\tUser_Disk_end = \t0x%X\n",User_Disk_end);
        printf("\tUser_Disk_obb = \t%d\n",User_Disk_oob);
        printf("\tUser_Disk_add_ecc = \t%d\n",User_Disk_add_ecc);
        fd = open(User_Disk, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr,"无法打开User_Disk镜像：[ %s ]\n", User_Disk);
            return (-1);
        }
        if (User_Disk_start % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(User_Disk镜像开始地址在0x%X，这不是页面大小的倍数\n", User_Disk_start);
            return (-1);
        }
        if (User_Disk_end % PAVO_NAND_PAGE_SIZE)
        {
            fprintf(stderr,"(User_Disk镜像结束地址在0x%X，这不是页面大小的倍数\n", User_Disk_end);
            return (-1);
        }
        close(fd);
    }

    printf("\n");
    
    return (0);
}


static int create_pavo_nandflash()
{
    int fd;
    uint32_t i;
    char buf[PAVO_NAND_PAGE_SIZE + PAVO_NAND_OOB_SIZE];
    fd = open(NAND_Flash, O_RDWR);
    if (fd < 0)
    {
        printf("NAND_Flash镜像 [ %s ] 不存在，正在创建它!\n", NAND_Flash);
        fd = open(NAND_Flash, O_RDWR | O_CREAT);
        if (fd < 0)
        {
            fprintf(stderr,"无法创建NAND_Flash镜像 [ %s ] !\n", NAND_Flash);
            return (-1);
        }
        /*write the empty nand flash image(all 0xff) */
        memset(buf, 0xff, sizeof(buf));
        for (i = 0; i < PAVO_NAND_PAGES; i++)
            write(fd, buf, sizeof(buf));
        printf("创建NAND_Flash镜像 [ %s ] 成功!\n", NAND_Flash);
    }
    return fd;
}

static void put_to_flash(int nand_flash_fd, int image_fd, int image_start, int image_end, int with_oob,int add_ecc)
{
    printf("\t\t开始地址:\t0x%X\n\t\t结束地址:\t0x%X\n\t\tOBB区:\t\t%d\n\t\tECC校验:\t%d\n", image_start, image_end, with_oob, add_ecc);
    
    unsigned char data[PAVO_NAND_PAGE_SIZE];
    unsigned char oob[PAVO_NAND_OOB_SIZE];

    int start_page, end_page, i;
    uint32_t is_continue=1;

    start_page = image_start / PAVO_NAND_PAGE_SIZE;
    end_page = image_end / PAVO_NAND_PAGE_SIZE;

    lseek(nand_flash_fd, start_page * (PAVO_NAND_PAGE_SIZE + PAVO_NAND_OOB_SIZE),
          SEEK_SET);
    lseek(image_fd, 0, SEEK_SET);
    memset(data, 0xff, PAVO_NAND_PAGE_SIZE);
    memset(oob, 0xff, PAVO_NAND_OOB_SIZE);
    for (i = start_page; i < end_page; i++)
    {
        if (read(image_fd, data, PAVO_NAND_PAGE_SIZE)< PAVO_NAND_PAGE_SIZE)
            is_continue=0;
        if (with_oob)
        {
            /*image has oob */
            if (read(image_fd, oob, PAVO_NAND_OOB_SIZE)<PAVO_NAND_OOB_SIZE)
            	is_continue=0;
        }
        else
        {
        	 /*if (add_ecc)
            	for (j = 0; j < PAVO_NAND_PAGE_SIZE / 256; j++)
                nand_calculate_ecc(data + j * 256,
                                   oob + PAVO_NAND_ECC_OFFSET + j * 3);*/
           /*TODO:ECC*/
        }
        write(nand_flash_fd, data, PAVO_NAND_PAGE_SIZE);
        write(nand_flash_fd, oob, PAVO_NAND_OOB_SIZE);
        if (!is_continue)
        	break;
    }
}


static void put_U_boot(int nand_flash_fd)
{
    int fd;
    fd = open(U_boot, O_RDWR);
    printf("\t正在写入U_boot镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", U_boot, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, U_boot_start, U_boot_end, 0,U_boot_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

static void put_Serial(int nand_flash_fd)
{
    int fd;
    fd = open(Serial, O_RDWR);
    printf("\t正在写入Serial镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", Serial, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, Serial_start, Serial_end, 0,Serial_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

static void put_Kernel(int nand_flash_fd)
{
    int fd;
    fd = open(Kernel, O_RDWR);
    printf("\t正在写入Kernel镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", Kernel, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, Kernel_start, Kernel_end, 0,Kernel_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

static void put_ROOT_FS(int nand_flash_fd)
{
    int fd;
    fd = open(ROOT_FS, O_RDWR);
    printf("\t正在写入ROOT_FS镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", ROOT_FS, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, ROOT_FS_start, ROOT_FS_end, ROOT_FS_oob,ROOT_FS_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

static void put_Settings(int nand_flash_fd)
{
    int fd;
    fd = open(Settings, O_RDWR);
    printf("\t正在写入Settings镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", Settings, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, Settings_start, Settings_end, Settings_oob,Settings_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

static void put_Program_FS(int nand_flash_fd)
{
    int fd;
    fd = open(Program_FS, O_RDWR);
    printf("\t正在写入Program_FS镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", Program_FS, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, Program_FS_start, Program_FS_end, Program_FS_oob,Program_FS_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

static void put_Data_FS(int nand_flash_fd)
{
    int fd;
    fd = open(Data_FS, O_RDWR);
    printf("\t正在写入Data_FS镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", Data_FS, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, Data_FS_start, Data_FS_end, Data_FS_oob,Data_FS_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

static void put_User_FS(int nand_flash_fd)
{
    int fd;
    fd = open(User_FS, O_RDWR);
    printf("\t正在写入User_FS镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", User_FS, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, User_FS_start, User_FS_end, User_FS_oob,User_FS_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

static void put_User_Disk(int nand_flash_fd)
{
    int fd;
    fd = open(User_Disk, O_RDWR);
    printf("\t正在写入User_Disk镜像：[ %s ] 到NAND_Flash镜像：[ %s ] \n", User_Disk, NAND_Flash);
    put_to_flash(nand_flash_fd, fd, User_Disk_start, User_Disk_end, User_Disk_oob,User_Disk_add_ecc);
    printf("\t成功！\n\n");
    close(fd);
}

int main(int argc, char **argv)
{
    int nand_flash_fd;
    printf("正在解析参数\n");
    if(read_configure(argc, argv) < 0)
    {
        fprintf(stderr,"解析参数时出错！\n");
        exit(-1);
    }

    if (valid_conf() < 0)
        exit(-1);
    nand_flash_fd = create_pavo_nandflash();
    if (nand_flash_fd < 0)
        exit(-1);
    if (U_boot)
        put_U_boot(nand_flash_fd);
    if (Serial)
        put_Serial(nand_flash_fd);
    if (Kernel)
        put_Kernel(nand_flash_fd);
    if (ROOT_FS)
        put_ROOT_FS(nand_flash_fd);
    if (Settings)
        put_Settings(nand_flash_fd);
    if (Program_FS)
        put_Program_FS(nand_flash_fd);
    if (Data_FS)
        put_Data_FS(nand_flash_fd);
    if (User_FS)
        put_User_FS(nand_flash_fd);
    if (User_Disk)
        put_User_Disk(nand_flash_fd);

    close(nand_flash_fd);

    return (0);
}
