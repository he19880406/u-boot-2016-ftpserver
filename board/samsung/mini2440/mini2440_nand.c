#include <common.h>
#include <s3c2410.h>

#define BUSY            1

#define NAND_SECTOR_SIZE    512
#define NAND_BLOCK_MASK     (NAND_SECTOR_SIZE - 1)

#define NAND_SECTOR_SIZE_LP    2048
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

void nand_init_ll(void);
void nand_read_ll(unsigned char *buf, unsigned long start_addr, int size);

static void nand_reset(void);
static void wait_idle(void);
static void nand_select_chip(void);
static void nand_deselect_chip(void);
static void write_cmd(int cmd);
static void write_addr(unsigned int addr);
static unsigned char read_data(void);

#if defined(CONFIG_S3C2410)
static void s3c2410_nand_reset(void);
static void s3c2410_wait_idle(void);
static void s3c2410_nand_select_chip(void);
static void s3c2410_nand_deselect_chip(void);
static void s3c2410_write_cmd(int cmd);
static void s3c2410_write_addr(unsigned int addr);
static unsigned char s3c2410_read_data(void);

#elif defined(CONFIG_S3C2440)
static void s3c2440_nand_reset(void);
static void s3c2440_wait_idle(void);
static void s3c2440_nand_select_chip(void);
static void s3c2440_nand_deselect_chip(void);
static void s3c2440_write_cmd(int cmd);
static void s3c2440_write_addr(unsigned int addr);
static unsigned char s3c2440_read_data(void);
#endif /* defined(CONFIG_S3C2410) | defined(CONFIG_S3C2440)*/

#if defined(CONFIG_S3C2410)
static void s3c2410_nand_reset(void)
{
    s3c2410_nand_select_chip();
    s3c2410_write_cmd(0xff);
    s3c2410_wait_idle();
    s3c2410_nand_deselect_chip();
}

static void s3c2410_wait_idle(void)
{
    int i;
	S3C2410_NAND * s3c2410nand = (S3C2410_NAND *)0x4e000000;

    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFSTAT;
    while(!(*p & BUSY))
        for(i=0; i<10; i++);
}

static void s3c2410_nand_select_chip(void)
{
    int i;
	S3C2410_NAND * s3c2410nand = (S3C2410_NAND *)0x4e000000;

    s3c2410nand->NFCONF &= ~(1<<11);
    for(i=0; i<10; i++);
}

static void s3c2410_nand_deselect_chip(void)
{
	S3C2410_NAND * s3c2410nand = (S3C2410_NAND *)0x4e000000;

    s3c2410nand->NFCONF |= (1<<11);
}

static void s3c2410_write_cmd(int cmd)
{
	S3C2410_NAND * s3c2410nand = (S3C2410_NAND *)0x4e000000;

    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFCMD;
    *p = cmd;
}

static void s3c2410_write_addr(unsigned int addr)
{
    int i;
	S3C2410_NAND * s3c2410nand = (S3C2410_NAND *)0x4e000000;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFADDR;

    *p = addr & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 9) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 17) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 25) & 0xff;
    for(i=0; i<10; i++);
}

static unsigned char s3c2410_read_data(void)
{
	S3C2410_NAND * s3c2410nand = (S3C2410_NAND *)0x4e000000;

    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFDATA;
    return *p;
}
#elif defined(CONFIG_S3C2440)
static void s3c2440_nand_reset(void)
{
    s3c2440_nand_select_chip();
    s3c2440_write_cmd(0xff);
    s3c2440_wait_idle();
    s3c2440_nand_deselect_chip();
}

static void s3c2440_wait_idle(void)
{
    int i;
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFSTAT;

    while(!(*p & BUSY))
        for(i=0; i<10; i++);
}

static void s3c2440_nand_select_chip(void)
{
    int i;
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

    s3c2440nand->NFCONT &= ~(1<<1);
    for(i=0; i<10; i++);
}

static void s3c2440_nand_deselect_chip(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

    s3c2440nand->NFCONT |= (1<<1);
}

static void s3c2440_write_cmd(int cmd)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFCMD;
    *p = cmd;
}

static void s3c2440_write_addr(unsigned int addr)
{
    int i;
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;

    *p = addr & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 9) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 17) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 25) & 0xff;
    for(i=0; i<10; i++);
}


static void s3c2440_write_addr_lp(unsigned int addr)
{
    int i;
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
	int col, page;

	col = addr & NAND_BLOCK_MASK_LP;
	page = addr / NAND_SECTOR_SIZE_LP;

    *p = col & 0xff;			/* Column Address A0~A7 */
    for(i=0; i<10; i++);
    *p = (col >> 8) & 0x0f;		/* Column Address A8~A11 */
    for(i=0; i<10; i++);
    *p = page & 0xff;			/* Row Address A12~A19 */
    for(i=0; i<10; i++);
    *p = (page >> 8) & 0xff;	/* Row Address A20~A27 */
    for(i=0; i<10; i++);
    *p = (page >> 16) & 0x03;	/* Row Address A28~A29 */
    for(i=0; i<10; i++);
}

