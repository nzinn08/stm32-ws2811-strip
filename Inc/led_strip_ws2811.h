//File Name: led_strip_ws2811.h
//Description: This provides the interface for using led strips with the ws2811 protocol

//Header Guards
#ifndef LED_STRIP_WS2811_H_
#define LED_STRIP_WS2811_H_

//Include Files
#include "main.h"
//C Interface
#ifdef __cplusplus
extern "C" {
#endif
//C Public Constants
//C Public Variables

//C Public Function Prototypes

#ifdef __cplusplus
}
#endif //End C Interface

//C++ Interface
//Class Definitions
template<uint16_t NUM_LEDS>
class LED_STRIP_WS2811
{
public:
//Constructors
LED_STRIP_WS2811()
{
	this->lastLedChanged = NUM_LEDS - 1;
}
//Public Function Prototypes
void writeEntireStrip(uint8_t red, uint8_t green, uint8_t blue);
/**
 * @brief This writes a new value for led node at @node_number
 * @param node_number: starts at 0 up to NUM_LEDS - 1
 */
void writeLedNode(uint16_t node_number, uint8_t red, uint8_t green, uint8_t blue);
void display();
//Public Constants
//Public Variable
private:
//Private Structs
struct LED_NODE
{
	LED_NODE(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
	{
		this->red = red;
		this->green = green;
		this->blue = blue;
	}
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};
//Private Constants
//Private Variables
int16_t lastLedChanged;
LED_NODE leds[NUM_LEDS];
//Private Function Prototypes
inline void outputByte(uint8_t byte);
inline void outputBit(bool high_nlow);
inline void outputLedNode(LED_NODE& node);
};

//Templated Class Function Definitions
template<uint16_t NUM_LEDS>
inline void LED_STRIP_WS2811<NUM_LEDS>::outputBit(bool high_nlow)
{
	  __asm(
			  ".syntax unified\n\t"
			  "MOV r0, %[gpio_odr]\n\t"
			  "LDR r1, [r0]\n\t"
			  "ORRS r1, r1, %[gpio_pin]\n\t"
			  "STR r1, [r0]\n\t"
			  :
			  : [gpio_odr] "r" (&DATA_OUT_GPIO_Port->ODR), [gpio_pin] "r" (DATA_OUT_Pin));
	if(high_nlow)
	{
		  __asm(
				  ".syntax unified\n\t"
				  "LDR r0, =4\n\t"
				  "DELAY_LOOP1:\n\t"
				  "SUBS r0, #1\n\t"
				  "BNE DELAY_LOOP1\n\t");
	}else{
	}
	  __asm(
			  ".syntax unified\n\t"
			  "MOV r0, %[gpio_odr]\n\t"
			  "LDR r1, [r0]\n\t"
			  "ANDS r1, r1, %[gpio_pin]\n\t"
			  "STR r1, [r0]\n\t"
			  "LDR r0, =15\n\t"
			  "DELAY_LOOP3:\n\t"
			  "SUBS r0, #1\n\t"
			  "BNE DELAY_LOOP3\n\t"
			  :
			  : [gpio_odr] "r" (&DATA_OUT_GPIO_Port->ODR), [gpio_pin] "r" (~DATA_OUT_Pin));
}

template<uint16_t NUM_LEDS>
inline void LED_STRIP_WS2811<NUM_LEDS>::outputByte(uint8_t byte)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		this->outputBit(byte & (1 << (7-i)));
	}
}

template<uint16_t NUM_LEDS>
inline void LED_STRIP_WS2811<NUM_LEDS>::outputLedNode(LED_NODE& node)
{
	this->outputByte(node.blue);
	this->outputByte(node.red);
	this->outputByte(node.green);

}

template<uint16_t NUM_LEDS>
void LED_STRIP_WS2811<NUM_LEDS>::writeEntireStrip(uint8_t red, uint8_t green, uint8_t blue)
{
	for(uint8_t i = 0; i < NUM_LEDS; i++)
	{
		this->leds[i] = LED_NODE(red, green, blue);
	}
	//Update last led changed
	this->lastLedChanged = NUM_LEDS - 1;
}

template<uint16_t NUM_LEDS>
void LED_STRIP_WS2811<NUM_LEDS>::writeLedNode(uint16_t node_number, uint8_t red, uint8_t green, uint8_t blue)
{
	this->leds[node_number] = LED_NODE(red, green, blue);
	//Update last led changed
	if(this->lastLedChanged < node_number)
	{
		this->lastLedChanged = node_number;
	}
}

template<uint16_t NUM_LEDS>
void LED_STRIP_WS2811<NUM_LEDS>::display()
{
	//Disable interrupts
	__disable_irq();
	//Write the changed leds
	if(lastLedChanged < NUM_LEDS)
	{
		for(int16_t i = 0; i <= lastLedChanged; i++)
		{
			this->outputLedNode(leds[i]);
		}
	}
	//Latch data
	HAL_Delay(1);
	//Reenable interrupts
	__enable_irq();
}

#endif //End Header Guard
