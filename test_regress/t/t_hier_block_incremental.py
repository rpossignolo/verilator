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
    '--hierarchical', '--binary', '--timing', '--Wno-TIMESCALEMOD',
    't/' + test.name + '__blka.v', 't/' + test.name + '__blkb.v'
])

test.execute()

# Each block depends on its own sources; only shared files reach every Verilation
test.file_grep(test.obj_dir + "/V" + test.name + "_hier.mk", r'VM_HIER_INPUT_FILES_Vblka')
test.file_grep(test.obj_dir + "/V" + test.name + "_hier.mk", r'VM_HIER_INPUT_FILES_SHARED')
# The top keys off each block's boundary stamp, not the regenerated wrapper
test.file_grep(test.obj_dir + "/V" + test.name + "_hier.mk", r'Vblka/blka.sv.stamp')

test.passes()
