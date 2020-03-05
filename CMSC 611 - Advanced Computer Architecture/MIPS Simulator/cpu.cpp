#include <iterator>
#include "cpu.h"
#define READY -1

const char* EXUnit[] = {"INTEGER", "FPADDER", "FPMULTIPLIER", "FPDIVIDER"};

CPU::CPU() : Mem(MAX_MEM)
{
    init();
}

CPU::CPU(bool *pipelinedUnit, unsigned int *clockCycles) : Mem(MAX_MEM)
{
    init();
    init(pipelinedUnit, clockCycles);
}

void CPU::init()
{
    for (int i = 0; i < NUM_INT_REG; ++i)
    {
        R[i] = 0;
        RReady[i] = READY;
    }
    for (int i = 0; i < NUM_FP_REG; ++i)
    {
        F[i] = 0;
        FReady[i] = READY;
    }
    PC = IR = LMD = ALUOutput = A = B = Imm = 0;
    newId = 0;
}

void CPU::init(bool *pipelinedUnit, unsigned int *clockCycles)
{
    for (int i = 0; i < NUM_EX_UNIT; ++i)
    {
        pipelined[i] = pipelinedUnit[i];
    }
    for (int i = 0; i < NUM_EX_UNIT + 2; ++i)
    {
        cycles[i] = clockCycles[i];
    }
}

void CPU::init(int insets, int ikway, int iblksz, int ireplace, int dnsets, int dkway, int dblksz, int dreplace)
{
    ICache.init(insets, ikway, iblksz, ireplace);
    DCache.init(dnsets, dkway, dblksz, dreplace);
}

unsigned int CPU::mask(int n, int k)
{
    return ((1 << n) - 1) << k;
}

unsigned int CPU::getMemory(unsigned int address)
{
    return isValid(address) ? Mem[address] : 0;
}

void CPU::loadMemory(unsigned int address, unsigned int value)
{
    if (isValid(address))
    {
        Mem[address] = value;
    }
}

bool CPU::isValid(unsigned int address)
{
    bool valid = address >= 0 && address < MAX_MEM;
    if (!valid)
    {
        throw memex;
    }
    return valid;
}

void CPU::dissamble(Instruction& ins)
{
    char instr[128];
    std::string opcode = ins.opcode;
    if (opcode == "LW" || opcode == "SW")
    {
        if (opcode == "LW" && RReady[b1620] == READY)
        {
            RReady[b1620] = ins.id;
        }
        ins.src1 = b2125; ins.dst = b1620;
    }
    else if (opcode == "LD" || opcode == "SD")
    {
        sprintf(instr, "%s F%d, %d(R%d)", opcode.c_str(), b1620, Imm, b2125);
        if (opcode == "LD" && FReady[b1620] == READY)
        {
            FReady[b1620] = ins.id;
        }
        ins.src1 = b2125; ins.dst = b1620;
    }
    else if (opcode == "ADD.D" || opcode == "SUB.D" || opcode == "MUL.D" || opcode == "DIV.D")
    {
        sprintf(instr, "%s F%d, F%d, F%d", opcode.c_str(), (Imm >> 6) & 0x1F, (Imm >> 11) & 0x1F, b1620);
        if (FReady[(Imm >> 6) & 0x1F] == READY)
        {
            FReady[(Imm >> 6) & 0x1F] = ins.id;
        }
        ins.dst = (Imm >> 6) & 0x1F; ins.src1 = (Imm >> 11) & 0x1F; ins.src2 = b1620;
    }
    else if (opcode == "DADD" || opcode == "DSUB")
    {
        sprintf(instr, "%s R%d, R%d, R%d", opcode.c_str(), (Imm >> 11) & 0x1F, b2125, b1620);
        if (RReady[(Imm >> 11) & 0x1F] == READY)
        {
            RReady[(Imm >> 11) & 0x1F] = ins.id;
        }
        ins.dst = (Imm >> 11) & 0x1F; ins.src1 = b2125; ins.src2 = b1620;
    }
    else if (opcode == "BEQ" || opcode == "BNE")
    {
        sprintf(instr, "%s R%d, R%d, %d", opcode.c_str(), b2125, b1620, Imm);
        ins.dst = (Imm >> 11) & 0x1F; ins.src1 = b2125; ins.src2 = b1620;
    }
    else if (opcode == "J")
    {
        sprintf(instr, "%s %d", opcode.c_str(), b025);
        ins.dst =  b025;
    }
    ins.srccode = (opcode == "HALT" || opcode == "NOP") ? ins.opcode: instr;
    ins.cycles = cycles[ins.unit];
}

