// DESCRIPTION: Verilator: Verilog Test module
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense
module deepa (input clk, output logic [7:0] o);
  logic [7:0] a = 0;
  always @(posedge clk) a <= a + 8'd1;
  assign o = a;
endmodule
module blka (input clk, output logic [7:0] o);
  /*verilator hier_block*/
  deepa u (.clk(clk), .o(o));
endmodule
