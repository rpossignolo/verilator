// Synthetic: many traced combinational nets sharing subexpressions.
module shared #(parameter N=64) (
  input  wire [31:0] a, b, c, d,
  input  wire        clk,
  output reg  [31:0] acc
);
  // Shared subexpressions across many traced combinational nets.
  wire [31:0] s1 = (a & b) ^ (c | d);        // shared core
  wire [31:0] s2 = ((a & b) ^ (c | d)) + a;  // reuses s1 core
  wire [31:0] s3 = ((a & b) ^ (c | d)) - b;  // reuses s1 core
  wire [31:0] s4 = (((a & b) ^ (c | d)) + a) & (((a & b) ^ (c | d)) - b);

  genvar i;
  generate
    for (i = 0; i < N; i = i + 1) begin : g
      wire [31:0] w = ((a & b) ^ (c | d)) + (a << (i % 5)) - (b >> (i % 3));
    end
  endgenerate

  always @(posedge clk) acc <= s1 + s2 + s3 + s4 + g[0].w + g[N-1].w;
endmodule
