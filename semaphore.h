#define SEMAPHORE_COUNT (2)
#define SEMAPHORE_KEY ((key_t) 2836)

#define READ_KEY ((key_t) 28366)

#define READER (1)
#define WRITER (0)

int* getReadCountPointer() {
	int id = shmget(READ_KEY, sizeof(int), IPC_CREAT | 0666);
	if (id < 0)
	{
		perror("error: shmget for readCount failed");
		exit(1);
	}

	int* ptr = shmat(id, 0, 0);

	if (ptr == (int*)0)
	{
		perror("error: shmat for readCount failed");
		exit(2);
	}

	return ptr;
}

void detachReadCount(int* readCount) {
	shmdt((void*) readCount);
}

void destroyReadCount() {
	int id = shmget(READ_KEY, sizeof(int), IPC_CREAT | 0666);
	if (id < 0)
	{
		perror("error: shmget for readCount failed");
		exit(1);
	}

	shmctl(id, IPC_RMID, (struct shmid_ds *) 0);
}

void semaphoreWait(int semaphore_set, int semaphore_id) {
	struct sembuf sop;
	sop.sem_num = semaphore_id;
	sop.sem_op  = -1;
	sop.sem_flg = 0;
	semop(semaphore_set, &sop, 1);
}

void semaphoreSignal(int semaphore_set, int semaphore_id) {
	struct sembuf sop;
	sop.sem_num = semaphore_id;
	sop.sem_op  = 1;
	sop.sem_flg = 0;
	semop(semaphore_set, &sop, 1);
}

int initSemaphores() {
	int semaphore_id;

	semaphore_id = semget(SEMAPHORE_KEY, SEMAPHORE_COUNT, IPC_CREAT | 0666);

	if (semaphore_id != -1) {
		semaphoreSignal(semaphore_id, READER);
		semaphoreSignal(semaphore_id, WRITER);
	} else {
		perror("error: semget failed");
		exit(3);
	}

	return semaphore_id;
}

int getSemaphores() {
	int semaphore_id;

	semaphore_id = semget(SEMAPHORE_KEY, SEMAPHORE_COUNT, IPC_CREAT | 0666);

	if (semaphore_id != -1) {
		// semaphoreSignal(semaphore_id, READER);
		// semaphoreSignal(semaphore_id, WRITER);
	} else {
		perror("error: semget failed");
		exit(3);
	}

	return semaphore_id;
}

void destroySemaphores(int semaphore_id) {
	semctl(semaphore_id, 0, IPC_RMID);
}
