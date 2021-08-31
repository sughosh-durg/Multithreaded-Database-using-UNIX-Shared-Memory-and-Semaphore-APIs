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

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		printf("usage: %s <student id> \n", argv[0]);
		exit(4);
	}

	int* readCount = getReadCountPointer();

	int semaphores = getSemaphores();

	semaphoreWait(semaphores, READER);
	*readCount = *readCount + 1;
	if (*readCount == 1) semaphoreWait(semaphores, WRITER);
	semaphoreSignal(semaphores, READER);

	StudentInfo* basePointer = getSharedMemoryPointer();
	StudentInfo* currentPointer = basePointer;

	int complen = strlen(argv[1]);
	complen = complen < ID_LEN ? complen : ID_LEN;

	int flag = 0;
	for (int i = 0; i < MAX_STUDENTS; ++i)
	{
		if (strncmp(currentPointer->studentID, argv[1], complen) == 0)
		{
			printf("Record found:\n");
			printf("%s\n", currentPointer->studentName);
			printf("%s\n", currentPointer->studentID);
			printf("%s\n", currentPointer->email);
			printf("%s\n", currentPointer->phoneNumber);
			flag = 1;
		}
		
		currentPointer++;
	}

	if (flag == 0)
	{
		printf("No record found.\n");
	}

	sleep(10);

	semaphoreWait(semaphores, READER);
	*readCount = *readCount - 1;
	if (*readCount == 0) semaphoreSignal(semaphores, WRITER);
	semaphoreSignal(semaphores, READER);

	detachSharedMemoryPointer(basePointer);
	detachReadCount(readCount);

	return 0;
}
