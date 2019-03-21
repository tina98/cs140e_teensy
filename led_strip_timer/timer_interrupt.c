#include <stdint.h>

#define PORTB_PCR16 0x4004A040
#define PORTB_PCR17 0x4004A044
#define PORTC_PCR5 0x4004B014
#define PORTC_PCR6 0x4004B018

#define GPIOC_PDOR 0x400FF080
#define GPIOC_PSOR 0x400FF084
#define GPIOC_PCOR 0x400FF088
#define GPIOC_PTOR 0x400FF08C
#define GPIOC_PDDR 0x400FF094

#define SIM_SCGC4  0x40048034
#define SIM_SCGC5  0x40048038

#define WDOG_STCTRLH 0x40052000
#define WDOG_UNLOCK  0x4005200E

#define STK_CSR 0xE000E010
#define STK_RVR 0xE000E014
#define STK_CVR 0xE000E018

#define OSC0_CR 0x40065000
#define MCG_C1  0x40064000
#define MCG_C2  0x40064001
#define MCG_C3  0x40064002
#define MCG_C4  0x40064003
#define MCG_C5  0x40064004
#define MCG_C6  0x40064005
#define MCG_S   0x40064006

#define SIM_CLKDIV1 0x40048044
#define SIM_CLKDIV2 0x40048048
#define SIM_SOPT2   0x40048004


#define UART0_BDH 0x4006A000
#define UART0_BDL 0x4006A001
#define UART0_C1  0x4006A002
#define UART0_C2  0x4006A003
#define UART0_S1  0x4006A004
#define UART0_C4  0x4006A00A
#define UART0_D   0x4006A007

#define SYST_CSR        *(volatile uint32_t *)0xE000E010 // SysTick Control and Status
#define SYST_CSR_COUNTFLAG      (uint32_t)0x00010000
#define SYST_CSR_CLKSOURCE      (uint32_t)0x00000004
#define SYST_CSR_TICKINT        (uint32_t)0x00000002
#define SYST_CSR_ENABLE         (uint32_t)0x00000001
#define SYST_RVR        *(volatile uint32_t *)0xE000E014 // SysTick Reload Value Register
#define SYST_CVR        *(volatile uint32_t *)0xE000E018 // SysTick Current Value Register
#define SYST_CALIB      *(const    uint32_t *)0xE000E01C // SysTick Calibration Value


void PUT8 ( unsigned int, unsigned int );
void PUT16 ( unsigned int, unsigned int );
void PUT32 ( unsigned int, unsigned int );
unsigned int GET8 ( unsigned int );
unsigned int GET16 ( unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );
void ASMDELAY ( unsigned int );
void systick_handler();

volatile unsigned int systick_interrupt_count = 0;

void led_on(){
    PUT32(GPIOC_PSOR,1<<6);
}
void led_off(){
    PUT32(GPIOC_PCOR,1<<6);
}
void systick_handler() {
    systick_interrupt_count++;
}

unsigned int current_time() {
    volatile unsigned int x = systick_interrupt_count;
    return x;
}

void teensy_delay(uint32_t delay_time) {
    uint32_t start = current_time();
    while ((current_time() - start) < delay_time);
}
void teensy_wait_40(){
    teensy_delay(1);
}
void teensy_wait_85(){
    teensy_delay(2);
}

void code_0(){
    led_on();
    teensy_wait_40();
    led_off();
    teensy_wait_85();
}

void code_1(){
    led_on();
    teensy_wait_85();
    led_off();
    teensy_wait_40();
}

void code_reset(){
    led_off();
    teensy_delay(150);

}

int notmain ( void )
{
    //Turn off watchdog and allow update
    PUT16(WDOG_UNLOCK,0xC520);
    PUT16(WDOG_UNLOCK,0xD928);
    PUT16(WDOG_STCTRLH,0x0010);

    PUT32(SIM_SCGC5,GET32(SIM_SCGC5)|(1<<11));

    //configure Port C5
    PUT32(PORTC_PCR5,(1<<8));
    PUT32(GPIOC_PDDR,GET32(GPIOC_PDDR)|(1<<5));

       // Pin 11 = C6
    PUT32(PORTC_PCR6,(1<<8));
    PUT32(GPIOC_PDDR,GET32(GPIOC_PDDR)|(1<<6));

    //switch to 96Mhz
    //start in FEI mode FLL Engaged Internal
    PUT8(OSC0_CR,0xA);
    PUT8(MCG_C2,(2<<4)|(1<<2));
    PUT8(MCG_C1,(2<<6)|(4<<3));
    while ((GET8(MCG_S) & (1<<1)) == 0) continue;
    while ((GET8(MCG_S) & (1<<4)) != 0) continue;
    while (((GET8(MCG_S)>>2)&3) != 2) continue ;
    //FBE mode FLL Bypassed External
    PUT8(MCG_C5,(3<<0));
    PUT8(MCG_C6,(1<<6)|(0<<0));
    while (!(GET8(MCG_S) & (1<<5))) continue;
    while (!(GET8(MCG_S) & (1<<6))) continue;
    // PBE mode PLL Bypassed External
    PUT32(SIM_CLKDIV1,(0<<28)|(1<<24)|(3<<16));
    PUT8(MCG_C1,(0<<6)|(4<<3));
    while (((GET8(MCG_S)>>2)&3)!=3) continue;
    // PEE mode PLL Engaged External
    PUT32(SIM_CLKDIV2,(1<<1));
    PUT32(SIM_SOPT2,(1<<18)|(1<<16)|(1<<12)|(6<<5));

    // SysTick Reload Value Register:
    //Every 72000 clock cycles -1 = 0xBB7F
    // PUT32(STK_RVR,3);
    PUT32(STK_RVR,35);
    // Systick control/status register: 
    //processor clock, count down to 0 asserts systick exception, enable counter
    PUT32(STK_CSR, 0b111);
    teensy_delay(10000);

    PUT32(GPIOC_PSOR, 1<<5);
    teensy_delay(13000000);
    PUT32(GPIOC_PCOR, 1<<5);

    while(1)
    {
        // PUT32(GPIOC_PTOR,1<<5);
        // teensy_delay(8000000);
        code_reset();
        int i,j;
        
        for (j=0;j<3;j++){
            for (i=0;i<8;i++) {
                code_0();
                code_0();
            }
            for (i=0;i<8;i++) {
                code_1();
                code_1();
            }
            for (i=0;i<8;i++) {
                code_0();
                code_0();
            }  
        }


        code_reset();
    }
        
        // PUT32(GPIOC_PSOR,1<<5);
        // teensy_delay(500);
        // PUT32(GPIOC_PCOR,1<<5);
        // teensy_delay(500);
    // }
    return 1;
}