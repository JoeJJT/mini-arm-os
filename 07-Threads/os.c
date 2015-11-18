#include <stddef.h>
#include <stdint.h>
#include "reg.h"
#include "threads.h"
#include "os.h"
#include "func.h"

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
#define USART_FLAG_TXE	((uint16_t) 0x0080)
#define USART_FLAG_RXNE ((uint16_t) 0x0020)

extern int fibonacci(int num);

void usart_init(void)
{
	*(RCC_APB2ENR) |= (uint32_t) (0x00000001 | 0x00000004);
	*(RCC_APB1ENR) |= (uint32_t) (0x00020000);

	/* USART2 Configuration, Rx->PA3, Tx->PA2 */
	*(GPIOA_CRL) = 0x00004B00;
	*(GPIOA_CRH) = 0x44444444;
	*(GPIOA_ODR) = 0x00000000;
	*(GPIOA_BSRR) = 0x00000000;
	*(GPIOA_BRR) = 0x00000000;

	*(USART2_CR1) = 0x0000000C;
	*(USART2_CR2) = 0x00000000;
	*(USART2_CR3) = 0x00000000;
	*(USART2_CR1) |= 0x2000;
}

char get_char() {
	while(!(*(USART2_SR) & USART_FLAG_RXNE));
	return *(USART2_DR) & 0xFF;
}

void print_char(const char *ch) {
	if(*ch) {
		while(!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (*ch & 0xFF);
	}
}

void print_str(const char *str)
{
	while (*str) {
		while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (*str & 0xFF);
		str++;
	}
}

static void delay(volatile int count)
{
	count *= 50000;
	while (count--);
}

static void busy_loop(void *str)
{
	while (1) {
		print_str(str);
		print_str(": Running...\n");
		delay(1000);
	}
}

void test1(void *userdata)
{
	busy_loop(userdata);
}

void test2(void *userdata)
{
	busy_loop(userdata);
}

void test3(void *userdata)
{
	busy_loop(userdata);
}

void fibonacci_task(int num) {

}

static void shell() {
	print_str("Hello World\n");
	char cmd[MAX_STR_SIZE] = {};
	int i;
	while(1) {
		i=0;
		print_str("joe@~:$");
		while(1) {
			cmd[i] = get_char();
			print_char(&cmd[i]);
			if(cmd[i] == '\n' || cmd[i] == '\r') {
				if(i == 0) {
					print_str("\n");
					break;
				}
				else {
					print_str("\n");
					cmd[i] = '\0';
					if(strcmp(cmd,"fib") == 1) {
						if(thread_create((void *)(fibonacci_task), (void *)(5)) == -1) {
							print_str("Fibnacci task create failed\r\n");
						}	
						else {
							print_str("Fibnacci task create successed\r\n");
						}
						break;
					}
					else {
						print_str(cmd);
						print_str(" : command not found\r\n");
						break;
					}
				}
			}
			else if(cmd[i] == 8 || cmd[i] == 127) {
				if(i != 0){ 
					print_str(" \b");
					i--;
				}
			}
			i++;
		}
	}
}

/* 72MHz */
#define CPU_CLOCK_HZ 72000000

/* 100 ms per tick. */
#define TICK_RATE_HZ 10

int main(void)
{
//	const char *str1 = "Task1", *str2 = "Task2", *str3 = "Task3";
	const char username[] = "joe";
	usart_init();
	if (thread_create(shell,(void *)username) == -1) 
		print_str("Shell thread creation failed\r\n");
	/*
	if (thread_create(test1, (void *) str1) == -1)
		print_str("Thread 1 creation failed\r\n");

	if (thread_create(test2, (void *) str2) == -1)
		print_str("Thread 2 creation failed\r\n");

	if (thread_create(test3, (void *) str3) == -1)
		print_str("Thread 3 creation failed\r\n");
	*/
	/* SysTick configuration */
	*SYSTICK_LOAD = (CPU_CLOCK_HZ / TICK_RATE_HZ) - 1UL;
	*SYSTICK_VAL = 0;
	*SYSTICK_CTRL = 0x07;

	thread_start();

	return 0;
}
