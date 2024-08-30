
#define F_CPU 8000000UL         /* Define CPU Frequency e.g. here 8MHz */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>  // Include the stdio.h library for sprintf function

#define LCD_Data_Dir DDRC
#define LCD_Data_Port PORTC
#define LCD_Command_Dir DDRB
#define LCD_Command_Port PORTB
#define RS PB0
#define RW PB1
#define EN PB2

void LCD_Command(unsigned char cmnd)
{
	LCD_Data_Port = cmnd;
	LCD_Command_Port &= ~(1 << RS);
	LCD_Command_Port &= ~(1 << RW);
	LCD_Command_Port |= (1 << EN);
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(2);
}

void LCD_Char(unsigned char char_data)
{
	LCD_Data_Port = char_data;
	LCD_Command_Port |= (1 << RS);
	LCD_Command_Port &= ~(1 << RW);
	LCD_Command_Port |= (1 << EN);
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(1);
}

void LCD_Init(void)
{
	LCD_Command_Dir = 0xFF;
	LCD_Data_Dir = 0xFF;
	_delay_ms(20);

	LCD_Command(0x38);
	LCD_Command(0x0C);
	LCD_Command(0x01);
	LCD_Command(0x80);
}

void LCD_String(char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		LCD_Char(str[i]);
		i++;
	}
}

void LCD_Clear()
{
	LCD_Command(0x01);
	LCD_Command(0x80);
}

int is_button_pressed(volatile uint8_t *port, uint8_t pin)
{
	if (!(*port & (1 << pin)))
	{
		_delay_ms(20);
		if (!(*port & (1 << pin)))
		{
			while (!(*port & (1 << pin)))
			_delay_ms(20);
			return 1;
		}
	}

	return 0;
}

int main(void)
{
	LCD_Init();
	LCD_String("vote to candidates");

	DDRA = 0x00; // Set all pins of PORTA as inputs
	PORTA = 0xFF; // Enable internal pull-up resistors for PORTA

	int person1 = 0;
	int person2 = 0;
	//int voteCounted = 0;

	while (1)
	{
		if (is_button_pressed(&PINA, PA0))
		{
			person1++;
			LCD_Clear();
			LCD_String("congress");
			_delay_ms(100);
		}
		else if (is_button_pressed(&PINA, PA1))
		{
			person2++;
			LCD_Clear();
			LCD_String("BJP");
			_delay_ms(100);
		}
		else if (is_button_pressed(&PINA, PA2))
		{
			//voteCounted = 1;
			LCD_Clear();
			
			char person1Votes[10];  
			char person2Votes[10];  
			
			sprintf(person1Votes, "%d", person1); 
			sprintf(person2Votes, "%d", person2); 
			
			LCD_String("Congr=");
			LCD_String(person1Votes);  
			LCD_String("  BJP=");
			LCD_String(person2Votes);
			LCD_Command(0xc0);
			
			if (person1 > person2)
			{
				LCD_String("Congress wins");
			}
			else if (person1 < person2)
			{
				LCD_String("BJP wins");
			}
			else
			{
				LCD_String("It's a tie");
			}
			person1 = 0;
			person2 = 0;
			_delay_ms(200);
		}
	}
}
