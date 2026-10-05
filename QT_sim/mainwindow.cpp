#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Default values
    rob_size = 8;
    load_rs_size = 2; store_rs_size = 1; add_sub_rs_size = 4;
    mul_rs_size = 1; nand_rs_size = 2; beq_rs_size = 2; call_ret_rs_size = 1;
    load_latency = 6; store_latency = 6; add_sub_latency = 2;
    mul_latency = 12; nand_latency = 1; beq_latency = 1; call_ret_latency = 1;

    setupUI();
    setWindowTitle("Tomasulo Simulator - Qt GUI with CALL/RET Support");
    resize(1200, 800);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Left panel - Configuration
    QScrollArea *scrollArea = new QScrollArea();
    QWidget *configWidget = new QWidget();
    QVBoxLayout *configLayout = new QVBoxLayout(configWidget);

    // ROB Configuration
    QGroupBox *robGroup = new QGroupBox("Reorder Buffer");
    QGridLayout *robLayout = new QGridLayout();
    robLayout->addWidget(new QLabel("ROB Size:"), 0, 0);
    robSizeSpinBox = new QSpinBox();
    robSizeSpinBox->setRange(1, 64);
    robSizeSpinBox->setValue(rob_size);
    robLayout->addWidget(robSizeSpinBox, 0, 1);
    robGroup->setLayout(robLayout);
    configLayout->addWidget(robGroup);

    // Reservation Stations Configuration
    QGroupBox *rsGroup = new QGroupBox("Reservation Stations");
    QGridLayout *rsLayout = new QGridLayout();

    rsLayout->addWidget(new QLabel("LOAD RS:"), 0, 0);
    loadRSSpinBox = new QSpinBox();
    loadRSSpinBox->setRange(1, 16);
    loadRSSpinBox->setValue(load_rs_size);
    rsLayout->addWidget(loadRSSpinBox, 0, 1);

    rsLayout->addWidget(new QLabel("STORE RS:"), 1, 0);
    storeRSSpinBox = new QSpinBox();
    storeRSSpinBox->setRange(1, 16);
    storeRSSpinBox->setValue(store_rs_size);
    rsLayout->addWidget(storeRSSpinBox, 1, 1);

    rsLayout->addWidget(new QLabel("ADD/SUB RS:"), 2, 0);
    addSubRSSpinBox = new QSpinBox();
    addSubRSSpinBox->setRange(1, 16);
    addSubRSSpinBox->setValue(add_sub_rs_size);
    rsLayout->addWidget(addSubRSSpinBox, 2, 1);

    rsLayout->addWidget(new QLabel("MUL RS:"), 3, 0);
    mulRSSpinBox = new QSpinBox();
    mulRSSpinBox->setRange(1, 16);
    mulRSSpinBox->setValue(mul_rs_size);
    rsLayout->addWidget(mulRSSpinBox, 3, 1);

    rsLayout->addWidget(new QLabel("NAND RS:"), 4, 0);
    nandRSSpinBox = new QSpinBox();
    nandRSSpinBox->setRange(1, 16);
    nandRSSpinBox->setValue(nand_rs_size);
    rsLayout->addWidget(nandRSSpinBox, 4, 1);

    rsLayout->addWidget(new QLabel("BEQ RS:"), 5, 0);
    beqRSSpinBox = new QSpinBox();
    beqRSSpinBox->setRange(1, 16);
    beqRSSpinBox->setValue(beq_rs_size);
    rsLayout->addWidget(beqRSSpinBox, 5, 1);

    rsLayout->addWidget(new QLabel("CALL/RET RS:"), 6, 0);
    callRetRSSpinBox = new QSpinBox();
    callRetRSSpinBox->setRange(1, 16);
    callRetRSSpinBox->setValue(call_ret_rs_size);
    rsLayout->addWidget(callRetRSSpinBox, 6, 1);

    rsGroup->setLayout(rsLayout);
    configLayout->addWidget(rsGroup);

    // Latencies Configuration
    QGroupBox *latGroup = new QGroupBox("Functional Unit Latencies (cycles)");
    QGridLayout *latLayout = new QGridLayout();

    latLayout->addWidget(new QLabel("LOAD:"), 0, 0);
    loadLatencySpinBox = new QSpinBox();
    loadLatencySpinBox->setRange(1, 100);
    loadLatencySpinBox->setValue(load_latency);
    latLayout->addWidget(loadLatencySpinBox, 0, 1);

    latLayout->addWidget(new QLabel("STORE:"), 1, 0);
    storeLatencySpinBox = new QSpinBox();
    storeLatencySpinBox->setRange(1, 100);
    storeLatencySpinBox->setValue(store_latency);
    latLayout->addWidget(storeLatencySpinBox, 1, 1);

    latLayout->addWidget(new QLabel("ADD/SUB:"), 2, 0);
    addSubLatencySpinBox = new QSpinBox();
    addSubLatencySpinBox->setRange(1, 100);
    addSubLatencySpinBox->setValue(add_sub_latency);
    latLayout->addWidget(addSubLatencySpinBox, 2, 1);

    latLayout->addWidget(new QLabel("MUL:"), 3, 0);
    mulLatencySpinBox = new QSpinBox();
    mulLatencySpinBox->setRange(1, 100);
    mulLatencySpinBox->setValue(mul_latency);
    latLayout->addWidget(mulLatencySpinBox, 3, 1);

    latLayout->addWidget(new QLabel("NAND:"), 4, 0);
    nandLatencySpinBox = new QSpinBox();
    nandLatencySpinBox->setRange(1, 100);
    nandLatencySpinBox->setValue(nand_latency);
    latLayout->addWidget(nandLatencySpinBox, 4, 1);

    latLayout->addWidget(new QLabel("BEQ:"), 5, 0);
    beqLatencySpinBox = new QSpinBox();
    beqLatencySpinBox->setRange(1, 100);
    beqLatencySpinBox->setValue(beq_latency);
    latLayout->addWidget(beqLatencySpinBox, 5, 1);

    latLayout->addWidget(new QLabel("CALL/RET:"), 6, 0);
    callRetLatencySpinBox = new QSpinBox();
    callRetLatencySpinBox->setRange(1, 100);
    callRetLatencySpinBox->setValue(call_ret_latency);
    latLayout->addWidget(callRetLatencySpinBox, 6, 1);

    latGroup->setLayout(latLayout);
    configLayout->addWidget(latGroup);

    // Test Case Selection
    QGroupBox *testGroup = new QGroupBox("Test Cases");
    QVBoxLayout *testLayout = new QVBoxLayout();
    testCaseCombo = new QComboBox();

    // ALL 17 TEST CASES
    testCaseCombo->addItem("Test 1: Simple STORE");
    testCaseCombo->addItem("Test 2: STORE with Data Dependency");
    testCaseCombo->addItem("Test 3: STORE with Address Dependency");
    testCaseCombo->addItem("Test 4: Multiple STOREs");
    testCaseCombo->addItem("Test 5: ADD Instruction");
    testCaseCombo->addItem("Test 6: SUB Instruction");
    testCaseCombo->addItem("Test 7: ADD/SUB Chain");
    testCaseCombo->addItem("Test 8: MUL Instruction");
    testCaseCombo->addItem("Test 9: MUL with Dependencies");
    testCaseCombo->addItem("Test 10: NAND Instruction");
    testCaseCombo->addItem("Test 11: NAND Chain");
    testCaseCombo->addItem("Test 12: BEQ Not Taken");
    testCaseCombo->addItem("Test 13: BEQ Taken (Misprediction)");
    testCaseCombo->addItem("Test 14: CALL Instruction");
    testCaseCombo->addItem("Test 15: CALL and RET");
    testCaseCombo->addItem("Test 16: Mixed Operations");
    testCaseCombo->addItem("Test 17: All Instructions");

    testLayout->addWidget(testCaseCombo);
    loadTestButton = new QPushButton("Load Test Case");
    connect(loadTestButton, &QPushButton::clicked, this, &MainWindow::loadTestCase);
    testLayout->addWidget(loadTestButton);
    testGroup->setLayout(testLayout);
    configLayout->addWidget(testGroup);

    // Buttons
    runButton = new QPushButton("Run Simulation");
    runButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 10px; }");
    connect(runButton, &QPushButton::clicked, this, &MainWindow::runSimulation);
    configLayout->addWidget(runButton);

    resetButton = new QPushButton("Reset");
    resetButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 10px; }");
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetSimulation);
    configLayout->addWidget(resetButton);

    configLayout->addStretch();

    scrollArea->setWidget(configWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMaximumWidth(350);

    // Right panel - Output display
    outputDisplay = new QTextEdit();
    outputDisplay->setReadOnly(true);
    outputDisplay->setFont(QFont("Courier", 9));
    outputDisplay->setText("Welcome to Tomasulo Simulator!\n\nSelect a test case and click 'Load Test Case' to begin.");

    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(outputDisplay, 1);
}

