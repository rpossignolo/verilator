// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// Interface ARRAY boundary port, flattened per element (constant index).
interface bus_ifs (input clk);
  logic [7:0] data;
  modport sink(input clk, input data);
  modport source(input clk, output data);
endinterface

module sub_arr (bus_ifs.sink in[2], bus_ifs.source out[2]);
  /*verilator hier_block*/
  assign out[0].data = in[0].data;
  assign out[1].data = in[1].data;
endmodule

module t;
  logic clk = 0;
  always #5 clk = ~clk;

  logic [7:0] stim = 0;
  int count = 0;

  bus_ifs in_ifs[2] (.clk(clk));
  bus_ifs out_ifs[2] (.clk(clk));
  assign in_ifs[0].data = stim;
  assign in_ifs[1].data = stim + 8'd100;

  sub_arr i_sub (.in(in_ifs), .out(out_ifs));

  always @(posedge clk) begin
    stim <= stim + 1;
    if (count > 2 && (out_ifs[0].data != in_ifs[0].data
                      || out_ifs[1].data != in_ifs[1].data)) begin
      $write("%%Error: array mismatch\n");
      $stop;
    end
    count <= count + 1;
    if (count == 20) begin
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
