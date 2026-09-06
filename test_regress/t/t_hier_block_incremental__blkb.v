// DESCRIPTION: Verilator: Verilog Test module
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense
module deepb (input clk, output logic [7:0] o);
  logic [7:0] b = 0;
  always @(posedge clk) b <= b + 8'd2;
  assign o = b;
endmodule
module blkb (input clk, output logic [7:0] o);
  /*verilator hier_block*/
  deepb u (.clk(clk), .o(o));
endmodule
