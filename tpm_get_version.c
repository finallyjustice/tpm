/** File:		tpm_get_version.c
 ** Author:		Dongli Zhang
 ** Contact:	dongli.zhang0129@gmail.com
 **
 ** Copyright (C) Dongli Zhang 2013
 **
 ** This program is free software;  you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY;  without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 ** the GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program;  if not, write to the Free Software 
 ** Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* compile: gcc tpm_get_version.c -o tpm_get_version -ltspi */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <tss/tss_error.h>
#include <tss/platform.h>
#include <tss/tss_defines.h>
#include <tss/tss_typedef.h>
#include <tss/tss_structs.h>
#include <tss/tspi.h>
#include <trousers/trousers.h>

#include <string.h>
#include <stdarg.h>
#include <getopt.h>

int logHex(int a_iLen, void *a_pData)
{
	int i, iByte;
	char *pData = a_pData;
		     
	for (i = 0; i < a_iLen; i++) {
		if ((i % 32) == 0) {
			if (a_iLen > 32) {
				printf("\n\t");
			}
		} else if ((i % 4) == 0) {
			printf(" ");
		}
					   
		iByte = pData[i];
		iByte &= 0x000000ff;
		printf("%02x", iByte);
	}
			 
	printf("\n");
			  
	return a_iLen;
}

int main(int argc, char **argv)
{
	TSS_HCONTEXT hContext = 0;
	TSS_HTPM hTpm;
	TSS_RESULT result;
	BYTE *pResult;
	UINT32 lenResult;
	UINT64 offset;
	TPM_CAP_VERSION_INFO versionInfo;

	// Create context
	result = Tspi_Context_Create(&hContext);
	if(result != TSS_SUCCESS)
	{
		printf("Tspi_Context_Create failed\n");
		return 0;
	}
	
	// Connect to context
	result = Tspi_Context_Connect(hContext, NULL);
	if(result != TSS_SUCCESS)
	{
		printf("Tspi_Context_Connect failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		return 0;
	}

	// Retrieve TPM object of context
	result = Tspi_Context_GetTpmObject(hContext, &hTpm);
	if(result != TSS_SUCCESS)
	{
		printf("Tspi_Context_GetTpmObject failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		return 0;
	}

	result=Tspi_TPM_GetCapability(hTpm, TSS_TPMCAP_VERSION_VAL, 
								0, NULL, 
								&lenResult, 
								&pResult);
	if(result != TSS_SUCCESS)
	{
		printf("Tspi_TPM_GetCapability failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		return 0;
	}

	offset = 0;
	result = Trspi_UnloadBlob_CAP_VERSION_INFO(&offset, pResult, &versionInfo);
	if(result != TSS_SUCCESS)
	{
		printf("Trspi_UnloadBlob_CAP_VERSIOB_INFO failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		return 0;
	}

	printf("  Chip Version:		%hhu.%hhu.%hhu.%hhu\n",
			versionInfo.version.major, versionInfo.version.minor,
			versionInfo.version.revMajor, versionInfo.version.revMinor);
	printf("  Spec Level:		%hu\n", versionInfo.specLevel);
	printf("  Errata Revision:	%hhu\n", versionInfo.errataRev);
	printf("  TPM Vendor ID:	%c%c%c%c\n", 
			versionInfo.tpmVendorID[0], versionInfo.tpmVendorID[1],
			versionInfo.tpmVendorID[2], versionInfo.tpmVendorID[3]);

	result = Tspi_TPM_GetCapability(hTpm, TSS_TPMCAP_VERSION, 
								0, NULL,
								&lenResult,
								&pResult);
	if(result != TSS_SUCCESS)
	{
		printf("Tspi_TPM_GetCapability failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		return 0;
	}

	printf("  TPM Version:		");
	logHex(lenResult, pResult);

	Tspi_Context_FreeMemory(hContext, NULL);
	Tspi_Context_Close(hContext);
	return 1;
}
