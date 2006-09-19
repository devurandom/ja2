//**************************************************************************
//
// Filename :	FileMan.c
//
//	Purpose :	function definitions for the memory manager
//
// Modification history :
//
//		24sep96:HJH		-> creation
//    08Apr97:ARM   -> Assign return value from Push() calls back to HStack
//                     handle, because it may possibly do a MemRealloc()
//		29Dec97:Kris Morness
//									-> Added functionality for setting file attributes which
//									   allows for read-only attribute overriding
//									-> Also added a simple function that clears all file attributes
//										 to normal.
//
//		5 Feb 98:Dave French -> extensive modification to support libraries
//
//**************************************************************************

#include "Container.h"
#include "Debug.h"
#include "FileMan.h"
#include "LibraryDataBase.h"
#include "MemMan.h"
#include "RegInst.h"
#include "Types.h"
#include <stdio.h>
#include <stdlib.h>


#define FILENAME_LENGTH					600

#define CHECKF(exp)  if (!(exp)) { return(FALSE); }


//The FileDatabaseHeader
DatabaseManagerHeaderStruct gFileDataBase;


WIN32_FIND_DATA Win32FindInfo[20];
BOOLEAN fFindInfoInUse[20] = {FALSE,FALSE,FALSE,FALSE,FALSE,
															FALSE,FALSE,FALSE,FALSE,FALSE,
															FALSE,FALSE,FALSE,FALSE,FALSE,
															FALSE,FALSE,FALSE,FALSE,FALSE };
HANDLE hFindInfoHandle[20] = {INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
															INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE };


void W32toSGPFileFind( GETFILESTRUCT *pGFStruct, WIN32_FIND_DATA *pW32Struct );


//**************************************************************************
//
// FileSystemInit
//
//		Starts up the file system.
//
// Parameter List :
// Return Value :
// Modification history :
//
//		24sep96:HJH		-> creation
//
//**************************************************************************

BOOLEAN	InitializeFileManager( STR strIndexFilename )
{
	RegisterDebugTopic( TOPIC_FILE_MANAGER, "File Manager" );
	return( TRUE );
}



//**************************************************************************
//
// FileSystemShutdown
//
//		Shuts down the file system.
//
// Parameter List :
// Return Value :
// Modification history :
//
//		24sep96:HJH		-> creation
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

void ShutdownFileManager( void )
{
	UnRegisterDebugTopic( TOPIC_FILE_MANAGER, "File Manager" );
}


//**************************************************************************
//
// FileExists
//
//		Checks if a file exists.
//
// Parameter List :
//
//		STR	-> name of file to check existence of
//
// Return Value :
//
//		BOOLEAN	-> TRUE if it exists
//					-> FALSE if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

BOOLEAN FileExists(const char *strFilename)
{
	BOOLEAN	fExists = FALSE;
	FILE		*file;
	//HANDLE	hRealFile;

	//open up the file to see if it exists on the disk
	file = fopen( strFilename, "r" );
	//hRealFile = CreateFile( strFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
	//								FILE_FLAG_RANDOM_ACCESS, NULL );
	if ( file )
	//if ( hRealFile != INVALID_HANDLE_VALUE )
	{
		fExists = TRUE;
		fclose( file );
		//CloseHandle( hRealFile );
	}

	//if the file wasnt on disk, check to see if its in a library
	if( fExists == FALSE )
	{
		//if the database is initialized
		if( gFileDataBase.fInitialized )
			fExists = CheckIfFileExistInLibrary( strFilename );
	}

	return( fExists );
}

//**************************************************************************
//
// FileExistsNoDB
//
//		Checks if a file exists, but doesn't check the database files.
//
// Parameter List :
//
//		STR	-> name of file to check existence of
//
// Return Value :
//
//		BOOLEAN	-> TRUE if it exists
//					-> FALSE if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//
//**************************************************************************

BOOLEAN FileExistsNoDB(const char *strFilename)
{
	BOOLEAN	fExists = FALSE;
	FILE		*file;
	//HANDLE	hRealFile;

	//open up the file to see if it exists on the disk
	file = fopen( strFilename, "r" );
	//hRealFile = CreateFile( strFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
	//								FILE_FLAG_RANDOM_ACCESS, NULL );
	if ( file )
	//if ( hRealFile != INVALID_HANDLE_VALUE )
	{
		fExists = TRUE;
		fclose( file );
		//CloseHandle( hRealFile );
	}

	return( fExists );

}

//**************************************************************************
//
// FileDelete
//
//		Deletes a file.
//
// Parameter List :
//
//		STR	-> name of file to delete
//
// Return Value :
//
//		BOOLEAN	-> TRUE if successful
//					-> FALSE if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//
//**************************************************************************

BOOLEAN FileDelete(const char *strFilename)
{
#if 1 // XXX TODO
	fprintf(stderr, "===> %s:%d: Ignoring %s(\"%s\")\n", __FILE__, __LINE__, __func__, strFilename);
	return FALSE;
	UNIMPLEMENTED();
#else
	return( DeleteFile( strFilename ) );
#endif
}

