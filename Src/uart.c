void UART_Write(char data);
void UART_Write(char data){
  while(!TRMT);
  TXREG = data;
}