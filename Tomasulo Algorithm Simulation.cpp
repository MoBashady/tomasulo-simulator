#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <cstdint>
using namespace std;

// Define constants (now mutable via configuration)
int registers = 8;
int memory_size = 65536;
int rob_size = 8;

//Latencies (configurable)
int load_latency = 6;
int store_latency = 6;
int beq_latency = 1;
int call_return_latency = 1;
int add_sub_latency = 2;
int nand_latency = 1;
int mult_latency = 12;

//Reservation Stations (configurable)
int load_rs_size = 2;
int store_rs_size = 1;
int beq_rs_size = 2;
int call_return_rs_size = 1;
int add_sub_rs_size = 4;
int nand_rs_size = 2;
int mult_rs_size = 1;

//Instruction types
enum InstructionType {
    LOAD, STORE, BEQ, CALL, RET, ADD, SUB, NAND, MUL
};

//Functional units
enum FunctionalUnit {
    LOAD_UNIT, STORE_UNIT, BEQ_UNIT, CALL_RETURN_UNIT, ADD_SUB_UNIT, NAND_UNIT, MULT_UNIT
};

//Instruction structure
struct Instruction {
    InstructionType type;
    int dest;
    int src1;
    int src2;
    int offset;
    int pc;
    bool taken;
};

//Reservation Station Entry
struct RS_Entry {
    bool busy;
    InstructionType op;
    uint16_t Vj, Vk;
    int Qj, Qk;
    int dest;
    int address;
    int cycles_remaining;
    int pc;
    int offset;
    int branch_target;
    bool predicted_taken;
};

//Reorder Buffer Entry
struct ROB_Entry {
    bool busy;
    InstructionType type;
    int dest;
    uint16_t value;
    bool ready;
    int pc;
    bool mispredicted;
    int branch_target;
    bool predicted_taken;
};

//Functional Unit structure
struct FunctionalUnit_structure {
    vector<RS_Entry> reservation_stations;
    int latency;
    queue<int> executing;
};

class TomasuloSimulator {
private:
    uint16_t register_file[8];
    uint16_t* memory;
    ROB_Entry* reorder_buffer;

    map<FunctionalUnit, FunctionalUnit_structure> functional_units;
    int* register_status;

    vector<Instruction> instruction_queue;
    vector<bool> instruction_committed; // Track which instructions have been committed

    int head, tail;
    int cycle;
    int instr_commited;
    int number_of_branches;
    int mispredicted_branches;
    int pc;

    struct cycle_timing{
        int issue;
        int start_exec;
        int end_exec;
        int write_result;
        int commit;
    };
    vector<cycle_timing> timing_table;

public:

    TomasuloSimulator(){
        memory = new uint16_t[memory_size];
        reorder_buffer = new ROB_Entry[rob_size];
        register_status = new int[registers];

        register_file[0] = 0;

        for (int i = 1; i < registers; i++) {
            register_file[i] = 0;
        }
        for (int i = 0; i < memory_size; i++) {
            memory[i] = 0;
        }

        tail = 0;
        head = 0;

        for (int i = 0; i < rob_size; i++) {
            reorder_buffer[i].busy = false;
            reorder_buffer[i].ready = false;
            reorder_buffer[i].branch_target = 0;
            reorder_buffer[i].predicted_taken = false;
        }

        for (int i = 0; i < registers; i++) {
            register_status[i] = -1;
        }

        functional_units[LOAD_UNIT].latency = load_latency;
        functional_units[STORE_UNIT].latency = store_latency;
        functional_units[BEQ_UNIT].latency = beq_latency;
        functional_units[CALL_RETURN_UNIT].latency = call_return_latency;
        functional_units[ADD_SUB_UNIT].latency = add_sub_latency;
        functional_units[NAND_UNIT].latency = nand_latency;
        functional_units[MULT_UNIT].latency = mult_latency;

        functional_units[LOAD_UNIT].reservation_stations.resize(load_rs_size);
        functional_units[STORE_UNIT].reservation_stations.resize(store_rs_size);
        functional_units[BEQ_UNIT].reservation_stations.resize(beq_rs_size);
        functional_units[CALL_RETURN_UNIT].reservation_stations.resize(call_return_rs_size);
        functional_units[ADD_SUB_UNIT].reservation_stations.resize(add_sub_rs_size);
        functional_units[NAND_UNIT].reservation_stations.resize(nand_rs_size);
        functional_units[MULT_UNIT].reservation_stations.resize(mult_rs_size);

        pc = 0;
        cycle = 0;
        instr_commited = 0;
        number_of_branches = 0;
        mispredicted_branches = 0;
    }