void MainWindow::loadTestCase()
{
    int testIndex = testCaseCombo->currentIndex();
    QString testDescription;

    switch(testIndex) {
    case 0:
        testDescription = "Test 1: Simple STORE - No Dependencies\n"
                          "Instructions:\n"
                          "1. LOAD R1, 500(R0)   -> R1 = 99\n"
                          "2. STORE R1, 200(R0)  -> mem[200] = 99\n"
                          "Expected: R1=99, mem[200]=99";
        break;
    case 1:
        testDescription = "Test 2: STORE with Data Dependency on ADD\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 10\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 20\n"
                          "3. ADD R3, R1, R2     -> R3 = 30\n"
                          "4. STORE R3, 300(R0)  -> mem[300] = 30\n"
                          "Expected: R1=10, R2=20, R3=30, mem[300]=30";
        break;
    case 2:
        testDescription = "Test 3: STORE with Address Dependency\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 5\n"
                          "2. LOAD R2, 200(R0)   -> R2 = 77\n"
                          "3. STORE R2, 10(R1)   -> mem[15] = 77\n"
                          "Expected: R1=5, R2=77, mem[15]=77";
        break;
    case 3:
        testDescription = "Test 4: Multiple STORE Instructions\n"
                          "Instructions:\n"
                          "1-3. LOAD R1-R3\n"
                          "4-6. STORE to mem[400-402]\n"
                          "Expected: mem[400]=11, mem[401]=22, mem[402]=33";
        break;
    case 4:
        testDescription = "Test 5: ADD Instruction\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 15\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 25\n"
                          "3. ADD R3, R1, R2     -> R3 = 40\n"
                          "Expected: R1=15, R2=25, R3=40";
        break;
    case 5:
        testDescription = "Test 6: SUB Instruction\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 50\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 30\n"
                          "3. SUB R3, R1, R2     -> R3 = 20\n"
                          "Expected: R1=50, R2=30, R3=20";
        break;
    case 6:
        testDescription = "Test 7: ADD/SUB Chain with Dependencies\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 10\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 5\n"
                          "3. ADD R3, R1, R2     -> R3 = 15\n"
                          "4. SUB R4, R3, R2     -> R4 = 10 (depends on R3)\n"
                          "5. ADD R5, R3, R4     -> R5 = 25 (depends on R3 and R4)\n"
                          "Expected: R1=10, R2=5, R3=15, R4=10, R5=25";
        break;
    case 7:
        testDescription = "Test 8: MUL Instruction (12 cycle latency)\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 7\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 6\n"
                          "3. MUL R3, R1, R2     -> R3 = 42\n"
                          "Expected: R1=7, R2=6, R3=42\n"
                          "Note: MUL takes 12 cycles to execute!";
        break;
    case 8:
        testDescription = "Test 9: MUL with Dependencies\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 3\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 4\n"
                          "3. MUL R3, R1, R2     -> R3 = 12\n"
                          "4. ADD R4, R3, R1     -> R4 = 15 (waits for MUL)\n"
                          "Expected: R1=3, R2=4, R3=12, R4=15";
        break;
    case 9:
        testDescription = "Test 10: NAND Instruction\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 0xFF00 (65280)\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 0x0FF0 (4080)\n"
                          "3. NAND R3, R1, R2    -> R3 = ~(0xFF00 & 0x0FF0) = 0xF0FF (61695)\n"
                          "Expected: R1=65280, R2=4080, R3=61695";
        break;
    case 10:
        testDescription = "Test 11: NAND Chain\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 0xAAAA (43690)\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 0x5555 (21845)\n"
                          "3. NAND R3, R1, R2    -> R3 = ~(0xAAAA & 0x5555) = 0xFFFF (65535)\n"
                          "4. NAND R4, R3, R1    -> R4 = ~(0xFFFF & 0xAAAA) = 0x5555 (21845)\n"
                          "Expected: R1=43690, R2=21845, R3=65535, R4=21845";
        break;
    case 11:
        testDescription = "Test 12: BEQ Not Taken\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 10\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 20\n"
                          "3. BEQ R1, R2, 2      -> Branch NOT taken (10 != 20)\n"
                          "4. ADD R3, R1, R2     -> R3 = 30 (executes)\n"
                          "Expected: R1=10, R2=20, R3=30";
        break;
    case 12:
        testDescription = "Test 13: BEQ Taken (Misprediction)\n"
                          "Instructions:\n"
                          "0. LOAD R1, 100(R0)   -> R1 = 15\n"
                          "1. LOAD R2, 100(R0)   -> R2 = 15\n"
                          "2. BEQ R1, R2, 2      -> Branch TAKEN (15 == 15), jump to 5\n"
                          "3. ADD R3, R1, R2     -> SKIPPED (flushed)\n"
                          "4. SUB R4, R1, R2     -> SKIPPED (flushed)\n"
                          "5. MUL R5, R1, R2     -> R5 = 225 (branch target)\n"
                          "Expected: R1=15, R2=15, R3=0, R4=0, R5=225\n"
                          "Expected: 4 instructions committed (not 6!)";
        break;
    case 13:
        testDescription = "Test 14: CALL Instruction\n"
                          "Instructions:\n"
                          "1. LOAD R2, 100(R0)   -> R2 = 42\n"
                          "2. CALL 4             -> R1 = 2 (return addr), jump to 4\n"
                          "3-4. (skipped)\n"
                          "5. ADD R3, R2, R0     -> R3 = 42 (CALL target)\n"
                          "Expected: R1=2, R2=42, R3=42";
        break;
    case 14:
        testDescription = "Test 15: CALL and RET\n"
                          "Instructions:\n"
                          "1. LOAD R2, 100(R0)   -> R2 = 10\n"
                          "2. CALL 4             -> R1 = 2, jump to 4\n"
                          "3. ADD R3, R2, R2     -> R3 = 20 (RETURN TARGET)\n"
                          "4. (dummy)\n"
                          "5. MUL R4, R2, R2     -> R4 = 100\n"
                          "6. RET                -> Jump to R1 (addr 2)\n"
                          "Expected: R1=2, R2=10, R3=20, R4=100";
        break;
    case 15:
        testDescription = "Test 16: Mixed Operations\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 8\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 3\n"
                          "3. ADD R3, R1, R2     -> R3 = 11\n"
                          "4. MUL R4, R1, R2     -> R4 = 24\n"
                          "5. SUB R5, R4, R3     -> R5 = 13\n"
                          "6. STORE R5, 500(R0)  -> mem[500] = 13\n"
                          "Expected: R1=8, R2=3, R3=11, R4=24, R5=13, mem[500]=13";
        break;
    case 16:
        testDescription = "Test 17: All Instructions\n"
                          "Instructions:\n"
                          "1. LOAD R1, 100(R0)   -> R1 = 5\n"
                          "2. LOAD R2, 101(R0)   -> R2 = 3\n"
                          "3. ADD R3, R1, R2     -> R3 = 8\n"
                          "4. SUB R4, R1, R2     -> R4 = 2\n"
                          "5. MUL R5, R3, R4     -> R5 = 16\n"
                          "6. NAND R6, R1, R2    -> R6 = ~(5 & 3) = ~1 = 65534\n"
                          "7. STORE R5, 200(R0)  -> mem[200] = 16\n"
                          "Expected: R1=5, R2=3, R3=8, R4=2, R5=16, R6=65534, mem[200]=16";
        break;
    }

    outputDisplay->clear();
    outputDisplay->append("=== TEST CASE LOADED ===\n");
    outputDisplay->append(testDescription);
    outputDisplay->append("\n\nClick 'Run Simulation' to execute.");
}
void MainWindow::runSimulation()
{
    // Read configuration
    rob_size = robSizeSpinBox->value();
    load_rs_size = loadRSSpinBox->value();
    store_rs_size = storeRSSpinBox->value();
    add_sub_rs_size = addSubRSSpinBox->value();
    mul_rs_size = mulRSSpinBox->value();
    nand_rs_size = nandRSSpinBox->value();
    beq_rs_size = beqRSSpinBox->value();
    call_ret_rs_size = callRetRSSpinBox->value();

    load_latency = loadLatencySpinBox->value();
    store_latency = storeLatencySpinBox->value();
    add_sub_latency = addSubLatencySpinBox->value();
    mul_latency = mulLatencySpinBox->value();
    nand_latency = nandLatencySpinBox->value();
    beq_latency = beqLatencySpinBox->value();
    call_ret_latency = callRetLatencySpinBox->value();

    outputDisplay->clear();
    outputDisplay->append("=== STARTING SIMULATION ===\n");

    TomasuloSimulator sim(rob_size,
                          load_rs_size, store_rs_size, add_sub_rs_size,
                          mul_rs_size, nand_rs_size, beq_rs_size, call_ret_rs_size,
                          load_latency, store_latency, add_sub_latency,
                          mul_latency, nand_latency, beq_latency, call_ret_latency);

    std::vector<Instruction> instructions;
    int testIndex = testCaseCombo->currentIndex();

    switch(testIndex) {
    // STORE TESTS
    case 0:
        sim.get_memory()[500] = 99;
        instructions = {
            {LOAD, 1, 0, 0, 500, 0, false},
            {STORE, 0, 0, 1, 200, 1, false}
        };
        break;
    case 1:
        sim.get_memory()[100] = 10;
        sim.get_memory()[101] = 20;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {ADD, 3, 1, 2, 0, 2, false},
            {STORE, 0, 0, 3, 300, 3, false}
        };
        break;
    case 2:
        sim.get_memory()[100] = 5;
        sim.get_memory()[200] = 77;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 200, 1, false},
            {STORE, 0, 1, 2, 10, 2, false}
        };
        break;
    case 3:
        sim.get_memory()[100] = 11;
        sim.get_memory()[101] = 22;
        sim.get_memory()[102] = 33;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {LOAD, 3, 0, 0, 102, 2, false},
            {STORE, 0, 0, 1, 400, 3, false},
            {STORE, 0, 0, 2, 401, 4, false},
            {STORE, 0, 0, 3, 402, 5, false}
        };
        break;

    // ADD/SUB TESTS
    case 4:
        sim.get_memory()[100] = 15;
        sim.get_memory()[101] = 25;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {ADD, 3, 1, 2, 0, 2, false}
        };
        break;
    case 5:
        sim.get_memory()[100] = 50;
        sim.get_memory()[101] = 30;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {SUB, 3, 1, 2, 0, 2, false}
        };
        break;
    case 6:
        sim.get_memory()[100] = 10;
        sim.get_memory()[101] = 5;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {ADD, 3, 1, 2, 0, 2, false},
            {SUB, 4, 3, 2, 0, 3, false},
            {ADD, 5, 3, 4, 0, 4, false}
        };
        break;

    // MUL TESTS
    case 7:
        sim.get_memory()[100] = 7;
        sim.get_memory()[101] = 6;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {MUL, 3, 1, 2, 0, 2, false}
        };
        break;
    case 8:
        sim.get_memory()[100] = 3;
        sim.get_memory()[101] = 4;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {MUL, 3, 1, 2, 0, 2, false},
            {ADD, 4, 3, 1, 0, 3, false}
        };
        break;

    // NAND TESTS
    case 9:
        sim.get_memory()[100] = 0xFF00;
        sim.get_memory()[101] = 0x0FF0;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {NAND, 3, 1, 2, 0, 2, false}
        };
        break;
    case 10:
        sim.get_memory()[100] = 0xAAAA;
        sim.get_memory()[101] = 0x5555;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {NAND, 3, 1, 2, 0, 2, false},
            {NAND, 4, 3, 1, 0, 3, false}
        };
        break;

    // BEQ TESTS
    case 11:
        sim.get_memory()[100] = 10;
        sim.get_memory()[101] = 20;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {BEQ, 0, 1, 2, 2, 2, false},
            {ADD, 3, 1, 2, 0, 3, false}
        };
        break;
    case 12:
        sim.get_memory()[100] = 15;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 100, 1, false},
            {BEQ, 0, 1, 2, 2, 2, false},
            {ADD, 3, 1, 2, 0, 3, false},
            {SUB, 4, 1, 2, 0, 4, false},
            {MUL, 5, 1, 2, 0, 5, false}
        };
        break;

    // CALL/RET TESTS
    case 13:
        sim.get_memory()[100] = 42;
        instructions = {
            {LOAD, 2, 0, 0, 100, 0, false},
            {CALL, 4, 0, 0, 0, 1, false},
            {ADD, 3, 2, 0, 0, 2, false},
            {SUB, 4, 2, 0, 0, 3, false},
            {ADD, 3, 2, 0, 0, 4, false}
        };
        break;
    case 14:
        sim.get_memory()[100] = 10;
        instructions = {
            {LOAD, 2, 0, 0, 100, 0, false},
            {CALL, 4, 0, 0, 0, 1, false},
            {ADD, 3, 2, 2, 0, 2, false},
            {LOAD, 7, 0, 0, 0, 3, false},
            {MUL, 4, 2, 2, 0, 4, false},
            {RET, 0, 0, 0, 0, 5, false}
        };
        break;

    // COMPLEX TESTS
    case 15:
        sim.get_memory()[100] = 8;
        sim.get_memory()[101] = 3;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {ADD, 3, 1, 2, 0, 2, false},
            {MUL, 4, 1, 2, 0, 3, false},
            {SUB, 5, 4, 3, 0, 4, false},
            {STORE, 0, 0, 5, 500, 5, false}
        };
        break;
    case 16:
        sim.get_memory()[100] = 5;
        sim.get_memory()[101] = 3;
        instructions = {
            {LOAD, 1, 0, 0, 100, 0, false},
            {LOAD, 2, 0, 0, 101, 1, false},
            {ADD, 3, 1, 2, 0, 2, false},
            {SUB, 4, 1, 2, 0, 3, false},
            {MUL, 5, 3, 4, 0, 4, false},
            {NAND, 6, 1, 2, 0, 5, false},
            {STORE, 0, 0, 5, 200, 6, false}
        };
        break;
    }

    sim.load_instructions(instructions);
    outputDisplay->append(sim.get_output_log());
    outputDisplay->append("\n=== SIMULATION COMPLETE ===");
}

