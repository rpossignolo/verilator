// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// Forwarded boundary interfaces whose member widths come from the interface's own
// localparam (itself off a package parameter) and from an interface parameter.
package ax_pkg;
  parameter int DATA_WIDTH = 64;
endpackage

interface ax_if;
  import ax_pkg::*;
  localparam int STRB_WIDTH = DATA_WIDTH / 8;
  logic [STRB_WIDTH-1:0] wstrb;
  logic [DATA_WIDTH-1:0] wdata;
  modport target(input wstrb, output wdata);
endinterface

interface pc_if #(
    parameter int PSTATE_WIDTH = 5
);
  logic [PSTATE_WIDTH-1:0] pstate;
  logic ack;
  modport target(input pstate, output ack);
endinterface

module leaf (ax_if.target a, pc_if.target p);
  assign a.wdata = {56'h0, a.wstrb};
  assign p.ack = (p.pstate == 5'h1B);
endmodule

module sub (ax_if.target a, pc_if.target p);
  /*verilator hier_block*/
  leaf u (.a(a), .p(p));
endmodule

module t;
  logic clk = 0;
  always #5 clk = ~clk;
  ax_if a ();
  pc_if p ();
  sub i_sub (.a(a), .p(p));

  int cyc = 0;
  initial begin
    a.wstrb = 8'hA5;
    p.pstate = 5'h1B;
  end
  always @(posedge clk) begin
    cyc <= cyc + 1;
    if (cyc == 4) begin
      if ($bits(a.wstrb) !== 8) $stop;
      if (a.wdata !== 64'hA5) $stop;
      if (p.ack !== 1'b1) $stop;
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
