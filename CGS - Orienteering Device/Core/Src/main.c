/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Orienteering Device definitions */
#define SLAVE		0
#define MASTER		1
#define OD_ID		0x43

/* SRR definitions */
#define ETX			0x03
#define NAK			0x15

/* XBee definitions */
#define SOF						0x7E
#define CMD_TX_REQUEST			0x10
#define XBEE_MAX_PACKET_SIZE	60

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Tracker variable for which mode we are in */
bool mode;

/* Incoming channel buffers */
volatile uint8_t radioRedBuffer  	[100];
volatile uint8_t radioBlueBuffer 	[100];
volatile uint8_t radioAuxBuffer		[100];

volatile uint8_t PCBuffer  			[100];
volatile uint8_t xbeeBuffer 		[100];

uint8_t xbeeTXBuffer	[XBEE_MAX_PACKET_SIZE];

/* Incoming single byte (we only handle 1 byte at a time) */
uint8_t radioRedIn = 0;
uint8_t radioBlueIn = 0;
uint8_t radioAuxIn = 0;

uint8_t PCIn = 0;
uint8_t xbeeIn = 0;

/* Tracker to keep track of where in the buffer we are */
uint8_t radioRedTracker = 0;
uint8_t radioBlueTracker = 0;
uint8_t radioAuxTracker = 0;

uint8_t PCTracker = 0;
uint8_t xbeeTracker = 0;

/* Boolean flag for when a packet is complete and ready to be interpretted */
bool radioRedPacketComplete = false;
bool radioBluePacketComplete = false;
bool radioAuxPacketComplete = false;

bool PCPacketComplete = false;
bool xbeePacketComplete = false;

/* Timeout counters */
uint32_t xbeeTimeout = 0;

/* Variable for which stage of the packet we are at */
volatile uint8_t xbeeStep = 1;
volatile uint16_t xbeeSize = 0;

/* Outgoing buffer (doesn't matter which channel) */
uint8_t transmitBuffer	[100];

