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

void blankRecord(StudentInfo* info) {
	memset(info, 0, sizeof(StudentInfo));
}

int readSingleRecord(FILE* inputFile, StudentInfo* info) {
	char* line = 0;
	size_t linecap = 0;
	ssize_t linelen;
	int copylen;

	linelen = getline(&line, &linecap, inputFile);
	if (linelen <= 0)
	{
		blankRecord(info);
		return 0;
	}
	copylen = NAME_LEN > linelen ? linelen : NAME_LEN;
	strncpy((char*)&(info->studentName), line, copylen);
	info->studentName[copylen-1] = '\0';

	linelen = getline(&line, &linecap, inputFile);
	if (linelen <= 0)
	{
		return 1;
	}
	copylen = ID_LEN > linelen ? linelen : ID_LEN;
	strncpy((char*)&(info->studentID), line, copylen);
	info->studentID[copylen-1] = '\0';

	linelen = getline(&line, &linecap, inputFile);
	if (linelen <= 0)
	{
		return 1;
	}
	copylen = EMAIL_ID > linelen ? linelen : EMAIL_ID;
	strncpy((char*)&(info->email), line, copylen);
	info->email[copylen-1] = '\0';

	linelen = getline(&line, &linecap, inputFile);
	if (linelen <= 0)
	{
		return 1;
	}
	copylen = PHONE_LEN > linelen ? linelen : PHONE_LEN;
	strncpy((char*)&(info->phoneNumber), line, copylen);
	info->phoneNumber[copylen-1] = '\0';

	return 0;
}

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		printf("usage: %s <data file>\n", argv[0]);
		exit(4);
	}

	int semaphores = initSemaphores();
	semaphoreWait(semaphores, WRITER);

	int* readCount = getReadCountPointer();
	*readCount = 0;

	FILE* inputFile;
	inputFile = fopen(argv[1], "r");
	if (inputFile == 0)
	{
		printf("Error: Could not open the input file");
		exit(5);
	}

	StudentInfo* basePointer = getSharedMemoryPointer();
	StudentInfo* currentPointer = basePointer;

	for (int i = 0; i < MAX_STUDENTS; ++i)
	{
		if (readSingleRecord(inputFile, currentPointer) != 0) {
			printf("Error reading the file\n");

			fclose(inputFile);

			semaphoreSignal(semaphores, WRITER);

			detachSharedMemoryPointer(basePointer);
			detachReadCount(readCount);

			exit(6);
		}
		currentPointer++;
	}

	fclose(inputFile);

	sleep(3);
	semaphoreSignal(semaphores, WRITER);

	detachSharedMemoryPointer(basePointer);
	detachReadCount(readCount);

	return 0;
}
