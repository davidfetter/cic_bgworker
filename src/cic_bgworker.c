/* Required for any background worker */
#include "miscadmin.h"
#include "pgstat.h"
#include "postmaster/bgworker.h"
#include "storage/ipc.h"
#include "storage/latch.h"
#include "storage/lwlock.h"
#include "storage/proc.h"
#include "storage/shmem.h"

/* I think these headers are going to be required for query execution, etc. */
#include "access/xact.h"
#include "executor/spi.h"
#include "fmgr.h"
/* less sure about these */
#include "lib/stringinfo.h"
#include "utils/builtins.h"
#include "utils/snapmgr.h"

PG_MODULE_MAGIC;

void _PG_init(void);
void cic_main(Datum main_arg) pg_attribute_noreturn();

static volatile sig_atomic_t got_sigterm = false;

static void
cic_sigterm(SIGNAL_ARGS)
{
	int		save_errno = errno;

	got_sigterm = true;
	if (MyProc)
		SetLatch(&Myproc->procLatch);

	errno = save_errno;
}

static void
cic_sighup(SIGNAL_ARGS)
{
	elog(LOG, "got sighup");
	if (MyProc)
		SetLatch(&Myproc->procLatch);
}

PG_FUNCTION_INFO_V1(cic_main);

void
cic_main(Datum main_arg)
{
	/* Register functions to handle SIG{HUP|TERM} */
	pqsignal(SIGHUP, cic_sighup);
	pqsignal(SIGTERM, cic_sigterm);

	BackgroundWorkerInitializeConnection("postgres", NULL, 0);

	while(!got_sigterm)
	{
		int				ret;
		StringInfoData	buf;

		WaitLatch(&MyProc->procLatch,
				  WL_LATCH_SET | WL_TIMEOUT | WL_EXIT_ON_PM_DEATH,
				  1000L,
				  PG_WAIT_EXTENSION);
		ResetLatch(&MyProc->procLatch);
/*
 * The idea here is to hand the EventTriggerData's Node *parsetree for CREATE
 * INDEX commands directly off to SPI (or whatever API underneath that is
 * needed) and have the CIC run in a bgworker.
 */
	}
}

void
_PG_init(void)
{
	BackgroundWorker	worker;

	/* Register worker processes */
	MemSet(&worker, 0, sizeof(BackgroundWorker));
	worker.bgw_flags = BGWORKER_SHMEM_ACCESS |
		BGWORKER_BACKEND_DATABASE_CONNECTION;
	worker.bgw_start_time = BgWorkerStart_RecoveryFinished;
	snprintf(worker.bgw_library_name, BGW_MAXLEN, "cic_bgworker");
	snprintf(worker.bgw_function_name, BGW_MAXLEN, "cic_main");
	snprintf(worker.bgw_name, BGW_MAXLEN, "CIC II: The Backgroundworkering");
	worker.bgw_restart_time = BGW_NEVER_RESTART;
	worker.bgw_main_arg = (Datum) 0;
	worker.bgw_notify_pid = 0;
	RegisterBackgroundWorker(&worker);
}
