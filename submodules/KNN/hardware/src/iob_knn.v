`timescale 1ns/1ps
`include "iob_lib.vh"
`include "interconnect.vh"
`include "iob_knn.vh"

module iob_knn
  #(
    parameter ADDR_W = `KNN_ADDR_W, 
    parameter DATA_W = 32, 
    parameter WDATA_W = `KNN_WDATA_W 
    )
	(
	//CPU native interface	
	//cpu generic slave interface copied from
	`include "cpu_nat_s_if.v"
	//clk rst generic interface copied from
	`include "gen_if.v"
	);
	`include "KNN_Header.vh"
 	`include "KNNsw_reg.v"
	`include "KNNsw_reg_gen.v"

	`SIGNAL_OUT(KNN_VALID_OUT_TOP,1)
	`SIGNAL_OUT(KN1_OUT_TOP,`WDATA_W)
	`SIGNAL_OUT(KN2_OUT_TOP,`WDATA_W)
	`SIGNAL_OUT(KN3_OUT_TOP,`WDATA_W)
	`SIGNAL_OUT(KN4_OUT_TOP,`WDATA_W)
	`SIGNAL_OUT(KN5_OUT_TOP,`WDATA_W)
	`SIGNAL_OUT(KN6_OUT_TOP,`WDATA_W)
	
	`SIGNAL_OUT(IN1_OUT_TOP,`K_NUM_DATA_PTS_BIT)
	`SIGNAL_OUT(IN2_OUT_TOP,`K_NUM_DATA_PTS_BIT)
	`SIGNAL_OUT(IN3_OUT_TOP,`K_NUM_DATA_PTS_BIT)
	`SIGNAL_OUT(IN4_OUT_TOP,`K_NUM_DATA_PTS_BIT)
	`SIGNAL_OUT(IN5_OUT_TOP,`K_NUM_DATA_PTS_BIT)
	`SIGNAL_OUT(IN6_OUT_TOP,`K_NUM_DATA_PTS_BIT)
	
	wire rst_comb;
	assign rst_comb = rst | KNN_RESET;
	 
	knn_core  knn_core_top
	 (
	  .KNN_START_CORE    (KNN_START),
	  .KNN_DATA_PT_CORE1  (KNN_DATA_PT1), 
	  .KNN_DATA_PT_CORE2  (KNN_DATA_PT2), 
	  .KNN_DATA_PT_CORE3  (KNN_DATA_PT3), 
	  .KNN_DATA_PT_CORE4  (KNN_DATA_PT4), 
  	  .KNN_DATA_PT_CORE5  (KNN_DATA_PT5), 
	  .KNN_DATA_PT_CORE6  (KNN_DATA_PT6), 
	  .KNN_DATA_PT_CORE7  (KNN_DATA_PT7), 
	  .KNN_DATA_PT_CORE8  (KNN_DATA_PT8), 
	  .KNN_TEST_PT_CORE  (KNN_TEST_PT),
	  .KNN_VALID_CORE    (KNN_VALID_IN),
	  .KNN_SAMPLE_CORE   (KNN_SAMPLE),
	  .KNN_VALID_OUT_CORE(KNN_VALID_OUT_TOP),
	 
	  .KN1_OUT_CORE(KN1_OUT_TOP),
	  .KN2_OUT_CORE(KN2_OUT_TOP),
	  .KN3_OUT_CORE(KN3_OUT_TOP),
	  .KN4_OUT_CORE(KN4_OUT_TOP),
	  .KN5_OUT_CORE(KN5_OUT_TOP),
	  .KN6_OUT_CORE(KN6_OUT_TOP),

	  .IN1_OUT_CORE(IN1_OUT_TOP),
	  .IN2_OUT_CORE(IN2_OUT_TOP),
	  .IN3_OUT_CORE(IN3_OUT_TOP),
	  .IN4_OUT_CORE(IN4_OUT_TOP),
	  .IN5_OUT_CORE(IN5_OUT_TOP),
	  .IN6_OUT_CORE(IN6_OUT_TOP),

	  .CLK_CORE(clk),
	  .RST_CORE(rst_comb)

	 );

	assign KNN_VALID_OUT = KNN_VALID_OUT_TOP;
	assign KNN_KN1 = KN1_OUT_TOP;
	assign KNN_KN2 = KN2_OUT_TOP;
	assign KNN_KN3 = KN3_OUT_TOP;
	assign KNN_KN4 = KN4_OUT_TOP;
	assign KNN_KN5 = KN5_OUT_TOP;
	assign KNN_KN6 = KN6_OUT_TOP;
	
	assign KNN_IN1 = IN1_OUT_TOP;
	assign KNN_IN2 = IN2_OUT_TOP;
	assign KNN_IN3 = IN3_OUT_TOP;
	assign KNN_IN4 = IN4_OUT_TOP;
	assign KNN_IN5 = IN5_OUT_TOP;
	assign KNN_IN6 = IN6_OUT_TOP;

		
endmodule
