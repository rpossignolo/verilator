// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// A module inside a hier_block names a path rooted at the block instance. That reference is
// resolved wholly within the block's own Verilation, so the boundary check must allow it.
module deep (input clk, output logic [7:0] o);
  logic [7:0] acc = 0;
  always @(posedge clk) acc <= acc + 1;
  assign o = acc;
endmodule
module inner (input clk, output logic [7:0] o);
  deep u (.clk(clk), .o(o));
  // Both forms are rooted at the block instance and stay inside it
  always @(posedge clk) begin
    if (sub.inner_i.u.acc == 8'hFF) $display("wrap");   // scope traversal into the block
    if (sub.internal_q !== 8'h5A) $stop;                // non-port symbol of the block
  end
endmodule
module sub (input clk, output logic [7:0] o);
  /*verilator hier_block*/
  logic [7:0] internal_q = 8'h5A;  // Non-port symbol, named directly from inside the block
  inner inner_i (.clk(clk), .o(o));
endmodule
module t;
  logic clk = 0;
  always #5 clk = ~clk;
  logic [7:0] o;
  sub sub (.clk(clk), .o(o));
  int cyc = 0;
  always @(posedge clk) begin
    cyc <= cyc + 1;
    if (cyc == 6) begin
      if (o == 0) $stop;
      $write("*-* All Finished *-*\n"); $finish;
    end
  end
endmodule
