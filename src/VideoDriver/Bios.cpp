#include "stdafx.h"
#include "Bios.h"
#include "Memory.h"
#include "DefResolutions.h"
#include "VESAGraphicsModefunction.h"

#pragma pack(push)
#pragma pack(1)

struct BiosRegisters
{
    ULONG eax;
    ULONG ecx;
    ULONG edx;
    ULONG ebx;
    ULONG ebp;
    ULONG esi;
    ULONG edi;
    USHORT ds;
    USHORT es;
};

struct VesaVbeInfo
{
    CHAR Signature[4];
    USHORT Version;
    ULONG OemStringPtr;
    LONG Capabilities;
    ULONG VideoModePtr;
    USHORT TotalMemory;
    USHORT OemSoftwareRevision;
    ULONG OemVendorNamePtr;
    ULONG OemProductNamePtr;
    ULONG OemProductRevPtr;
    CHAR Reserved[222];
    CHAR OemData[256];
};

struct VBE_MODEINFO
{
    /* Mandatory information for all VBE revisions */
    USHORT ModeAttributes;
    UCHAR WinAAttributes;
    UCHAR WinBAttributes;
    USHORT WinGranularity;
    USHORT WinSize;
    USHORT WinASegment;
    USHORT WinBSegment;
    ULONG WinFuncPtr;
    USHORT BytesPerScanLine;
    /* Mandatory information for VBE 1.2 and above */
    USHORT XResolution;
    USHORT YResolution;
    UCHAR XCharSize;
    UCHAR YCharSize;
    UCHAR NumberOfPlanes;
    UCHAR BitsPerPixel;
    UCHAR NumberOfBanks;
    UCHAR MemoryModel;
    UCHAR BankSize;
    UCHAR NumberOfImagePages;
    UCHAR Reserved1;
    /* Direct Color fields (required for Direct/6 and YUV/7 memory models) */
    UCHAR RedMaskSize;
    UCHAR RedFieldPosition;
    UCHAR GreenMaskSize;
    UCHAR GreenFieldPosition;
    UCHAR BlueMaskSize;
    UCHAR BlueFieldPosition;
    UCHAR ReservedMaskSize;
    UCHAR ReservedFieldPosition;
    UCHAR DirectColorModeInfo;
    /* Mandatory information for VBE 2.0 and above */
    ULONG PhysBasePtr;
    ULONG Reserved2;
    USHORT Reserved3;
    /* Mandatory information for VBE 3.0 and above */
    USHORT LinBytesPerScanLine;
    UCHAR BnkNumberOfImagePages;
    UCHAR LinNumberOfImagePages;
    UCHAR LinRedMaskSize;
    UCHAR LinRedFieldPosition;
    UCHAR LinGreenMaskSize;
    UCHAR LinGreenFieldPosition;
    UCHAR LinBlueMaskSize;
    UCHAR LinBlueFieldPosition;
    UCHAR LinReservedMaskSize;
    UCHAR LinReservedFieldPosition;
    ULONG MaxPixelClock;
    CHAR Reserved4[189];
};

#pragma pack(pop)

extern "C"
{
    NTHALAPI BOOLEAN x86BiosCall(ULONG interrupt, BiosRegisters* regs);

    NTHALAPI NTSTATUS x86BiosAllocateBuffer(ULONG* len, USHORT* segment, USHORT* offset);
    NTHALAPI NTSTATUS x86BiosFreeBuffer(USHORT segment, USHORT offset);

    NTHALAPI NTSTATUS x86BiosReadMemory(USHORT segment, USHORT offset, void* ptr, ULONG size);
    NTHALAPI NTSTATUS x86BiosWriteMemory(USHORT segment, USHORT offset, const void* ptr, ULONG size);
}

class BiosMemory
{
public:
    BiosMemory(ULONG len)
        : m_length(len)
        , m_segment(0)
        , m_offset(0)
    {
    }

    ~BiosMemory()
    {
        if (m_segment != 0)
        {
            NTSTATUS status = x86BiosFreeBuffer(m_segment, m_offset);
            if (!NT_SUCCESS(status))
            {
                //"Failed to free bios buffer, status = 0x%lx", status;
            }
        }
    }

    NTSTATUS Init()
    {
        return x86BiosAllocateBuffer(&m_length, &m_segment, &m_offset);
    }

    USHORT Segment() const
    {
        return m_segment;
    }

    USHORT Offset() const
    {
        return m_offset;
    }

private:
    ULONG m_length;
    USHORT m_segment;
    USHORT m_offset;
};

bool Isx86BiosFunctionsAvailable()
{
    BiosMemory memory(1);
    NTSTATUS status = memory.Init();
    if (!NT_SUCCESS(status))
    {
        return false;
    }

    return true;
}

