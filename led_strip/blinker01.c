void PUT16 ( unsigned int, unsigned int );
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );
void ASMDELAY ( unsigned int );

#define PORTC_PCR5 0x4004B014
#define PORTC_PCR6 0x4004B018
#define GPIOC_PDOR 0x400FF080
#define GPIOC_PSOR 0x400FF084
#define GPIOC_PCOR 0x400FF088
#define GPIOC_PTOR 0x400FF08C
#define GPIOC_PDDR 0x400FF094
#define SIM_SCGC5  0x40048038

#define WDOG_STCTRLH 0x40052000
#define WDOG_UNLOCK  0x4005200E


int notmain ( void )
{
    // int rx=0;
    //Turn off watchdog and allow update
    PUT16(WDOG_UNLOCK,0xC520);
    PUT16(WDOG_UNLOCK,0xD928);
    PUT16(WDOG_STCTRLH,0x0010);

    //Enable GPIO
    PUT32(SIM_SCGC5,GET32(SIM_SCGC5)|(1<<11));

    //configure Port C5
    PUT32(PORTC_PCR5,(1<<8));
    PUT32(GPIOC_PDDR,GET32(GPIOC_PDDR)|(1<<5));

    // Pin 11 = C6
    PUT32(PORTC_PCR6,(1<<8));
    PUT32(GPIOC_PDDR,GET32(GPIOC_PDDR)|(1<<6));

    // int i=0;
    while(1)
    {
        // for (i=0; i<24; i++)
        //     PUT32(GPIOC_PSOR,1<<6);
        // for (i=0; i<16; i++)
        //     PUT32(GPIOC_PCOR,1<<6);
        // PUT32(GPIOC_PTOR,1<<5);
        PUT32(GPIOC_PSOR,1<<5);
        PUT32(GPIOC_PTOR,1<<6);
            // for(rx=0;rx<1000000;rx++) dummy(rx);
        // PUT32(GPIOC_PTOR,1<<5);
        // for(rx=0;rx<1000000;rx++) dummy(rx);
    }
}

