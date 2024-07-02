#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#include "cuda_memory.h"

static int init_gpu(struct cuda_memory_ctx *ctx)
{
	int cuda_device_id = ctx->device_id;
	int cuda_pci_bus_id;
	int cuda_pci_device_id;
	int index;
	CUdevice cu_device;

	printf("initializing CUDA\n");
	CUresult error = cuInit(0);
	if (error != CUDA_SUCCESS) {
		printf("cuInit(0) returned %d\n", error);
		return FAILURE;
	}

	int deviceCount = 0;
	error = cuDeviceGetCount(&deviceCount);
	if (error != CUDA_SUCCESS) {
		printf("cuDeviceGetCount() returned %d\n", error);
		return FAILURE;
	}
	/* This function call returns 0 if there are no CUDA capable devices. */
	if (deviceCount == 0) {
		printf("There are no available device(s) that support CUDA\n");
		return FAILURE;
	}
	if (cuda_device_id >= deviceCount) {
		fprintf(stderr, "No such device ID (%d) exists in system\n", cuda_device_id);
		return FAILURE;
	}

	printf("Listing all CUDA devices in system:\n");
	for (index = 0; index < deviceCount; index++) {
		CUCHECK(cuDeviceGet(&cu_device, index));
		cuDeviceGetAttribute(&cuda_pci_bus_id, CU_DEVICE_ATTRIBUTE_PCI_BUS_ID , cu_device);
		cuDeviceGetAttribute(&cuda_pci_device_id, CU_DEVICE_ATTRIBUTE_PCI_DEVICE_ID , cu_device);
		printf("CUDA device %d: PCIe address is %02X:%02X\n", index, (unsigned int)cuda_pci_bus_id, (unsigned int)cuda_pci_device_id);
	}

	printf("\nPicking device No. %d\n", cuda_device_id);

	CUCHECK(cuDeviceGet(&ctx->cuDevice, cuda_device_id));

	char name[128];
	CUCHECK(cuDeviceGetName(name, sizeof(name), cuda_device_id));
	printf("[pid = %d, dev = %d] device name = [%s]\n", getpid(), ctx->cuDevice, name);
	printf("creating CUDA Ctx\n");

	/* Create context */
	error = cuCtxCreate(&ctx->cuContext, CU_CTX_MAP_HOST, ctx->cuDevice);
	if (error != CUDA_SUCCESS) {
		printf("cuCtxCreate() error=%d\n", error);
		return FAILURE;
	}

	printf("making it the current CUDA Ctx\n");
	error = cuCtxSetCurrent(ctx->cuContext);
	if (error != CUDA_SUCCESS) {
		printf("cuCtxSetCurrent() error=%d\n", error);
		return FAILURE;
	}

	return SUCCESS;
}