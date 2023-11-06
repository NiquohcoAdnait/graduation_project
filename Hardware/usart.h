#ifndef __USART_H
#define __USART_H

void Usart_Init(uint32_t baud);
void Usart_SendString(USART_TypeDef *USARTx, uint8_t *str, uint16_t len);

#endif