    ~TomasuloSimulator() {
        delete[] memory;
        delete[] reorder_buffer;
        delete[] register_status;
    }

    uint16_t* get_memory() { return memory; }

    uint16_t get_registers(int i) {
        return register_file[i];
    }

    void load_instructions(const vector<Instruction>& instructions) {
        instruction_queue = instructions;
        instruction_committed.resize(instructions.size(), false);

        for (size_t i = 0; i < instruction_queue.size(); i++) {
            instruction_queue[i].pc = i;
        }

        pc = 0;
        int max_cycles = 1000;

        while(cycle < max_cycles) {
            // Count committed instructions
            int committed_count = 0;
            for (bool c : instruction_committed) {
                if (c) committed_count++;
            }

            // Exit if all instructions committed
            if (committed_count >= (int)instruction_queue.size()) {
                cout << "\nAll instructions committed. Ending simulation." << endl;
                break;
            }

            // Check if ROB is empty and no more instructions to issue
            bool rob_empty = true;
            for (int i = 0; i < rob_size; i++) {
                if (reorder_buffer[i].busy) {
                    rob_empty = false;
                    break;
                }
            }

            if (rob_empty && pc >= (int)instruction_queue.size()) {
                cout << "\nROB empty and no more instructions to issue. Ending simulation." << endl;
                break;
            }

            run();
        }

        if (cycle >= max_cycles) {
            cout << "\n!!! WARNING: Simulation stopped at max cycles !!!" << endl;
        }

        print_results();
    }

    void run() {
        cycle++;
        cout << "\n=== Cycle " << cycle << " ===" << endl;
        cout << "PC: " << pc
             << ", Queue: " << instruction_queue.size()
             << ", Committed: " << instr_commited
             << ", ROB head: " << head << ", tail: " << tail << endl;

        execute();
        write_back();
        commit();
        issue();

        cout << "Executing: ";
        for(auto& fu_pair : functional_units){
            queue<int> temp = fu_pair.second.executing;
            while(!temp.empty()){
                cout << "ROB[" << temp.front() << "] ";
                temp.pop();
            }
        }
        cout << endl;
    }