//**************************************************************************
//
// FileOpen
//
//		Opens a file.
//
// Parameter List :
//
//		STR	   -> filename
//		UIN32		-> access - read or write, or both
//		BOOLEAN	-> delete on close
//
// Return Value :
//
//		HWFILE	-> handle of opened file
//
// Modification history :
//
//		24sep96:HJH		-> creation
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

HWFILE FileOpen(const char *strFilename, UINT32 uiOptions, BOOLEAN fDeleteOnClose )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	HWFILE	hFile;
	HANDLE	hRealFile;
	DWORD		dwAccess;
	DWORD		dwFlagsAndAttributes;
	BOOLEAN	fExists;
	DWORD		dwCreationFlags;
	HWFILE hLibFile;

	hFile = 0;
	dwCreationFlags = 0;

	// check if the file exists - note that we use the function FileExistsNoDB
	// because it doesn't check the databases, and we don't want to do that here
	fExists = FileExistsNoDB( strFilename );

	dwAccess = 0;
	if ( uiOptions & FILE_ACCESS_READ )
		dwAccess |= GENERIC_READ;
	if ( uiOptions & FILE_ACCESS_WRITE )
		dwAccess |= GENERIC_WRITE;

	dwFlagsAndAttributes = FILE_FLAG_RANDOM_ACCESS;
	if ( fDeleteOnClose )
		dwFlagsAndAttributes |= FILE_FLAG_DELETE_ON_CLOSE;

	//if the file is on the disk
	if ( fExists )
	{
		hRealFile = CreateFile( strFilename, dwAccess, 0, NULL, OPEN_ALWAYS,
										dwFlagsAndAttributes, NULL );

		if ( hRealFile == INVALID_HANDLE_VALUE )
		{
			return(0);
		}

		//create a file handle for the 'real file'
		hFile = CreateRealFileHandle( hRealFile );
	}

	// if the file did not exist, try to open it from the database
	else if ( gFileDataBase.fInitialized )
	{
		//if the file is to be opened for writing, return an error cause you cant write a file that is in the database library
		if( fDeleteOnClose )
		{
			return( 0 );
		}

		//if the file doesnt exist on the harddrive, but it is to be created, dont try to load it from the file database
		if( uiOptions & FILE_ACCESS_WRITE )
		{
			//if the files is to be written to
			if( ( uiOptions & FILE_CREATE_NEW ) || ( uiOptions & FILE_OPEN_ALWAYS ) || ( uiOptions & FILE_CREATE_ALWAYS ) || ( uiOptions & FILE_TRUNCATE_EXISTING ) )
			{
				hFile = 0;
			}
		}
		//else if the file is to be opened using FILE_OPEN_EXISTING, and the file doesnt exists, fail out of the function)
//		else if( uiOptions & FILE_OPEN_EXISTING )
//		{
			//fail out of the function
//			return( 0 );
//		}
		else
		{
			//If the file is in the library, get a handle to it.
			hLibFile = OpenFileFromLibrary( strFilename );

			//tried to open a file that wasnt in the database
			if( !hLibFile )
				return( 0 );
			else
				return( hLibFile );		//return the file handle
		}
	}

	if ( !hFile )
	{
		if ( uiOptions & FILE_CREATE_NEW )
		{
			dwCreationFlags = CREATE_NEW;
		}
		else if ( uiOptions & FILE_CREATE_ALWAYS )
		{
			dwCreationFlags = CREATE_ALWAYS;
		}
		else if ( uiOptions & FILE_OPEN_EXISTING || uiOptions & FILE_ACCESS_READ )
		{
			dwCreationFlags = OPEN_EXISTING;
		}
		else if ( uiOptions & FILE_OPEN_ALWAYS )
		{
			dwCreationFlags = OPEN_ALWAYS;
		}
		else if ( uiOptions & FILE_TRUNCATE_EXISTING )
		{
			dwCreationFlags = TRUNCATE_EXISTING;
		}
		else
		{
			dwCreationFlags = OPEN_ALWAYS;
		}


		hRealFile = CreateFile( strFilename, dwAccess, 0, NULL, dwCreationFlags,
										dwFlagsAndAttributes, NULL );
		if ( hRealFile == INVALID_HANDLE_VALUE )
		{
				UINT32 uiLastError = GetLastError();
				char zString[1024];
				FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, uiLastError, 0, zString, 1024, NULL);

			return(0);
		}

		hFile = CreateRealFileHandle( hRealFile );
	}

	if ( !hFile )
		return(0);

	return(hFile);
#endif
}



//**************************************************************************
//
// FileClose
//
//
// Parameter List :
//
//		HWFILE hFile	-> handle to file to close
//
// Return Value :
// Modification history :
//
//		24sep96:HJH		-> creation
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

