#include "cracking.hpp"

int cracking_nop(int from, int to)
{
	int i;
	for (i=from; i<to; i++)
	{
		*(unsigned char *)i = 0x90;
		//printf("%d ", *(unsigned char *)i);
	}
	return to - from; // bytes overwritten
}

void cracking_hook_function(int from, int to)
{
	int relative = to - (from+5); // +5 is the position of next opcode
	memset((void *)from, 0xE9, 1); // JMP-OPCODE
	memcpy((void *)(from+1), &relative, 4); // set relative address with endian
}

void cracking_hook_call(int from, int to)
{
	int relative = to - (from+5); // +5 is the position of next opcode
	memcpy((void *)(from+1), &relative, 4); // set relative address with endian
}



int singleHexToNumber(char hexchar)
{
	switch (hexchar)
	{
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		case 'a':
		case 'A': return 10;
		case 'b':
		case 'B': return 11;
		case 'c':
		case 'C': return 12;
		case 'd':
		case 'D': return 13;
		case 'e':
		case 'E': return 14;
		case 'f':
		case 'F': return 15;
	}
	return -1;
}
int hexToBuffer(char *hex, char *buffer, int bufferLen)
{
	int len, neededBytes, i, padding, first, pos, leftPart, rightPart;
	len = strlen(hex); // every byte of hex is taking 4 bits. F=1111
	padding = 0; // just for "a", "abc" etc... "a" = 0x0a, "abc" = 0x0abc
	// we dont handle 4-bits for one hex-number, so round up to bytes...
	// three bytes will not take 12 bits, they will use 16 bits = 2 bytes
	if (len % 2 != 0)
	{
		padding = 1;
		len++;
	}
	neededBytes = len >> 1; // its like dividing by 2
	//printf("len=%d neededBytes=%d\n", len, neededBytes);
	first = 1;
	pos = 0;
	for (i=0; i<neededBytes; i++)
	{
		char twochars[2] = {'0', '0'};
		if (first)
		{
			if (padding) {
				twochars[1] = hex[0];
				pos++;
			} else {
				twochars[0] = hex[0];
				twochars[1] = hex[1];
				pos += 2;
			}
			first = 0;
		} else {
			twochars[0] = hex[pos];
			twochars[1] = hex[pos+1];
			pos += 2;
		}
		//printf("twochars=%.2s\n", twochars);
		leftPart = singleHexToNumber(twochars[0]);
		rightPart = singleHexToNumber(twochars[1]);
		if (leftPart == -1 || rightPart == -1)
			return i;
		buffer[i] = (leftPart << 4) + rightPart;
		// buffer end:
		if (i == bufferLen)
			return i;
	}
	return neededBytes;
}



int cracking_write_hex(int address, char *hex)
{
	unsigned char *ptr = (unsigned char *)address;
	char buffer[128] = {0};
	int bytes;
	int i;
	bytes = hexToBuffer(hex, buffer, 128);
	
	
	for (i=0; i<bytes; i++)
		ptr[i] = buffer[i];
		
	return bytes;
}


cHook::cHook(int from, int to) {
	this->from = from;
	this->to = to;
}
void cHook::hook() {
	memcpy((void *)oldCode, (void *)from, 5);
	cracking_hook_function(from, to);
}
void cHook::unhook() {
	memcpy((void *)from, (void *)oldCode, 5);
}


int cracking_call_function(int func_address, char *args, unsigned char *data) {
	int data_pos = 0;
	unsigned char stack[128];
	int stack_pos = 0;
	
	int mode_varargs = 0;
	int i;
	for (i=0; args[i]; i++) {
		if (args[i] == '.') {
			mode_varargs = 1;
			continue;
		}
		//printf("args[i=%d]=%c stack_pos=%.2d data=%.8p\n", i, args[i], stack_pos, *(int *)(data + data_pos));
		*(int *)(stack + stack_pos) = *(int*)(data + data_pos);

		if (args[i] == 'f' && mode_varargs) {
			double tmp_double = (double)*(float *)(data + data_pos);
			memcpy(stack + stack_pos, &tmp_double, 8);
		}
		if (args[i] == 'd') {
			double tmp_double = *(double *)(data + data_pos);
			memcpy(stack + stack_pos, &tmp_double, 8);
		}

		stack_pos += 4; // even 1-byte-chars will be 4-byte-aligned on stack
		if (args[i] == 'f' && mode_varargs)
			stack_pos += 4; // use 8 bytes for varargs/float, aka double
		if (args[i] == 'd')
			stack_pos += 4; // use 8 bytes for normal double
			
		switch (args[i]) {
			case 'i':
			case 's':
			case 'f': data_pos += 4; break;
			case 'c': data_pos += 1; break;
			case 'd': data_pos += 8; break;
		}
	}
	
	// http://wiki.osdev.org/Inline_Assembly
	unsigned char *sp;
	asm("movl %%esp, %0" : "=g" (sp));
	//printf("sp=%.8p\n", sp);

	memcpy(sp, stack, stack_pos);
	
	asm("movl %0, %%eax" : : "g" (func_address));
	asm("call *%eax");

	int ret;
	asm("movl %%eax, %0" : "=g" (ret));
	return ret;
}