    void issue() {
        if (pc >= (int)instruction_queue.size()) return;

        Instruction& instr = instruction_queue[pc];
        FunctionalUnit fu = get_functional_unit(instr.type);

        if (reorder_buffer[tail].busy) return;

        bool rs_found = false;
        int rs_index = -1;
        FunctionalUnit_structure& fu_struct = functional_units[fu];

        for (int i = 0; i < (int)fu_struct.reservation_stations.size(); i++) {
            if (!fu_struct.reservation_stations[i].busy) {
                rs_found = true;
                rs_index = i;
                break;
            }
        }
        if (!rs_found) return;

        ROB_Entry& rob_entry = reorder_buffer[tail];
        rob_entry.busy = true;
        rob_entry.type = instr.type;
        rob_entry.dest = instr.dest;
        rob_entry.pc = instr.pc;
        rob_entry.ready = false;
        rob_entry.mispredicted = false;

        RS_Entry& rs_entry = fu_struct.reservation_stations[rs_index];
        rs_entry.busy = true;
        rs_entry.op = instr.type;
        rs_entry.dest = tail;
        rs_entry.pc = instr.pc;
        rs_entry.cycles_remaining = fu_struct.latency;

        if (instr.type == LOAD || instr.type == STORE) {
            int base_reg = instr.src1;
            if (register_status[base_reg] > 0) {
                rs_entry.Qj = register_status[base_reg] - 1;
                rs_entry.Vj = 0;
                rs_entry.address = 0;
            } else {
                rs_entry.Qj = -1;
                rs_entry.Vj = (base_reg == 0) ? 0 : register_file[base_reg];
                rs_entry.address = rs_entry.Vj + instr.offset;
            }
            rs_entry.offset = instr.offset;

            if (instr.type == STORE) {
                rob_entry.dest = rs_entry.address;

                int src_value_reg = instr.src2;
                if (register_status[src_value_reg] > 0) {
                    rs_entry.Qk = register_status[src_value_reg] - 1;
                    rs_entry.Vk = 0;
                } else {
                    rs_entry.Qk = -1;
                    rs_entry.Vk = (src_value_reg == 0) ? 0 : register_file[src_value_reg];
                }
            } else {
                rs_entry.Qk = -1;
                rs_entry.Vk = 0;
                register_status[instr.dest] = tail + 1;
            }
        }
        else if (instr.type == BEQ) {
            if (register_status[instr.src1] > 0) {
                rs_entry.Qj = register_status[instr.src1] - 1;
                rs_entry.Vj = 0;
            } else {
                rs_entry.Qj = -1;
                rs_entry.Vj = (instr.src1 == 0) ? 0 : register_file[instr.src1];
            }

            if (register_status[instr.src2] > 0) {
                rs_entry.Qk = register_status[instr.src2] - 1;
                rs_entry.Vk = 0;
            } else {
                rs_entry.Qk = -1;
                rs_entry.Vk = (instr.src2 == 0) ? 0 : register_file[instr.src2];
            }

            rs_entry.offset = instr.offset;
            rs_entry.branch_target = instr.pc + 1 + instr.offset;
            rs_entry.predicted_taken = false;

            rob_entry.branch_target = instr.pc + 1 + instr.offset;
            rob_entry.predicted_taken = false;
        }
        else if (instr.type == ADD || instr.type == SUB ||
                 instr.type == NAND || instr.type == MUL) {
            if (register_status[instr.src1] > 0) {
                rs_entry.Qj = register_status[instr.src1] - 1;
                rs_entry.Vj = 0;
            } else {
                rs_entry.Qj = -1;
                rs_entry.Vj = (instr.src1 == 0) ? 0 : register_file[instr.src1];
            }

            if (register_status[instr.src2] > 0) {
                rs_entry.Qk = register_status[instr.src2] - 1;
                rs_entry.Vk = 0;
            } else {
                rs_entry.Qk = -1;
                rs_entry.Vk = (instr.src2 == 0) ? 0 : register_file[instr.src2];
            }

            register_status[instr.dest] = tail + 1;
        }
        else if (instr.type == CALL) {
            // CALL: Save return address (PC+1) to R1, jump to target address
            register_status[1] = tail + 1;
            rs_entry.Qj = -1;
            rs_entry.Qk = -1;
            rs_entry.Vj = instr.pc + 1;  // Return address is PC+1

            // Store the jump target in rob_entry.dest
            rob_entry.dest = instr.dest;  // CALL target address
        }
        else if (instr.type == RET) {
            // RET: Jump to address in R1
            if (register_status[1] > 0) {
                rs_entry.Qj = register_status[1] - 1;
                rs_entry.Vj = 0;
            } else {
                rs_entry.Qj = -1;
                rs_entry.Vj = register_file[1];
            }
            rs_entry.Qk = -1;
        }

        if (rs_entry.Qj == -1 && rs_entry.Qk == -1) {
            fu_struct.executing.push(tail);
        }

        cout << "Issued ROB[" << tail << "]: Qj=" << rs_entry.Qj << ", Qk=" << rs_entry.Qk << endl;

        timing_table.push_back({cycle, cycle, 0, 0, 0});
        tail = (tail + 1) % rob_size;
        pc++;
    }

    void execute() {
        for(auto& fu_pair : functional_units){
            FunctionalUnit_structure& fu_struct = fu_pair.second;

            vector<int> still_executing;
            while(!fu_struct.executing.empty()){
                int rob_index = fu_struct.executing.front();
                fu_struct.executing.pop();

                // Check if instruction was flushed
                if(!reorder_buffer[rob_index].busy){
                    cout << "  ROB[" << rob_index << "] was flushed, skipping execution" << endl;
                    continue;
                }

                for(auto& rs : fu_struct.reservation_stations){
                    if(rs.busy && rs.dest == rob_index){
                        rs.cycles_remaining--;

                        if(rs.cycles_remaining == 0){
                            uint16_t result = calculate_result(rs);
                            cout << "  ROB[" << rob_index << "] finished! Value=" << result << endl;
                            reorder_buffer[rob_index].ready = true;
                            reorder_buffer[rob_index].value = result;

                            if(rob_index < (int)timing_table.size()){
                                timing_table[rob_index].end_exec = cycle;
                            }

                            if(rs.op == STORE && reorder_buffer[rob_index].dest == 0){
                                reorder_buffer[rob_index].dest = rs.address;
                            }

                            rs.busy = false;
                        } else {
                            still_executing.push_back(rob_index);
                        }
                        break;
                    }
                }
            }
            for(auto idx : still_executing){
                fu_struct.executing.push(idx);
            }
        }
    }

