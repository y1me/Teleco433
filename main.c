#include <xc.h>

// CONFIG1
#pragma config FOSC = INTOSC    //  (INTOSC oscillator; I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOREN = OFF    // Low Power Brown-out Reset enable bit (LPBOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#define OUT         PORTAbits.RA4
#define CODELENGTH  36
#define REPEAT      120

volatile struct chbits{
						unsigned Tim320u:1; 
						unsigned TxUart:1; 
						unsigned Sys_Init:1; 
						unsigned tim100u:1; 
						unsigned Button:1; 
						unsigned Data1:1; 
						unsigned Data2:1; 
						unsigned cont:1;
		
}flag ;

volatile unsigned char timer0ReloadVal;

unsigned int number = 0, process;
const unsigned int code = 0b0000100000011001;
int test;
    


void TMR0_Initialize(void)
{
    // Set TMR0 to the options selected in the User Interface

    // PSA assigned; PS 1:4; TMRSE Increment_hi_lo; mask the nWPUEN and INTEDG bits
    OPTION_REGbits.nWPUEN = 1;
    OPTION_REGbits.INTEDG = 1;
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.TMR0SE = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0;

    // TMR0 251; 
    TMR0 = 247;

    // Load the TMR value to reload variable
    timer0ReloadVal= 108;

    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;

    // Enabling TMR0 interrupt
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IE == 1;

}

void OSCILLATOR_Initialize(void)
{
    // SCS FOSC; SPLLEN disabled; IRCF 250KHz_MF; 
    OSCCON = 0x30;
    OSCCONbits.IRCF = 0xD;
    // TUN 0; 
    OSCTUNE = 0x00;
    // Set the secondary oscillator
    
}

void WDT_Initialize(void)
{
    // WDTPS 1:65536; SWDTEN OFF; 
    WDTCON = 0x16;
}

void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */   
    LATA = 0x00;    

    /**
    TRISx registers
    */    
    TRISAbits.TRISA4 = 0;
    PORTAbits.RA4 = 0;
    //portest
    TRISAbits.TRISA2 = 1;
    PORTAbits.RA2 = 0;
    
    /**
    ANSELx registers
    */   
    ANSELA = 0x00;

    /**
    WPUx registers
    */ 
    WPUA = 0x3F;
    OPTION_REGbits.nWPUEN = 0;

    
    /**
    APFCONx registers
    */
    APFCON = 0x00;


   
    
}  

void interrupt INTERRUPT_InterruptManager (void)
{
    // interrupt handler
    if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1)
    {
        INTCONbits.TMR0IF = 0;
        flag.Tim320u = 1;
        PORTAbits.RA2 = ~PORTAbits.RA2;

        TMR0 = timer0ReloadVal;
    }
    else
    {
        //Unhandled Interrupt
    }
}
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
void main(void)
{
    // initialize the device
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    TMR0_Initialize();
    
    //Enable Interrupts
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    flag.Tim320u = 0;
    number = 0;

    while (1)
    {
        if (flag.Tim320u)
        {
            
            if (number == 0)
            {
                OUT = 1;
                process = code;
                test = 0;
            }
            else
            {
                if (number <= CODELENGTH)
                {
                    
                    //OUT = ~OUT;
                    if (test == 0) OUT = 0;
                    if (test == 1)
                    {
                        if (process & 1) OUT = 1;
                        else OUT = 0;
                        process = process >> 1;       
                    }
                    if (test == 2) OUT = 1;
                    test++;
                    if (test == 3)  test = 0;
                    

                    /*
                    

                    

                    if ((number % 3) == 2)
                    {
                        if (process & 1) OUT = 1;
                        else OUT = 0;
                        process = process >> 1;       
                    }
 * */
                }
                else OUT = 0;
            }
            
            flag.Tim320u = 0;
            
            number++;
            
            if (number > REPEAT)    
            {
                number = 0;
            }
            
               
        }

    }
}
/**
 End of File
*/