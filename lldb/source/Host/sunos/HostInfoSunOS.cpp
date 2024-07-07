//===-- HostInfoFreeBSD.cpp -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "lldb/Host/sunos/HostInfoSunOS.h"
#include "lldb/Utility/ProcessInfo.h"
#include "lldb/Host/Host.h"
#include "lldb/Host/HostInfo.h"

#include <cstdio>
#include <cstring>
#include <optional>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

using namespace lldb_private;

llvm::VersionTuple HostInfoSunOS::GetOSVersion() {
  struct utsname un;

  ::memset(&un, 0, sizeof(utsname));
  if (uname(&un) < 0)
    return llvm::VersionTuple();

  unsigned major, minor;
  if (2 == sscanf(un.release, "%u.%u", &major, &minor))
    return llvm::VersionTuple(major, minor);
  return llvm::VersionTuple();
}

std::optional<std::string> HostInfoSunOS::GetOSBuildString() {
  struct utsname un;

  ::memset(&un, 0, sizeof(utsname));
  if (uname(&un) < 0)
    return std::nullopt;

  return std::string(un.version);
}

FileSpec HostInfoSunOS::GetProgramFileSpec() {
  static FileSpec g_program_filespec;
  if (!g_program_filespec) {
    ::pid_t pid = getpid();
    ProcessInstanceInfo pinfo;
    if (Host::GetProcessInfo(pid, pinfo)) {
      g_program_filespec = pinfo.GetExecutableFile();
    }
  }
  return g_program_filespec;
}
