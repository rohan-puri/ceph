// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2010 Sage Weil <sage@newdream.net>
 * Copyright (C) 2010 Dreamhost
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include "common/ceph_argparse.h"
#include "global/global_init.h"
#include "common/errno.h"
#include "common/safe_io.h"
#include "common/config.h"
#include "tools/common.h"

#include "include/compat.h"

#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <sys/socket.h>
#include <linux/un.h>
#include <unistd.h>
#include <string.h>

using std::vector;

static void usage()
{
  cout << "usage: ceph [options] [commands]\n";
  cout << "If no commands are specified, enter interactive mode.\n";
  cout << "Commands:\n";
  cout << "   stop              -- cleanly shut down file system\n"
       << "   (osd|pg|mds) stat -- get monitor subsystem status\n"
       << "   ...\n";
  cout << "Options:\n";
  cout << "   -i infile\n"
       << "   -o outfile\n"
       << "        specify input or output file (for certain commands)\n"
       << "   -s or --status\n"
       << "        print current system status\n"
       << "   -w or --watch\n"
       << "        watch system status changes in real time (push)\n";
  generic_client_usage(); // Will exit()
}

static void parse_cmd_args(vector<const char*> &args,
		std::string *in_file, std::string *out_file,
			   ceph_tool_mode_t *mode, bool *concise,
			   string *admin_socket, uint32_t *admin_socket_cmd)
{
  std::vector<const char*>::iterator i;
  std::string val;
  for (i = args.begin(); i != args.end(); ) {
    if (ceph_argparse_double_dash(args, i)) {
      break;
    } else if (ceph_argparse_witharg(args, i, &val, "-i", "--in-file", (char*)NULL)) {
      *in_file = val;
    } else if (ceph_argparse_witharg(args, i, &val, "-o", "--out-file", (char*)NULL)) {
      *out_file = val;
    } else if (ceph_argparse_witharg(args, i, &val, "--dump-perf-counters", (char*)NULL)) {
      *admin_socket = val;
      *admin_socket_cmd = 1;
    } else if (ceph_argparse_witharg(args, i, &val, "--dump-perf-counters-schema", (char*)NULL)) {
      *admin_socket = val;
      *admin_socket_cmd = 2;
    } else if (ceph_argparse_witharg(args, i, &val, "--admin-daemon", (char*)NULL)) {
      *admin_socket = val;
      if (i == args.end())
	usage();
      const char *start = *i;
      char *end = (char *)start;
      *admin_socket_cmd = strtol(start, &end, 10);
      if (*end != '\0')
	usage();
    } else if (ceph_argparse_flag(args, i, "-s", "--status", (char*)NULL)) {
      *mode = CEPH_TOOL_MODE_ONE_SHOT_OBSERVER;
    } else if (ceph_argparse_flag(args, i, "-w", "--watch", (char*)NULL)) {
      *mode = CEPH_TOOL_MODE_OBSERVER;
    } else if (ceph_argparse_flag(args, i, "--concise", (char*)NULL)) {
      *concise = true;
    } else if (ceph_argparse_flag(args, i, "-h", "--help", (char*)NULL)) {
      usage();
    } else {
      ++i;
    }
  }
}

static int get_indata(const char *in_file, bufferlist &indata)
{
  int fd = TEMP_FAILURE_RETRY(::open(in_file, O_RDONLY));
  if (fd < 0) {
    int err = errno;
    derr << "error opening in_file '" << in_file << "': "
	 << cpp_strerror(err) << dendl;
    return 1;
  }
  struct stat st;
  if (::fstat(fd, &st)) {
    int err = errno;
    derr << "error getting size of in_file '" << in_file << "': "
	 << cpp_strerror(err) << dendl;
    return 1;
  }

  indata.push_back(buffer::create(st.st_size));
  indata.zero();
  int ret = safe_read_exact(fd, indata.c_str(), st.st_size);
  if (ret) {
    derr << "error reading in_file '" << in_file << "': "
	 << cpp_strerror(ret) << dendl;
    TEMP_FAILURE_RETRY(::close(fd));
    return 1;
  }

  TEMP_FAILURE_RETRY(::close(fd));
  derr << "read " << st.st_size << " bytes from " << in_file << dendl;
  return 0;
}

