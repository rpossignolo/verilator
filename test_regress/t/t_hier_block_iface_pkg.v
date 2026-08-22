// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// hier_block boundary interface with a PACKAGE-typed member: the child re-verilation
// must honor the interface's package import to resolve the member type.
package mypkg;
  typedef logic [15:0] t_data;
endpackage

interface pkg_ifs (input clk);
  import mypkg::*;
  t_data data;
  modport sender(input clk, output data);
  modport receiver(input clk, input data);
endinterface

module sub1 (pkg_ifs.receiver in, pkg_ifs.sender out);
  /*verilator hier_block*/
  assign out.data = in.data;
endmodule

module t;
  import mypkg::*;
  logic clk = 0;
  always #5 clk = ~clk;
  t_data stim = 0;
  int count = 0;

  pkg_ifs in_ifs (.clk(clk));
  pkg_ifs out_ifs (.clk(clk));
  assign in_ifs.data = stim;

  sub1 i_sub1 (.in(in_ifs), .out(out_ifs));

  always @(posedge clk) begin
    stim <= stim + 1;
    if (count > 2 && out_ifs.data != in_ifs.data) begin
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