NTSTATUS GetVideoMemorySize(unsigned long& size)
{
    BiosMemory memory(sizeof(VesaVbeInfo));
    NTSTATUS status = memory.Init();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    static const char signature[4] = {'V', 'B', 'E', '2'}; //VESA?

    status = x86BiosWriteMemory(memory.Segment(), memory.Offset(), signature, sizeof(signature));
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    BiosRegisters regs = {};
    regs.es = memory.Segment();
    regs.edi = memory.Offset();
    regs.eax = 0x4F00;
    if (!x86BiosCall(0x10, &regs))
    {
        return STATUS_UNSUCCESSFUL;
    }

    VesaVbeInfo info = {};
    status = x86BiosReadMemory(memory.Segment(), memory.Offset(), &info, sizeof(VesaVbeInfo));
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    size = static_cast<unsigned long>(info.TotalMemory) * 64 * 1024; //TotalMemory is count of 64k blocks
    return STATUS_SUCCESS;
}

NTSTATUS SetModeBIOS(ULONG mode)
{
    ULONG enableLinearFramebufferMode = 0x4000;
    BiosRegisters regs = {0};
    regs.eax = VESAGraphicsModeFunction::SetVideoModeFunction;
    regs.ebx = mode | enableLinearFramebufferMode;

    if(!x86BiosCall(0x10, &regs))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS GetVideoModeInfoFromBIOS(
    ScopedArray<VIDEO_MODE_INFORMATION>& videoModeInfo, 
    ScopedArray<USHORT>& modeNumbers, 
    USHORT& modeCount,
    USHORT bitsPerPixel)
{
    BiosMemory biosMemory(sizeof(VBE_MODEINFO));
    NTSTATUS status = biosMemory.Init();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    static const char signature[4] = {'V', 'B', 'E', '2'}; //VESA?
    status = x86BiosWriteMemory(
        biosMemory.Segment(), 
        biosMemory.Offset(), 
        signature, 
        sizeof(signature));
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    BiosRegisters regs = {};
    regs.es = biosMemory.Segment();
    regs.edi = biosMemory.Offset();
    regs.eax = VESAGraphicsModeFunction::GetVESAInfoFunction;
    if (!x86BiosCall(0x10, &regs))
    {
        return STATUS_UNSUCCESSFUL;
    }

    VesaVbeInfo vbeInfo = {0};
    status = x86BiosReadMemory(
        biosMemory.Segment(), 
        biosMemory.Offset(), 
        &vbeInfo, 
        sizeof(VesaVbeInfo));
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    if (!RtlEqualMemory(vbeInfo.Signature, "VESA", 4))
    {
        return STATUS_UNSUCCESSFUL;
    }

    modeCount = 0;
    for (USHORT count = 0; ; count++)
    {
        USHORT modeTemp;
        /* Read the VBE mode number. */
        status = x86BiosReadMemory (
            HIWORD(vbeInfo.VideoModePtr),
            LOWORD(vbeInfo.VideoModePtr) + (count << 1),
            &modeTemp,
            sizeof(modeTemp));

        if (!NT_SUCCESS (status))
        {
            break;
        }

        /* End of list? */
        if (modeTemp == 0xFFFF || modeTemp == 0)
        {
            modeCount = count;
            break;
        }
    }

    videoModeInfo.Reset(new (PagedPool) VIDEO_MODE_INFORMATION[modeCount]);
    modeNumbers.Reset(new (PagedPool) USHORT[modeCount]);

    auto videoModeInfoCache = videoModeInfo.Get();
    auto modeNumbersCache = modeNumbers.Get();
    USHORT suitableModeCount = 0;
    const unsigned minW = g_defResolutions[0].width;
    const unsigned minH = g_defResolutions[0].height;

    for(USHORT currentMode = 0; currentMode < modeCount; ++currentMode)
    {
        USHORT modeTemp;
        status = x86BiosReadMemory (
            HIWORD(vbeInfo.VideoModePtr),
            LOWORD(vbeInfo.VideoModePtr) + (currentMode << 1),
            &modeTemp,
            sizeof(modeTemp));

        if(!NT_SUCCESS(status))
        {
            return status;
        }

        BiosRegisters regs = {0};
        regs.eax = VESAGraphicsModeFunction::GetVESAModeInfoFunction;
        regs.ecx = modeTemp;
        regs.edi = biosMemory.Offset() + sizeof (vbeInfo);
        regs.es = biosMemory.Segment();
        if (!x86BiosCall (0x10, &regs))
        {
            return STATUS_UNSUCCESSFUL;
        }

        VBE_MODEINFO tmpModeInfo = {0};
        status = x86BiosReadMemory (
            biosMemory.Segment(),
            biosMemory.Offset() + sizeof (vbeInfo),
            &tmpModeInfo,
            sizeof(VBE_MODEINFO));

        if (tmpModeInfo.XResolution >= minW &&
            tmpModeInfo.YResolution >= minH &&
            tmpModeInfo.BitsPerPixel == bitsPerPixel &&
            tmpModeInfo.PhysBasePtr != 0)
        {
            modeNumbersCache[suitableModeCount] = modeTemp;
            VIDEO_MODE_INFORMATION* pMode = &videoModeInfoCache[suitableModeCount];
            pMode->VisScreenWidth = tmpModeInfo.XResolution;
            pMode->VisScreenHeight = tmpModeInfo.YResolution;
            ++suitableModeCount;
        }
    }

    if (suitableModeCount == 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    modeCount = suitableModeCount;

    return STATUS_SUCCESS;
}

