//https://laptrinharmst.blogspot.com/2018/03/bai-09-adc-trong-stm32f103.html
//https://www.youtube.com/watch?v=T0E3_9u4ds4
//https://stackoverflow.com/questions/59546305/stm32f103-gpio-ports
//-------UART 2
//PA2 = tx
//PA3 = rx
//-------ADC
// kenh 0 = PA0
#include "stm32f10x.h"
#include <string.h>
void delay1ms(void);
void delayms(uint32_t u32delayms);
void delayus(uint32_t u32delayus);
void delay1ms(void){
    TIM_SetCounter(TIM2, 0);
    while(TIM_GetCounter(TIM2) < 1000){

    }
}
void delayus(uint32_t u32delayus){
    TIM_SetCounter(TIM2, 0);
    while(TIM_GetCounter(TIM2) < u32delayus){

    }
}
void delayms(uint32_t u32delayms){
    while(u32delayms){
        delay1ms();
        --u32delayms;
    }
}

void UART_Write(char data){
    USART_SendData(USART2, data);           // gui du lieu
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){        //neu ko gui dc du lieu thi lap vo han

    }
}
void UART_Write_Text( char *text){
    int i;
    for(i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}
void send_sms(char *n){
    UART_Write_Text("AT+CMGS=\"+84334610035\"\r\n");
    delayms(500);
    UART_Write_Text(n);
    UART_Write_Text("\n\r");
    delayms(500);
    USART_SendData(USART2, 0x1A);           // gui du lieu
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){        //neu ko gui dc du lieu thi lap vo han

    }
    delayms(1000);
}
void ATinit(){
    UART_Write_Text("AT\r\n");          //ACK command
    delayms(500);
    UART_Write_Text("ATE0\r\n");        //disable echo   
    delayms(500);
    UART_Write_Text("AT&W\r\n");        // save this command
    delayms(500);
    UART_Write_Text("AT+CSCS=\"GSM\"\r\n");
    delayms(500);
    UART_Write_Text("AT+CMGF=1\r\n");   //format text
    delayms(500);
    UART_Write_Text("AT+CNMI=1,2,0,0,0\r\n");
    delayms(500);
    UART_Write_Text("AT+CSAS\r\n");
    delayms(500);  
    send_sms("Khoi dong xong roi!!");   
    delayms(500);
}
unsigned char *ptr, i, rev, rev1, rev2, rev3, rev4, buffer[90], xbuffer;
unsigned int new_sms;

