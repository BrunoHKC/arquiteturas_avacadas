#include "simulator.hpp"

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {

	_latency = 0;
	checkBranchPrediction = false;
	updateBTB = false;
	_btb.allocate();

};

// =====================================================================
void processor_t::clock() {

	/// Emulate the latecy cused by branch miss prediction/BTB miss/...
	if(_latency)
	{
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
		bool branchTaken = new_instruction.opcode_address != nextOpcodeAddress;
		_btb.update(previousOpcodeAddress,orcs_engine.get_global_cycle(),branchTaken);
	}

	if(checkBranchPrediction)
	{
		checkBranchPrediction = false;
		bool branchTaken = new_instruction.opcode_address != nextOpcodeAddress;
		_btb.updatePredictionStatistics(prediction != branchTaken);
		if(prediction != branchTaken)
		{
			//conditional branch misprediction
			_latency += 512;
		}
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
				_latency += 512;
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
	ORCS_PRINTF("processor_t\n");

	ORCS_PRINTF("Total de ciclos: %llu\n",orcs_engine.get_global_cycle());
	ORCS_PRINTF("BTB Hit: %llu\n",_btb.btb_hit);
	ORCS_PRINTF("BTB Miss: %llu\n",_btb.btb_miss);
	ORCS_PRINTF("BTB Wrong prediction: %llu\n",_btb.wrongPrediction);
	ORCS_PRINTF("BTB Correct prediction: %llu\n",_btb.correctPrediction);

};
