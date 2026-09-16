// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2026 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// A forwarded boundary interface ARRAY whose interface declares its own ports.
// One pin expression covers every element, so it cannot be rebuilt per element.
interface chi_if (input logic clk);
  logic [7:0] req;
  logic       ack;
  modport ccu(output req, input ack, input clk);
endinterface

module bank (chi_if.ccu p[1:0]);
  assign p[0].req = 8'h1;
  assign p[1].req = 8'h2;
endmodule

module sub (chi_if.ccu soc[1:0]);
  /*verilator hier_block*/
  bank u (.p(soc));
endmodule

module t (input logic clk);
  chi_if soc[1:0] (.clk(clk));
  sub i_sub (.soc(soc));
endmodule
