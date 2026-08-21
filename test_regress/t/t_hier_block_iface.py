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

# The block boundary interface port is flattened to plain member ports
test.file_grep(test.obj_dir + "/Vsub1/sub1.sv", r'^module\s+(\S+)\s+', "sub1")
test.file_grep(test.obj_dir + "/Vsub1/sub1.sv", r'(in_ifm_data)')
test.file_grep(test.obj_dir + "/Vsub1/sub1.sv", r'(out_ifm_data)')

test.passes()
