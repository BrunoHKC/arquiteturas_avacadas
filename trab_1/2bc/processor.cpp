#include "simulator.hpp"

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {

	_latency = 0;
	latecy_cycles = 0;
	checkBranchPrediction = false;
	checkConditionalMiss = false;
	updateBTB = false;
	_btb.allocate();

};

// =====================================================================
void processor_t::clock() {

	/// Emulate the latecy cused by branch miss prediction/BTB miss/...
	if(_latency)
	{
		latecy_cycles++;
		_latency--;
		return;
	}

	/// Get the next instruction from the trace
	opcode_package_t new_instruction;
	if (!orcs_engine.trace_reader->trace_fetch(&new_instruction)) {
		/// If EOF
		orcs_engine.simulator_alive = false;
	}

	if(updateBTB)
	{
		updateBTB = false;
		bool outcome = new_instruction.opcode_address != nextOpcodeAddress;
		_btb.update(previousOpcodeAddress,orcs_engine.get_global_cycle(),outcome);
	}

	if(checkBranchPrediction)
	{
		checkBranchPrediction = false;
		bool outcome = new_instruction.opcode_address != nextOpcodeAddress;
		_btb.updatePredictionStatistics(prediction != outcome);
		_latency += prediction != outcome?512:0;
	}

	if(checkConditionalMiss)
	{
		checkConditionalMiss = false;
		bool outcome = new_instruction.opcode_address != nextOpcodeAddress;
		_latency += outcome?512:0;

	}


	if(new_instruction.opcode_operation == INSTRUCTION_OPERATION_BRANCH)
	{
		updateBTB = true;
		if(new_instruction.branch_type == BRANCH_COND)
		{
			//conditional branch
			if(!_btb.hit(new_instruction.opcode_address))
			{
				//BTB miss
				checkConditionalMiss = true;
			}
			else
			{
				// branch predict
				checkBranchPrediction = true;
				prediction = _btb.predict(new_instruction.opcode_address);
			}
		}
		else{
			//unconditionl branch
			if(!_btb.hit(new_instruction.opcode_address))
			{
				//BTB miss
				_latency += 12;
			}
		}
	}
	nextOpcodeAddress = new_instruction.opcode_address + new_instruction.opcode_size;
	previousOpcodeAddress = new_instruction.opcode_address;

};

// =====================================================================
void processor_t::statistics() {
	ORCS_PRINTF("######################################################\n");
	ORCS_PRINTF("processor_t\n\n");

	ORCS_PRINTF("Total cycles: %llu\n",orcs_engine.get_global_cycle());
	ORCS_PRINTF("Total latency: %llu cycles\n\n",latecy_cycles);
	ORCS_PRINTF("BTB Hit: %llu\n",_btb.btb_hit);
	ORCS_PRINTF("BTB Miss: %llu\n\n",_btb.btb_miss);
	ORCS_PRINTF("Wrong prediction: %llu\n",_btb.wrongPrediction);
	ORCS_PRINTF("Correct prediction: %llu\n",_btb.correctPrediction);

};
