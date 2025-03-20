/*-----------------------------------------------------------------------------

4190.308-001 Computer Architecture

Instructor: Prof. Jae W. Lee (jaewlee@snu.ac.kr)

Homework #3: RISC-V Pipeline in Verilog

Description:
	This module is your verilog pipeline.

-----------------------------------------------------------------------------*/

	module riscv_pipeline
(
	input clk,
	input reset
);

	//IF stage
	reg [63:0] IF_pc;
	wire [31:0] IF_inst;

	//ID stage
	reg [31:0] ID_inst;
	reg [63:0] ID_pc;

	wire [6:0] ID_opcode;
	wire [2:0] ID_funct3;
	wire [6:0] ID_funct7;
	wire [4:0] ID_rs1, ID_rs2, ID_rd;
	wire [63:0] ID_rf_data_rs1, ID_rf_data_rs2;
	wire [63:0] ID_rs1_data, ID_rs2_data;
	wire [63:0] ID_imm64;
	wire ID_alu_src;
	wire ID_mem_to_reg;
	wire ID_reg_write;
	wire ID_mem_read;
	wire ID_mem_write;
	wire [2:0] ID_alu_op;
	wire ID_branch;
	wire ID_halt;

	//EX stage
	reg [63:0] EX_pc;
	reg [4:0] EX_rs1, EX_rs2;
	reg [63:0] EX_rs1_data, EX_rs2_data;
	reg [63:0] EX_imm64;
	reg [4:0] EX_rd;
	reg EX_alu_src;
	reg EX_mem_to_reg;
	reg EX_reg_write;
	reg EX_mem_read;
	reg EX_mem_write;
	reg [2:0] EX_alu_op;
	reg EX_branch;
	reg EX_halt;

    //alu
	wire [63:0] alu_lhs, alu_rhs, EX_alu_result;
	wire alu_zero, alu_sign;

	//Mem stage
	reg [63:0] MEM_pc;
	reg [63:0] MEM_rs2_data;
	reg [63:0] MEM_alu_result;
	reg MEM_zero, MEM_sign;
	reg [4:0] MEM_rd;
	reg MEM_mem_to_reg;
	reg MEM_reg_write;
	reg MEM_mem_read;
	reg MEM_mem_write;
	reg MEM_branch;
	reg MEM_halt;
	
	wire [63:0] MEM_read_data;

	//WB stage
	reg [63:0] WB_alu_result;
	reg [63:0] WB_mem_rddata;
	reg [4:0] WB_rd;
	reg WB_reg_write;
	reg WB_mem_to_reg;
	reg WB_mem_read;

	wire [63:0] WB_write_data;
	
	wire lu_hazard;
	wire branch_taken;
	wire [63:0] ID_rf_data_rs1_fw;
	wire [63:0] ID_rf_data_rs2_fw;
	reg beq_after_load = 1'b0;
	wire stall;

	//--------------------IF stage-----------------------
	rom64 imem(IF_pc, IF_inst);

	always @(posedge clk)
	begin
		if (lu_hazard && IF_inst[6:0] == 7'b1100011)
		begin
			beq_after_load <= 1'b1;
		end
		else if (beq_after_load)
		begin
			beq_after_load <= 1'b0;
		end
		
		IF_pc <= reset ? 64'h0 :
					lu_hazard || beq_after_load || beq_stall ? IF_pc :
					branch_taken ? (ID_pc + (ID_imm64 <<1)) :
					IF_pc + 4;
		ID_inst <= reset || branch_taken || lu_hazard || beq_stall || beq_after_load ? 32'h0 :
						//lu_hazard || beq_stall || beq_after_load ? ID_inst :
						IF_inst;
		ID_pc <= reset ? 64'h0 :
					lu_hazard || beq_after_load || beq_stall ? ID_pc :
					IF_pc;
	end

	//--------------------ID stage-----------------------

	inst_decoder id_unit(	.inst(ID_inst),
				.opcode(ID_opcode),
				.funct3(ID_funct3),
				.funct7(ID_funct7),
				.rs1(ID_rs1),
				.rs2(ID_rs2),
				.rd(ID_rd),
				.imm64(ID_imm64));

	pipeline_ctl id_ctl(	.opcode(ID_opcode),
				.funct3(ID_funct3),
				.funct7(ID_funct7),
				.hazarded(stall),
				.alu_src(ID_alu_src),
				.mem_to_reg(ID_mem_to_reg),
				.reg_write(ID_reg_write),
				.mem_read(ID_mem_read),
				.mem_write(ID_mem_write),
				.alu_operation(ID_alu_op),
				.branch(ID_branch),
				.halt(ID_halt)
				);
				
	//--------------------ID/WB stage-----------------------
	// We use inverted clk signal to enable "write-back" and "register read" occur at the same time.
	reg_file id_regfile(	.clk(~clk), .rs1(ID_rs1), .rs2(ID_rs2),
				.rs1_data(ID_rf_data_rs1),
				.rs2_data(ID_rf_data_rs2),
				.rd(WB_rd), .write(WB_reg_write),
				.write_data(WB_write_data));


	//ID/EX pipeline registers
	assign stall = lu_hazard || beq_stall || branch_taken;
	assign lu_hazard = (ID_mem_read) && ((ID_rd == IF_inst[19:15]) || (ID_rd == IF_inst[24:20]));
	assign beq_stall = (IF_inst[6:0] == 7'b1100011 && ID_rd && (((ID_rd == IF_inst[19:15]) || (ID_rd == IF_inst[24:20])) || (EX_mem_read &&((EX_rd == IF_inst[19:15]) || (EX_rd == IF_inst[24:20])))));
	assign branch_taken = (ID_opcode == 7'b1100011 && (ID_rf_data_rs1_fw == ID_rf_data_rs2_fw));
	assign ID_rf_data_rs1_fw = (ID_opcode == 7'b1100011 && MEM_reg_write && MEM_rd && (MEM_rd == ID_rs1)) ? MEM_alu_result :
										(ID_opcode == 7'b1100011 && WB_reg_write && WB_rd && (WB_rd == ID_rs1)) ? (WB_mem_read ? WB_mem_rddata: WB_alu_result) :
										 ID_rf_data_rs1;
	assign ID_rf_data_rs2_fw = (ID_opcode == 7'b1100011 && MEM_reg_write && MEM_rd && (MEM_rd == ID_rs2)) ? MEM_alu_result :
										(ID_opcode == 7'b1100011 && WB_reg_write && WB_rd && (WB_rd == ID_rs2)) ? (WB_mem_read ? WB_mem_rddata: WB_alu_result) :
										 ID_rf_data_rs2;

	always @(posedge clk)
	begin
		EX_pc <= reset ? 64'h0 : ID_pc;
		EX_rs1 <= reset ? 5'h0 : ID_rs1;
		EX_rs2 <= reset ? 5'h0 : ID_rs2;
		EX_alu_src		<= reset ? 1'h0 : ID_alu_src;
		EX_mem_to_reg	<= reset ? 1'h0 : ID_mem_to_reg;
		EX_reg_write	<= reset ? 1'h0 : ID_reg_write;
		EX_mem_read		<= reset ? 1'h0 : ID_mem_read;
		EX_mem_write	<= reset ? 1'h0 : ID_mem_write;
		EX_alu_op		<= reset ? 3'h0 : ID_alu_op;
		EX_branch		<= reset ? 1'h0 : ID_branch;
		EX_halt			<= reset ? 1'h0 : ID_halt;

		EX_rs1_data		<= reset ? 64'h0 :
											(EX_reg_write && EX_rd && ID_opcode!= 7'b1100011 && (ID_rs1 == EX_rd)) ? EX_alu_result :
											(MEM_reg_write && MEM_rd && ID_opcode!= 7'b1100011 && (ID_rs1 == MEM_rd)) ?
											(MEM_mem_read ? MEM_read_data: MEM_alu_result) :
											ID_rf_data_rs1_fw;
		EX_rs2_data		<= reset ? 64'h0 :
											(EX_reg_write && EX_rd && ID_opcode!= 7'b1100011 && (ID_rs2 == EX_rd)) ? EX_alu_result :
											(MEM_reg_write && MEM_rd && ID_opcode!= 7'b1100011 && (ID_rs2 == MEM_rd)) ? 
											(MEM_mem_read ? MEM_read_data: MEM_alu_result) :
											ID_rf_data_rs2_fw;
		EX_imm64		<= reset ? 64'h0 : ID_imm64;
		EX_rd			<= reset ? 5'h0 : ID_rd;
	end

	//------------------EX stage-----------------------

	assign alu_lhs = EX_rs1_data;
	assign alu_rhs = EX_alu_src ? EX_imm64 : EX_rs2_data;

	//ALU
	alu EX_alu(	.op(EX_alu_op),
			.lhs(alu_lhs), .rhs(alu_rhs),
			.result(EX_alu_result),
			.zero(alu_zero), .sign(alu_sign));

	//EX/MEM pipeline registers
	always @(posedge clk)
	begin
		MEM_pc <= reset ? 64'h0 : (EX_pc + (EX_imm64 <<1));
		MEM_mem_to_reg	<= reset ? 1'h0 : EX_mem_to_reg;
		MEM_reg_write	<= reset ? 1'h0 : EX_reg_write;
		MEM_mem_read	<= reset ? 1'h0 : EX_mem_read;
		MEM_mem_write	<= reset ? 1'h0 : EX_mem_write;
		MEM_branch	<= reset ? 1'h0 : EX_branch;
		MEM_halt		<= reset ? 1'h0 : EX_halt;

		MEM_rs2_data	<= reset ? 64'h0 : EX_rs2_data;
		MEM_alu_result	<= reset ? 64'h0 : EX_alu_result;
		MEM_zero		<= reset ? 1'h0 : alu_zero;
		MEM_sign		<= reset ? 1'h0 : alu_sign;
		MEM_rd			<= reset ? 5'h0 : EX_rd;
	end

	//--------------------Mem stage------------------------

	mem64 MEM_dmem(	.clk(clk),
			.write_enable(MEM_mem_write),
			.halt(MEM_halt),
			.address(MEM_alu_result),
			.write_data(MEM_rs2_data),
			.read_data(MEM_read_data));

	//MEM/WB pipeline registers
	always @(posedge clk)
	begin
		WB_mem_read <= (reset) ? 1'h0 : MEM_mem_read;
		WB_reg_write	<= (reset) ? 1'h0 : MEM_reg_write;
		WB_rd			<= (reset) ? 5'h0 : MEM_rd;
		WB_alu_result	<= (reset) ? 64'h0 : MEM_alu_result;
		WB_mem_rddata	<= (reset) ? 64'h0 : MEM_read_data;
		WB_mem_to_reg	<= (reset) ? 1'h0 : MEM_mem_to_reg;
	end

	//--------------------WB stage-----------------------
	assign WB_write_data = WB_mem_to_reg ? WB_mem_rddata : WB_alu_result;

endmodule
