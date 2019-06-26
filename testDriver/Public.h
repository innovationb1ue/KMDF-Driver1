/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_testDriver,
    0x2d6e4481,0x4d0e,0x4f56,0xb6,0x37,0x3b,0x42,0x85,0x00,0x1f,0xa7);
// {2d6e4481-4d0e-4f56-b637-3b4285001fa7}
