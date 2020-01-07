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
//TODO: make this only change timer_freq
/**
 * @brief This is the frequency of the timer to be passed to this object and if this changes the timer tick values should also change
 */
#define TIMER_FREQ 48E6
/*
 * @brief These values are in timer ticks
 */
#define T0H 1
#define T1H 11
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
LED_STRIP_WS2811(TIM_HandleTypeDef* tim_ptr)
{
	this->lastLedChanged = NUM_LEDS - 1;
	this->tim_ptr = tim_ptr;
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
TIM_HandleTypeDef* tim_ptr;
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
	if(high_nlow)
	{
		this->tim_ptr->Instance->CCR1 = T1H;
	}else{
		this->tim_ptr->Instance->CCR1 = T0H;
	}
	//Wait until the previous pwm stops
	while(this->tim_ptr->Instance->CR1 & TIM_CR1_CEN);
	this->tim_ptr->Instance->CR1 |= TIM_CR1_CEN;
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
		//Set first CC value
//		this->tim_ptr->Instance->SR &= ~TIM_SR_UIF;
//		this->tim_ptr->Instance->CCR1 = ((leds[0].blue & 0x80) ? T1H : T0H);
//		this->tim_ptr->Instance->EGR = TIM_EGR_UG;
		this->tim_ptr->Instance->CCR1 = 0;
		this->tim_ptr->Instance->CR1 |= TIM_CR1_CEN;
		for(int16_t i = 0; i <= lastLedChanged; i++)
		{
			this->outputLedNode(leds[i]);
		}
	}
	this->tim_ptr->Instance->CCR1 = 0;
	this->tim_ptr->Instance->EGR = TIM_EGR_UG;
	this->lastLedChanged = -1;
	//Reenable interrupts
	__enable_irq();
	HAL_Delay(1);
}

#endif //End Header Guard
