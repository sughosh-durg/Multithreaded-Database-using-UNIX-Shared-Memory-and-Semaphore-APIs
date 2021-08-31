#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include "semaphore.h"
#include "shared_memory.h"

void readInput(char* destination, int max_len) {
	char* line = 0;
	size_t linecap = 0;
	ssize_t linelen;
	int copylen;

	linelen = getline(&line, &linecap, stdin);
	if (linelen <= 0)
	{
		return;
	}
	copylen = max_len > linelen ? linelen : max_len;
	strncpy(destination, line, copylen);
	destination[copylen-1] = '\0';
}

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		printf("usage: %s <student id>\n", argv[0]);
		exit(4);
	}

	// check password
	printf("Please input password: ");
	char* line = 0;
	size_t linecap = 0;
	ssize_t linelen;
	linelen = getline(&line, &linecap, stdin);

	if (linelen == 4 && strncmp(line, "000\n", 4) == 0)
	{
		printf("password accepted\n");
	} else {
		printf("password incorrect\n");
		exit(8);
	}

	int semaphores = getSemaphores();
	semaphoreWait(semaphores, WRITER);

	StudentInfo* basePointer = getSharedMemoryPointer();
	StudentInfo* currentPointer = basePointer;

	int complen = strlen(argv[1]);
	complen = complen < ID_LEN ? complen : ID_LEN;

	int flag = 0;
	for (int i = 0; i < MAX_STUDENTS; ++i)
	{
		if (strncmp(currentPointer->studentID, argv[1], complen) == 0)
		{
			flag = 1;

			printf("Modifying record:\n");
			printf("%s\n", currentPointer->studentName);
			printf("%s\n", currentPointer->studentID);
			printf("%s\n", currentPointer->email);
			printf("%s\n", currentPointer->phoneNumber);

			printf("NAME: ");
			readInput(currentPointer->studentName, NAME_LEN);
			printf("ID: ");
			readInput(currentPointer->studentID, ID_LEN);
			printf("ADDRESS: ");
			readInput(currentPointer->email, EMAIL_ID);
			printf("PHONE NUMBER: ");
			readInput(currentPointer->phoneNumber, PHONE_LEN);

			printf("Record changed:\n");
			printf("%s\n", currentPointer->studentName);
			printf("%s\n", currentPointer->studentID);
			printf("%s\n", currentPointer->email);
			printf("%s\n", currentPointer->phoneNumber);
		}

		currentPointer++;
	}
	if (flag == 0)
	{
		printf("Record not found.\n");
	}

	sleep(10);
	semaphoreSignal(semaphores, WRITER);

	detachSharedMemoryPointer(basePointer);

	return 0;
}