void MainWindow::resetSimulation()
{
    outputDisplay->clear();
    outputDisplay->setText("Simulator reset. Ready for new simulation.\n\nSelect a test case and click 'Load Test Case'.");
}

void MainWindow::updateDisplay(const QString& message)
{
    outputDisplay->append(message);
}

// ===== TomasuloSimulator Implementation =====

TomasuloSimulator::TomasuloSimulator(int rob_sz,
                                     int load_rs, int store_rs, int addsub_rs,
                                     int mul_rs, int nand_rs, int beq_rs, int callret_rs,
                                     int load_lat, int store_lat, int addsub_lat,
                                     int mul_lat, int nand_lat, int beq_lat, int callret_lat)
{
    rob_size = rob_sz;
    memory_size = 65536;
    registers = 8;

    memory = new uint16_t[memory_size];
    reorder_buffer = new ROB_Entry[rob_size];
    register_status = new int[registers];

    register_file[0] = 0;
    for (int i = 1; i < 8; i++) register_file[i] = 0;
    for (int i = 0; i < memory_size; i++) memory[i] = 0;

    tail = 0;
    head = 0;

    for (int i = 0; i < rob_size; i++) {
        reorder_buffer[i].busy = false;
        reorder_buffer[i].ready = false;
        reorder_buffer[i].branch_target = 0;
        reorder_buffer[i].predicted_taken = false;
    }

    for (int i = 0; i < registers; i++) register_status[i] = -1;

    functional_units[LOAD_UNIT].latency = load_lat;
    functional_units[STORE_UNIT].latency = store_lat;
    functional_units[BEQ_UNIT].latency = beq_lat;
    functional_units[CALL_RETURN_UNIT].latency = callret_lat;
    functional_units[ADD_SUB_UNIT].latency = addsub_lat;
    functional_units[NAND_UNIT].latency = nand_lat;
    functional_units[MULT_UNIT].latency = mul_lat;

    functional_units[LOAD_UNIT].reservation_stations.resize(load_rs);
    functional_units[STORE_UNIT].reservation_stations.resize(store_rs);
    functional_units[BEQ_UNIT].reservation_stations.resize(beq_rs);
    functional_units[CALL_RETURN_UNIT].reservation_stations.resize(callret_rs);
    functional_units[ADD_SUB_UNIT].reservation_stations.resize(addsub_rs);
    functional_units[NAND_UNIT].reservation_stations.resize(nand_rs);
    functional_units[MULT_UNIT].reservation_stations.resize(mul_rs);

    pc = 0;
    cycle = 0;
    instr_commited = 0;
    number_of_branches = 0;
    mispredicted_branches = 0;
}