void USART2_IRQHandler(void){           //neu interrupt ko chay thi check lai muc logic
    if(USART_GetITStatus(USART2, USART_IT_RXNE)){    //flag is set
        
        rev  = USART2->DR;       // thanh ghi DR chua data 
        buffer[xbuffer++] = rev;
        if(xbuffer>90) xbuffer = 0;
        if(rev == '+') rev1 = rev;           
        if(rev == 'C') rev2 = rev;
        if(rev == 'M') rev3 = rev;
        if(rev == 'T') rev4 = rev;
        if(rev1 == '+' && rev2 == 'C' && rev3 == 'M' && rev4 =='T') new_sms = 1;       
    }      
}
void clear_buffer(){
    for(i =0; i<90; i++)    buffer[i] = ' ';
}
void delete_sms(){
    UART_Write_Text("AT+CMGDA=\"DEL ALL\"\r\n");
    delayms(500);
}
void reverse(char* str, int len) {
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
}
int intToStr(int x, char str[], int d) {
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    }  // If number of digits required is more, then add 0s at the beginning 
    while (i < d) 
        str[i++] = '0';  
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
}
long int mypow(int x,int n) {
    int i; /* Variable used in loop counter */ 
    int number = 1;  
    for (i = 0; i < n; ++i) 
        number *= x; 
    return(number); 
} 
void ftos(float n, char* res, int afterpoint){     
    int ipart = (int)n;              // Extract integer part    
    float fpart = n - (float)ipart;  // Extract floating part  
    int i = intToStr(ipart, res, 0); // convert integer part to string   
    if (afterpoint != 0) {           // check for display option after point 
        res[i] = '.';                // add dot  
        // Get the value of fraction part upto given no. 
        // of points after dot. The third parameter  
        // is needed to handle cases like 233.007 
        fpart = fpart * mypow(10, afterpoint);  
        intToStr((int)fpart, res + i + 1, afterpoint); 
    } 
}
int main(void){

    TIM_TimeBaseInitTypeDef timerInit;                      // khai bao struct timerInit
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    //cap xung cho timer2
    timerInit.TIM_CounterMode = TIM_CounterMode_Up;         //che do dem len
    timerInit.TIM_Period = 0xffff;                          //gia tri thanh ghi reset qua trinh dem
    timerInit.TIM_Prescaler = 72 - 1;          
    TIM_TimeBaseInit(TIM2, &timerInit);
    TIM_Cmd(TIM2, ENABLE);

    GPIO_InitTypeDef gpioInit;          //khoi tao struct gpio
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Pin = GPIO_Pin_13;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInit);
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP ;          
    gpioInit.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;  // led
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOA, GPIO_Pin_9);
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
    gpioInit.GPIO_Mode = GPIO_Mode_IPU ;          // input pull-up
    gpioInit.GPIO_Pin = GPIO_Pin_12;  // A12 as button
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);
    //PA2 (TX)
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP ;          // luc chua truyen thi dat muc high
    gpioInit.GPIO_Pin = GPIO_Pin_2;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);
    //PA3 (RX)
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;     // nhan du lieu nen input floating
    gpioInit.GPIO_Pin = GPIO_Pin_3;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);
    //PA0 read ADC
    gpioInit.GPIO_Mode = GPIO_Mode_AIN;          
    gpioInit.GPIO_Pin = GPIO_Pin_0;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

    USART_InitTypeDef uartInit;         //khoi tao struct uart
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    uartInit.USART_BaudRate = 9600;
    uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      //chon che do khong bat tay
    uartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;           // che do vua truyen vua nhan (song cong)
    uartInit.USART_Parity = USART_Parity_No;                // khong kiem tra tinh chan le
    uartInit.USART_StopBits = USART_StopBits_1 ;            // so bit dung = 1
    uartInit.USART_WordLength = USART_WordLength_8b;        // 8 bit du lieu
    USART_Init(USART2, &uartInit);
    USART_Cmd(USART2, ENABLE);
          
    NVIC_InitTypeDef nvicInit;
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    nvicInit.NVIC_IRQChannel = USART2_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    ADC_InitTypeDef adcInit;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);    // cap clock cho ADC1
    adcInit.ADC_ContinuousConvMode = ENABLE;
    adcInit.ADC_DataAlign = ADC_DataAlign_Right;
    adcInit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adcInit.ADC_Mode = ADC_Mode_Independent;
    adcInit.ADC_ScanConvMode = DISABLE;
    adcInit.ADC_NbrOfChannel = 1;       // channel 1
    ADC_Init(ADC1, &adcInit);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);// cau hinh channel, rank, thoi gian lay mau
    ADC_Cmd(ADC1, ENABLE);  // cho phep bo adc hoat dong
    ADC_TempSensorVrefintCmd(ENABLE); // cho phep cam bie nhiet hoat dong
    ADC_ResetCalibration(ADC1); // reset thanh ghi cablib
    while(ADC_GetResetCalibrationStatus(ADC1)); // cho thanh ghi reset xong
    ADC_StartCalibration(ADC1);  // khoi dong bo adc
    while(ADC_GetCalibrationStatus(ADC1)); // cho trang thai cablib duoc bat
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // bat dau chuyen doi adc


    ATinit();

    while(1){
        int sum_adc1 = 0;
        int value_adc1 = 0;
        for(int b=0; b<10; b++){    // doc 10 lan adc roi lay trung binh ket qua
            value_adc1 = ADC_GetConversionValue(ADC1);
            sum_adc1 = sum_adc1 + value_adc1;
            delayms(1);
        }
        float adc_tb = sum_adc1/10;
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0){
            GPIOA -> ODR ^= (1 << 12); // toggle pin A12
            while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12));
        }      
        if(new_sms){
            new_sms = 0;
            UART_Write_Text("AT+CMGR=1\r\n");   // read the first SMS
            delayms(2000);
            ptr = strstr(buffer, "Ngu1");       // use pointer looking for string     
            if(strncmp(ptr, "Ngu1", 4)==0){     // compare string
                GPIO_ResetBits(GPIOC, GPIO_Pin_13);              // when receive "Ngu1" turn on the light
                send_sms("da bat den phong ngu");
            }else{
                ptr = strstr(buffer, "Ngu0");
                if(strncmp(ptr, "Ngu0", 4)==0){      
                GPIO_SetBits(GPIOC, GPIO_Pin_13);
                send_sms("da tat den phong ngu");
                }
            }
            ptr = strstr(buffer, "Khach1");      
            if(strncmp(ptr, "Khach1", 4)==0){    
                GPIO_SetBits(GPIOA, GPIO_Pin_8);              
                send_sms("da bat den phong khach");
            }else{
                ptr = strstr(buffer, "Khach0");
                if(strncmp(ptr, "Khach0", 4)==0){      
                GPIO_ResetBits(GPIOA, GPIO_Pin_8);
                send_sms("da tat den phong khach");
                }
            }
            ptr = strstr(buffer, "Bep1");          
            if(strncmp(ptr, "Bep1", 4)==0){      
                GPIO_SetBits(GPIOA, GPIO_Pin_9);                
                send_sms("da bat den phong bep");
            }else{
                ptr = strstr(buffer, "Bep0");
                if(strncmp(ptr, "Bep0", 4)==0){      
                GPIO_ResetBits(GPIOA, GPIO_Pin_9);
                send_sms("da tat den phong bep");
                }
            }
            ptr = strstr(buffer, "info");         // check celsius
            if(strncmp(ptr, "info", 4)==0){       // compare string
                unsigned char a[20];  
                            
                ftos(adc_tb, a, 2);        // float to string, number 2 is the amount of index after point sign
                
                //-----------------
                UART_Write_Text("AT+CMGS=\"+84334610035\"\r\n");
                delayms(500);
                if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 1) UART_Write_Text("den phong bep dang bat\r\n");
                if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0)  UART_Write_Text("den phong bep dang tat\r\n");
                if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 1) UART_Write_Text("den phong khach dang bat\r\n");
                if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)  UART_Write_Text("den phong khach dang tat\r\n");
                if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0) UART_Write_Text("den phong ngu dang bat\r\n");
                if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)  UART_Write_Text("den phong ngu dang tat\r\n");
                
                UART_Write_Text("nhiet do phong la: ");
                UART_Write_Text(a);
                UART_Write_Text("\n\r");
                delayms(500);
                USART_SendData(USART2, 0x1A);           // gui du lieu
                while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){        //neu ko gui dc du lieu thi lap vo han
                }       // '26' character(terminator character)
                delayms(1000);
                //------------------
            }
            clear_buffer();
            delete_sms();
            rev = 0;
            rev1 = 0;
            rev2 = 0;
            rev3 = 0;
            rev4 = 0;
        }
    }
}