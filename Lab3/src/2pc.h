#define ASK_COMMIT 0	// loop ask to commit msg, phase one
#define MSG_COMMIT 1	// continue to commit msg, phase two
#define MSG_ABORT 2		// ask to abort msg, phase two
#define MSG_ROLLBACK 3
#define MSG_COMPLETE 4
#define MSG_TERMINATE 10
#define RPLY_YES 0	//	cohort available to commit, phase one
#define RPLY_NO -1	//	cohort unavaileble to commit, phase one 
#define	COMMIT_SUCCESS	0	// cohort successfully commit, phase two
#define COMMIT_FAIL	-1	// cohort failed to commit, phase two
#define TIMEOUT 0.5	// in 0.5 second
