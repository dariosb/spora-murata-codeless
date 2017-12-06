/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization. 
 *
 * Copyright (C) 2012. Dialog Semiconductor Ltd, unpublished work. This computer 
 * program includes Confidential, Proprietary Information and is a Trade Secret of 
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited 
 * unless authorized in writing. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration
#include "user_periph_setup.h"            // peripheral configuration
#include "global_io.h"
#include "gpio.h"
#include "uart.h"                    // UART initialization
#include "user_cmd_interpreter.h"

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 *
 * @return void
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
* Globally reserved GPIOs reservation
*/

/*
* Application specific GPIOs reservation. Used only in Development mode (#if DEVELOPMENT_DEBUG)
    
i.e.  
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);    //Reserve P_01 as Generic Purpose I/O
*/
#ifdef CFG_PRINTF_UART2
	RESERVE_GPIO( UART2_TX, UART2_TX_GPIO_PORT,  UART2_TX_GPIO_PIN, PID_UART2_TX);
	RESERVE_GPIO( UART2_RX, UART2_RX_GPIO_PORT,  UART2_RX_GPIO_PIN, PID_UART2_RX);
#endif
    
}

#endif //DEVELOPMENT_DEBUG

/**
 ****************************************************************************************
 * @brief Map port pins
 *
 * The Uart and SPI port pins and GPIO ports are mapped
 ****************************************************************************************
 */
void set_pad_functions(void)        // set gpio port function mode
{

/*
* Configure application ports.
i.e.    
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false ); // Set P_01 as Generic purpose Output
*/    
    
#ifdef CFG_PRINTF_UART2
	GPIO_ConfigurePin( UART2_TX_GPIO_PORT, UART2_TX_GPIO_PIN, OUTPUT, PID_UART2_TX, false );
	GPIO_ConfigurePin( UART2_RX_GPIO_PORT, UART2_RX_GPIO_PIN, INPUT, PID_UART2_RX, false );
#endif

	// Go through all the IOs 
	for(uint8_t i=0; i<40; i++)
	{
	  
		GPIO_PORT port_number = (GPIO_PORT)(i / 10);
    GPIO_PIN pin_number = (GPIO_PIN)(i % 10);
		IO_FUNC_t func = codeless_env.port_configuration[i];
		
    // Set port configuration accordingly
    if(func == IO_FUNC_INPUT)
      // Standard input port
      GPIO_ConfigurePin(port_number, pin_number, INPUT, PID_GPIO, false );
    
		else if(func == IO_FUNC_INPUT_PULLUP)
      // Input port with pull-up resistor
      GPIO_ConfigurePin(port_number, pin_number, INPUT_PULLUP, PID_GPIO, false );
    
		else if(func == IO_FUNC_INPUT_PULLDOWN)
      // Input port with pull-down resistor
      GPIO_ConfigurePin(port_number, pin_number, INPUT_PULLDOWN, PID_GPIO, false );
    
		else if(func == IO_FUNC_OUTPUT)
    {
			// Output port must be set according to the state it was in before sleep
      if(codeless_env.port_io_state[i])
				GPIO_ConfigurePin(port_number, pin_number, OUTPUT, PID_GPIO, true);
			else
			  GPIO_ConfigurePin(port_number, pin_number, OUTPUT, PID_GPIO, false);
    }
  	
		else if((func == IO_FUNC_ANALOG_INPUT || func == IO_FUNC_ANALOG_INPUT_ATT3) && ((uint8_t)port_number == 0) && ((uint8_t)pin_number <= 3))
      // Analog input port
      GPIO_ConfigurePin(port_number, pin_number, INPUT, PID_ADC, false );            
    
		else if(func == IO_FUNC_I2C_CLOCK)
    {
		  // I2C clock output port
			GPIO_ConfigurePin(port_number, pin_number, OUTPUT, PID_I2C_SCL, false );
    }
    
		else if(func == IO_FUNC_I2C_DATA)
    {
      // I2C data port
      GPIO_ConfigurePin(port_number, pin_number, OUTPUT, PID_I2C_SDA, false );  
    }
	}
}


/**
 ****************************************************************************************
 * @brief Enable pad's and peripheral clocks assuming that peripherals' power domain is down. The Uart and SPi clocks are set.
 *
 * @return void
 ****************************************************************************************
 */
void periph_init(void) 
{
	// Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP)) ; 
    
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
	
	//rom patch
	patch_func();
	
	//Init pads
	set_pad_functions();

    // (Re)Initialize peripherals
    // i.e.
    //    uart_init(UART_BAUDRATE_115K2, 3);

#ifdef CFG_PRINTF_UART2
		SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
		uart2_init(UART_BAUDRATE_57K6, 3);
#endif
    
   // Enable the pads
	SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);
}