    void write_back() {
        cout << "  Write_back stage:" << endl;
        for (int i = 0; i < rob_size; i++){
            if(reorder_buffer[i].ready && reorder_buffer[i].busy){
                cout << "    ROB[" << i << "] ready, broadcasting..." << endl;
                if(i < (int)timing_table.size()){
                    timing_table[i].write_result = cycle;
                }
                broadcasting(i, reorder_buffer[i].value);
            }
        }
    }

    void commit() {
        if(!reorder_buffer[head].busy || !reorder_buffer[head].ready)
            return;

        ROB_Entry& re = reorder_buffer[head];

        if(head < (int)timing_table.size()){
            timing_table[head].commit = cycle;
        }

        if(re.type == BEQ){
            number_of_branches++;
            bool actual_taken = (re.value == 1);

            cout << "  Committing BEQ (PC=" << re.pc << "): actual=" << actual_taken
                 << ", predicted=" << re.predicted_taken << endl;

            // Mark instruction as committed
            if(re.pc < (int)instruction_committed.size()){
                instruction_committed[re.pc] = true;
            }

            if(actual_taken != re.predicted_taken){
                re.mispredicted = true;
                mispredicted_branches++;

                cout << "  *** BRANCH MISPREDICTION DETECTED ***" << endl;

                reorder_buffer[head].busy = false;
                int branch_rob = head;
                head = (head + 1) % rob_size;
                instr_commited++;

                deal_with_misprediction(branch_rob);

                if(actual_taken){
                    pc = re.branch_target;
                    cout << "  Redirecting PC to " << pc << " (branch target)" << endl;
                } else {
                    pc = re.pc + 1;
                    cout << "  Redirecting PC to " << pc << " (fall-through)" << endl;
                }

                return;
            }
        }
        else if(re.type == CALL) {
            // CALL: Store return address in R1 and jump to target
            register_file[1] = re.value;  // Return address (PC+1)
            if(register_status[1] == head + 1){
                register_status[1] = -1;
            }

            cout << "  Committing CALL (PC=" << re.pc << "): R1=" << re.value
                 << ", jumping to " << re.dest << endl;

            // Mark as committed
            if(re.pc < (int)instruction_committed.size()){
                instruction_committed[re.pc] = true;
            }

            // Flush speculatively issued instructions and redirect PC
            reorder_buffer[head].busy = false;
            int call_rob = head;
            head = (head + 1) % rob_size;
            instr_commited++;

            // Flush everything after CALL
            deal_with_misprediction(call_rob);

            // Jump to CALL target
            pc = re.dest;  // The target address is stored in dest field
            cout << "  Jumping to address " << pc << endl;

            return;
        }
        else if(re.type == RET) {
            // RET: Jump to address in R1
            int return_addr = re.value;  // Value contains R1 (return address)

            cout << "  Committing RET (PC=" << re.pc << "): Returning to address "
                 << return_addr << endl;

            // Mark as committed
            if(re.pc < (int)instruction_committed.size()){
                instruction_committed[re.pc] = true;
            }

            // Flush speculatively issued instructions and redirect PC
            reorder_buffer[head].busy = false;
            int ret_rob = head;
            head = (head + 1) % rob_size;
            instr_commited++;

            // Flush everything after RET
            deal_with_misprediction(ret_rob);

            // Jump to return address
            pc = return_addr;
            cout << "  Returning to address " << pc << endl;

            return;
        }
        else if(re.type == STORE){
            memory[re.dest] = re.value;
            cout << "  Committed STORE: mem[" << re.dest << "] = " << re.value << endl;
        }
        else if(re.type != BEQ && re.type != CALL && re.type != RET){
            register_file[re.dest] = re.value;
            if(register_status[re.dest] == head + 1){
                register_status[re.dest] = -1;
            }
        }

        // Mark instruction as committed
        if(re.pc < (int)instruction_committed.size()){
            instruction_committed[re.pc] = true;
        }

        reorder_buffer[head].busy = false;
        head = (head + 1) % rob_size;
        instr_commited++;
    }

