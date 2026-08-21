// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// Clock-in-interface + multi-member modport + sequential logic in the block.
interface bus_ifs (input clk);
  logic       valid;
  logic [7:0] data;
  modport sink(input clk, input valid, input data);
  modport source(input clk, output valid, output data);
endinterface

module sub_seq (bus_ifs.sink in, bus_ifs.source out);
  /*verilator hier_block*/
  always_ff @(posedge in.clk) begin
    out.valid <= in.valid;
    out.data  <= in.data;
  end
endmodule

module t;
  logic clk = 0;
  always #5 clk = ~clk;

  logic [7:0] stim = 0;
  int count = 0;

  bus_ifs in_ifs (.clk(clk));
  bus_ifs out_ifs (.clk(clk));
  assign in_ifs.valid = 1'b1;
  assign in_ifs.data = stim;

  sub_seq i_sub (.in(in_ifs), .out(out_ifs));

  logic [7:0] expect_data = 0;
  always @(posedge clk) begin
    stim <= stim + 1;
    expect_data <= stim;  // one-cycle delay mirrors the block's register
    count <= count + 1;
    if (count > 2 && out_ifs.data != expect_data) begin
      $write("%%Error: seq mismatch out=%0d exp=%0d\n", out_ifs.data, expect_data);
      $stop;
    end
    if (count == 20) begin
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
