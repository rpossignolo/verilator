#!/usr/bin/env python3
# DESCRIPTION: Verilator: Verilog Test driver/expect definition
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of either the GNU Lesser General Public License Version 3
# or the Perl Artistic License Version 2.0.
# SPDX-FileCopyrightText: 2026 Wilson Snyder
# SPDX-License-Identifier: LGPL-3.0-only OR Artistic-2.0

import vltest_bootstrap

test.scenarios('vlt')

# 'mem' is the target of N*C = 64 NBAs, so at this threshold the commit queue
# replaces the 64 per-site commit flags.
test.compile(verilator_flags2=["--stats", "-Wno-MULTIDRIVEN", "--nba-queue-sites 64"])

test.file_grep(test.stats, r'NBA, variables using ValueQueuePartial scheme\s+(\d+)', 1)
test.file_grep(test.stats, r'NBA, variables using FlagShared scheme\s+(\d+)', 0)

test.execute()

test.passes()
