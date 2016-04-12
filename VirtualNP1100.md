# 介绍 #
这个页面教你如何在设备上运行NP1100的Linux


# 目录 #
  * [编译用来虚拟JZ4740芯片的Qemu虚拟机](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#编译_用来虚拟_JZ4740_芯片的_Qemu_虚拟机)
  * [编译用来生成虚拟Nand镜像的NandFlash工具](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#编译_用来生成_虚拟_Nand_镜像的_工具)
  * [编译Yaffs2镜像生成工具](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#编译_Yaffs2_镜像生成工具)
  * [编译NP1100的U-Boot引导程序](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#编译_NP1100_的_U-Boot_引导程序)
  * [编译NP1100的Linux内核](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#编译_NP1100_的_Linux_内核)
  * [制作或下载根文件系统镜像](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#制作或下载根文件系统镜像)
    * [制作根目录镜像](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#制作_根目录_镜像)
    * [下载根目录镜像](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#下载_根目录_镜像)
  * [下载ProgramFS镜像](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#下载_ProgramFS_镜像)
  * [下载Settings镜像](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#下载_Settings_镜像)
  * [下载DataFS镜像](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#下载_DataFS_镜像)
  * [下载UsrFS镜像](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#下载_UsrFS_镜像)
  * [开始制作NandFlash镜像](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#开始制作_镜像)
  * [运行你的虚拟机](http://code.google.com/p/virtualnoah/wiki/VirtualNP1100#运行你的虚拟机)


# 具体操作 #

## 编译 用来虚拟 JZ4740 芯片的 Qemu 虚拟机 ##
在你的工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/qemu-NP_120730.tar.bz2
tar jxvf qemu-NP_120730.tar.bz2
cd qemu-NP
```
接下来，如果你在Linux、win等系统下，那么执行：
```
./configure --target-list=mipsel-softmmu
```
如果你是Mac系统，那么执行：
```
./configure --disable-sdl --disable-aio --enable-cocoa --prefix=/opt/qemu --target-list=mipsel-softmmu
```
接下来执行
```
make
```
如果说，你编译时候出现一些莫名其妙的错误，请尝试使用GCC-3.4编译器。
当你顺利完成后，可以在./mipsel-softmmu/这个目录下找到qemu-system-mipsel，将它拷贝出来，待用。

## 编译 用来生成 虚拟 Nand 镜像的 NandFlash 工具 ##
在你的工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/nandflash-NP_120730.tar.bz2
tar jxvf nandflash-NP_120730.tar.bz2 
cd nandflash-NP
make
```
然后，你可以在当前目录下找到pavo\_nandflash，将它拷贝到工作目录，待用。

## 编译 Yaffs2 镜像生成工具 ##
在你的工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/yaffs2-NP_120730.tar.bz2
tar jxvf yaffs2-NP_120730.tar.bz2
cd yaffs2-NP/utils
make
```
然后，你可以在当前目录下找到mkyaffs2image，将它拷贝到工作目录，待用。

## 编译 NP1100 的 U-Boot 引导程序 ##
在你的工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/u-boot-NP1100_120730.tar.bz2
tar jxvf u-boot-NP1100_120730.tar.bz2
cd u-boot-NP1100
make
```
然后，你可以在当前目录下找到u-boot-nand.bin，将它拷贝到工作目录，待用。

## 编译 NP1100 的 Linux 内核 ##
在你的工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/linux-2.6.24.3-NP1100_120730.tar.bz2
tar jxvf linux-2.6.24.3-NP1100_120730.tar.bz2
cd linux-2.6.24.3-NP1100
make uImage
```
然后，你可以在./arch/mips/boot/这个目录下找到uImage，将它拷贝到工作目录，待用。

## 制作或下载根文件系统镜像 ##
对于根目录镜像，接下来你有两个选择，一个是下载根目录的压缩包，然后用上面编译生成的mkyaffs2image工具来生成根目录的镜像，或者另一个方法是是直接下载我制作好的根目录镜像。
所以，请在接下来的两个小标题「制作 根目录 镜像」和「下载 根目录 镜像」任选其一进行操作。
### 制作 根目录 镜像 ###
在你的工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/rootfs-NP1100_120730.tar.bz2
```
接下来是解压压缩包，因为根目录中的dev目录下存在很多设备，所以用普通用户是创建不了的，一定要用超级用户，
取得超级用户权限后，执行：
```
tar jxvf rootfs-NP1100_120730.tar.bz2
```
如果是Mac或者Ubuntu等发行版的还可以通过在前面加上sudo，赋予他root权限。
```
sudo tar jxvf rootfs-NP1100_120730.tar.bz2
```
好，接下来是调用刚刚我们的mkyaffs2image工具。
```
./mkyaffs2image 1 rootfs rootfs.yaffs2
```
至此，根目录的yaffs2镜像已经制作完毕，将rootfs.yaffs2拷贝至工作目录，待用。
### 下载 根目录 镜像 ###
如果选择下载我制作好的根目录，你将没法对他进行二次修改，但是你可以在需要修改时候重新进行「制作 根目录 镜像」的操作。
在工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/rootfs-NP1100_120730.yaffs2.tar.bz2
tar jxvf rootfs-NP1100_120730.yaffs2.tar.bz2
```

## 下载 ProgramFS 镜像 ##
这个镜像是从NP1100官方的升级包中提取出来的，所以，就没法修改喽。
在工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/ProgramFS-NP1100_120730.yaffs2.tar.bz2
tar jxvf ProgramFS-NP1100_120730.yaffs2.tar.bz2
```

## 下载 Settings 镜像 ##
这个镜像也是从NP1100官方的升级包中提取出来的。
在工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/Settings-NP1100_120730.yaffs2.tar.bz2
tar jxvf Settings-NP1100_120730.yaffs2.tar.bz2
```

## 下载 DataFS 镜像 ##
这个镜像也是从NP1100官方的升级包中提取出来的。
在工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/DataFS-NP1100_120730.yaffs2.tar.bz2
tar jxvf DataFS-NP1100_120730.yaffs2.tar.bz2
```

## 下载 UsrFS 镜像 ##
这个镜像也是从NP1100官方的升级包中提取出来的。
在工作目录执行：
```
wget http://virtualnoah.googlecode.com/files/UsrFS-NP1100_120730.yaffs2.tar.bz2
tar jxvf UsrFS-NP1100_120730.yaffs2.tar.bz2
```
至此，所有需要的东西已经全部准备完毕了。

## 开始制作 NandFlash 镜像 ##
在你的工作目录执行：
```
./pavo_nandflash \
	--NAND_Flash \
		np1100.bin \
	--U_boot \
		u-boot-nand.bin \
		0x00000000 \
		0x00200000 \
		0 \
	--Kernel \
		uImage \
		0x00400000 \
		0x00600000 \
		0 \
	--ROOT_FS \
		rootfs.yaffs2 \
		0x00600000 \
		0x01800000 \
		1 \
		0 \
	--Settings \
		Settings.yaffs2 \
		0x01800000 \
		0x02300000 \
		1 \
		0 \
	--Program_FS \
		ProgramFS.yaffs2 \
		0x02300000 \
		0x0cd00000 \
		1 \
		0 \
	--Data_FS \
		DataFS.yaffs2 \
		0x0cd00000 \
		0x15900000 \
		1 \
		0 \
	--User_FS \
		UsrFS.yaffs2 \
		0x15900000 \
		0x16500000 \
		1 \
		0 
chmod 777 np1100.bin
```
完成，np1100.bin这个文件就是我们所需要的NandFlash镜像。

## 运行你的虚拟机 ##
接下来，就可以运行你的虚拟机了。
在工作目录执行：
```
./qemu-system-mipsel -M pavo -cpu jz4740 -mtdblock np1100.bin
```
然后，你就可以等待他的启动了。


# 效果图 #

![http://link3c.youzhaopian.com/pplink/aVgSKcrYjDCrTIJI39fE4S7W4X5Evdfe49vCXNQsL6ASUJ8CSKMG7A==.jpg](http://link3c.youzhaopian.com/pplink/aVgSKcrYjDCrTIJI39fE4S7W4X5Evdfe49vCXNQsL6ASUJ8CSKMG7A==.jpg)

![http://link3c.youzhaopian.com/pplink/aVgSKcrYjDAE/3zSyc8j8Lk7pGwWp5huFqfGqzVNcqMxGDQT3nduaQ==.jpg](http://link3c.youzhaopian.com/pplink/aVgSKcrYjDAE/3zSyc8j8Lk7pGwWp5huFqfGqzVNcqMxGDQT3nduaQ==.jpg)

![http://link3c.youzhaopian.com/pplink/aVgSKcrYjDDzKtKOs5925t2CVkIBJVT3jvZH00T9ZWELAiH6iEs7zQ==.jpg](http://link3c.youzhaopian.com/pplink/aVgSKcrYjDDzKtKOs5925t2CVkIBJVT3jvZH00T9ZWELAiH6iEs7zQ==.jpg)

![http://link3c.youzhaopian.com/pplink/aVgSKcrYjDDXB9oyQG96njFw0IoYylXMlHA2sbl6AbOEJE/qRPWIxg==.jpg](http://link3c.youzhaopian.com/pplink/aVgSKcrYjDDXB9oyQG96njFw0IoYylXMlHA2sbl6AbOEJE/qRPWIxg==.jpg)

![http://link3c.youzhaopian.com/pplink/aVgSKcrYjDCL53GNQKK9JMK1Wp1bLEXNwqO9DoJGRVN3j5Ooch2gIQ==.jpg](http://link3c.youzhaopian.com/pplink/aVgSKcrYjDCL53GNQKK9JMK1Wp1bLEXNwqO9DoJGRVN3j5Ooch2gIQ==.jpg)