void CPU::IFetch(Instruction& ins)
{
    ins.id = ++newId;
    ins.objcode = IR = getMemory(PC);
    ++PC;
}

void CPU::IDecode(Instruction& ins)
{
    b05 = (IR & 0x3F);
    b1620 = (IR >> 16) & 0x1F;
    b2125 = (IR >> 21) & 0x1F;
    b025 = (IR & 0x1FFFFF);
    b2631 = (IR >> 26);
    A = R[b2125];
    B = R[b1620];
    Imm = (IR & 0xFFFF);
    std::string opcode;
    switch (b2631)
    {
        case 0x23 : opcode = "LW"; ins.unit = MEMORY; break;
        case 0x2B : opcode = "SW"; ins.unit = MEMORY;  break;
        case 0x37 : opcode = "LD"; ins.unit = MEMORY;  break;
        case 0x3F : opcode = "SD"; ins.unit = MEMORY;  break;
        case 0x02 : opcode = "J";  ins.unit = INTEGER; PC = b025; break;
        case 0x04 : opcode = "BEQ"; if (A == B) PC += Imm;  ins.unit = INTEGER; break;
        case 0x05 : opcode = "BNE"; if (A != B) PC += Imm;  ins.unit = INTEGER; break;
        case 0x00 :
                    switch (b05)
                    {
                        case 0x2C : opcode = "DADD";  ins.unit = INTEGER; break;
                        case 0x2E : opcode = "DSUB";  ins.unit = INTEGER; break;
                        case 0x12 : opcode = "HALT"; ins.unit = INTEGER; break;
                        default : opcode = "INVALID"; ins.unit = INTEGER;
                    }
                    break;
        case 0x11 :
                    switch (b05)
                    {
                        case 0x00 : opcode = "ADD.D";  ins.unit = FPADD; break;
                        case 0x01 : opcode = "SUB.D";  ins.unit = FPADD;  break;
                        case 0x02 : opcode = "MUL.D";  ins.unit = FPMUL; break;
                        case 0x03 : opcode = "DIV.D";  ins.unit = FPDIV; break;
                        default : opcode = "INVALID"; ins.unit = INTEGER;
                    }
                    break;
        default : opcode = "INVALID"; ins.unit = INTEGER;
    }    
    if (IR == 0x00000000)
    {
        opcode = "NOP"; ins.unit = INTEGER;
    }
    ins.opcode = this->opcode = opcode;
    dissamble(ins);
}

void CPU::IExecute(Instruction& ins)
{
    std::string opcode = ins.opcode;
    if (opcode == "LW" || opcode == "SW" || opcode == "LD" || opcode == "SD")
    {
        ALUOutput = A + Imm;
    }
    if (opcode == "LW" || opcode == "LD")
    {
       LMD = getMemory(ALUOutput);
    }
    else if (opcode == "SW" || opcode == "SD")
    {
       loadMemory(ALUOutput, B);
    }
}

/*void CPU::IMemory(Instruction& ins)
{
    std::string opcode = ins.opcode;
    if (opcode == "LW" || opcode == "LD")
    {
       LMD = getMemory(ALUOutput);       
    }
    else if (opcode == "SW" || opcode == "SD")
    {
       loadMemory(ALUOutput, B);
    }
}*/

