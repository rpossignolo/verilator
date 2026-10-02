// DESCRIPTION: Verilator: Verilog Test module
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of either the GNU Lesser General Public License Version 3
// or the Perl Artistic License Version 2.0.
// SPDX-FileCopyrightText: 2026 Wilson Snyder
// SPDX-License-Identifier: LGPL-3.0-only OR Artistic-2.0

// verilog_format: off
`define stop $stop
`define check(got,exp) do if ((got) !== (exp)) begin $write("%%Error: %s:%0d: time=%t got='h%x exp='h%x\n", `__FILE__,`__LINE__, $time, (got), (exp)); `stop; end while(0)
// verilog_format: on

module sub (output logic hclk);
  int n = 0;
  initial hclk = 0;
  always @(posedge hclk) ++n;
endmodule

module t;

  // Each clock below reaches its trigger through a different kind of writer
  logic clk = 0;
  always #5 clk = ~clk;

  logic en = 0;
  wire gclk = clk & en;

  logic div = 0;
  always @(posedge clk) div <= ~div;

  logic tclk = 0;
  task automatic toggle(ref logic x);
    x = ~x;
  endtask
  always @(negedge clk) toggle(tclk);

  logic fclk = 0;
  initial
    fork
      forever #7 fclk = ~fclk;
    join_none

  bit aclk;
  assign #4 aclk = ~aclk;

  sub u (.hclk());
  initial forever #3 t.u.hclk = ~t.u.hclk;

  int n_g = 0;
  int n_div = 0;
  int n_t = 0;
  int n_f = 0;
  int n_a = 0;
  always @(posedge gclk) ++n_g;
  always @(posedge div) ++n_div;
  always @(posedge tclk) ++n_t;
  always @(posedge fclk) ++n_f;
  always @(posedge aclk) ++n_a;

  initial begin
    #52 en = 1;
    #146;
    `check(n_g, 15);
    `check(n_div, 10);
    `check(n_t, 10);
    `check(n_f, 14);
    `check(n_a, 25);
    `check(u.n, 33);
    $write("*-* All Finished *-*\n");
    $finish;
  end

endmodule
