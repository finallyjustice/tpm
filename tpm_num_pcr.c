/** File:       tpm_num_pcr.c
 ** Author:     Dongli Zhang
 ** Contact:    dongli.zhang0129@gmail.com
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

/* compile: gcc tpm_num_pcr.c -o tpm_num_pcr -ltspi */

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

int main(int argc, char **argv)
{
	UINT32			pulRespDataLength;
	BYTE			*pNumPCRs;
	UINT32			subCap, subCapLength, numPcrs;
	TSS_HCONTEXT	hContext;
	TSS_HTPM		hTPM;
	TSS_RESULT		result;

	// Create Context
	result = Tspi_Context_Create(&hContext);
	if (result != TSS_SUCCESS)
	{
		printf("Tspi_Context_Create failed\n");
		return 0;
	}

	// Connect to Context
	result = Tspi_Context_Connect(hContext, NULL);
	if (result != TSS_SUCCESS)
	{
		printf("Tspi_Context_Connect failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		return 0;
	}

	// Retrieve TPM object of context
	result = Tspi_Context_GetTpmObject(hContext, &hTPM);
	if (result != TSS_SUCCESS)
	{
		printf("Tspi_Context_GetTpmObject failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		return 0;
	}

	subCap = TSS_TPMCAP_PROP_PCR;
	subCapLength = sizeof(UINT32);

	result = Tspi_TPM_GetCapability(hTPM, TSS_TPMCAP_PROPERTY,
								subCapLength, (BYTE *)&subCap,
								&pulRespDataLength,
								&pNumPCRs);
	if(result != TSS_SUCCESS)
	{
		printf("Tspi_TPM_GetCapability failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		return 0;
	}

	printf("There are %u PCRs supported by this TPM\n", *(UINT32 *)pNumPCRs);

	Tspi_Context_FreeMemory(hContext, NULL);
	Tspi_Context_Close(hContext);
	return 0;
}
