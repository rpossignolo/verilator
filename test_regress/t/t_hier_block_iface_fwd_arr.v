// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// A forwarded boundary interface ARRAY bounded by a package parameter, handed to one
// submodule whole and to another per element, with internal members only.
package chi_pkg;
  typedef struct packed {logic [3:0] op; logic [7:0] addr;} t_flit;
  parameter int NBUN = 2;
endpackage

interface chi_if
  import chi_pkg::*;
  ();
  t_flit req;
  logic ack;
  modport ccu(output req, input ack);
  modport chi(input req, output ack);
endinterface

module cdc (chi_if.chi c, chi_if.ccu s);
  assign s.req.addr = c.req.addr;
  assign c.ack = s.ack;
endmodule

module bank (chi_if.ccu p[chi_pkg::NBUN-1:0], input [3:0] o);
  for (genvar g = 0; g < chi_pkg::NBUN; g++) assign p[g].req.op = o + g[3:0];
endmodule

module sub (chi_if.ccu soc[chi_pkg::NBUN-1:0], chi_if.chi core[chi_pkg::NBUN-1:0],
            input [3:0] o);
  /*verilator hier_block*/
  bank u (.p(soc), .o(o));
  for (genvar g = 0; g < chi_pkg::NBUN; g++) cdc ucdc (.c(core[g]), .s(soc[g]));
endmodule

module t;
  import chi_pkg::*;
  logic clk = 0;
  always #5 clk = ~clk;
  chi_if soc[NBUN-1:0] ();
  chi_if core[NBUN-1:0] ();
  logic [3:0] o = 4'h3;
  sub i_sub (.soc(soc), .core(core), .o(o));

  int cyc = 0;
  initial begin
    soc[0].ack = 1'b1;
    soc[1].ack = 1'b0;
    core[0].req.addr = 8'h5A;
    core[1].req.addr = 8'hC7;
  end
  always @(posedge clk) begin
    cyc <= cyc + 1;
    if (cyc == 4) begin
      if (soc[0].req.op !== 4'h3) $stop;
      if (soc[1].req.op !== 4'h4) $stop;
      if (soc[0].req.addr !== 8'h5A) $stop;
      if (soc[1].req.addr !== 8'hC7) $stop;
      if (core[0].ack !== 1'b1) $stop;
      if (core[1].ack !== 1'b0) $stop;
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