void FileClose( HWFILE hFile )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	INT16 sLibraryID;
	UINT32 uiFileNum;

	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	//if its the 'real file' library
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		//if its not already closed
		if( gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].uiFileID != 0 )
		{
			CloseHandle( gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle );
			gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].uiFileID = 0;
			gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle= 0;
			gFileDataBase.RealFiles.iNumFilesOpen--;
			if( gFileDataBase.RealFiles.iNumFilesOpen < 0 )
			{
				//if for some reason we are below 0, report an error ( should never be )
				Assert( 0 );
			}
		}
	}
	else
	{
		//if the database is initialized
		if( gFileDataBase.fInitialized )
			CloseLibraryFile( sLibraryID, uiFileNum );
	}
#endif
}

//**************************************************************************
//
// FileRead
//
//		To read a file.
//
// Parameter List :
//
//		HWFILE		-> handle to file to read from
//		void	*	-> source buffer
//		UINT32	-> num bytes to read
//		UINT32	-> num bytes read
//
// Return Value :
//
//		BOOLEAN	-> TRUE if successful
//					-> FALSE if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//		08Dec97:ARM		-> return FALSE if bytes to read != bytes read
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

#ifdef JA2TESTVERSION
	extern UINT32 uiTotalFileReadTime;
	extern UINT32 uiTotalFileReadCalls;
	#include "Timer_Control.h"
#endif

BOOLEAN FileRead( HWFILE hFile, PTR pDest, UINT32 uiBytesToRead, UINT32 *puiBytesRead )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	HANDLE	hRealFile;
	DWORD		dwNumBytesToRead, dwNumBytesRead;
	BOOLEAN	fRet = FALSE;
	INT16 sLibraryID;
	UINT32 uiFileNum;

#ifdef JA2TESTVERSION
	UINT32 uiStartTime = GetJA2Clock();
#endif

	//init the variables
	dwNumBytesToRead = dwNumBytesRead = 0;

	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	dwNumBytesToRead	= (DWORD)uiBytesToRead;

	//if its a real file, read the data from the file
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		//if the file is opened
		if( uiFileNum != 0 )
		{
			hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle;

			fRet = ReadFile( hRealFile, pDest, dwNumBytesToRead, &dwNumBytesRead, NULL );
			if ( dwNumBytesToRead != dwNumBytesRead )
			{
				UINT32 uiLastError = GetLastError();
				char zString[1024];
				FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, uiLastError, 0, zString, 1024, NULL);

				fRet = FALSE;
			}

			if ( puiBytesRead )
				*puiBytesRead = (UINT32)dwNumBytesRead;
		}
	}
	else
	{
		//if the database is initialized
		if( gFileDataBase.fInitialized )
		{
			//if the library is open
			if( IsLibraryOpened( sLibraryID ) )
			{
				//if the file is opened
				if( gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].uiFileID != 0 )
				{
					//read the data from the library
					fRet = LoadDataFromLibrary( sLibraryID, uiFileNum, pDest, dwNumBytesToRead, &dwNumBytesRead );
					if ( puiBytesRead )
					{
						*puiBytesRead = (UINT32)dwNumBytesRead;
					}
				}
			}
		}
	}
	#ifdef JA2TESTVERSION
		//Add the time that we spent in this function to the total.
		uiTotalFileReadTime += GetJA2Clock() - uiStartTime;
		uiTotalFileReadCalls++;
	#endif

	return(fRet);
#endif
}

//**************************************************************************
//
// FileWrite
//
//		To write a file.
//
// Parameter List :
//
//		HWFILE		-> handle to file to write to
//		void	*	-> destination buffer
//		UINT32	-> num bytes to write
//		UINT32	-> num bytes written
//
// Return Value :
//
//		BOOLEAN	-> TRUE if successful
//					-> FALSE if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//		08Dec97:ARM		-> return FALSE if dwNumBytesToWrite != dwNumBytesWritten
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

BOOLEAN FileWrite( HWFILE hFile, PTR pDest, UINT32 uiBytesToWrite, UINT32 *puiBytesWritten )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	HANDLE	hRealFile;
	DWORD		dwNumBytesToWrite, dwNumBytesWritten;
	BOOLEAN	fRet;
	INT16 sLibraryID;
	UINT32 uiFileNum;


	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	//if its a real file, read the data from the file
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		dwNumBytesToWrite = (DWORD)uiBytesToWrite;

		//get the real file handle to the file
		hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle;

		fRet = WriteFile( hRealFile, pDest, dwNumBytesToWrite, &dwNumBytesWritten, NULL );

		if (dwNumBytesToWrite != dwNumBytesWritten)
			fRet = FALSE;

		if ( puiBytesWritten )
			*puiBytesWritten = (UINT32)dwNumBytesWritten;
	}
	else
	{
		//we cannot write to a library file
		if ( puiBytesWritten )
			*puiBytesWritten = 0;
		return(FALSE);
	}

	return(fRet);
#endif
}

