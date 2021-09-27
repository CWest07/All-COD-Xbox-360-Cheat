#pragma once

#include "stdafx.h"

//#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L)

// Create disposition values.
#define FILE_SUPERSEDE							0x00000000
#define FILE_OPEN								0x00000001
#define FILE_CREATE								0x00000002
#define FILE_OPEN_IF							0x00000003
#define FILE_OVERWRITE							0x00000004
#define FILE_OVERWRITE_IF						0x00000005
#define FILE_MAXIMUM_DISPOSITION				0x00000005

typedef struct _DRIVE
{
	LPCSTR DriveName;
	LPCSTR DevicePath;
	DWORD Visibility; // 0 = Not visible, 1 = internal, 2 = visible
} DRIVE;

typedef struct _TRANSLATE_SYMLINK
{
	DWORD reserved;
	PANSI_STRING name;
} TRANSLATE_SYMLINK, *PTRANSLATE_SYMLINK;

DRIVE Drives[] =
{
	{ "FLASH",		"\\Device\\Flash",							1 },

	{ "SystemExt",	"\\Device\\Harddisk0\\SystemExtPartition",	1 },

	{ "INTUSB",		"\\Device\\BuiltInMuUsb\\Storage",			2 },
	{ "MUINT",		"\\Device\\BuiltInMuSfc",					2 },

	{ "HDD",		"\\Device\\Harddisk0\\Partition1",			2 },
	{ "DEVKIT",		"\\Device\\Harddisk0\\Partition1\\DEVKIT",	2 },

	{ "MU0",		"\\Device\\Mu0",							2 },
	{ "MU1",		"\\Device\\Mu1",							2 },

	{ "SystemAuxPartition",		"\\Device\\Harddisk0\\SystemAuxPartition", 1 },

	{ "USB0",		"\\Device\\Mass0",							2 },
	{ "USB1",		"\\Device\\Mass1",							2 },
	{ "USB2",		"\\Device\\Mass2",							2 },
	{ "USBMU0",		"\\Device\\Mass0PartitionFile\\Storage",	2 },
	{ "USBMU1",		"\\Device\\Mass1PartitionFile\\Storage",	2 },
	{ "USBMU2",		"\\Device\\Mass2PartitionFile\\Storage",	2 },

	{ "DVD",		"\\Device\\CdRom0",							2 },
};

extern int cchDrives = sizeof(Drives) / sizeof(DRIVE);


extern NTSTATUS ObTranslateSymbolicLink(IN PVOID, OUT PANSI_STRING);
NTSTATUS NtClose(HANDLE Handle);
NTSTATUS NtCreateFile(HANDLE* FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes,
	ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions);

HRESULT FFileNameToObName(LPCSTR szFileName, CHAR *szObName, DWORD cchObName)
{
	HRESULT hr = S_OK;
	char szName[MAX_PATH], oszName[MAX_PATH];
	int cchName, i;
	ANSI_STRING as, as2;
	TRANSLATE_SYMLINK obName;

	if (!cchObName || !szFileName || !szObName)
		return E_INVALIDARG;

	strcpy_s(szName, sizeof(szName), szFileName);

	for (cchName = 0; szName[cchName]; cchName++)
	{
		if (szName[cchName] == ':')
		{
			szName[cchName] = 0;

			break;
		}
	}

	if (!strcmpi(szName, "F") || !strcmpi(szName, "SMB"))
		return E_INVALIDARG;

	oszName[0] = 0;

	for (i = 0; i < cchDrives; i++)
		if (!strcmpi(Drives[i].DriveName, szName))
		{
			strcpy(oszName, Drives[i].DevicePath);
		}

	if (!oszName[0])
	{
		obName.reserved = 0;
		obName.name = &as;

		strcat(szName, ":\\");

		RtlSnprintf(szObName, cchObName, "\\??\\%s", szName);

		RtlInitAnsiString(&as, szObName);

		as2.Buffer = oszName;
		as2.Length = 0;
		as2.MaximumLength = sizeof(oszName);

		if (NT_SUCCESS(ObTranslateSymbolicLink(&obName, &as2)))
			oszName[as2.Length] = 0;
		else
			hr = E_FAIL;
	}

	if (!FAILED(hr))
		RtlSnprintf(szObName, cchObName, "%s%s", oszName, szFileName + cchName + 1);
	else
		szObName[0] = 0;

	return hr;
}

NTSTATUS FCreateFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, LPCSTR FileName, PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions)
{
	char oszName[MAX_PATH];
	ANSI_STRING as;
	IO_STATUS_BLOCK iosb;
	OBJECT_ATTRIBUTES attr;

	if (FileName[0] == '\\')
		strcpy_s(oszName, sizeof(oszName), FileName);
	else if (FAILED(FFileNameToObName(FileName, oszName, sizeof(oszName))))
		return STATUS_ACCESS_DENIED;

	RtlInitAnsiString(&as, oszName);
	attr.Attributes = OBJ_CASE_INSENSITIVE;
	attr.ObjectName = &as;
	attr.RootDirectory = NULL;

	return NtCreateFile(FileHandle, DesiredAccess, &attr, &iosb, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions);
}