TomasuloSimulator::~TomasuloSimulator()
{
    delete[] memory;
    delete[] reorder_buffer;
    delete[] register_status;
}

void TomasuloSimulator::log(const QString& msg)
{
    output_log += msg + "\n";
}

void TomasuloSimulator::load_instructions(const std::vector<Instruction>& instructions)
{
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
            log("\nAll instructions committed. Ending simulation.");
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
            log("\nROB empty and no more instructions to issue. Ending simulation.");
            break;
        }

        run();
    }

    if (cycle >= max_cycles) {
        log("\n!!! WARNING: Simulation stopped at max cycles !!!");
    }

    // Print results
    log(QString("\n=== SIMULATION RESULTS ==="));
    log(QString("Total cycles: %1").arg(cycle));
    log(QString("Instructions committed: %1").arg(instr_commited));
    log(QString("IPC: %1").arg((float)instr_commited / cycle, 0, 'f', 3));
    log(QString("Branches: %1").arg(number_of_branches));
    log(QString("Mispredictions: %1").arg(mispredicted_branches));
    if (number_of_branches > 0) {
        log(QString("Misprediction rate: %1%").arg((float)(mispredicted_branches * 100) / number_of_branches, 0, 'f', 1));
    }

    // Print registers
    log(QString("\n=== REGISTER FILE ==="));
    for (int i = 0; i < 8; i++) {
        log(QString("R%1 = %2").arg(i).arg(register_file[i]));
    }

    // Print memory
    log(QString("\n=== MEMORY (Non-Zero Locations) ==="));
    bool found_memory = false;
    for (int i = 0; i < memory_size; i++) {
        if (memory[i] != 0) {
            log(QString("mem[%1] = %2").arg(i).arg(memory[i]));
            found_memory = true;
        }
    }
    if (!found_memory) {
        log(QString("(No non-zero memory locations)"));
    }

    // Print timing table
    log(QString("\n=== INSTRUCTION TIMING ==="));
    for (size_t i = 0; i < timing_table.size(); i++) {
        log(QString("Inst %1: Issue=%2 Start=%3 Finish=%4 Write=%5 Commit=%6")
                .arg(i).arg(timing_table[i].issue).arg(timing_table[i].start_exec)
                .arg(timing_table[i].end_exec).arg(timing_table[i].write_result)
                .arg(timing_table[i].commit));
    }
}

