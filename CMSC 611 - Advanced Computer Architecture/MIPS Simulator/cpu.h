/* // ------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ------------------------------------------------------------ */

#ifndef CPU_H
#define CPU_H

#include <string>
#include <queue>
#include <vector>
#include <map>

#define NUM_INT_REG 32
#define NUM_FP_REG 32
#define MAX_MEM 2 << 20
#define INTEGER 0
#define FPADD 1
#define FPMUL 2
#define FPDIV 3
#define NUM_EX_UNIT 4
#define MEMORY 4
#define CACHE 5
#define NOT_STARTED -1
#define MAX_INS 20
#define MAX_STAGE 50
#define BOX_SIDE 30
#define RANDOM 0
#define LRU 1

#include <exception>

enum eStage
{
    IF, ID, EX, /*MEM,*/ WB, ST
};

static const char *Stage[] = {"IF", "ID", "EX", /*"MEM",*/ "WB", "STALL"};

class MemException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "Memory protection violation";
  }
};

struct Instruction
{
    unsigned int id;
    std::string opcode, srccode;
    unsigned int objcode, unit;
    unsigned int src1, src2, dst, cycles;
    unsigned int ift, idt, ext, /*mt,*/ wt;
    Instruction() : ift(NOT_STARTED), idt(NOT_STARTED), ext(NOT_STARTED), /*mt(NOT_STARTED),*/ wt(NOT_STARTED)
    {
    }
};

struct CacheBlock
{
    bool valid;
    unsigned int tag;
    std::vector<unsigned int> data;
};

struct CacheSet
{
    std::vector<CacheBlock> blocks;
    CacheSet(int kway = 1) : blocks(kway)
    {
    }
};

struct Cache
{
    std::vector<CacheSet> sets;
    int nSets, kWay, blkSz, replacementStrategy;
    Cache(int nsets = 8, int kway = 1, int blksz = 2, int replacement = LRU)
            : nSets(nsets), kWay(kway), blkSz(blksz), replacementStrategy(replacement)
    {
        sets.resize(nsets, CacheSet(kway));
    }
    void init(int nsets, int kway, int blksz, int replacement = LRU)
    {
        nSets = nsets;
        kWay = kway;
        blkSz = blksz;
        replacementStrategy = replacement;
        sets.clear();
        sets.resize(nsets, CacheSet(kway));
    }
    std::vector<unsigned int> get(unsigned int address)
    {
        return sets[0].blocks[address].data;
    }
    void put(unsigned int address, int data)
    {
        //sets[0].blocks[address] = data;
    }
};

class CPU
{
    public:

    // Integer registers
    unsigned int R[NUM_INT_REG];
    // FP registers
    double F[NUM_FP_REG];
    // PC
    unsigned int PC;
    // IR
    unsigned int IR;
    // LMD
    unsigned int LMD;
    unsigned int ALUOutput;

    unsigned int A, B, Imm;

    unsigned int b05;
    unsigned int b1620;
    unsigned int b2125;
    unsigned int b025;
    unsigned int b2631;

    unsigned int newId;

    unsigned int RReady[NUM_INT_REG];
    unsigned int FReady[NUM_FP_REG];

    std::string opcode;

    MemException memex;

    Cache ICache, DCache;

    // Memory
    std::vector<unsigned int> Mem;  //Mem[MAX_MEM];

    std::queue<Instruction> ifq, idq; /*imq,*/
    std::vector<Instruction> ieq[NUM_EX_UNIT];
    std::queue<Instruction> iwq;
    std::vector<Instruction> fwdIns;

    bool pipelined[NUM_EX_UNIT];
    unsigned int cycles[NUM_EX_UNIT + 2];
    std::map<int, Instruction> inst;

    unsigned int clock;
    void clockTick();

    CPU::CPU();
    CPU::CPU(bool *pipelinedUnit, unsigned int *clockCycles);
    void init();
    void init(bool *pipelinedUnit, unsigned int *clockCycles);
    void init(int insets, int ikway, int iblksz, int ireplace, int dnsets, int dkway, int dblksz, int dreplace);
    void loadMemory(unsigned int address, unsigned int value);
    unsigned int getMemory(unsigned int address);
    bool isValid(unsigned int address);
    void IFetch(Instruction&);
    void IDecode(Instruction&);
    void IExecute(Instruction&);
    //void IMemory(Instruction&);
    void IWriteBack(Instruction&);
    void dissamble(Instruction&);
    void doForwarding(std::vector<Instruction>&);
    void startPipelinedExecution(bool step);
    bool isRAWID();
    bool isRAWEX(Instruction&);
    bool isRAWMEM(Instruction&);
    void flushPipeline();
    unsigned int mask(int n, int k);
};

#endif // CPU_H
