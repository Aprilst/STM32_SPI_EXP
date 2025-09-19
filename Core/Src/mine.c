  int *ExpCommand = 0x5000; //读取位置
  int *Resived = 0x00;

  while (1)
  {
    /* USER CODE END WHILE */

    HAL_GPIO_WritePin(SPI1_cs_GPIO_Port, SPI1_cs_Pin, 0);
    HAL_SPI_Transmit(&hspi1, ExpCommand, 2, 2000);
    HAL_Delay(1000);
    HAL_SPI_Receive(&hspi1, Resived, 1, 1000);
    HAL_GPIO_WritePin(SPI1_cs_GPIO_Port, SPI1_cs_Pin, 1);
    HAL_Delay(1000);
    HAL_UART_Transmit(&huart4, Resived, 1, 1000);
    HAL_Delay(1000);

    /* USER CODE BEGIN 3 */
  }

    uint16_t *ExpCommand = {0x5000}; //读取位置
  uint8_t *Resived = 0x00;

  while (1)
  {
    /* USER CODE END WHILE */

    HAL_GPIO_WritePin(SPI1_cs_GPIO_Port, SPI1_cs_Pin, 0);
    HAL_SPI_Transmit(&hspi1, ExpCommand, 2, 2000);
    HAL_Delay(1000);
    HAL_SPI_Receive(&hspi1, Resived, 1, 1000);
    HAL_GPIO_WritePin(SPI1_cs_GPIO_Port, SPI1_cs_Pin, 1);
    HAL_Delay(1000);
    HAL_UART_Transmit(&huart4, Resived, 1, 1000);
    HAL_Delay(1000);

    /* USER CODE BEGIN 3 */
  }

  char *ExpCommand[2] = {0x50, 0x00}; //读取位置

  while (1)
  {
    /* USER CODE END WHILE */

    HAL_GPIO_WritePin(SPI1_cs_GPIO_Port, SPI1_cs_Pin, 0);
    HAL_SPI_Transmit(&hspi1, ExpCommand, 2, 100);

    HAL_GPIO_WritePin(SPI1_cs_GPIO_Port, SPI1_cs_Pin, 1);

    HAL_UART_Transmit(&huart4, ExpCommand, 2, 1000);
    HAL_Delay(1000);
    

    /* USER CODE BEGIN 3 */
  }