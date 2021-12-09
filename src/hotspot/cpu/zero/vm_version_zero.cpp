/*
 * Copyright (c) 1997, 2021, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2009 Red Hat, Inc.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#include "precompiled.hpp"
#include "asm/assembler.inline.hpp"
#include "memory/resourceArea.hpp"
#include "runtime/arguments.hpp"
#include "runtime/globals_extension.hpp"
#include "runtime/java.hpp"
#include "runtime/stubCodeGenerator.hpp"
#include "runtime/vm_version.hpp"


void VM_Version::initialize() {
  // This machine does not allow unaligned memory accesses
  if (! FLAG_IS_DEFAULT(UseUnalignedAccesses)) {
    warning("Unaligned memory access is not available on this CPU");
    FLAG_SET_DEFAULT(UseUnalignedAccesses, false);
  }
  // Disable prefetching for Zero
  if (! FLAG_IS_DEFAULT(AllocatePrefetchDistance)) {
    warning("Prefetching is not available for a Zero VM");
  }
  FLAG_SET_DEFAULT(AllocatePrefetchDistance, 0);

  // If lock diagnostics is needed, always call to runtime
  if (DiagnoseSyncOnValueBasedClasses != 0) {
    FLAG_SET_DEFAULT(UseHeavyMonitors, true);
  }

  if (UseAESIntrinsics) {
    warning("AES intrinsics are not available on this CPU");
    FLAG_SET_DEFAULT(UseAESIntrinsics, false);
  }

  if (UseAES) {
    warning("AES instructions are not available on this CPU");
    FLAG_SET_DEFAULT(UseAES, false);
  }

  if (UseAESCTRIntrinsics) {
    warning("AES/CTR intrinsics are not available on this CPU");
    FLAG_SET_DEFAULT(UseAESCTRIntrinsics, false);
  }

  if (UseFMA) {
    warning("FMA instructions are not available on this CPU");
    FLAG_SET_DEFAULT(UseFMA, false);
  }

  if (UseMD5Intrinsics) {
    warning("MD5 intrinsics are not available on this CPU");
    FLAG_SET_DEFAULT(UseMD5Intrinsics, false);
  }

  if (UseSHA) {
    warning("SHA instructions are not available on this CPU");
    FLAG_SET_DEFAULT(UseSHA, false);
  }

  if (UseSHA1Intrinsics) {
    warning("Intrinsics for SHA-1 crypto hash functions not available on this CPU.");
    FLAG_SET_DEFAULT(UseSHA1Intrinsics, false);
  }

  if (UseSHA256Intrinsics) {
    warning("Intrinsics for SHA-224 and SHA-256 crypto hash functions not available on this CPU.");
    FLAG_SET_DEFAULT(UseSHA256Intrinsics, false);
  }

  if (UseSHA512Intrinsics) {
    warning("Intrinsics for SHA-384 and SHA-512 crypto hash functions not available on this CPU.");
    FLAG_SET_DEFAULT(UseSHA512Intrinsics, false);
  }

  if (UseSHA3Intrinsics) {
    warning("Intrinsics for SHA3-224, SHA3-256, SHA3-384 and SHA3-512 crypto hash functions not available on this CPU.");
    FLAG_SET_DEFAULT(UseSHA3Intrinsics, false);
  }

  if (UseCRC32Intrinsics) {
    warning("CRC32 intrinsics are not available on this CPU");
    FLAG_SET_DEFAULT(UseCRC32Intrinsics, false);
  }

  if (UseAdler32Intrinsics) {
    warning("Adler32 intrinsics are not available on this CPU");
    FLAG_SET_DEFAULT(UseAdler32Intrinsics, false);
  }

  if (UseVectorizedMismatchIntrinsic) {
    warning("vectorizedMismatch intrinsic is not available on this CPU.");
    FLAG_SET_DEFAULT(UseVectorizedMismatchIntrinsic, false);
  }

  // Not implemented
  UNSUPPORTED_OPTION(UseCompiler);
#ifdef ASSERT
  UNSUPPORTED_OPTION(CountCompiledCalls);
#endif
}

// VM_Version statics
static const size_t      CPU_TYPE_DESC_BUF_SIZE = 256;
static const size_t      CPU_DETAILED_DESC_BUF_SIZE = 4096;

static int _no_of_threads = 0;
static int _no_of_cores = 0;
static int _no_of_sockets = 0;
static bool _initialized = false;
static char _cpu_name[CPU_TYPE_DESC_BUF_SIZE] = {0};
static char _cpu_desc[CPU_DETAILED_DESC_BUF_SIZE] = {0};

void VM_Version::initialize_cpu_information(void) {
  // do nothing if cpu info has been initialized
  if (_initialized) {
    return;
  }

  _no_of_cores  = os::processor_count();
  _no_of_threads = _no_of_cores;
  _no_of_sockets = _no_of_cores;
  snprintf(_cpu_name, CPU_TYPE_DESC_BUF_SIZE - 1, "Zero VM");
  snprintf(_cpu_desc, CPU_DETAILED_DESC_BUF_SIZE, "%s", _features_string);
  _initialized = true;
}

int VM_Version::number_of_threads(void) {
  initialize_cpu_information();
  return _no_of_threads;
}

int VM_Version::number_of_cores(void) {
  initialize_cpu_information();
  return _no_of_cores;
}

int VM_Version::number_of_sockets(void) {
  initialize_cpu_information();
  return _no_of_sockets;
}

const char* VM_Version::cpu_name(void) {
  initialize_cpu_information();
  char* tmp = NEW_C_HEAP_ARRAY_RETURN_NULL(char, CPU_TYPE_DESC_BUF_SIZE, mtTracing);
  if (NULL == tmp) {
    return NULL;
  }
  strncpy(tmp, _cpu_name, CPU_TYPE_DESC_BUF_SIZE);
  return tmp;
}

const char* VM_Version::cpu_description(void) {
  initialize_cpu_information();
  char* tmp = NEW_C_HEAP_ARRAY_RETURN_NULL(char, CPU_DETAILED_DESC_BUF_SIZE, mtTracing);
  if (NULL == tmp) {
    return NULL;
  }
  strncpy(tmp, _cpu_desc, CPU_DETAILED_DESC_BUF_SIZE);
  return tmp;
}
