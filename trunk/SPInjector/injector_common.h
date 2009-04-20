/*-----------------------------------------------------------------------------
 * injector_common.h
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * Revision History:
 * Date					Who					What
 * ----------------		----------------	----------------
 * 28.11.2007			somma		        birth
**---------------------------------------------------------------------------*/

#ifndef _PLUG_IN_H_
#define _PLUG_IN_H_

#define SHARE_BUFFER_SIZE		20480		// 2Mb
#define PROC_NAME_LEN			128

typedef enum _AK_INJECTOR_COMMAND
{
	CMD_UNKNOWN = 0,
	CMD_QUIT,
	CMD_CONTINUE,

	CMD_HELLO,
	CMD_LOAD_PLUGIN,
	CMD_UNLOAD_PLUGIN,
	CMD_CALL_PLUGIN
} AK_INJECTOR_COMMAND;
#define INJECTOR_CMD_TO_STRING(cmd)     L##cmd

typedef enum _AK_INJECTOR_STATUS
{
	STATUS_UNKNOWN = 0,
	STATUS_READY,			// ready to receive event from injector.
	STATUS_WAIT_CONTINUE,	// waiting for injector's command (CMD_CONTINUE)
	STATUS_THREAD_EXITED	// injected thread is returned.

} AK_INJECTOR_STATUS;

typedef struct _CMD_CALL_PLUGIN_REC
{
	union
	{
		// input parameter for procedure
		//
		struct PROC
		{
			CHAR	ProcName[PROC_NAME_LEN];		// no TCHAR, CHAR ! (cf. GetProcAddress)
			BYTE	Param_1[128];
			BYTE	Param_2[128];
			BYTE	Param_3[128];
			BYTE	Param_4[128];
		} Input;

		// output buffer for procedure
		//
		BYTE	Output[SHARE_BUFFER_SIZE];
	};
} CMD_CALL_PLUGIN_REC, *PCMD_CALL_PLUGIN_REC;

typedef int (__cdecl *PLUGIN_PROC) (PCMD_CALL_PLUGIN_REC Param);


#endif//_PLUG_IN_H_