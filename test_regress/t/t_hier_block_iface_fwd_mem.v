// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// A forwarded boundary interface whose modport exposes only INTERNAL members (not the
// interface's own ports), including a package-typed one, in both directions.
package flit_pkg;
  typedef struct packed {logic [3:0] op; logic [7:0] addr;} t_flit;
endpackage

interface bus_if (input clk);
  import flit_pkg::*;
  t_flit req;
  logic [7:0] rsp;
  modport mp(input clk, input rsp, output req);
endinterface

module leaf (bus_if.mp p);
  assign p.req.op = 4'h5;
  assign p.req.addr = p.rsp + 8'h11;
endmodule

module sub (bus_if.mp p);
  /*verilator hier_block*/
  leaf u (.p(p));
endmodule

module t;
  logic clk = 0;
  always #5 clk = ~clk;
  bus_if bus (.clk(clk));
  sub i_sub (.p(bus));

  int cyc = 0;
  initial bus.rsp = 8'h22;
  always @(posedge clk) begin
    cyc <= cyc + 1;
    if (cyc == 4) begin
      if (bus.req.op !== 4'h5) $stop;
      if (bus.req.addr !== 8'h33) $stop;
      bus.rsp <= 8'h40;
    end
    if (cyc == 8) begin
      if (bus.req.addr !== 8'h51) $stop;
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
