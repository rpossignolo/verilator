// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// hier_block boundary interface with an ARRAY member accessed by index inside a generate
// loop (port.member[i]) -- the child must rewrite these indexed body references too.
interface bus_ifs (input clk);
  logic [7:0] data [0:3];
  modport sender(input clk, output data);
  modport receiver(input clk, input data);
endinterface

module sub1 (bus_ifs.receiver in, bus_ifs.sender out);
  /*verilator hier_block*/
  for (genvar i = 0; i < 4; i++) begin : g
    assign out.data[i] = in.data[i];
  end
endmodule

module t;
  logic clk = 0;
  always #5 clk = ~clk;
  logic [7:0] stim = 0;
  int count = 0;

  bus_ifs in_ifs (.clk(clk));
  bus_ifs out_ifs (.clk(clk));
  for (genvar i = 0; i < 4; i++) assign in_ifs.data[i] = stim + 8'(i);

  sub1 i_sub1 (.in(in_ifs), .out(out_ifs));

  always @(posedge clk) begin
    stim <= stim + 1;
    if (count > 2 && (out_ifs.data[0] != in_ifs.data[0]
                      || out_ifs.data[3] != in_ifs.data[3])) begin
      $write("%%Error: mismatch\n");
      $stop;
    end
    count <= count + 1;
    if (count == 20) begin
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