//**************************************************************************
//
// FileLoad
//
//		To open, read, and close a file.
//
// Parameter List :
//
//
// Return Value :
//
//		BOOLEAN	-> TRUE if successful
//					-> FALSE if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//		08Dec97:ARM		-> return FALSE if bytes to read != bytes read (CHECKF is inappropriate?)
//
//**************************************************************************

BOOLEAN FileLoad( STR strFilename, PTR pDest, UINT32 uiBytesToRead, UINT32 *puiBytesRead )
{
	HWFILE	hFile;
	UINT32	uiNumBytesRead;
	BOOLEAN	fRet;

	hFile = FileOpen( strFilename, FILE_ACCESS_READ, FALSE );
	if ( hFile )
	{
		fRet = FileRead( hFile, pDest, uiBytesToRead, &uiNumBytesRead );
		FileClose( hFile );

		if (uiBytesToRead != uiNumBytesRead)
			fRet = FALSE;

		if ( puiBytesRead )
			*puiBytesRead = uiNumBytesRead;

		CHECKF( uiNumBytesRead == uiBytesToRead );
	}
	else
		fRet = FALSE;

	return(fRet);
}


//**************************************************************************
//
// FileSeek
//
//		To seek to a position in a file.
//
// Parameter List :
//
//		HWFILE	-> handle to file to seek in
//		UINT32	-> distance to seek
//		UINT8		-> how to seek
//
// Return Value :
//
//		BOOLEAN	-> TRUE if successful
//					-> FALSE if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

BOOLEAN FileSeek( HWFILE hFile, UINT32 uiDistance, UINT8 uiHow )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	HANDLE	hRealFile;
	LONG		lDistanceToMove;
	DWORD		dwMoveMethod;
	INT32		iDistance=0;

	INT16 sLibraryID;
	UINT32 uiFileNum;

	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	//if its a real file, read the data from the file
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		//Get the handle to the real file
		hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle;

		iDistance = (INT32) uiDistance;

		if ( uiHow == FILE_SEEK_FROM_START )
			dwMoveMethod = FILE_BEGIN;
		else if ( uiHow == FILE_SEEK_FROM_END )
		{
			dwMoveMethod = FILE_END;
			if( iDistance > 0 )
				iDistance = -(iDistance);
		}
		else
			dwMoveMethod = FILE_CURRENT;

		lDistanceToMove = (LONG)uiDistance;

		if ( SetFilePointer( hRealFile, iDistance, NULL, dwMoveMethod ) == 0xFFFFFFFF )
			return(FALSE);
	}
	else
	{
		//if the database is initialized
		if( gFileDataBase.fInitialized )
			LibraryFileSeek( sLibraryID, uiFileNum, uiDistance, uiHow );
	}

	return(TRUE);
#endif
}

//**************************************************************************
//
// FileGetPos
//
//		To get the current position in a file.
//
// Parameter List :
//
//		HWFILE	-> handle to file
//
// Return Value :
//
//		INT32		-> current offset in file if successful
//					-> -1 if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

INT32 FileGetPos( HWFILE hFile )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	HANDLE	hRealFile;
	UINT32	uiPositionInFile=0;

	INT16 sLibraryID;
	UINT32 uiFileNum;

	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	//if its a real file, read the data from the file
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		//Get the handle to the real file
		hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle;

		uiPositionInFile = SetFilePointer( hRealFile, 0, NULL, FILE_CURRENT);
		if( uiPositionInFile == 0xFFFFFFFF )
		{
			uiPositionInFile = 0;
		}
		return( uiPositionInFile );
	}
	else
	{
		//if the library is open
		if( IsLibraryOpened( sLibraryID ) )
		{
			//check if the file is open
			if( gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].uiFileID != 0 )
			{
				uiPositionInFile = gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].uiFilePosInFile;
				return( uiPositionInFile );
			}
		}
	}

	return(BAD_INDEX);
#endif
}

//**************************************************************************
//
// FileGetSize
//
//		To get the current file size.
//
// Parameter List :
//
//		HWFILE	-> handle to file
//
// Return Value :
//
//		INT32		-> file size in file if successful
//					-> 0 if not
//
// Modification history :
//
//		24sep96:HJH		-> creation
//
//		9 Feb 98	DEF - modified to work with the library system
//
//**************************************************************************

UINT32 FileGetSize( HWFILE hFile )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	HANDLE  hRealHandle;
	UINT32	uiFileSize = 0xFFFFFFFF;

	INT16 sLibraryID;
	UINT32 uiFileNum;

	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	//if its a real file, read the data from the file
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		//Get the handle to a real file
		hRealHandle = gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle;

		uiFileSize = GetFileSize( hRealHandle, NULL );
	}
	else
	{
		//if the library is open
		if( IsLibraryOpened( sLibraryID ) )
			uiFileSize = gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].pFileHeader->uiFileLength;
	}


	if ( uiFileSize == 0xFFFFFFFF )
		return(0);
	else
		return( uiFileSize );
#endif
}


BOOLEAN SetFileManCurrentDirectory(const char *pcDirectory )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	 return( SetCurrentDirectory( pcDirectory ) );
#endif
}


BOOLEAN GetFileManCurrentDirectory( STRING512 pcDirectory )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	if (GetCurrentDirectory( 512, pcDirectory ) == 0)
	{
		return( FALSE );
	}
	return( TRUE );
#endif
}


BOOLEAN DirectoryExists( STRING512 pcDirectory )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	UINT32	uiAttribs;
	DWORD		uiLastError;

	uiAttribs = GetFileAttributes( pcDirectory );

	if ( uiAttribs == 0xFFFFFFFF )
	{
		// an error, make sure it's the right error
		uiLastError =	GetLastError();

		if (uiLastError != ERROR_FILE_NOT_FOUND)
		{
   		FastDebugMsg(String("DirectoryExists: ERROR - GetFileAttributes failed, error #%d on file %s", uiLastError, pcDirectory));
		}
	}
	else
	{
		// something's there, make sure it's a directory
		if ( uiAttribs & FILE_ATTRIBUTE_DIRECTORY )
		{
			return TRUE;
		}
	}

	// this could also mean that the name given is that of a file, or that an error occurred
	return FALSE;
#endif
}


BOOLEAN MakeFileManDirectory(const char *pcDirectory)
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	return CreateDirectory( pcDirectory, NULL );
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Removes ALL FILES in the specified directory (and all subdirectories with their files if fRecursive is TRUE)
// Use EraseDirectory() to simply delete directory contents without deleting the directory itself
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOLEAN RemoveFileManDirectory(const char *pcDirectory, BOOLEAN fRecursive)
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	WIN32_FIND_DATA sFindData;
	HANDLE		SearchHandle;
	const CHAR8	*pFileSpec = "*.*";
	BOOLEAN	fDone = FALSE;
	BOOLEAN fRetval=FALSE;
	CHAR8		zOldDir[512];
	CHAR8		zSubdirectory[512];

	GetFileManCurrentDirectory( zOldDir );

	if( !SetFileManCurrentDirectory( pcDirectory ) )
	{
		FastDebugMsg(String("RemoveFileManDirectory: ERROR - SetFileManCurrentDirectory on %s failed, error %d", pcDirectory, GetLastError()));
		return( FALSE );		//Error going into directory
	}

	//If there are files in the directory, DELETE THEM
	SearchHandle = FindFirstFile( pFileSpec, &sFindData);
	if( SearchHandle !=  INVALID_HANDLE_VALUE )
	{

		fDone = FALSE;
		do
		{
			// if the object is a directory
			if( GetFileAttributes( sFindData.cFileName ) == FILE_ATTRIBUTE_DIRECTORY )
			{
				// only go in if the fRecursive flag is TRUE (like Deltree)
				if (fRecursive)
				{
					sprintf(zSubdirectory, "%s\\%s", pcDirectory, sFindData.cFileName);

					if ((strcmp(sFindData.cFileName, ".") != 0) && (strcmp(sFindData.cFileName, "..") != 0))
					{
						if (!RemoveFileManDirectory(zSubdirectory, TRUE))
						{
				   		FastDebugMsg(String("RemoveFileManDirectory: ERROR - Recursive call on %s failed", zSubdirectory));
							break;
						}
					}
				}
				// otherwise, all the individual files will be deleted, but the subdirectories remain, causing
				// RemoveDirectory() at the end to fail, thus this function will return FALSE in that event (failure)
			}
			else
			{
				FileDelete( sFindData.cFileName );
			}

			//find the next file in the directory
			fRetval = FindNextFile( SearchHandle, &sFindData );
			if( fRetval == 0 )
			{
				fDone = TRUE;
			}
		}	while(!fDone);

		// very important: close the find handle, or subsequent RemoveDirectory() calls will fail
		FindClose( SearchHandle );
	}

	if( !SetFileManCurrentDirectory( zOldDir ) )
	{
		FastDebugMsg(String("RemoveFileManDirectory: ERROR - SetFileManCurrentDirectory on %s failed, error %d", zOldDir, GetLastError()));
		return( FALSE );		//Error returning from subdirectory
	}


	// The directory MUST be empty
	fRetval = RemoveDirectory( pcDirectory );
	if (!fRetval)
	{
		FastDebugMsg(String("RemoveFileManDirectory: ERROR - RemoveDirectory on %s failed, error %d", pcDirectory, GetLastError()));
	}

	return fRetval;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Removes ALL FILES in the specified directory but leaves the directory alone.  Does not affect any subdirectories!
// Use RemoveFilemanDirectory() to also delete the directory itself, or to recursively delete subdirectories.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOLEAN EraseDirectory(const char *pcDirectory)
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	WIN32_FIND_DATA sFindData;
	HANDLE		SearchHandle;
	const CHAR8	*pFileSpec = "*.*";
	BOOLEAN	fDone = FALSE;
	CHAR8		zOldDir[512];

	GetFileManCurrentDirectory( zOldDir );

	if( !SetFileManCurrentDirectory( pcDirectory ) )
	{
		FastDebugMsg(String("EraseDirectory: ERROR - SetFileManCurrentDirectory on %s failed, error %d", pcDirectory, GetLastError()));
		return( FALSE );		//Error going into directory
	}

	//If there are files in the directory, DELETE THEM
	SearchHandle = FindFirstFile( pFileSpec, &sFindData);
	if( SearchHandle !=  INVALID_HANDLE_VALUE )
	{

		fDone = FALSE;
		do
		{
			// if it's a file, not a directory
			if( GetFileAttributes( sFindData.cFileName ) != FILE_ATTRIBUTES_DIRECTORY )
			{
				FileDelete( sFindData.cFileName );
			}

			//find the next file in the directory
			if ( !FindNextFile( SearchHandle, &sFindData ))
			{
				fDone = TRUE;
			}
		} while(!fDone);

		// very important: close the find handle, or subsequent RemoveDirectory() calls will fail
		FindClose( SearchHandle );
	}

	if( !SetFileManCurrentDirectory( zOldDir ) )
	{
		FastDebugMsg(String("EraseDirectory: ERROR - SetFileManCurrentDirectory on %s failed, error %d", zOldDir, GetLastError()));
		return( FALSE );		//Error returning from directory
	}

	return( TRUE );
#endif
}


BOOLEAN GetExecutableDirectory( STRING512 pcDirectory )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	SGPFILENAME	ModuleFilename;
	UINT32 cnt;

	if ( GetModuleFileName( NULL, ModuleFilename, sizeof( ModuleFilename ) ) == 0 )
	{
		return( FALSE );
	}

	// Now get directory
	strcpy( pcDirectory, ModuleFilename );

	for ( cnt = strlen( pcDirectory ) - 1; cnt >= 0; cnt -- )
	{
		if ( pcDirectory[ cnt ] == '\\' )
		{
			 pcDirectory[ cnt ] = '\0';
			 break;
		}
	}

	return( TRUE );
#endif
}


BOOLEAN GetFileFirst(const char *pSpec, GETFILESTRUCT *pGFStruct )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	INT32 x,iWhich=0;
	BOOLEAN fFound;

	CHECKF( pSpec != NULL );
	CHECKF( pGFStruct != NULL );

	fFound = FALSE;
	for( x = 0; x < 20 && !fFound; x++)
	{
		if( !fFindInfoInUse[x] )
		{
			iWhich = x;
			fFound = TRUE;
		}
	}

	if ( !fFound )
		return(FALSE);

	pGFStruct->iFindHandle = iWhich;

	hFindInfoHandle[iWhich] = FindFirstFile( pSpec, &Win32FindInfo[iWhich] );

	if ( hFindInfoHandle[iWhich] == INVALID_HANDLE_VALUE )
		return(FALSE);
	fFindInfoInUse[iWhich] = TRUE;

	W32toSGPFileFind( pGFStruct, &Win32FindInfo[iWhich] );

	return(TRUE);
#endif
}

BOOLEAN GetFileNext( GETFILESTRUCT *pGFStruct )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	CHECKF( pGFStruct != NULL );

	if ( FindNextFile(hFindInfoHandle[pGFStruct->iFindHandle], &Win32FindInfo[pGFStruct->iFindHandle]) )
	{
		W32toSGPFileFind( pGFStruct, &Win32FindInfo[pGFStruct->iFindHandle] );
		return(TRUE);
	}
	return(FALSE);
#endif
}

void GetFileClose( GETFILESTRUCT *pGFStruct )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	if ( pGFStruct == NULL )
		return;

	FindClose( hFindInfoHandle[pGFStruct->iFindHandle] );
	hFindInfoHandle[pGFStruct->iFindHandle] = INVALID_HANDLE_VALUE;
	fFindInfoInUse[pGFStruct->iFindHandle] = FALSE;

	return;
#endif
}

void W32toSGPFileFind( GETFILESTRUCT *pGFStruct, WIN32_FIND_DATA *pW32Struct )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	UINT32 uiAttribMask;

	// Copy the filename
	strcpy(pGFStruct->zFileName, pW32Struct->cFileName);

	// Get file size
	if ( pW32Struct->nFileSizeHigh != 0 )
		pGFStruct->uiFileSize = 0xffffffff;
	else
		pGFStruct->uiFileSize = pW32Struct->nFileSizeLow;

	// Copy the file attributes
	pGFStruct->uiFileAttribs = 0;

	for( uiAttribMask = 0x80000000; uiAttribMask > 0; uiAttribMask >>= 1)
	{
		switch( pW32Struct->dwFileAttributes & uiAttribMask )
		{
			case FILE_ATTRIBUTE_ARCHIVE:
				pGFStruct->uiFileAttribs |= FILE_IS_ARCHIVE;
				break;

			case FILE_ATTRIBUTE_DIRECTORY:
				pGFStruct->uiFileAttribs |= FILE_IS_DIRECTORY;
				break;

			case FILE_ATTRIBUTE_HIDDEN:
				pGFStruct->uiFileAttribs |= FILE_IS_HIDDEN;
				break;

			case FILE_ATTRIBUTE_NORMAL:
				pGFStruct->uiFileAttribs |= FILE_IS_NORMAL;
				break;

			case FILE_ATTRIBUTE_READONLY:
				pGFStruct->uiFileAttribs |= FILE_IS_READONLY;
				break;

			case FILE_ATTRIBUTE_SYSTEM:
				pGFStruct->uiFileAttribs |= FILE_IS_SYSTEM;
				break;

			case FILE_ATTRIBUTE_TEMPORARY:
				pGFStruct->uiFileAttribs |= FILE_IS_TEMPORARY;
				break;

			case FILE_ATTRIBUTE_COMPRESSED:
				pGFStruct->uiFileAttribs |= FILE_IS_COMPRESSED;
				break;

			case FILE_ATTRIBUTE_OFFLINE:
				pGFStruct->uiFileAttribs |= FILE_IS_OFFLINE;
				break;
		}
	}
#endif
}


UINT32 FileGetAttributes(const char *strFilename)
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	UINT32	uiAttribs = 0;
	UINT32	uiFileAttrib = 0;

	uiAttribs = GetFileAttributes( strFilename );

	if( uiAttribs == 0xFFFFFFFF )
		return( uiAttribs );

	if( uiAttribs & FILE_ATTRIBUTE_ARCHIVE )
		uiFileAttrib |= FILE_ATTRIBUTES_ARCHIVE;

	if( uiAttribs & FILE_ATTRIBUTE_HIDDEN )
		uiFileAttrib |= FILE_ATTRIBUTES_HIDDEN;

	if( uiAttribs & FILE_ATTRIBUTE_NORMAL )
		uiFileAttrib |= FILE_ATTRIBUTES_NORMAL;

	if( uiAttribs & FILE_ATTRIBUTE_OFFLINE )
		uiFileAttrib |= FILE_ATTRIBUTES_OFFLINE;

	if( uiAttribs & FILE_ATTRIBUTE_READONLY )
		uiFileAttrib |= FILE_ATTRIBUTES_READONLY;

	if( uiAttribs & FILE_ATTRIBUTE_SYSTEM	)
		uiFileAttrib |= FILE_ATTRIBUTES_SYSTEM;

	if( uiAttribs & FILE_ATTRIBUTE_TEMPORARY )
		uiFileAttrib |= FILE_ATTRIBUTES_TEMPORARY;

	if( uiAttribs & FILE_ATTRIBUTE_DIRECTORY )
		uiFileAttrib |= FILE_ATTRIBUTES_DIRECTORY;



	return( uiFileAttrib );
#endif
}




BOOLEAN FileClearAttributes(const char *strFilename)
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	return SetFileAttributes( strFilename, FILE_ATTRIBUTE_NORMAL );
#endif
}


//returns true if at end of file, else false
BOOLEAN	FileCheckEndOfFile( HWFILE hFile )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	INT16 sLibraryID;
	UINT32 uiFileNum;
	HANDLE	hRealFile;
//	UINT8		Data;
	UINT32	uiNumberOfBytesRead=0;
	UINT32	uiOldFilePtrLoc=0;
	UINT32	uiEndOfFilePtrLoc=0;
	UINT32	temp=0;

	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	//if its a real file, read the data from the file
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		//Get the handle to the real file
		hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle;

		//Get the current position of the file pointer
		uiOldFilePtrLoc = SetFilePointer( hRealFile, 0, NULL, FILE_CURRENT );

		//Get the end of file ptr location
		uiEndOfFilePtrLoc = SetFilePointer( hRealFile, 0, NULL, FILE_END );

		//reset back to the original location
		temp = SetFilePointer( hRealFile, -( (INT32)( uiEndOfFilePtrLoc - uiOldFilePtrLoc ) ), NULL, FILE_END );

		//if the 2 pointers are the same, we are at the end of a file
		if( uiEndOfFilePtrLoc <= uiOldFilePtrLoc )
		{
			return( 1 );
		}
	}

	//else it is a library file
	else
	{
		//if the database is initialized
		if( gFileDataBase.fInitialized )
		{
			//if the library is open
			if( IsLibraryOpened( sLibraryID ) )
			{
				//if the file is opened
				if( gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].uiFileID != 0 )
				{
					UINT32	uiLength;					//uiOffsetInLibrary
//					HANDLE	hLibraryFile;
//					UINT32	uiNumBytesRead;
					UINT32	uiCurPos;

					uiLength = gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].pFileHeader->uiFileLength;
					uiCurPos = gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].uiFilePosInFile;

					//if we are trying to read more data then the size of the file, return an error
					if( uiCurPos >= uiLength )
					{
						return( TRUE );
					}
				}
			}
		}
	}

	//we are not and the end of a file
	return( 0 );
#endif
}



BOOLEAN GetFileManFileTime( HWFILE hFile, SGP_FILETIME	*pCreationTime, SGP_FILETIME *pLastAccessedTime, SGP_FILETIME *pLastWriteTime )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	HANDLE	hRealFile;
	INT16 sLibraryID;
	UINT32 uiFileNum;

	FILETIME	sCreationUtcFileTime;
	FILETIME	sLastAccessedUtcFileTime;
	FILETIME	sLastWriteUtcFileTime;

	//Initialize the passed in variables
	memset( pCreationTime, 0, sizeof( SGP_FILETIME ) );
	memset( pLastAccessedTime, 0, sizeof( SGP_FILETIME ) );
	memset( pLastWriteTime, 0, sizeof( SGP_FILETIME ) );


	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	//if its a real file, read the data from the file
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		//get the real file handle to the file
		hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle;

		//Gets the UTC file time for the 'real' file
		GetFileTime( hRealFile, &sCreationUtcFileTime, &sLastAccessedUtcFileTime, &sLastWriteUtcFileTime );

		//converts the creation UTC file time to the current time used for the file
		FileTimeToLocalFileTime( &sCreationUtcFileTime, pCreationTime );

		//converts the accessed UTC file time to the current time used for the file
		FileTimeToLocalFileTime( &sLastAccessedUtcFileTime, pLastAccessedTime );

		//converts the write UTC file time to the current time used for the file
		FileTimeToLocalFileTime( &sLastWriteUtcFileTime, pLastWriteTime );
	}
	else
	{
		//if the database is initialized
		if( gFileDataBase.fInitialized )
		{
			//if the library is open
			if( IsLibraryOpened( sLibraryID ) )
			{
				//if the file is opened
				if( gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].uiFileID != 0 )
				{
					if( !GetLibraryFileTime( sLibraryID, uiFileNum, pLastWriteTime ) )
					{
						return( FALSE );
					}
				}
			}
		}
	}

	return( TRUE );
#endif
}


INT32	CompareSGPFileTimes( SGP_FILETIME	*pFirstFileTime, SGP_FILETIME *pSecondFileTime )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	return( CompareFileTime( pFirstFileTime, pSecondFileTime ) );
#endif
}


UINT32 FileSize(const char *strFilename)
{
HWFILE hFile;
UINT32 uiSize;

	if((hFile=FileOpen(strFilename, FILE_OPEN_EXISTING | FILE_ACCESS_READ, FALSE))==0)
		return(0);

	uiSize=FileGetSize(hFile);
	FileClose(hFile);

	return(uiSize);
}


HANDLE	GetRealFileHandleFromFileManFileHandle( HWFILE hFile )
{
	INT16 sLibraryID;
	UINT32 uiFileNum;

	GetLibraryAndFileIDFromLibraryFileHandle( hFile, &sLibraryID, &uiFileNum );

	//if its the 'real file' library
	if( sLibraryID == REAL_FILE_LIBRARY_ID )
	{
		//if its not already closed
		if( gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].uiFileID != 0 )
		{
			return( gFileDataBase.RealFiles.pRealFilesOpen[ uiFileNum ].hRealFileHandle );
		}
	}
	else
	{
		//if the file is not opened, dont close it
		if( gFileDataBase.pLibraries[ sLibraryID ].pOpenFiles[ uiFileNum ].uiFileID != 0 )
		{
			return( gFileDataBase.pLibraries[ sLibraryID ].hLibraryHandle );
		}
	}
	return( 0 );
}


UINT32 GetFreeSpaceOnHardDriveWhereGameIsRunningFrom( )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
  STRING512		zExecDir;
  STRING512		zDrive;
	STRING512		zDir;
	STRING512		zFileName;
	STRING512		zExt;

	UINT32 uiFreeSpace = 0;

	GetExecutableDirectory( zExecDir );

	//get the drive letter from the exec dir
	_splitpath( zExecDir, zDrive, zDir, zFileName, zExt);

	sprintf( zDrive, "%s\\", zDrive );

	uiFreeSpace = GetFreeSpaceOnHardDrive( zDrive );

	return( uiFreeSpace );
#endif
}


UINT32 GetFreeSpaceOnHardDrive( STR pzDriveLetter )
{
#if 1 // XXX TODO
	UNIMPLEMENTED();
#else
	UINT32			uiBytesFree=0;

	UINT32			uiSectorsPerCluster=0;
	UINT32			uiBytesPerSector=0;
	UINT32			uiNumberOfFreeClusters=0;
	UINT32			uiTotalNumberOfClusters=0;

	if( !GetDiskFreeSpace( pzDriveLetter, &uiSectorsPerCluster, &uiBytesPerSector, &uiNumberOfFreeClusters, &uiTotalNumberOfClusters ) )
	{
		UINT32 uiLastError = GetLastError();
		char zString[1024];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, uiLastError, 0, zString, 1024, NULL);

		return( TRUE );
	}

	uiBytesFree = uiBytesPerSector * uiNumberOfFreeClusters * uiSectorsPerCluster;

	return( uiBytesFree );
#endif
}
