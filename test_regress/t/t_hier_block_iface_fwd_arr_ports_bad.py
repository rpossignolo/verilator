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

test.clean_objs()

# The error comes from the hier child's Verilation, whose log carries absolute
# make paths, so the golden documents the message rather than being diffed.
test.compile(fails='any', make_main=False, verilator_flags2=['--hierarchical'])

test.file_grep(test.compile_log_filename, r'hier_block forwarded interface array port')

test.passes()
