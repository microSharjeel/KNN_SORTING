`timescale 1ns/1ps
`include "iob_lib.vh"
`include "KNN_Header.vh"
module	knn_fsm
  #(
	parameter SIZE =     3,
	parameter IDLE  =    3'b001,
	parameter COMPUTE =  3'b010,
    	parameter OUTPUT  =  3'b100
    )
	(
	//to and from datapath
	 output reg [`WDATA_W-1:0] KNN_TEST_PT_O,
	 output reg [`WDATA_W-1:0] KNN_DATA_PT_O,
	 output reg KNN_START_O,
	
	//to and from KNN top level and KNN core 
	`INPUT(KNN_START_FSM,      1),
	`INPUT(KNN_DATA_PT_FSM1,    `WDATA_W),
	`INPUT(KNN_DATA_PT_FSM2,    `WDATA_W),
	`INPUT(KNN_DATA_PT_FSM3,    `WDATA_W),
	`INPUT(KNN_DATA_PT_FSM4,    `WDATA_W),
	`INPUT(KNN_DATA_PT_FSM5,    `WDATA_W),
	`INPUT(KNN_DATA_PT_FSM6,    `WDATA_W),
	`INPUT(KNN_DATA_PT_FSM7,    `WDATA_W),
	`INPUT(KNN_DATA_PT_FSM8,    `WDATA_W),
	`INPUT(KNN_TEST_PT_FSM,    `WDATA_W),
	`INPUT(KNN_VALID_IN_FSM,   `KNN_VALID_W),
	 output reg KNN_VALID_O,
	`INPUT(clk,                1),
	`INPUT(rst,                1)
	);

reg [(`KNN_LOW_W+`KNN_HIGH_W)-1:0]data_points[`K_NUM_DATA_PTS-1:0];

`SIGNAL(state, SIZE)
`SIGNAL(count_wr, `K_NUM_DATA_PTS_BIT+1)
`SIGNAL(addr, `K_NUM_DATA_PTS_BIT)
`SIGNAL(cnt, 3)
integer i;

always @(posedge clk or posedge rst)
begin
	if(rst)	
	begin
		for (i = 0; i <= (`K_NUM_DATA_PTS-1); i = i + 1) begin data_points[i] <= 0; end
		addr <= 7'd0;
	end	
	else if(KNN_VALID_IN_FSM)
	begin

	 	   data_points[0]<=KNN_DATA_PT_FSM1;
	 	   data_points[1]<=KNN_DATA_PT_FSM2;
	 	   data_points[2]<=KNN_DATA_PT_FSM3;
	 	   data_points[3]<=KNN_DATA_PT_FSM4;
	 	   data_points[4]<=KNN_DATA_PT_FSM5;
	 	   data_points[5]<=KNN_DATA_PT_FSM6;
	 	   data_points[6]<=KNN_DATA_PT_FSM7;
	 	   data_points[7]<=KNN_DATA_PT_FSM8;
		   addr <= addr+1;	

	end
	else
	begin
		addr<=7'd0;
	end
    
end

 always @(posedge clk or posedge rst)

 begin : FSM_SEQ
    
	if(rst)
	begin
		state<= IDLE;
	end
	else
	begin
	  case(state)
	     IDLE:	
			if((KNN_START_FSM==1)) 
			begin	
				state <= COMPUTE;
			end	
	    		else if((KNN_START_FSM==0))
	    		begin
				state <=  IDLE;
     			end
	    
		 COMPUTE:
			if( (count_wr <= `K_NUM_DATA_PTS+1)  &&(KNN_START_FSM==1) )
			begin
				state <= COMPUTE;
			end
			else if ( (count_wr > `K_NUM_DATA_PTS+1)&&(KNN_START_FSM==1)  )
			begin
	
				state <= OUTPUT;
			end
		 OUTPUT:
		 	if( (KNN_START_FSM==1))
			begin
				state <= OUTPUT;
			end
			else if( (KNN_START_FSM==0))
			begin
				state<= IDLE;
			end 
	      default : 
			state <= IDLE;

		endcase
	end
  
 end 

always @(posedge clk or posedge rst)
 begin
	if(rst)
	begin
		count_wr<=0;
		KNN_START_O   <= 1'b0;
		KNN_TEST_PT_O <= 32'd0;
		KNN_DATA_PT_O <= 32'd0;
		cnt<=3'd0;
		
	end
	else 
	begin
		 if(state==COMPUTE &&  count_wr <= (`K_NUM_DATA_PTS+1))
		 begin	
			if(count_wr<=(`K_NUM_DATA_PTS-1))
			begin
				KNN_TEST_PT_O <= KNN_TEST_PT_FSM;
				KNN_DATA_PT_O <= data_points[cnt];//[count_wr];
				KNN_START_O   <= 1'b1;
				cnt<=cnt+1;
			end
			else if(count_wr>(`K_NUM_DATA_PTS-1))
			begin
				KNN_TEST_PT_O <= 32'd0;
				KNN_DATA_PT_O <= 32'd0;
				KNN_START_O   <= 1'b1;
				cnt<=0;
			end	
			count_wr <= count_wr +1;
			
		 end
		 else 
		 begin
			KNN_TEST_PT_O <= 32'd0;
			KNN_DATA_PT_O <= 32'd0;
			KNN_START_O   <= 1'b0;
			count_wr<= 0;
			cnt<=0;
			
		 end
	end
end	

always @(posedge clk or posedge rst)
 begin
	if(rst)
	begin
		KNN_VALID_O   <= 1'b0;
	end
	else 
	begin
		 if( (state==OUTPUT))
		 begin	
			KNN_VALID_O   <= 1'b1;
		 end

		 else
		 	KNN_VALID_O   <= 1'b0;
		 
	end
end
endmodule 


	
