// Mimic trace bit-reconstruction: many traced nets are slices/ops over a few
// shared wide combinational nets that verilator inlines away.
module recon #(parameter M=128) (
  input  wire [63:0] x, y, z,
  input  wire        clk,
  output reg  [63:0] o
);
  wire [63:0] base = (x ^ y) & (y | z);   // shared wide net
  wire [63:0] base2 = (x + y) - (z << 1);

  logic [63:0] taps [M];
  always_comb begin
    for (int i = 0; i < M; i++)
      taps[i] = base ^ (base2 >> (i % 7)) ^ {base[31:0], base2[63:32]};
  end

  always @(posedge clk) o <= taps[0] ^ taps[M-1];
endmodule
