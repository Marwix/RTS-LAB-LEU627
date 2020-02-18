#include "TinyTimber.h"
#include "sciTinyTimber.h"
#include "canTinyTimber.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct 
{
    Object super;
	char buf[100];
    int count;
    char c;
	int myNum;
	int mySum;
} App;

int indices[32] = { 0, 2, 4, 0, 0, 2, 4, 0, 4, 5, 7, 4, 5, 7, 7, 9, 7, 5, 4, 0, 7, 9, 7, 5, 4, 0, 0, -5, 0, 0, -5, 0 };
int periods[25] = { 2024, 1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275, 1203, 1136, 1072, 1012, 955, 901, 851, 803, 758, 715, 675, 637, 601, 568, 536, 506 };

App app = { initObject(), 0, 'X', 0};

void reader(App*, int);
void receiver(App*, int);

Serial sci0 = initSerial(SCI_PORT0, &app, reader);

Can can0 = initCan(CAN_PORT0, &app, receiver);

void write_frequencies(App *self, int y)
{
	for (int i = 0; i <= 31; i++)
	{
		int index = indices[i] + (y + 10);
		snprintf(self->buf, 100, "Frequency index: %d \t", (index-10-y));
		SCI_WRITE(&sci0, self->buf);

		int array_index = periods[index];
		snprintf(self->buf, 100, "Period: %d\n", array_index);
		SCI_WRITE(&sci0, self->buf);
	}
}
void receiver(App *self, int unused) {
    CANMsg msg;
    CAN_RECEIVE(&can0, &msg);
    SCI_WRITE(&sci0, "Can msg received: ");
    SCI_WRITE(&sci0, msg.buff);
}

// Reader Part 0 / Problem 2b
void reader(App *self, int c)
{
	if (c != 'e')
	{
		SCI_WRITE(&sci0, "Rcv: \'");
		SCI_WRITECHAR(&sci0, c);
		SCI_WRITE(&sci0, "\'\n");
		self->buf[self->count] = c;
		self->count++;
	}
	else if (c == 'e')
	{
		self->buf[(self->count)+1] = '\0';
		write_frequencies(self, atoi(self->buf));
		self->count = 0;
	}
}

void startApp(App *self, int arg) 
{
    CANMsg msg;
	
    CAN_INIT(&can0);
    SCI_INIT(&sci0);
    SCI_WRITE(&sci0, "Hello, hello...\n");

    msg.msgId = 1;
    msg.nodeId = 1;
    msg.length = 6;
	
    msg.buff[0] = 'H';
    msg.buff[1] = 'e';
    msg.buff[2] = 'l';
    msg.buff[3] = 'l';
    msg.buff[4] = 'o';
    msg.buff[5] = 0;
    CAN_SEND(&can0, &msg);
}

int main() {
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
	INSTALL(&can0, can_interrupt, CAN_IRQ0);
    TINYTIMBER(&app, startApp, 0);
    return 0;
}

/* if (c != 'e')
	    {
		    SCI_WRITE(&sci0, "Rcv: \'");
            SCI_WRITECHAR(&sci0, c);
            SCI_WRITE(&sci0, "\'\n");
			self->buf[self->count] = c;
			self->count++;
	    }
	if (c == 'e')
	{
		// Skriv the entered number
		self->buf[(self->count)+1] = '\0';
		self->myNum = atoi(self->buf);
		snprintf(self->buf, 100, "The entered number is %d\n", self->myNum);
		SCI_WRITE(&sci0, self->buf);
		
		// Skriv the entered sum
		self->mySum = self->mySum + self->myNum;
		snprintf(self->buf, 100, "The running sum is %d\n", self->mySum);
		SCI_WRITE(&sci0, self->buf);
		for (int i = 0; i < 101; i++)
		{
			self->buf[i] = 0;
		}
	}
	if (c == 'F')
	{
		self->mySum = 0;
		snprintf(self->buf, 100, "The running sum is %d\n", self->mySum);
		SCI_WRITE(&sci0, self->buf);
	} */