NTSTATUS FReadFile(HANDLE FileHandle, PVOID Buffer, ULONG Length)
{
	IO_STATUS_BLOCK io;

	return NtReadFile(FileHandle, NULL, NULL, NULL, &io, Buffer, Length, NULL);
}

bool FileExists(std::string filename) {
	std::ifstream file(filename);
	bool Return = false;
	if (file)
		Return = true;
	file.close();
	return Return;
}

BOOL CReadFile(PCHAR File, PVOID Out, DWORD Length) {

	//return SUCCEEDED(XeKeysReadFile(File, Out, Length, Length, 0));

	NTSTATUS st;
	HANDLE h;

	st = FCreateFile(&h, GENERIC_READ | SYNCHRONIZE, File,
		NULL, 0, 0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);

	if (FAILED(st))
		return FALSE;

	st = FReadFile(h, Out, Length);
	NtClose(h);
	if (FAILED(st)) {
		ZeroMemory(Out, Length);
		return FALSE;
	}
	return TRUE;
}

BOOL CReadFile(PCHAR File, byte** Out, PDWORD pLength) {

	// test this out later
	//return SUCCEEDED(XeKeysReadFile(File, Out, Length, Length, 0));

	NTSTATUS st;
	HANDLE h;
	DWORD Length = 0;

	st = FCreateFile(&h, GENERIC_READ | SYNCHRONIZE, File,
		NULL, 0, 0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);

	if (FAILED(st))
		return FALSE;

	Length = GetFileSize(h, 0);
	if (pLength)
		*pLength = Length;

	*Out = new byte[Length];

	st = FReadFile(h, *Out, Length);
	NtClose(h);
	if (FAILED(st))
		return FALSE;

	return TRUE;
}

typedef struct _INF {
	int cbBuf;
	void *pbBuf;

	int cbUsed;
} INF, *PINF;

NTSTATUS FFlushFile(HANDLE FileHandle, PINF pinf) {
	IO_STATUS_BLOCK io;

	NTSTATUS status = NtWriteFile(FileHandle, NULL, NULL, NULL, &io, pinf->pbBuf, pinf->cbUsed, NULL);
	pinf->cbUsed = 0;

	return status;
}

NTSTATUS FWriteFile(HANDLE FileHandle, PVOID pb2, ULONG cb, PINF pinf) {
	IO_STATUS_BLOCK io;
	NTSTATUS status = (NTSTATUS)0;

	char* pb = (char*)pb2;

	// Buffered io stuff
	if (pb) {
		while (cb) {
			DWORD cbT = pinf->cbBuf - pinf->cbUsed;
			if (cb >= cbT) {
				memcpy((char*)pinf->pbBuf + pinf->cbUsed, pb, cbT);
				status = NtWriteFile(FileHandle, NULL, NULL, NULL, &io, pinf->pbBuf,
					pinf->cbBuf, NULL);
				pb += cbT;
				cb -= cbT;
				pinf->cbUsed = 0;
			}
			else {
				memcpy((char*)pinf->pbBuf + pinf->cbUsed, pb, cb);
				pb += cb;
				pinf->cbUsed += cb;
				cb = 0;
			}
		}
	}

	return status;
}

NTSTATUS FWriteString(HANDLE FileHandle, const char *pch, PINF pinf) {
	return FWriteFile(FileHandle, (PVOID)pch, strlen(pch), pinf);
}

// CreateFile to use with \Device\Harddisk0\Partition1\Devkit\ paths
NTSTATUS DirectCreateFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, const char* FileName, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions) {
	ANSI_STRING fileName;
	RtlInitAnsiString(&fileName, FileName);

	IO_STATUS_BLOCK io;

	OBJECT_ATTRIBUTES attr;
	attr.Attributes = 0;
	attr.ObjectName = &fileName;
	attr.RootDirectory = NULL;

	return NtCreateFile(FileHandle, DesiredAccess, &attr, &io, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions);
}

NTSTATUS FCloseFile(HANDLE FileHandle, PINF pinf)
{
	IO_STATUS_BLOCK iosb;

	if (pinf && pinf->cbUsed)
	{
		NtWriteFile(FileHandle, NULL, NULL, NULL, &iosb, pinf->pbBuf, pinf->cbUsed, NULL);
		pinf->cbUsed = 0;
	}

	return NtClose(FileHandle);
}

BOOL CWriteFile(PCHAR File, PBYTE Buffer, DWORD Length) {

	NTSTATUS st;
	HANDLE h;
	INF inf;

	inf.cbBuf = Length;
	inf.cbUsed = 0;
	inf.pbBuf = Buffer;

	st = FCreateFile(&h, GENERIC_WRITE | SYNCHRONIZE, File, NULL, 0, 0, FILE_OVERWRITE_IF, FILE_SYNCHRONOUS_IO_NONALERT);

	if (FAILED(st))
		return FALSE;

	st = FWriteFile(h, Buffer, Length, &inf);
	FCloseFile(h, &inf);

	if (FAILED(st))
		return FALSE;

	return TRUE;
}
