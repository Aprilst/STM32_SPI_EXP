 int data = 0x00;
  while (1)
  {
    /* USER CODE END WHILE */
    HAL_Delay(1000);
    HAL_UART_Transmit(UART4, data, 8, 1000);
    data += 0x09;
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */