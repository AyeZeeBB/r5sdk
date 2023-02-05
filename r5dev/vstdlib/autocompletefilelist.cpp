﻿//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//
#include "core/stdafx.h"
#include "completion.h"
#include "autocompletefilelist.h"

//-----------------------------------------------------------------------------
// Purpose: Fills in a list of commands based on specified subdirectory and extension into the format:
//  commandname subdir/filename.ext
//  commandname subdir/filename2.ext
// Returns number of files in list for autocompletion
//-----------------------------------------------------------------------------
int CAutoCompleteFileList::AutoCompletionFunc(const char* partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH])
{
	return v_CAutoCompleteFileList_AutoCompletionFunc(this, partial, commands);
}