int do_admin_socket(string path, uint32_t cmd)
{
  struct sockaddr_un address;
  int fd;
  int r;
  
  fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if(fd < 0) {
    cerr << "socket failed with " << cpp_strerror(errno) << std::endl;
    return -1;
  }

  memset(&address, 0, sizeof(struct sockaddr_un));
  address.sun_family = AF_UNIX;
  snprintf(address.sun_path, UNIX_PATH_MAX, "%s", path.c_str());

  if (connect(fd, (struct sockaddr *) &address, 
	      sizeof(struct sockaddr_un)) != 0) {
    cerr << "connect to " << path << " failed with " << cpp_strerror(errno) << std::endl;
    return -1;
  }
  
  char *buf;
  uint32_t len;
  cmd = htonl(cmd);
  r = safe_write(fd, &cmd, sizeof(cmd));
  if (r < 0) {
    cerr << "write to " << path << " failed with " << cpp_strerror(errno) << std::endl;
    goto out;
  }
  
  r = safe_read(fd, &len, sizeof(len));
  if (r < 0) {
    cerr << "read " << len << " length from " << path << " failed with " << cpp_strerror(errno) << std::endl;
    goto out;
  }
  if (r < 4) {
    cerr << "read only got " << r << " bytes of 4 expected for response length; invalid command?" << std::endl;
    goto out;
  }
  len = ntohl(len);

  buf = new char[len+1];
  r = safe_read(fd, buf, len);
  if (r < 0) {
    cerr << "read " << len << " bytes from " << path << " failed with " << cpp_strerror(errno) << std::endl;
    goto out;
  }
  buf[len] = '\0';

  cout << buf << std::endl;
  r = 0;

 out:
  ::close(fd);
  return r;
}

int main(int argc, const char **argv)
{
  std::string in_file, out_file;
  enum ceph_tool_mode_t mode = CEPH_TOOL_MODE_CLI_INPUT;
  vector<const char*> args;
  argv_to_vec(argc, argv, args);
  env_to_vec(args);

  // initialize globals
  global_init(args, CEPH_ENTITY_TYPE_CLIENT, CODE_ENVIRONMENT_UTILITY, 0);
  common_init_finish(g_ceph_context);

  // parse user input
  bool concise = false;
  string admin_socket;
  uint32_t admin_socket_cmd = 0;
  parse_cmd_args(args, &in_file, &out_file, &mode, &concise, &admin_socket, &admin_socket_cmd);

  // daemon admin socket?
  if (admin_socket.length()) {
    return do_admin_socket(admin_socket, admin_socket_cmd);
  }

  // input
  bufferlist indata;
  if (!in_file.empty()) {
    if (get_indata(in_file.c_str(), indata)) {
      derr << "failed to get data from '" << in_file << "'" << dendl;
      return 1;
    }
  }

  // output
  int out_fd;
  bool close_out_fd = false;
  if (out_file.empty() || out_file == "-") {
    out_fd = STDOUT_FILENO;
  } else {
    out_fd = TEMP_FAILURE_RETRY(::open(out_file.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644));
    if (out_fd < 0) {
      int ret = errno;
      derr << " failed to create file '" << out_file << "': "
	   << cpp_strerror(ret) << dendl;
      return 1;
    }
    close_out_fd = true;
  }

  CephToolCtx *ctx = ceph_tool_common_init(mode, concise);
  if (!ctx) {
    derr << "ceph_tool_common_init failed." << dendl;
    return 1;
  }
  signal(SIGINT, SIG_DFL);
  signal(SIGTERM, SIG_DFL);

  int ret = 0;
  switch (mode) {
    case CEPH_TOOL_MODE_ONE_SHOT_OBSERVER: // fall through
    case CEPH_TOOL_MODE_OBSERVER: {
      ctx->lock.Lock();
      send_observe_requests(ctx);
      ctx->lock.Unlock();
      break;
    }

    case CEPH_TOOL_MODE_CLI_INPUT: {
      if (args.empty()) {
	if (ceph_tool_do_cli(ctx))
	  ret = 1;
      } else {
	while (!args.empty()) {
	  vector<string> cmd;
	  for (vector<const char*>::iterator n = args.begin();
	       n != args.end(); ) {
	    std::string np(*n);
	    n = args.erase(n);
	    if (np == ";")
	      break;
	    cmd.push_back(np);
	  }

	  bufferlist obl;
	  if (do_command(ctx, cmd, indata, obl))
	    ret = 1;
	  if (obl.length()) {
	    int err = obl.write_fd(out_fd);
	    if (err) {
	      derr << " failed to write " << obl.length() << " bytes to " << out_file << ": "
		   << cpp_strerror(err) << dendl;
	      goto out;
	    }
	    if (!concise && !out_file.empty())
	      cerr << " wrote " << obl.length() << " byte payload to " << out_file << std::endl;
	  }
	}
      }
      if (ceph_tool_messenger_shutdown())
	ret = 1;
      break;
    }

    default: {
      derr << "logic error: illegal ceph command mode " << mode << dendl;
      ret = 1;
      break;
    }
  }

 out:
  if (close_out_fd)
    ::close(out_fd);
  if (ceph_tool_common_shutdown(ctx))
    ret = 1;
  return ret;
}
