// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// Two independent hier_blocks. Each block's Verilation should depend only on the files
// that feed it, and the top's only on shared files plus each block's boundary.
module t;
  logic clk = 0;
  always #5 clk = ~clk;
  logic [7:0] oa, ob;
  blka ia (.clk(clk), .o(oa));
  blkb ib (.clk(clk), .o(ob));
  int cyc = 0;
  always @(posedge clk) begin
    cyc <= cyc + 1;
    if (cyc == 6) begin
      if (oa == 0 || ob == 0) $stop;
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
