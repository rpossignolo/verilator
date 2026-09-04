// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// A boundary interface handed over as a PLAIN HANDLE (no modport), forwarded on to a
// submodule that also takes it as a plain handle. With no modport to read directions
// from, each member's boundary direction follows from whether the block drives it.
interface bus_if;
  logic [7:0] req;     // driven inside the block (through a submodule)
  logic [7:0] rsp;     // driven outside, read inside
  logic [7:0] tag;     // driven inside the block directly
  logic [7:0] unused;  // touched by neither side
  logic [3:0] vec [1:0];  // driven inside, element at a time
endinterface
module leaf (bus_if p);
  assign p.req = p.rsp + 8'h11;
endmodule
module sub (bus_if p, input [7:0] k);
  /*verilator hier_block*/
  leaf u (.p(p));
  assign p.tag = p.rsp ^ k;
  // Indexed member write: the select binds tighter than the dot
  for (genvar i = 0; i < 2; i++) assign p.vec[i] = k[3:0] + i[3:0];
endmodule
module t;
  logic clk = 0; always #5 clk = ~clk;
  bus_if bus();
  logic [7:0] k = 8'h0F;
  sub i_sub (.p(bus), .k(k));
  initial bus.rsp = 8'h22;
  int n = 0;
  always @(posedge clk) begin
    n++;
    if (n == 2) begin
      if (bus.req !== 8'h33) begin $display("FAIL req=%h", bus.req); $stop; end
      if (bus.tag !== 8'h2D) begin $display("FAIL tag=%h", bus.tag); $stop; end
      if (bus.vec[0] !== 4'hF || bus.vec[1] !== 4'h0) begin $display("FAIL vec"); $stop; end
      bus.rsp <= 8'h40;
    end
    if (n == 4) begin
      if (bus.req !== 8'h51) begin $display("FAIL req2=%h", bus.req); $stop; end
      $write("*-* All Finished *-*\n"); $finish;
    end
  end
endmodule
