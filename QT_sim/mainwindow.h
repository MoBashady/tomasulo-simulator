#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QComboBox>
#include <vector>
#include <map>
#include <queue>
#include <cstdint>

// Enums
enum InstructionType {
    LOAD, STORE, BEQ, CALL, RET, ADD, SUB, NAND, MUL
};

enum FunctionalUnit {
    LOAD_UNIT, STORE_UNIT, BEQ_UNIT, CALL_RETURN_UNIT, ADD_SUB_UNIT, NAND_UNIT, MULT_UNIT
};

// Structures
struct Instruction {
    InstructionType type;
    int dest;
    int src1;
    int src2;
    int offset;
    int pc;
    bool taken;
};

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

struct FunctionalUnit_structure {
    std::vector<RS_Entry> reservation_stations;
    int latency;
    std::queue<int> executing;
};

class TomasuloSimulator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void runSimulation();
    void resetSimulation();
    void loadTestCase();

private:
    void setupUI();
    void updateDisplay(const QString& message);

    // Configuration spinboxes
    QSpinBox *robSizeSpinBox;
    QSpinBox *loadRSSpinBox;
    QSpinBox *storeRSSpinBox;
    QSpinBox *addSubRSSpinBox;
    QSpinBox *mulRSSpinBox;
    QSpinBox *nandRSSpinBox;
    QSpinBox *beqRSSpinBox;
    QSpinBox *callRetRSSpinBox;

    QSpinBox *loadLatencySpinBox;
    QSpinBox *storeLatencySpinBox;
    QSpinBox *addSubLatencySpinBox;
    QSpinBox *mulLatencySpinBox;
    QSpinBox *nandLatencySpinBox;
    QSpinBox *beqLatencySpinBox;
    QSpinBox *callRetLatencySpinBox;

    // UI elements
    QTextEdit *outputDisplay;
    QPushButton *runButton;
    QPushButton *resetButton;
    QPushButton *loadTestButton;

    // Test case selector
    QComboBox *testCaseCombo;

    // Hardware config values
    int rob_size;
    int load_rs_size, store_rs_size, add_sub_rs_size;
    int mul_rs_size, nand_rs_size, beq_rs_size, call_ret_rs_size;
    int load_latency, store_latency, add_sub_latency;
    int mul_latency, nand_latency, beq_latency, call_ret_latency;
};

// Tomasulo Simulator class
class TomasuloSimulator {
private:
    uint16_t register_file[8];
    uint16_t* memory;
    ROB_Entry* reorder_buffer;
    std::map<FunctionalUnit, FunctionalUnit_structure> functional_units;
    int* register_status;
    std::vector<Instruction> instruction_queue;
    std::vector<bool> instruction_committed;
    int head, tail;
    int cycle;
    int instr_commited;
    int number_of_branches;
    int mispredicted_branches;
    int pc;
    int memory_size;
    int rob_size;
    int registers;

    struct cycle_timing {
        int issue;
        int start_exec;
        int end_exec;
        int write_result;
        int commit;
    };
    std::vector<cycle_timing> timing_table;

    QString output_log;

public:
    TomasuloSimulator(int rob_sz,
                      int load_rs, int store_rs, int addsub_rs,
                      int mul_rs, int nand_rs, int beq_rs, int callret_rs,
                      int load_lat, int store_lat, int addsub_lat,
                      int mul_lat, int nand_lat, int beq_lat, int callret_lat);
    ~TomasuloSimulator();

    uint16_t* get_memory() { return memory; }
    uint16_t get_registers(int i) { return register_file[i]; }
    void load_instructions(const std::vector<Instruction>& instructions);
    QString get_output_log() { return output_log; }

private:
    void run();
    void issue();
    void execute();
    void write_back();
    void commit();
    void broadcasting(int rob_index, uint16_t value);
    uint16_t calculate_result(const RS_Entry& rs);
    FunctionalUnit get_functional_unit(InstructionType type);
    void deal_with_misprediction(int rob_index);
    void log(const QString& msg);
};

#endif // MAINWINDOW_H
