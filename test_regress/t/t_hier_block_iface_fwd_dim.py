#!/usr/bin/env python3
# DESCRIPTION: Verilator: Verilog Test driver/expect definition
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of either the GNU Lesser General Public License Version 3
# or the Perl Artistic License Version 2.0.
# SPDX-FileCopyrightText: 2024 Wilson Snyder
# SPDX-License-Identifier: LGPL-3.0-only OR Artistic-2.0

import vltest_bootstrap

test.priority(30)
test.scenarios('vlt')

# Libs of hierarchical blocks are cached; force a clean generation.
test.clean_objs()

test.compile(verilator_flags2=[
    '--hierarchical', '--binary', '--timing', '--Wno-TIMESCALEMOD'
])

test.execute()

# Member widths off interface parameters are folded into the boundary port
test.file_grep(test.obj_dir + "/Vsub/sub.sv", r'input logic \[7:0\]  a_ifm_wstrb')

test.passes()
