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

module t;

  // Many otherwise empty time steps, as from a free-running delay model
  logic noise = 0;
  always #1 noise = ~noise;

  logic clk = 0;
  always #5 clk = ~clk;

  // Written by a timing process through a task, read by continuous logic
  logic [7:0] mem[4];
  wire [31:0] packed_mem = {mem[3], mem[2], mem[1], mem[0]};
  task automatic poke(int i, logic [7:0] v);
    mem[i] = v;
  endtask

  // Written inside a task that suspends between writes
  logic [7:0] slow = 0;
  wire [7:0] slow_p1 = slow + 8'd1;
  task automatic slow_set(logic [7:0] v);
    #1 slow = v;
    #1 slow = v + 8'd1;
  endtask

  // Intra-assignment delay: the write statement itself suspends
  logic [7:0] intra = 0;
  wire [7:0] intra_x2 = intra * 8'd2;

  // Written through a ref argument of a non-suspending task
  logic [7:0] tog = 0;
  wire [7:0] tog_inv = ~tog;
  task automatic bump(ref logic [7:0] x);
    x = x + 8'd3;
  endtask

  // Written from a forked process
  logic [7:0] forked = 0;
  wire [7:0] forked_p2 = forked + 8'd2;

  // Written through a ref argument of a suspending task: keeps the process domains
  logic [7:0] rv = 0;
  wire [7:0] rv_p1 = rv + 8'd1;
  task automatic slow_ref(ref logic [7:0] x);
    /*verilator no_inline_task*/
    #1 x = 8'h07;
    #1 x = 8'h08;
  endtask

  // Non-blocking write from a timing process
  logic [7:0] nb = 0;
  wire [7:0] nb_m1 = nb - 8'd1;

  int n_changes = 0;
  always @(packed_mem) ++n_changes;

  initial begin
    mem[0] = 0; mem[1] = 0; mem[2] = 0; mem[3] = 0;
    #3;
    poke(0, 8'h11);
    poke(2, 8'h33);
    #1 `check(packed_mem, 32'h00330011);
    @(posedge clk);
    poke(1, 8'h22);
    poke(3, 8'h44);
    #0.5 `check(packed_mem, 32'h44332211);
    poke(3, 8'h44);  // same value: no change event
    #2 `check(n_changes, 3);
    slow_set(8'h50);
    #0.5 `check(slow_p1, 8'h52);
    intra = #2 8'h21;
    #1 `check(intra_x2, 8'h42);
    bump(tog);
    #1 `check(tog_inv, 8'hfc);
    fork
      begin
        #2 forked = 8'h10;
        #1 forked = 8'h20;
      end
    join_none
    #2.5 `check(forked_p2, 8'h12);
    #1 `check(forked_p2, 8'h22);
    fork
      slow_ref(rv);
    join_none
    #1.5 `check(rv_p1, 8'h08);
    #1 `check(rv_p1, 8'h09);
    nb <= 8'h80;
    #0.5 `check(nb_m1, 8'h7f);
    @(negedge clk);
    `check(packed_mem, 32'h44332211);
    `check(n_changes, 3);
    $write("*-* All Finished *-*\n");
    $finish;
  end

endmodule
