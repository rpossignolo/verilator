// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2024 Wilson Snyder
// SPDX-License-Identifier: Unlicense

// A boundary interface FORWARDED whole to a submodule (not member-accessed). The hier_block
// child rebuilds it as an internal interface instance so the submodule pin reconnects.
interface bus_if (input clk);
  modport mp(input clk);
endinterface

module leaf (bus_if.mp p);
  int cnt = 0;
  always @(posedge p.clk) cnt <= cnt + 1;
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
  always @(posedge clk) begin
    cyc <= cyc + 1;
    if (cyc == 20) begin
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