void TomasuloSimulator::run()
{
    cycle++;
    log(QString("\n=== Cycle %1 ===").arg(cycle));
    log(QString("PC: %1, Committed: %2, ROB head: %3, tail: %4")
            .arg(pc).arg(instr_commited).arg(head).arg(tail));

    execute();
    write_back();
    commit();
    issue();
}

void TomasuloSimulator::issue()
{
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
    else if (instr.type == ADD || instr.type == SUB || instr.type == NAND || instr.type == MUL) {
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

    log(QString("Issued ROB[%1]: Qj=%2, Qk=%3").arg(tail).arg(rs_entry.Qj).arg(rs_entry.Qk));

    timing_table.push_back({cycle, cycle, 0, 0, 0});
    tail = (tail + 1) % rob_size;
    pc++;
}

void TomasuloSimulator::execute()
{
    for(auto& fu_pair : functional_units) {
        FunctionalUnit_structure& fu_struct = fu_pair.second;
        std::vector<int> still_executing;

        while(!fu_struct.executing.empty()) {
            int rob_index = fu_struct.executing.front();
            fu_struct.executing.pop();

            // Check if instruction was flushed
            if(!reorder_buffer[rob_index].busy) {
                log(QString("  ROB[%1] was flushed, skipping").arg(rob_index));
                continue;
            }

            for(auto& rs : fu_struct.reservation_stations) {
                if(rs.busy && rs.dest == rob_index) {
                    rs.cycles_remaining--;

                    if(rs.cycles_remaining == 0) {
                        uint16_t result = calculate_result(rs);
                        log(QString("  ROB[%1] finished! Value=%2").arg(rob_index).arg(result));
                        reorder_buffer[rob_index].ready = true;
                        reorder_buffer[rob_index].value = result;

                        if(rob_index < (int)timing_table.size()) {
                            timing_table[rob_index].end_exec = cycle;
                        }

                        if(rs.op == STORE && reorder_buffer[rob_index].dest == 0) {
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

        for(auto idx : still_executing) {
            fu_struct.executing.push(idx);
        }
    }
}

void TomasuloSimulator::write_back()
{
    for (int i = 0; i < rob_size; i++) {
        if(reorder_buffer[i].ready && reorder_buffer[i].busy) {
            if(i < (int)timing_table.size()) {
                timing_table[i].write_result = cycle;
            }
            broadcasting(i, reorder_buffer[i].value);
        }
    }
}

void TomasuloSimulator::commit()
{
    if(!reorder_buffer[head].busy || !reorder_buffer[head].ready)
        return;

    ROB_Entry& re = reorder_buffer[head];

    if(head < (int)timing_table.size()) {
        timing_table[head].commit = cycle;
    }

    if(re.type == BEQ) {
        number_of_branches++;
        bool actual_taken = (re.value == 1);

        log(QString("  Committing BEQ (PC=%1): actual=%2, predicted=%3")
                .arg(re.pc).arg(actual_taken).arg(re.predicted_taken));

        // Mark as committed
        if(re.pc < (int)instruction_committed.size()) {
            instruction_committed[re.pc] = true;
        }

        if(actual_taken != re.predicted_taken) {
            re.mispredicted = true;
            mispredicted_branches++;

            log(QString("  *** BRANCH MISPREDICTION DETECTED ***"));

            reorder_buffer[head].busy = false;
            int branch_rob = head;
            head = (head + 1) % rob_size;
            instr_commited++;

            deal_with_misprediction(branch_rob);

            if(actual_taken) {
                pc = re.branch_target;
                log(QString("  Redirecting PC to %1 (branch target)").arg(pc));
            } else {
                pc = re.pc + 1;
                log(QString("  Redirecting PC to %1 (fall-through)").arg(pc));
            }

            return;
        }
    }
    else if(re.type == CALL) {
        // CALL: Store return address in R1 and jump to target
        register_file[1] = re.value;  // Return address (PC+1)
        if(register_status[1] == head + 1) {
            register_status[1] = -1;
        }

        log(QString("  Committing CALL (PC=%1): R1=%2, jumping to %3")
                .arg(re.pc).arg(re.value).arg(re.dest));

        // Mark as committed
        if(re.pc < (int)instruction_committed.size()) {
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
        log(QString("  Jumping to address %1").arg(pc));

        return;
    }
    else if(re.type == RET) {
        // RET: Jump to address in R1
        int return_addr = re.value;  // Value contains R1 (return address)

        log(QString("  Committing RET (PC=%1): Returning to address %2")
                .arg(re.pc).arg(return_addr));

        // Mark as committed
        if(re.pc < (int)instruction_committed.size()) {
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
        log(QString("  Returning to address %1").arg(pc));

        return;
    }
    else if(re.type == STORE) {
        memory[re.dest] = re.value;
        log(QString("  Committed STORE: mem[%1] = %2").arg(re.dest).arg(re.value));
    }
    else if(re.type != BEQ && re.type != CALL && re.type != RET) {
        register_file[re.dest] = re.value;
        if(register_status[re.dest] == head + 1) {
            register_status[re.dest] = -1;
        }
    }

    // Mark as committed
    if(re.pc < (int)instruction_committed.size()) {
        instruction_committed[re.pc] = true;
    }

    reorder_buffer[head].busy = false;
    head = (head + 1) % rob_size;
    instr_commited++;
}

void TomasuloSimulator::broadcasting(int rob_index, uint16_t value)
{
    log(QString("  Broadcasting ROB[%1] = %2").arg(rob_index).arg(value));

    for(auto& fu_pair : functional_units) {
        for(auto& rs : fu_pair.second.reservation_stations) {
            if(rs.busy) {
                bool was_waiting = (rs.Qj == rob_index) || (rs.Qk == rob_index);

                if(rs.Qj == rob_index) {
                    rs.Vj = value;
                    rs.Qj = -1;
                    if((rs.op == LOAD || rs.op == STORE) && rs.address == 0) {
                        rs.address = value + rs.offset;
                    }
                }
                if(rs.Qk == rob_index) {
                    rs.Vk = value;
                    rs.Qk = -1;
                }

                if(was_waiting && rs.Qj == -1 && rs.Qk == -1) {
                    fu_pair.second.executing.push(rs.dest);
                }
            }
        }
    }
}

uint16_t TomasuloSimulator::calculate_result(const RS_Entry& rs)
{
    uint16_t result = 0;
    switch (rs.op) {
    case ADD: result = rs.Vj + rs.Vk; break;
    case SUB: result = rs.Vj - rs.Vk; break;
    case NAND: result = ~(rs.Vj & rs.Vk); break;
    case MUL: result = rs.Vj * rs.Vk; break;
    case LOAD: result = memory[rs.address]; break;
    case STORE: result = rs.Vk; break;
    case BEQ: result = (rs.Vj == rs.Vk) ? 1 : 0; break;
    case CALL: result = rs.Vj; break;
    case RET: result = rs.Vj; break;
    default: result = 0; break;
    }
    return result;
}

FunctionalUnit TomasuloSimulator::get_functional_unit(InstructionType type)
{
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
    default: return LOAD_UNIT;
    }
}

void TomasuloSimulator::deal_with_misprediction(int rob_index)
{
    log(QString("  Flushing ROB entries after control flow..."));

    int index = (rob_index + 1) % rob_size;
    int flushed_count = 0;

    while(index != tail) {
        if (reorder_buffer[index].busy) {
            log(QString("    Flushing ROB[%1] (PC=%2)").arg(index).arg(reorder_buffer[index].pc));

            if(reorder_buffer[index].type != BEQ &&
                reorder_buffer[index].type != CALL &&
                reorder_buffer[index].type != RET &&
                reorder_buffer[index].type != STORE) {
                for(int i = 0; i < registers; i++) {
                    if(register_status[i] == index + 1) {
                        register_status[i] = -1;
                    }
                }
            }

            reorder_buffer[index].busy = false;
            reorder_buffer[index].ready = false;
            flushed_count++;
        }
        index = (index + 1) % rob_size;
    }

    log(QString("  Flushed %1 instructions").arg(flushed_count));

    // Clear reservation stations and executing queues
    for(auto& fu_pair : functional_units) {
        for(auto& rs : fu_pair.second.reservation_stations) {
            if(rs.busy && !reorder_buffer[rs.dest].busy) {
                rs.busy = false;
            }
        }

        std::queue<int> new_queue;
        while(!fu_pair.second.executing.empty()) {
            int rob_idx = fu_pair.second.executing.front();
            fu_pair.second.executing.pop();
            if(reorder_buffer[rob_idx].busy) {
                new_queue.push(rob_idx);
            }
        }
        fu_pair.second.executing = new_queue;
    }

    tail = (rob_index + 1) % rob_size;
    log(QString("  ROB tail reset to %1").arg(tail));
}
