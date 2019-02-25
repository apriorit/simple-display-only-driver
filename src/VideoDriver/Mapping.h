#pragma once

NTSTATUS MapFrameBuffer(PHYSICAL_ADDRESS physicalAddr, ULONG length, VOID** virtualAddr);
NTSTATUS UnmapFrameBuffer(VOID* virtualAddr, ULONG length);