    void broadcasting(int rob_index, uint16_t value){
        cout << "  Broadcasting ROB[" << rob_index << "] = " << value << endl;

        for(auto& fu_pair : functional_units){
            for(auto& rs: fu_pair.second.reservation_stations){
                if(rs.busy){
                    bool was_waiting = (rs.Qj == rob_index) || (rs.Qk == rob_index);

                    if(rs.Qj == rob_index){
                        cout << "    RS dest=" << rs.dest << ": Qj updated" << endl;
                        rs.Vj = value;
                        rs.Qj = -1;

                        if((rs.op == LOAD || rs.op == STORE) && rs.address == 0){
                            rs.address = value + rs.offset;
                        }
                    }
                    if(rs.Qk == rob_index){
                        cout << "    RS dest=" << rs.dest << ": Qk updated" << endl;
                        rs.Vk = value;
                        rs.Qk = -1;
                    }

                    if(was_waiting && rs.Qj == -1 && rs.Qk == -1){
                        cout << "    RS dest=" << rs.dest << ": Both ready, starting!" << endl;
                        fu_pair.second.executing.push(rs.dest);
                    }
                }
            }
        }
    }

    uint16_t calculate_result(const RS_Entry& rs){
        uint16_t result = 0;
        switch (rs.op) {
            case ADD:
                result = rs.Vj + rs.Vk;
                break;
            case SUB:
                result = rs.Vj - rs.Vk;
                break;
            case NAND:
                result = ~(rs.Vj & rs.Vk);
                break;
            case MUL:
                result = rs.Vj * rs.Vk;
                break;
            case LOAD:
                result = memory[rs.address];
                break;
            case STORE:
                result = rs.Vk;
                break;
            case BEQ:
                result = (rs.Vj == rs.Vk) ? 1 : 0;
                break;
            case CALL:
                result = rs.Vj;
                break;
            case RET:
                result = rs.Vj;
                break;
            default:
                result = 0;
                break;
        }
        return result;
    }

    FunctionalUnit get_functional_unit(InstructionType type) {
        switch(type) {
            case LOAD: return LOAD_UNIT;
            case STORE: return STORE_UNIT;
            case BEQ: return BEQ_UNIT;
            case CALL:
            case RET: return CALL_RETURN_UNIT;
            case ADD:
            case SUB: return ADD_SUB_UNIT;
            case NAND: return NAND_UNIT;
            case MUL: return MULT_UNIT;
            default: throw invalid_argument("Unknown instruction type");
        }
    }

    void deal_with_misprediction(int rob_index){
        cout << "  Flushing ROB entries after branch..." << endl;

        // Flush all instructions in ROB after the branch
        int index = (rob_index + 1) % rob_size;
        int flushed_count = 0;

        while(index != tail){
            if (reorder_buffer[index].busy) {
                cout << "    Flushing ROB[" << index << "] (PC=" << reorder_buffer[index].pc << ")" << endl;

                // Clear register status for flushed instructions
                if(reorder_buffer[index].type != BEQ &&
                   reorder_buffer[index].type != CALL &&
                   reorder_buffer[index].type != RET &&
                   reorder_buffer[index].type != STORE){
                    for(int i = 0; i < registers; i++){
                        if(register_status[i] == index + 1){
                            register_status[i] = -1;
                            cout << "      Cleared R" << i << " status" << endl;
                        }
                    }
                }

                reorder_buffer[index].busy = false;
                reorder_buffer[index].ready = false;
                flushed_count++;
            }
            index = (index + 1) % rob_size;
        }

        cout << "  Flushed " << flushed_count << " instructions" << endl;

        // Clear all reservation stations that belong to flushed instructions
        for(auto& fu_pair : functional_units){
            for(auto& rs : fu_pair.second.reservation_stations){
                if(rs.busy) {
                    // Check if this RS belongs to a flushed instruction
                    int rs_rob = rs.dest;
                    if (!reorder_buffer[rs_rob].busy) {
                        cout << "  Clearing RS for ROB[" << rs_rob << "]" << endl;
                        rs.busy = false;
                    }
                }
            }

            // Clear executing queue - only keep non-flushed instructions
            queue<int> new_queue;
            while(!fu_pair.second.executing.empty()){
                int rob_idx = fu_pair.second.executing.front();
                fu_pair.second.executing.pop();
                if(reorder_buffer[rob_idx].busy) {
                    new_queue.push(rob_idx);
                }
            }
            fu_pair.second.executing = new_queue;
        }

        tail = (rob_index + 1) % rob_size;
        cout << "  ROB tail reset to " << tail << endl;
    }

