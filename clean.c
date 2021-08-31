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
		printf("usage: %s <data file>\n", argv[0]);
		exit(4);
	}

	int semaphores = getSemaphores();
	semaphoreWait(semaphores, WRITER);

	FILE* outputFile;
	outputFile = fopen(argv[1], "w");
	if (outputFile == 0)
	{
		printf("error: couldn't open output file");
		exit(5);
	}

	StudentInfo* basePointer = getSharedMemoryPointer();
	StudentInfo* currentPointer = basePointer;

	for (int i = 0; i < MAX_STUDENTS; ++i)
	{
		fprintf(outputFile, "%s\n", currentPointer->studentName);
		fprintf(outputFile, "%s\n", currentPointer->studentID);
		fprintf(outputFile, "%s\n", currentPointer->email);
		fprintf(outputFile, "%s\n", currentPointer->phoneNumber);
		
		currentPointer++;
	}

	fclose(outputFile);

	detachSharedMemoryPointer(basePointer);

	destroySharedMemory();
	destroyReadCount();

	semaphoreSignal(semaphores, WRITER);

	destroySemaphores(semaphores);
	return 0;
}
