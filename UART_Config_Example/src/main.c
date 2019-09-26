/*  Test Program for USART on the SAMD20 XPlained ***
*   Init: Sept 23, 2019
*   V Kaiser
*
* Overview: 
* Writes characters to Terminal
* Turns LED0 on/off Via Button0 state, and reports state in terminal
* Read/Echo user-entered character(s) from terminal
*/

/* ASF Modules Used: ******************************************
* Generic board support (driver)
* PORT - GPIO Pin control (driver)
* SERCOM USART - Serial Communications (driver) : callback
* SYSTEM - Core System Driver (driver) 
*/

/**** SERCOM USART Notes ***************************************
* PAD[3:0] Digital IO General SERCOM pins
* TxD - Output, RxD - Input, XCK Output or Input
* According to ASF - EDBG_CDC_MODULE is SERCOM3
* EDBG: Embedded DeBuGger
* CDC: Communications Device Class (eg USB for USART)
*/
#include <asf.h>
#include <stdio.h>
#include <string.h>

struct usart_module usart_instance;

/* Prototypes */
void configure_usart(void);	

/* Functions */
//This is from the SERCOM APP NOTE AT11626 Example Code
void configure_usart(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate = 9600;

	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;	
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	while (usart_init(&usart_instance,
	EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}
	usart_enable(&usart_instance);
} //configure_usart()


int main (void)
{
/* This works, but appears to be bad practice */
	//const char str1[] = "Hello World...\r\n";	
	//const char strON[] = "ON\r\n";					
	//const char strOFF[] = "OFF\r\n";					
	//usart_write_buffer_wait(&usart_instance, &str1, (unsigned)strlen(str1));		
	//usart_write_buffer_wait(&usart_instance, &strON,(unsigned)strlen(strON));
	//usart_write_buffer_wait(&usart_instance, &strOFF,(unsigned)strlen(strOFF));

	uint8_t string[] = "Hello World!\r\n";
	uint8_t strON[] = "On\r\n";
	uint8_t strOFF[] = "Off\r\n";

	uint8_t pushFlag = 0;
	uint8_t rx_char = 0;

	system_init();
	configure_usart();
	
	//Write Hello World to the Serial Terminal:
	usart_write_buffer_wait(&usart_instance, string, sizeof(string));
	uint16_t temp;

	while (1) {
		/* Is button pressed? - GPIO LED and USART Terminal Status Indicators */

		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			// Yes, so turn LED on
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			
			if(pushFlag != 1)
			{
				//And write "ON" - but only once per press.
				usart_write_buffer_wait(&usart_instance, strON, sizeof(strON));
				pushFlag = 1;
			}
			
		} else {
			// No, so turn LED off.
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
			if(pushFlag == 1)
			{
				//And write "OFF" - but only once per press.
				usart_write_buffer_wait(&usart_instance, strOFF, sizeof(strOFF));
				pushFlag = 0;
			}
		}
		// Echo characters typed into Terminal
		if (usart_read_wait(&usart_instance, &temp) == STATUS_OK) {
			
			while (usart_write_wait(&usart_instance, temp) != STATUS_OK) {
			}
		}
	} //while(true)
} //main()
