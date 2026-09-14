// DESCRIPTION: Verilator: Verilog Test module
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of either the GNU Lesser General Public License Version 3
// or the Perl Artistic License Version 2.0.
// SPDX-FileCopyrightText: 2026 Wilson Snyder
// SPDX-License-Identifier: LGPL-3.0-only OR Artistic-2.0

typedef struct {
   byte     b;
   shortint s;
   int      i;
   longint  l;
   longint  arr[4];
} all_s;

module t (/*AUTOARG*/);

   import "DPI-C" function void i_struct_input(input all_s val);
   import "DPI-C" function void i_struct_output(output all_s val);
   import "DPI-C" function void i_struct_inout(inout all_s val);
   import "DPI-C" context function void i_call_exports();

   export "DPI-C" function e_struct_input;
   export "DPI-C" function e_struct_output;

   function automatic void e_struct_input(input all_s val);
      if (val.b !== 8'sh33) $stop;
      if (val.i !== 32'sh33333333) $stop;
   endfunction

   function automatic void e_struct_output(output all_s val);
      val.b = 8'sh44;
      val.s = 16'sh4444;
      val.i = 32'sh44444444;
      val.l = 64'sh44444444_44444444;
      for (int j = 0; j < 4; j++) val.arr[j] = (longint'(j) + 40);
   endfunction

   all_s v;

   initial begin
      v.b = 8'sh12;
      v.s = 16'sh1234;
      v.i = 32'sh12345678;
      v.l = 64'sh12345678_9abcdef0;
      for (int j = 0; j < 4; j++) v.arr[j] = (longint'(j) + 1);
      i_struct_input(v);

      i_struct_output(v);
      if (v.b !== 8'sh21) $stop;
      if (v.s !== 16'sh4321) $stop;
      if (v.i !== 32'sh87654321) $stop;
      if (v.l !== 64'shf0debc9a_78563412) $stop;
      for (int j = 0; j < 4; j++) if (v.arr[j] !== (longint'(j) + 10)) $stop;

      i_struct_inout(v);
      if (v.b !== 8'sh22) $stop;
      if (v.s !== 16'sh4322) $stop;
      if (v.i !== 32'sh87654322) $stop;
      if (v.l !== 64'shf0debc9a_78563413) $stop;
      for (int j = 0; j < 4; j++) if (v.arr[j] !== (longint'(j) + 11)) $stop;

      i_call_exports();

      $write("*-* All Finished *-*\n");
      $finish;
   end
endmodule
