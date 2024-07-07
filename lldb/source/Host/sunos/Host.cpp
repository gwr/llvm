//===-- source/Host/linux/Host.cpp ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
// #include <optional>

#include "llvm/TargetParser/Host.h"

#include "lldb/Utility/LLDBLog.h"
#include "lldb/Utility/Log.h"
#include "lldb/Utility/ProcessInfo.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/StreamString.h"

#include "lldb/Host/Host.h"
#include "lldb/Host/HostInfo.h"

// Oof, regset defines exposed...
// breaks compile if this is earlier
#include <procfs.h>

extern "C" {
extern char **environ;
}

using namespace lldb;
using namespace lldb_private;

namespace {
enum class ProcessState {
  Unknown  = 0,
  Idle     = 'I',
  OnCPU    = 'O',
  Running  = 'R',
  Sleeping = 'S',
  TracedOrStopped = 'T',
  Zombie   = 'Z',
};
}

namespace lldb_private {
class ProcessLaunchInfo;
}

static bool IsDirNumeric(const char *dname) {
  for (; *dname; dname++) {
    if (!isdigit(*dname))
      return false;
  }
  return true;
}


// return true on success
static bool GetProcessAndStateInfo(::pid_t pid,
				  ProcessInstanceInfo &ProcessInfo,
                                  ProcessState &State) {
  std::string fname;
  psinfo_t pr;
  int fd;
  int sz;

  ProcessInfo.Clear();

  fname = "/proc/" + std::to_string(pid) + "psinfo";
  fd = open(fname.c_str(), O_RDONLY, 0);
  if (fd == -1)
    return false;

  sz = read(fd, &pr, sizeof (pr));
  close(fd);
  if (sz != sizeof (pr))
    return false;

  // OK, have pr.pr_kitchensink
  if (pr.pr_pid != pid)
    return false;
  pr.pr_psargs[PRARGSZ-1] = '\0';

  ProcessInfo.SetProcessID(pr.pr_pid);
  ProcessInfo.SetParentProcessID(pr.pr_ppid);

  ProcessInfo.SetUserID(pr.pr_uid);
  ProcessInfo.SetEffectiveUserID(pr.pr_euid);

  ProcessInfo.SetGroupID(pr.pr_gid);
  ProcessInfo.SetEffectiveGroupID(pr.pr_egid);

  std::string xfile(pr.pr_fname);
  ProcessInfo.GetExecutableFile().SetFile(xfile, FileSpec::Style::native);

  std::string args(pr.pr_psargs);
  ProcessInfo.GetArguments().AppendArgument(args);

  State = (ProcessState) pr.pr_lwp.pr_sname;

  return true;
}

uint32_t Host::FindProcessesImpl(const ProcessInstanceInfoMatch &match_info,
                                 ProcessInstanceInfoList &process_infos) {
  static const char procdir[] = "/proc/";

  DIR *dirproc = opendir(procdir);
  if (dirproc) {
    struct dirent *direntry = nullptr;
    const uid_t our_uid = getuid();
    const lldb::pid_t our_pid = getpid();
    bool all_users = match_info.GetMatchAllUsers();

    while ((direntry = readdir(dirproc)) != nullptr) {
      if (!IsDirNumeric(direntry->d_name))
        continue;

      lldb::pid_t pid = atoi(direntry->d_name);
      if (pid == 0)
	continue;

      // Skip this process.
      if (pid == our_pid)
        continue;

      ProcessState State;
      ProcessInstanceInfo process_info;

      if (!GetProcessAndStateInfo(pid, process_info, State))
        continue;

      if (State == ProcessState::Zombie)
        continue;

      // Check for user match if we're not matching all users and not running
      // as root.
      if (!all_users && (our_uid != 0) && (process_info.GetUserID() != our_uid))
        continue;

      if (match_info.Matches(process_info)) {
        process_infos.push_back(process_info);
      }
    }

    closedir(dirproc);
  }

  return process_infos.size();
}


bool Host::GetProcessInfo(lldb::pid_t pid, ProcessInstanceInfo &process_info) {
  ProcessState State;
  return GetProcessAndStateInfo(pid, process_info, State);
}

Environment Host::GetEnvironment() { return Environment(environ); }

Status Host::ShellExpandArguments(ProcessLaunchInfo &launch_info) {
  return Status("unimplemented");
}