/* Tracker variable for last time LED blinked */
uint32_t timeSinceLastBlink = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void Boot_Sequence(bool mode);
void XBee_Transmit(uint8_t* txBuffer, uint8_t txBufferSize);
static uint8_t XBee_Checksum(uint8_t *buffer, uint16_t length);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART4_UART_Init();
  MX_USART5_UART_Init();
  /* USER CODE BEGIN 2 */

  /* Read the switch to see if we are a master or slave */
  mode = (bool)(HAL_GPIO_ReadPin(GPIOC, SWITCH_Pin));

  /* If we are in slave mode, enable 3 radio inputs, and the XBee channel */
  if (mode == SLAVE)
  {
	  HAL_UART_Receive_IT(&huart4, &radioRedIn, 1);
	  HAL_UART_Receive_IT(&huart2, &radioBlueIn, 1);
	  HAL_UART_Receive_IT(&huart5, &radioAuxIn, 1);
	  HAL_UART_Receive_IT(&huart1, xbeeBuffer, 1);
  }
  /* If we are in master mode, enable PC communications and the XBee channel */
  else if (mode == MASTER)
  {
	  HAL_UART_Receive_IT(&huart5, &PCIn, 1);
	  HAL_UART_Receive_IT(&huart1, xbeeBuffer, 1);
  }

  /* Wake up the XBee */
  HAL_GPIO_WritePin(GPIOA, XBEE_RESETn_Pin, GPIO_PIN_SET);

  /* Boot up the system */
  Boot_Sequence(mode);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (true)
  {

		/* Handle the slave mode */
		/* Here, 4 things could happen:	 */
		/* - A packet from the red radio comes in. We send this out via the XBee */
		/* - A packet from the blue radio comes in. We send this out via the XBee */
		/* - A packet from the aux radio comes in. We send this out via the XBee */
		/* - A packet from the XBee comes in. We handle this command */
		if (mode == SLAVE)
		{

			if (radioRedPacketComplete == true)
			{

				XBee_Transmit(radioRedBuffer, radioRedTracker);

				memset(radioRedBuffer, 0, 100);
				radioRedTracker = 0;

				radioRedPacketComplete = false;

			}

			if (radioBluePacketComplete == true)
			{

				XBee_Transmit(radioBlueBuffer, radioBlueTracker);

				memset(radioBlueBuffer, 0, 100);
				radioBlueTracker = 0;

				radioBluePacketComplete = false;

			}

			if (radioAuxPacketComplete == true)
			{

				XBee_Transmit(radioAuxBuffer, radioAuxTracker);

				memset(radioAuxBuffer, 0, 100);
				radioAuxTracker = 0;

				radioAuxPacketComplete = false;

			}

		}

		/* Handle the master mode */
		/* Here, 3 things could happen:	 */
		/* - A packet from the Xbee comes in. We handle this data */
		/* - A packet from the PC comes in. We handle this command (TODO) */
		/* - The heartbeat button is pushed. We send out a signal to make the slaves blink (TODO) */
		if (mode == MASTER)
		{

			if (xbeePacketComplete == true)
			{

				/* We need to find where the packet starts. Sometimes this is 14, 15, 16 etc. indexes into the buffer */
				/* There's also a chance both the XBee AND the radio packet are atypical sizes, and with both having their own overhead/structure */
				/* You can see, it's actually proving quite hard to work out where one starts and the other ends because we have embedded radio packets */
				/* We also can't just look for the radio EOF indicator (0x03) since the XBee packet might have this inside of it, too! */
				/* Temporary solution is to look for what seems to be a unique key at the start of the punch radio message: */
				/* - 0xFF (alert, packet incoming) */
				/* - 0x02 (STX, start of text) */
				/* - 0xD3 (transmit punch data) */
				/* This is taken from Page 5 of the SportIdent PC programmers guide */
				/* We can then use the next byte to work out the length. This is similar to the XBee radio structure but is more appropriate to use here */
				/* If this exact key isn't received, then we can discard it and it must be noise or some message we aren't interested in */
				/* This is STILL flawed though! THere's a chance the XBee might have that exact ^ structure embedded in its header */
				uint8_t index;
				uint8_t radioPacketLength;

				for (index = 0; index <= 100; index++)
				{

					/* See if this matches the start of a valid punch data radio packet */
					if (xbeeBuffer[index] == 0xFF && xbeeBuffer[index + 1] == 0x02 && xbeeBuffer[index + 2] == 0xD3)
					{

						/* Grab the length - this is the next byte, but also add back in the overhead (4 bytes) and checksum (3 bytes) */
						/* This means rather than the interesting data, we are transmitting exactly as if the SRR was plugged into the PC */
						/* This can be easily removed later, if we don't want to worry about the checksum and overhead and just want the pure punch data */
						radioPacketLength = xbeeBuffer[index + 3] + 4 + 3;

						memcpy(transmitBuffer, &xbeeBuffer[index], radioPacketLength);
						HAL_UART_Transmit(&huart5, transmitBuffer, 20, 100);
						break;
					}

				}

				/* If the for loop ends normally, then nothing was transmitted and the message was discarded */
				memset(xbeeBuffer, 0, 100);
				xbeePacketComplete = false;

				HAL_UART_Receive_IT(&huart1, xbeeBuffer, 1);

			}

			if (PCPacketComplete == true)
			{


				/* Do something with the PC Packet */

				memset(PCBuffer, 0, 100);
				PCTracker = 0;

				PCPacketComplete = false;

			}

		}

	  	/* In either master or slave mode, blink the status LED */
		if (HAL_GetTick() - timeSinceLastBlink > 500)
		{
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
			timeSinceLastBlink = HAL_GetTick();
		}

		/* If it's taken 100ms to get through the process of receiving a whole Xbee packet, it must have failed */
		/* So, we reset the steps */
		if ((HAL_GetTick() - xbeeTimeout) > 100)
		{
			xbeeStep = 1;
			HAL_UART_Receive_IT(&huart1, &xbeeBuffer[1], 1);
		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

static void Boot_Sequence(bool mode)
{

	if (mode == SLAVE)
	{

		/* Turn LEDS ON and OFF and make buzzer beep once and longer */
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

	}
	else if (mode == MASTER)
	{

		/* Turn LEDS ON and OFF and make buzzer beep twice */
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
		HAL_Delay(100);
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

		/* Lastly, turn the MASTER LED on, and don't change it */
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

	}
}

void XBee_Transmit(uint8_t* txBuffer, uint8_t txBufferSize)
{

	uint16_t packetLength;
	uint8_t checksum;

	/* Let's build the packet */
	/* First, it's the Start of File indicator byte */
	xbeeTXBuffer[0] = SOF;

	/* Second, the Packet Length. This equals Payload Length + 14 Bytes (CMD-ID(1B) + frame id(1B) +  64-bit addr + 16 bit-addr + 2B) */
	packetLength = txBufferSize + 14;

	xbeeTXBuffer[1] = (uint8_t) (packetLength >> 8);
	xbeeTXBuffer[2] = (uint8_t) (packetLength);

	/* Command ID */
	xbeeTXBuffer[3] = CMD_TX_REQUEST;

	/* Frame ID */
	xbeeTXBuffer[4] = 0x00;

	/* 64-Bit Destination Address. In broadcast mode, this is 0x000000000000FFFF */
	xbeeTXBuffer[5] = 0x00;
	xbeeTXBuffer[6] = 0x00;
	xbeeTXBuffer[7] = 0x00;
	xbeeTXBuffer[8] = 0x00;
	xbeeTXBuffer[9] = 0x00;
	xbeeTXBuffer[10] = 0x00;
	xbeeTXBuffer[11] = 0xFF;
	xbeeTXBuffer[12] = 0xFF;

	/* 16-Bit Address. In broadcast mode, this is 0xFFFE */
	xbeeTXBuffer[13] = 0xFF;
	xbeeTXBuffer[14] = 0xFE;

	/* Broadcast Radius. We'll set this to 1 */
	xbeeTXBuffer[15] = 0x01;

	/* Transmit Options. We won't use any here */
	xbeeTXBuffer[16] = 0x00;

	/* Paste the payload in */
	memcpy(&xbeeTXBuffer[17], txBuffer, txBufferSize);

	/* Calculate the checksum */
	checksum = XBee_Checksum(&xbeeTXBuffer[3], packetLength);

	/* Insert the checksum */
	xbeeTXBuffer[17 + txBufferSize] = checksum;

	/* Transmit the packet to the XBee */
	HAL_UART_Transmit(&huart1, xbeeTXBuffer, packetLength + 4, 100);

}

static uint8_t XBee_Checksum(uint8_t *buffer, uint16_t length)
{
	uint16_t add = 0;
	uint16_t i;
	uint8_t checksum;

	for (i = 0; i < length; ++i)
	{
		add += buffer[i];
	}

	// Keep only the lowest 8 bits of the result
	add = add & 0x00FF;

	checksum = 0xFF - (uint8_t)add;

	return checksum;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{


	/* Handle XBee input - this happens independent of slave or master mode */

	/* Unlike PC where we just look for newline ('\n'), or the radios where we look for NAK/ETX byte, this isn't trivial */
	/* First, we look for the SOF (start of file) byte. When we get this, we know the next 2 bytes will be the length */
	/* Second, we read the length which is 2 bytes. Then, we use this information to wait for the final amount of data incoming */
	/* Third and final, we read the rest of the data */
	if (huart->Instance == USART1)
	{

		xbeeTimeout = HAL_GetTick();

		if (xbeeStep == 1)
		{

			/* This is a valid XBee packet */
			/* Proceed to the next step, shift the buffer pointer up 1, and wait for 2 bytes */
			/* These 2 bytes will be the length */
			if (xbeeBuffer[0] == SOF)
			{

				xbeeStep = 2;
				xbeeSize = 2;
				HAL_UART_Receive_IT(&huart1, &xbeeBuffer[1], xbeeSize);

			}
			/* Otherwise, this is an invalid packet/we got out of sync */
			/* Restart and continue the hunt for a valid packet */
			else
			{
				xbeeStep = 1;
				HAL_UART_Receive_IT(&huart1, xbeeBuffer, 1);
			}

		}
		else if (xbeeStep == 2)
		{

			/* Here, we calculate the length of the packet incoming */
			/* This is then used for the final step where we grab the data we want */
			xbeeSize = (xbeeBuffer[1] << 8) | (xbeeBuffer[2] << 0);

			/* The real size is an additional 1 bytes on top of this for the checksum */
			xbeeSize += 1;

			/* We add a check here to make sure the length is sensible */
			/* If it is sensible, we move up the buffer a few positions and look for the remaining packet size */
			if (xbeeSize <= XBEE_MAX_PACKET_SIZE)
			{
				xbeeStep = 3;
				HAL_UART_Receive_IT(&huart1, &xbeeBuffer[3], xbeeSize);
			}
			/* Otherwise, this is an invalid packet/we got out of sync */
			/* Restart and continue the hunt for a valid packet */
			else
			{
				xbeeStep = 1;
				HAL_UART_Receive_IT(&huart1, xbeeBuffer, 1);
			}
		}
		else if (xbeeStep == 3)
		{

			/* Here, we have received the packet of data */
			/* There are several checks we need to make here */
			/* First, check that it's a valid Orienteering Device packet */
//			if (xbeeBuffer[0] == OD_ID
//			 && xbeeBuffer[1] == currentChannel)
//			{

				xbeePacketComplete = true;

//			}
			/* Whether it was a valid packet or not, we begin scanning again for new data */
			xbeeStep = 1;
			HAL_UART_Receive_IT(&huart1, xbeeBuffer, 1);

		}

	}

	/* Manage data coming in from the radios */
	if (mode == SLAVE)
	{

		/* Handle Red Radio input */
		if (huart->Instance == USART4)
		{
			radioRedBuffer[radioRedTracker] = radioRedIn;
			radioRedTracker++;

			if (radioRedIn == NAK || radioRedIn == ETX)
			{
				radioRedPacketComplete = true;
			}

			HAL_UART_Receive_IT(&huart4, &radioRedIn, 1);

		}

		/* Handle Blue Radio input */
		if (huart->Instance == USART2)
		{
			radioBlueBuffer[radioBlueTracker] = radioBlueIn;
			radioBlueTracker++;

			if (radioBlueIn == NAK || radioBlueIn == ETX)
			{
				radioBluePacketComplete = true;
			}

			HAL_UART_Receive_IT(&huart2, &radioBlueIn, 1);

		}

		/* Handle Auxiliary Radio input */
		if (huart->Instance == USART5)
		{
			radioAuxBuffer[radioAuxTracker] = radioAuxIn;
			radioAuxTracker++;

			if (radioAuxIn == NAK || radioAuxIn == ETX)
			{
				radioAuxPacketComplete = true;
			}

			HAL_UART_Receive_IT(&huart5, &radioAuxIn, 1);

		}

	}

	/* Manage data coming from the PC */
	else if (mode == MASTER)
	{

		/* Handle PC input */
		if (huart->Instance == USART5)
		{
			PCBuffer[PCTracker] = PCIn;
			PCTracker++;

			if (PCIn == '\n')
			{
				PCPacketComplete = true;
			}

			HAL_UART_Receive_IT(&huart5, &PCIn, 1);

		}

	}


}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