    void print_results() {
        cout << "\n=== SIMULATION RESULTS ===\n";
        cout << "Total cycles: " << cycle << "\n";
        cout << "Instructions committed: " << instr_commited << "\n";
        cout << "IPC: " << (float)instr_commited / cycle << "\n";
        cout << "Branches encountered: " << number_of_branches << "\n";
        cout << "Branch mispredictions: " << mispredicted_branches << "\n";
        if (number_of_branches > 0) {
            cout << "Misprediction rate: "
                 << (float)(mispredicted_branches * 100) / number_of_branches
                 << "%\n";
        }

        // Print registers
        cout << "\n=== REGISTER FILE ===\n";
        for (int i = 0; i < 8; i++) {
            cout << "R" << i << " = " << register_file[i] << endl;
        }

        // Print non-zero memory
        cout << "\n=== MEMORY (Non-Zero Locations) ===\n";
        bool found = false;
        for (int i = 0; i < memory_size; i++) {
            if (memory[i] != 0) {
                cout << "mem[" << i << "] = " << memory[i] << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "(No non-zero memory locations)\n";
        }

        cout << "\n=== INSTRUCTION TIMELINE ===\n";
        for (size_t i = 0; i < timing_table.size(); i++) {
            cout << "Inst " << i << ": Issue=" << timing_table[i].issue
                 << " Start=" << timing_table[i].start_exec
                 << " Finish=" << timing_table[i].end_exec
                 << " Write=" << timing_table[i].write_result
                 << " Commit=" << timing_table[i].commit << "\n";
        }
    }
};

int main() {
    cout << "====================================================\n";
    cout << "TOMASULO SIMULATOR - ALL TEST CASES\n";
    cout << "====================================================\n";

    // TEST 1: BEQ Taken (Branch Misprediction)
    {
        cout << "\n========================================\n";
        cout << "TEST 1: BEQ Taken - Branch Misprediction\n";
        cout << "========================================\n";

        TomasuloSimulator sim;
        sim.get_memory()[100] = 15;

        vector<Instruction> instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},     // 0: R1 = 15
            {LOAD, 2, 0, 0, 100, 1, false},     // 1: R2 = 15
            {BEQ, 0, 1, 2, 2, 2, false},        // 2: if R1==R2, jump to PC+1+2 = 5
            {ADD, 3, 1, 2, 0, 3, false},        // 3: R3 = 30 (SHOULD BE SKIPPED)
            {SUB, 4, 1, 2, 0, 4, false},        // 4: R4 = 0  (SHOULD BE SKIPPED)
            {MUL, 5, 1, 2, 0, 5, false},        // 5: R5 = 225 (BRANCH TARGET)
        };

        cout << "Instructions:\n";
        cout << "0. LOAD R1, 100(R0)   -> R1 = 15\n";
        cout << "1. LOAD R2, 100(R0)   -> R2 = 15\n";
        cout << "2. BEQ R1, R2, 2      -> Branch TAKEN (15 == 15), jump to 5\n";
        cout << "3. ADD R3, R1, R2     -> SKIPPED (flushed)\n";
        cout << "4. SUB R4, R1, R2     -> SKIPPED (flushed)\n";
        cout << "5. MUL R5, R1, R2     -> R5 = 225 (branch target)\n";
        cout << "\nExpected: R1=15, R2=15, R3=0, R4=0, R5=225\n";
        cout << "Expected: 4 instructions committed (not 6!)\n\n";

