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
	if (argc != 1)
	{
		printf("usage: %s \n", argv[0]);
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

	for (int i = 0; i < MAX_STUDENTS; ++i)
	{
		printf("%s\n", currentPointer->studentName);
		printf("%s\n", currentPointer->studentID);
		printf("%s\n", currentPointer->email);
		printf("%s\n", currentPointer->phoneNumber);
		
		currentPointer++;
	}

	sleep(3);

	semaphoreWait(semaphores, READER);
	*readCount = *readCount - 1;
	if (*readCount == 0) semaphoreSignal(semaphores, WRITER);
	semaphoreSignal(semaphores, READER);

	detachSharedMemoryPointer(basePointer);
	detachReadCount(readCount);

	return 0;
}