void CPU::IWriteBack(Instruction& ins)
{
    std::string opcode = ins.opcode;
    if (opcode == "LD")
    {
       F[b1620] = LMD;
    }
    else if (opcode == "LW")
    {
       R[b1620] = LMD;
    }
}

void CPU::flushPipeline()
{
    while (!iwq.empty())
    {
        iwq.pop();
    }
    /*while (!imq.empty())
    {
        imq.pop();
    }*/
    ieq[INTEGER].clear();
    ieq[FPADD].clear();
    ieq[FPMUL].clear();
    ieq[FPDIV].clear();
    while (!idq.empty())
    {
        idq.pop();
    }
    while (!idq.empty())
    {
        idq.pop();
    }
}

void CPU::startPipelinedExecution(bool step = true)
{
    bool stop = false;
    clock = 1;
    newId = 0;

    flushPipeline();
    inst.clear();

    FILE *f = fopen("out.txt", "w");

    for (int i = 0; i < 10000; ++i)
    {
        fwdIns.clear();

        if (!iwq.empty())
        {
            Instruction ins = iwq.front();
            IWriteBack(ins);
            ins.wt = clock;
            iwq.pop();
            inst.insert(std::pair<int, Instruction>(ins.ift, ins));
        }

        /*if (!imq.empty())
        {
            Instruction ins = imq.front();
            stall = isRAWMEM(ins);
                if (!stall)
                {
                IMemory(ins);
                ins.mt = clock;
                iwq.push(ins);
                imq.pop();
                fwdIns.push_back(ins);
                fprintf(f, "%25s (MEM) : %6d %6d %6d %6d %6d\n", ins.srccode.c_str(), ins.ift, ins.idt, ins.ext, ins.mt, ins.wt);
                if (!imq.empty())
                {
                    ins = imq.front();
                    if (!stallAfter || ins.id < stallAfter)
                    {
                        stallAfter = ins.id;
                    }
                }
            }
        }*/

        for (int k = 0; k < NUM_EX_UNIT; ++k)
        {
            if (ieq[k].empty())
            {
                continue;
            }
            for (std::vector<Instruction>::iterator it = ieq[k].begin(); it != ieq[k].end(); )
            {
                Instruction ins = *it;
                int unit = ins.unit % NUM_EX_UNIT;
                if (isRAWEX(ins))
                {
                    break;
                }
                else
                {
                    if (ins.cycles == cycles[ins.unit]) // instruction starts execution
                    {
                        if (unit == FPADD || unit == FPMUL || unit == FPDIV || ins.opcode == "LD")
                        {
                            FReady[ins.dst] = ins.id;
                        }
                        else if (unit == INTEGER)
                        {
                            RReady[ins.dst] = ins.id;
                        }
                    }
                    --ins.cycles;
                    *it = ins;
                    if (!ins.cycles)    // instruction finishes execution
                    {
                        IExecute(ins);
                        ins.ext = clock;
                        iwq.push(ins);
                        it = ieq[k].erase(it);
                        fwdIns.push_back(ins);
                    }
                    else if (pipelined[k]) // process next instruction
                    {
                        ++it;
                    }
                    else
                    {
                        break;
                    }
                }
                fprintf(f, "%6d %25s : (EX: %s) %6d %6d %6d %6d\n", ins.cycles, ins.srccode.c_str(), EXUnit[k], ins.ift, ins.idt, ins.ext, /*ins.mt,*/ ins.wt);
            }
        }

        /*bool stall = occupied[unit] || (!pipelined[unit] && qsize[unit] && ins.cycles == cycles[ins.unit])
                    || ((ins.cycles == cycles[ins.unit]) && isRAWEX(ins))
                    || (stallAfter && ins.id > stallAfter);*/

        if (!idq.empty())
        {
            Instruction ins = idq.front();            
            if (!isRAWID())
            {
                IDecode(ins);
                stop = ins.opcode == "HALT";
                ins.idt = clock;
                ieq[ins.unit % NUM_EX_UNIT].push_back(ins);
                idq.pop();
                fprintf(f, "%25s : (ID) %6d %6d %6d %6d\n", ins.srccode.c_str(), ins.ift, ins.idt, ins.ext, /*ins.mt,*/ ins.wt);
            }
        }

        if (!stop)
        {
            Instruction ins;
            IFetch(ins);
            ins.ift = clock;
            idq.push(ins);
            fprintf(f, "%25s : (IF) %6d %6d %6d %6d\n", ins.srccode.c_str(), ins.ift, ins.idt, ins.ext, /*ins.mt,*/ ins.wt);
        }

        clockTick();

        if (step || (ifq.empty() && idq.empty()
            && ieq[INTEGER].empty() && ieq[FPADD].empty() && ieq[FPMUL].empty() && ieq[FPDIV].empty()
            && iwq.empty()))
        {
            fwdIns.clear();
            break;
        }

        if (fwdIns.size() > 0)
        {
            doForwarding(fwdIns);
        }

        /*for (int i = 0; i < fwdIns.size(); ++i)
        {
           if (fwdIns[i].unit == INTEGER)
           {
               fprintf(f, "FWD: R[%d] %d", fwdIns[i].dst, RReady[fwdIns[i].dst]);
           }
        }*/

        fprintf(f, "\n\n");
    }

    std::map<int, Instruction>::iterator it;
    for (it = inst.begin(); it != inst.end(); ++it )
    {
        Instruction ins = (*it).second;
        fprintf(f, "%-20s%6d%6d%6d%6d\n", ins.srccode.c_str(), ins.ift, ins.idt, ins.ext, ins.wt);
    }
    fclose(f);
}

