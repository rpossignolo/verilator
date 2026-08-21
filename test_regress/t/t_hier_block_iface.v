// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

interface byte_ifs (
    input clk
);
  logic [7:0] data;
  modport sender(input clk, output data);
  modport receiver(input clk, input data);
endinterface

module sub1 (
    byte_ifs.receiver in,
    byte_ifs.sender out
);
  /*verilator hier_block*/
  assign out.data = in.data;
endmodule

module t;
  logic clk = 0;
  always #5 clk = ~clk;

  logic [7:0] stim = 0;
  int count = 0;

  byte_ifs in_ifs (.clk(clk));
  byte_ifs out_ifs (.clk(clk));
  assign in_ifs.data = stim;

  sub1 i_sub1 (
      .in(in_ifs),
      .out(out_ifs)
  );

  always @(posedge clk) begin
    stim <= stim + 1;
    if (out_ifs.data != in_ifs.data) begin
      $write("%%Error: mismatch out=%0d in=%0d\n", out_ifs.data, in_ifs.data);
      $stop;
    end
    count <= count + 1;
    if (count == 20) begin
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
