// S3LAB
// vim: ts=2:sw=2:expandtab
#ifndef SIDECAR_DEVICE_H
#define SIDECAR_DEVICE_H

#include "../../../../sidecar-driver/x86-64/ptw.h"

namespace __sidecar {

extern bool sidecar_opened;

void SidecarDeviceInit(bool fake = false);
void SidecarDeviceEnable();
void SidecarDeviceDisable();
void SidecarSetBase(struct dso_info *info);

} // namespace __sidecar

#endif