        sim.load_instructions(instructions);

        cout << "\n=== FINAL VERIFICATION ===\n";
        cout << "R1 = " << sim.get_registers(1) << " (expected 15)\n";
        cout << "R2 = " << sim.get_registers(2) << " (expected 15)\n";
        cout << "R3 = " << sim.get_registers(3) << " (expected 0 - skipped)\n";
        cout << "R4 = " << sim.get_registers(4) << " (expected 0 - skipped)\n";
        cout << "R5 = " << sim.get_registers(5) << " (expected 225)\n";
    }

    // TEST 2: CALL Instruction
    {
        cout << "\n========================================\n";
        cout << "TEST 2: CALL Instruction\n";
        cout << "========================================\n";

        TomasuloSimulator sim;
        sim.get_memory()[100] = 42;

        vector<Instruction> instructions = {
            {LOAD, 2, 0, 0, 100, 0, false},     // 0: R2 = 42
            {CALL, 4, 0, 0, 0, 1, false},       // 1: R1 = 2, jump to 4
            {ADD, 3, 2, 0, 0, 2, false},        // 2: SHOULD BE SKIPPED
            {SUB, 4, 2, 0, 0, 3, false},        // 3: SHOULD BE SKIPPED
            {ADD, 3, 2, 0, 0, 4, false}         // 4: R3 = 42 (CALL target)
        };

        cout << "Instructions:\n";
        cout << "0. LOAD R2, 100(R0)   -> R2 = 42\n";
        cout << "1. CALL 4             -> R1 = 2 (return addr), jump to 4\n";
        cout << "2. ADD R3, R2, R0     -> SKIPPED\n";
        cout << "3. SUB R4, R2, R0     -> SKIPPED\n";
        cout << "4. ADD R3, R2, R0     -> R3 = 42 (CALL target)\n";
        cout << "\nExpected: R1=2, R2=42, R3=42\n\n";

        sim.load_instructions(instructions);

        cout << "\n=== FINAL VERIFICATION ===\n";
        cout << "R1 = " << sim.get_registers(1) << " (expected 2)\n";
        cout << "R2 = " << sim.get_registers(2) << " (expected 42)\n";
        cout << "R3 = " << sim.get_registers(3) << " (expected 42)\n";
    }

    // TEST 3: CALL and RET
    {
        cout << "\n========================================\n";
        cout << "TEST 3: CALL and RET\n";
        cout << "========================================\n";

        TomasuloSimulator sim;
        sim.get_memory()[100] = 10;

        vector<Instruction> instructions = {
            {LOAD, 2, 0, 0, 100, 0, false},     // 0: R2 = 10
            {CALL, 4, 0, 0, 0, 1, false},       // 1: R1 = 2, jump to 4
            {ADD, 3, 2, 2, 0, 2, false},        // 2: R3 = 20 (RETURN TARGET)
            {LOAD, 7, 0, 0, 0, 3, false},       // 3: dummy
            {MUL, 4, 2, 2, 0, 4, false},        // 4: R4 = 100
            {RET, 0, 0, 0, 0, 5, false}         // 5: Jump to R1 (addr 2)
        };

        cout << "Instructions:\n";
        cout << "0. LOAD R2, 100(R0)   -> R2 = 10\n";
        cout << "1. CALL 4             -> R1 = 2, jump to 4\n";
        cout << "2. ADD R3, R2, R2     -> R3 = 20 (RETURN TARGET)\n";
        cout << "3. (dummy)\n";
        cout << "4. MUL R4, R2, R2     -> R4 = 100\n";
        cout << "5. RET                -> Jump to R1 (addr 2)\n";
        cout << "\nExpected: R1=2, R2=10, R3=20, R4=100\n\n";

        sim.load_instructions(instructions);

        cout << "\n=== FINAL VERIFICATION ===\n";
        cout << "R1 = " << sim.get_registers(1) << " (expected 2)\n";
        cout << "R2 = " << sim.get_registers(2) << " (expected 10)\n";
        cout << "R3 = " << sim.get_registers(3) << " (expected 20)\n";
        cout << "R4 = " << sim.get_registers(4) << " (expected 100)\n";
    }

    return 0;
}