static unsigned char s3c2440_read_data(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFDATA;
    return *p;
}
#endif

static void nand_reset(void)
{
#if defined(CONFIG_S3C2410)
    if (isS3C2410)
	{
	    s3c2410_nand_reset();
	}
	else
#endif
	{
	    s3c2440_nand_reset();
	}
}

static void wait_idle(void)
{
#if defined(CONFIG_S3C2410)
    if (isS3C2410)
	{
	    s3c2410_wait_idle();
	}
	else
#endif
	{
	    s3c2440_wait_idle();
	}
}

static void nand_select_chip(void)
{
    int i;
#if defined(CONFIG_S3C2410)
    if (isS3C2410)
	{
	    s3c2410_nand_select_chip();
	}
	else
#endif
	{
	    s3c2440_nand_select_chip();
	}

    for(i=0; i<10; i++);
}

static void nand_deselect_chip(void)
{
#if defined(CONFIG_S3C2410)
    if (isS3C2410)
	{
	    s3c2410_nand_deselect_chip();
	}
	else
#endif
	{
	    s3c2440_nand_deselect_chip();
	}
}

static void write_cmd(int cmd)
{
#if defined(CONFIG_S3C2410)
    if (isS3C2410)
	{
	    s3c2410_write_cmd(cmd);
	}
	else
#endif
	{
	    s3c2440_write_cmd(cmd);
	}
}
static void write_addr(unsigned int addr)
{
#if defined(CONFIG_S3C2410)
    if (isS3C2410)
	{
	    s3c2410_write_addr(addr);
	}
	else
#endif
	{
	    s3c2440_write_addr(addr);
	}
}

static void write_addr_lp(unsigned int addr)
{
#if defined(CONFIG_S3C2410)
    if (isS3C2410)
	{
	    s3c2410_write_addr(addr);
	}
	else
#endif
	{
	    s3c2440_write_addr_lp(addr);
	}
}

static unsigned char read_data(void)
{
#if defined(CONFIG_S3C2410)
    if (isS3C2410)
	{
	    return s3c2410_read_data();
	}
	else
#endif
	{
	    return s3c2440_read_data();
	}
}

void nand_init_ll(void)
{
#if defined(CONFIG_S3C2410)
	S3C2410_NAND * s3c2410nand = (S3C2410_NAND *)0x4e000000;
#endif
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0

#if defined(CONFIG_S3C2410)
    if (isS3C2410)
    {
        s3c2410nand->NFCONF = (1<<15)|(1<<12)|(1<<11)|(TACLS<<8)|(TWRPH0<<4)|(TWRPH1<<0);
    }
    else
#endif
    {
        s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
        s3c2440nand->NFCONT = (1<<4)|(1<<1)|(1<<0);
    }

	nand_reset();
}


void nand_read_ll(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;

    if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) {
        return ;
    }

    nand_select_chip();

    for(i=start_addr; i < (start_addr + size);) {
      write_cmd(0);

      write_addr(i);
      wait_idle();

      for(j=0; j < NAND_SECTOR_SIZE; j++, i++) {
          *buf = read_data();
          buf++;
      }
    }

    nand_deselect_chip();

    return ;
}


void nand_read_ll_lp(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;

    if ((start_addr & NAND_BLOCK_MASK_LP) || (size & NAND_BLOCK_MASK_LP)) {
        return ;
    }

    nand_select_chip();

    for(i=start_addr; i < (start_addr + size);) {
      write_cmd(0);

      write_addr_lp(i);
	  write_cmd(0x30);
      wait_idle();

      for(j=0; j < NAND_SECTOR_SIZE_LP; j++, i++) {
          *buf = read_data();
          buf++;
      }
    }

    nand_deselect_chip();

    return ;
}

int bBootFrmNORFlash(void)
{
    volatile unsigned int *pdw = (volatile unsigned int *)0;
    unsigned int dwVal;

    dwVal = *pdw;
    *pdw = 0x12345678;
    if (*pdw != 0x12345678)
    {
        return 1;
    }
    else
    {
        *pdw = dwVal;
        return 0;
    }
}

int CopyCode2Ram(unsigned long start_addr, unsigned char *buf, int size)
{
    unsigned int *pdwDest;
    unsigned int *pdwSrc;
    int i;

    if (bBootFrmNORFlash())
    {
        pdwDest = (unsigned int *)buf;
        pdwSrc  = (unsigned int *)start_addr;

        for (i = 0; i < size / 4; i++)
        {
            pdwDest[i] = pdwSrc[i];
        }
        return 0;
    }
    else
    {
		nand_init_ll();
        nand_read_ll_lp(buf, start_addr, (size + NAND_BLOCK_MASK_LP)&~(NAND_BLOCK_MASK_LP));
		return 0;
    }
}

