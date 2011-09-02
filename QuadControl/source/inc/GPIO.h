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
//#define TP1_GPIO_PORT           GPIOA
//#define TP2_GPIO_PORT           GPIOB
//#define TP3_GPIO_PORT           GPIOB
//#define TP4_GPIO_PORT           GPIOB
//#define TP5_GPIO_PORT           GPIOB
//#define TP1_PIN                 GPIO_Pin_15
//#define TP2_PIN                 GPIO_Pin_3
//#define TP3_PIN                 GPIO_Pin_4
//#define TP4_PIN                 GPIO_Pin_5
//#define TP5_PIN                 GPIO_Pin_12


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
void DISCC_TPToggle(TP_TypeDef TP);
void DISCC_TPOn(TP_TypeDef TP);
void DISCC_TPOff(TP_TypeDef TP);
