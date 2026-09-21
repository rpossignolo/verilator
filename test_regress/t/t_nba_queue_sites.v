// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain.
// SPDX-FileCopyrightText: 2026 Wilson Snyder
// SPDX-License-Identifier: CC0-1.0

// A behavioural memory with a per-entry gated write clock, so every element of
// 'mem' is the target of its own non-blocking assignment in its own sensitivity
// domain. N*C = 64 NBA sites target 'mem'.
module t (
    input clk
);

  localparam int N = 32;
  localparam int C = 2;

  int cyc = 0;
  logic [N-1:0] sel_q;
  logic [N-1:0] wclk;
  logic [C-1:0][31:0] wdata;
  logic [C-1:0][31:0] mem[N-1:0];

  // Select is updated on the opposite phase so the gate is stable at posedge.
  always_ff @(negedge clk) begin
    sel_q <= '0;
    if (cyc >= 1 && cyc <= N) sel_q[cyc-1] <= 1'b1;
  end

  for (genvar i = 0; i < N; ++i) begin : gen_gate
    assign wclk[i] = clk & sel_q[i];
  end

  always_comb for (int c = 0; c < C; ++c) wdata[c] = 32'((cyc * 3) + 1 + c);

  for (genvar i = 0; i < N; ++i) begin : gen_ent
    always_ff @(posedge wclk[i]) begin
      for (int c = 0; c < C; ++c) mem[i][c] <= wdata[c];
    end
  end

  always @(posedge clk) begin
    cyc <= cyc + 1;
    // Entry e was written in the cycle after sel_q[e] was set, with cyc == e+1.
    if (cyc > N + 1 && cyc <= 2 * N + 1) begin
      automatic int e = cyc - N - 2;
      for (int c = 0; c < C; ++c) begin
        if (mem[e][c] !== 32'(((e + 1) * 3) + 1 + c)) begin
          $write("%%Error: mem[%0d][%0d]=%0x exp=%0x\n", e, c, mem[e][c], ((e + 1) * 3) + 1 + c);
          $stop;
        end
      end
    end
    if (cyc == 2 * N + 3) begin
      $write("*-* All Finished *-*\n");
      $finish;
    end
  end
endmodule
