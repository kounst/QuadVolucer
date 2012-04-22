// Header:
// File Name: 	GPIO.h
// Author: 		Konstantin Reichmeyer
// Date: 		28.04.10


/* Private define ------------------------------------------------------------*/
#define LED1_PIN                GPIO_Pin_4
#define LED2_PIN						    GPIO_Pin_5
#define SUMMER_PIN              GPIO_Pin_5
#define LED1_GPIO_PORT					GPIOA
#define LED2_GPIO_PORT					GPIOA
#define SUMMER_GPIO_PORT        GPIOB


/* Private typedef -----------------------------------------------------------*/
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  SUMMER = 2
} Led_TypeDef;

typedef enum 
{
  TP1 = 0,
  TP2 = 1,
  TP3 = 2,
  TP4 = 3,
  TP5 = 4,
} TP_TypeDef;


void GPIO_Configuration(void);
void QuadC_LEDToggle(Led_TypeDef Led);
void QuadC_LEDOn(Led_TypeDef Led);
void QuadC_LEDOff(Led_TypeDef Led);