void CPU::doForwarding(std::vector<Instruction>& vIns)
{
    for (int i = 0; i < vIns.size(); ++i)
    {
       std::string opcode = vIns[i].opcode;
       if (opcode == "DADD" || opcode == "DSUB" || opcode == "LW")
       {
           RReady[vIns[i].dst] = READY;
       }
       else if (opcode == "ADD.D" || opcode == "SUB.D" ||  opcode == "MUL.D" || opcode == "DIV.D"  || opcode == "LD")
       {
           FReady[vIns[i].dst] = READY;
       }
   }
}

bool CPU::isRAWID()
{
    bool raw = false;
    if ((IR >> 26) == 0x04 || (IR >> 26) == 0x05)
    {
        unsigned int src1 = (IR >> 21) & 0x1F, src2 = (IR >> 16) & 0x1F, id = newId + 1;
        raw = (RReady[src1] != READY && RReady[src1] < id) || (RReady[src2] != READY && RReady[src2] < id);
    }
    return raw;
}

bool CPU::isRAWEX(Instruction& ins)
{
    bool raw = false;
    if (ins.opcode == "DADD" || ins.opcode == "DSUB")
    {
        raw = (RReady[ins.src1] != READY && RReady[ins.src1] < ins.id) || (RReady[ins.src2] != READY && RReady[ins.src2] < ins.id);
    }
    else if (ins.unit == FPADD || ins.unit == FPMUL || ins.unit == FPDIV)
    {
        raw = (FReady[ins.src1] != READY && FReady[ins.src1] < ins.id) || (FReady[ins.src2] != READY && FReady[ins.src2] < ins.id);
    }
    else if (ins.opcode == "SD")
    {
        raw = FReady[ins.src1] != READY && FReady[ins.src1] < ins.id;
    }
    else if (ins.opcode == "SW")
    {
        raw = RReady[ins.src1] != READY && RReady[ins.src1] < ins.id;
    }
    return raw;
}

/*bool CPU::isRAWMEM(Instruction& ins)
{
    return (ins.opcode == "SD"  && FReady[ins.src1] != READY && FReady[ins.src1] < ins.id) || (ins.opcode == "SW" && RReady[ins.src1] != READY && RReady[ins.src1] < ins.id);
}*/

void CPU::clockTick()
{
    ++clock;
}
