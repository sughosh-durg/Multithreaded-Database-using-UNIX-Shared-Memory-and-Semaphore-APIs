#define SHM_KEY ((key_t) 62836)
#define MAX_STUDENTS (20)
#define SEGMENT_SIZE (sizeof(StudentInfo) * MAX_STUDENTS)

#define NAME_LEN (40)
#define ID_LEN (15)
#define EMAIL_ID (50)
#define PHONE_LEN (15)

typedef struct StudentInfo_s {
	char studentName[NAME_LEN];
	char studentID[ID_LEN];
	char email[EMAIL_ID];
	char phoneNumber[PHONE_LEN];
} StudentInfo;

StudentInfo* getSharedMemoryPointer() {
	int id = -1;

	id = shmget(SHM_KEY, SEGMENT_SIZE, IPC_CREAT | 0666);

	if (id < 0) {
		perror("error: shmget failed");
		exit(1);
	}

	StudentInfo* basePointer = 0;

	basePointer = shmat(id, 0, 0);

	if (basePointer == (StudentInfo*)0) {
		perror("error: shmat failed");
		exit(2);
	}

	return basePointer;
}

void detachSharedMemoryPointer(StudentInfo* basePointer) {
	shmdt((void*) basePointer);
}

void destroySharedMemory() {
	int id = -1;

	id = shmget(SHM_KEY, SEGMENT_SIZE, IPC_CREAT | 0666);

	if (id < 0) {
		perror("error: shmget failed");
		exit(1);
	}

	shmctl(id, IPC_RMID, (struct shmid_ds *) 0);
}
