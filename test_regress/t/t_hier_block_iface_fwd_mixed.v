// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// A boundary interface both handed whole to a submodule AND member-accessed in the block
// itself, on a PARAMETERIZED hier_block (so the block is renamed for its parameter).
interface bus_if (input clk);
  logic [7:0] req;
  logic [7:0] rsp;
  modport mp(input clk, input rsp, output req);
endinterface

module leaf (bus_if.mp p);
  assign p.req = p.rsp + 8'h11;
endmodule

module sub #(
    parameter int ADD = 3
) (
    bus_if.mp p,
    output logic [7:0] seen
);
  /*verilator hier_block*/
  leaf u (.p(p));
  // Same port, member-accessed directly rather than through the submodule pin
  assign seen = p.rsp + ADD[7:0];
endmodule

module t;
  logic clk = 0;
  always #5 clk = ~clk;
  bus_if bus (.clk(clk));
  logic [7:0] seen;
  sub #(.ADD(7)) i_sub (.p(bus), .seen(seen));

  int cyc = 0;
  initial bus.rsp = 8'h22;
  always @(posedge clk) begin
    cyc <= cyc + 1;
    if (cyc == 4) begin
      if (bus.req !== 8'h33) $stop;
      if (seen !== 8'h29) $stop;
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
