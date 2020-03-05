#include "mipswindow.h"
#include "ui_mipswindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QPainter>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include "cpu.h"

#define MAX_BUF 512
#define START_ADDRESS 0x00000000
#define NUM_LOC 1000
#define MAX_CYC 100000000
#define IFETCH 0
#define IDECODE 1
#define IEXEC 2
#define IMEM 3
#define IWRITE 4

const char* ExUnit[] = {"INTEGER", "FPADDER", "FPMULTIPLIER", "FPDIVIDER", "MAINMEMORY", "CACHE", "INSTRUCTIONCACHE", "DATACACHE"};

std::string toBin(unsigned int n)
{
        std::string s;
        for (int i = 31; i >= 0; --i)
        {
            s += ((n >> i) & 0x01) ? "1" : "0";
        }
        return s;
}

MIPSWindow::MIPSWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MIPSWindowClass)
{
    ui->setupUi(this);
    for (int i = 0; i < NUM_INT_REG; ++i)
    {
        ui->cGPR->addItem(QString("R").append(QString::number(i)));
    }
    for (int i = 0; i < NUM_FP_REG; ++i)
    {
        ui->cFPR->addItem(QString("F").append(QString::number(i)));
    }
    bool pipelined[NUM_EX_UNIT] = {false};
    pipelined[INTEGER] = true;
    pipelined[FPADD] = true;
    unsigned int cycles[NUM_EX_UNIT + 1];
    cycles[INTEGER] = 2;
    cycles[FPADD] = 4;
    cycles[FPMUL] = 6;
    cycles[FPDIV] = 20;
    cycles[MEMORY] = 5;
    cpu.init(pipelined, cycles);
    showMemory();
    loadRegisters();
    showPipeline = false;
}

MIPSWindow::~MIPSWindow()
{
    delete ui;
}

char* MIPSWindow::loadSource(char *file)
{
    char *fileName = fileEdited.toLatin1().data();
    std::string objCode = "";
    std::ifstream infile(fileName);
    char line[MAX_BUF];
    while (infile)
    {
        infile.getline(line, MAX_BUF);
        objCode += line;
        objCode += "\n";
    }
    return (char*)objCode.c_str();
}

void MIPSWindow::on_bLoadSrc_clicked()
{
   fileEdited = QFileDialog::getOpenFileName(this, tr("Open MIPS Instruction Src File"),
                                                 "C:/Users/sandipan/Desktop/proj/MIPS",
                                                 tr("MIPS (*.txt *.mip)"));
    if (fileEdited != QString::null)
    {
        txtEditorText = loadSource(fileEdited.toLatin1().data());
        ui->tEditor->setText(txtEditorText);
    }
}

void MIPSWindow::assembleSource()
{
       QProcess assembler;
       assembler.start("assemble.exe", QStringList() << fileEdited);
       if (!assembler.waitForStarted())
       {
         return;
       }

       if (!assembler.waitForFinished())
       {
         return;
       }
}

void MIPSWindow::writeHalt(unsigned int address)
{
    unsigned int halt = 0x12;
    cpu.loadMemory(address, halt);
}

bool MIPSWindow::loadBinary()
{
       unsigned int address = atoi(ui->tLoad->text().toLatin1().data());
       if (address < 0)
       {
           address = START_ADDRESS;
       }
       loadAt = address;
       char *pch = strtok (fileEdited.toLatin1().data(), "."); // "ins";
       if (pch != NULL)
       {
           int n;
           char file[MAX_BUF];
           sprintf(file, "%s.o", pch);
           QString fs = file;
           FILE *f = fopen(file, "rb");
           if (!f)
           {
               return false;
           }
           while (1)
           {
                fread(&n, 1, sizeof(int), f);
                if (feof(f))
                {
                    break;
                }
                cpu.loadMemory(address++, n);
           }
           fclose(f);
           writeHalt(address);
           showMemory();
       }
       return true;
}

void MIPSWindow::on_bAssemble_clicked()
{
    if (fileEdited != QString::null)
    {
       assembleSource();
       QString msg;
       if (loadBinary())
       {
            msg = fileEdited + " successfully assembled \n loaded at address: " + QString::number(loadAt);
            QMessageBox::information(this, tr("MIPS Simulator"), msg, QMessageBox::Ok);
       }
       else
       {
            msg = "could not load file " + fileEdited;
            QMessageBox::warning(this, tr("MIPS Simulator"), msg, QMessageBox::Ok);
       }
    }
    ui->tPC->setText("000000000");
}

void MIPSWindow::loadRegisters()
{
    ui->tPC->setText(QString::number(cpu.PC, 10));
    ui->tIR->setText(toBin(cpu.IR).c_str());
    ui->tOp->setText(cpu.opcode.c_str());
    ui->tImm->setText(QString::number(cpu.Imm, 10));
    ui->tA->setText(QString::number(cpu.A, 10));
    ui->tB->setText(QString::number(cpu.B, 10));
    ui->tALU->setText(QString::number(cpu.ALUOutput, 10));
    ui->tLMD->setText(QString::number(cpu.LMD, 10));
    ui->tGPR->setText(QString::number(cpu.R[ui->cGPR->currentIndex()]));
    ui->tFPR->setText(QString::number(cpu.F[ui->cFPR->currentIndex()]));
}

void MIPSWindow::saveRegisters()
{
    cpu.PC = atol(ui->tPC->text().toLatin1().data());
    //cpu.IR = atol(ui->tIR->text().toLatin1().data());
    //cpu.opcode = ui->tOp->text().toLatin1().data();
    //cpu.Imm = atol(ui->tImm->text().toLatin1().data());
    //cpu.A = atol(ui->tA->text().toLatin1().data());
    //cpu.B = atol(ui->tB->text().toLatin1().data());
    //cpu.ALUOutput = atol(ui->tALU->text().toLatin1().data());
    //cpu.LMD = atol(ui->tLMD->text().toLatin1().data());
    cpu.R[ui->cGPR->currentIndex()] = atol(ui->tGPR->text().toLatin1().data());
    cpu.F[ui->cFPR->currentIndex()] = atol(ui->tFPR->text().toLatin1().data());
}

void MIPSWindow::showMemory()
{
    int start = atoi(ui->tLoc->text().toLatin1().data());
    std::string text;
    for (int i = start; i < start + NUM_LOC; ++i)
    {
        text += toBin(cpu.getMemory(i)) + "\n";
    }
    ui->tMemory->setText(text.c_str());
}

void MIPSWindow::on_bShow_clicked()
{
    showMemory();
}

void MIPSWindow::execute(bool step = false)
{
    try
    {
        cpu.startPipelinedExecution(step);
        std::string results;
        char buf[512];
        for (std::map<int, Instruction>::iterator it = cpu.inst.begin(); it != cpu.inst.end(); ++it )
        {
            Instruction ins = it->second;
            sprintf(buf, "%-20s%6d%6d%6d%6d\n", ins.srccode.c_str(), ins.ift, ins.idt, ins.ext, /*ins.mt,*/ ins.wt);
            results += buf;
        }
        QString text = results.c_str();
        text += "\n\n";
        for (int i = 0; i < NUM_EX_UNIT + 1; ++i)
        {
           char txt[MAX_BUF];
           sprintf(txt, "%s: %d %d\n", ExUnit[i], cpu.cycles[i], cpu.pipelined[i % NUM_EX_UNIT]);
           text += txt;
        }
        ui->tResults->setText(text);
        ui->gResult->setTitle("Results");
    }
    catch (MemException e)
    {
        QMessageBox::warning(this, tr("MIPS Simulator"),
                            tr("Memory protection violation!.\n"),
                            QMessageBox::Ok);
    }
}

void MIPSWindow::on_bExec_clicked()
{
    cpu.PC = atoi(ui->tPC->text().toLatin1().data());
    execute();
    loadRegisters();
    showPipeline = true;
    update();
}

void MIPSWindow::on_cGPR_currentIndexChanged(int index)
{
    ui->tGPR->setText(QString::number(cpu.R[index]));
}

void MIPSWindow::on_cFPR_currentIndexChanged(int index)
{
    ui->tFPR->setText(QString::number(cpu.F[index]));
}

void MIPSWindow::on_bSaveReg_clicked()
{
    saveRegisters();
}

void MIPSWindow::on_bStep_clicked()
{
    cpu.PC = atoi(ui->tPC->text().toLatin1().data());
    execute(true);
    loadRegisters();
}

void MIPSWindow::paintEvent ( QPaintEvent * event )
{
    QPainter p(this);
    if (showPipeline)
    {
        p.setBrush(Qt::cyan);
        for (int i = 0, x = 110, y = 250; i < MAX_STAGE; x += 25, ++i)
        {
            p.drawRect(x, y, 25, 20);
            p.drawText(x + 2, y + 15, QString::number(i + 1));
        }
        int y = 270;
        for (std::map<int, Instruction>::iterator it = cpu.inst.begin(); it != cpu.inst.end(); ++it )
        {
            int x = 10, i = 0;
            Instruction ins = (*it).second;
            p.drawRect(x, y, 100, 20);
            p.drawText(x + 2, y + 15, ins.srccode.c_str());
            for (i = 1, x = 110; i <= MAX_STAGE; x += 25, ++i)
            {
                p.drawRect(x, y, 25, 20);
                QString msg;
                if (i == ins.ift)
                {
                    msg = "IF";
                }
                else if (i == ins.idt)
                {
                    msg = "ID";
                }
                else if (i > ins.ext - cpu.cycles[ins.unit] && i <= ins.ext)
                {
                    msg = "EX" + QString::number(cpu.cycles[ins.unit] - ins.ext + i);
                }
                /*else if (i == ins.mt)
                {
                    msg = "MEM";
                }*/
                else if (i == ins.wt)
                {
                    msg = "WB";
                }
                else if ((i > ins.ift && i < ins.idt)
                       || (i > ins.idt && i < ins.ext)
                       || (i > ins.ext && i < ins.wt))
                {
                    msg = "Stall";
                }
                p.drawText(x + 2, y + 15, msg);
            }
            y += 20;
        }
    }
    else
    {
       p.fillRect(10, 270, MAX_STAGE * 25, MAX_INS * 20, Qt::white);
    }
}

void MIPSWindow::on_cShowPipeline_stateChanged(int state)
{
    showPipeline = Qt::Checked;
    update();
}

void MIPSWindow::parseConfig(char *cfg)
{
    std::ifstream infile(cfg);
    char line[MAX_BUF];
    unsigned int cycles[NUM_EX_UNIT + 1];
    bool pipelined[NUM_EX_UNIT];

    /*cycles[INTEGER] = 1;
    pipelined[INTEGER] = true;
    cycles[FPADD] = 4;
    cycles[FPMUL] = 6;
    cycles[FPDIV] = 20;
    cycles[MEMORY] = 5;*/

    for (int i = 0; i < NUM_EX_UNIT + 1; ++i)
    {
        cycles[i] = 1;
        pipelined[i] = false;
    }
    cycles[INTEGER] = 1;
    pipelined[INTEGER] = true;

    int iblksize = 2, inblk = 8, inassoc = 1, ireplace = LRU;
    int dblksize = 2, dnblk = 8, dnassoc = 4, dreplace = LRU;
    while (infile)
    {
        infile.getline(line, MAX_BUF);
        std::string sline = line;
        sline.erase(std::remove_if(sline.begin(), sline.end(), static_cast<int(*)(int)>(std::isspace)), sline.end());
        std::transform(sline.begin(), sline.end(), sline.begin(), toupper);
        strcpy(line, sline.c_str());
        char *pch = strtok (line, ":,");
        int val = -1;
        if (pch)
        {
            for (int i = 0; i < NUM_EX_UNIT + 2; ++i)
            {
                if (!strcmp(pch, ExUnit[i]))
                {
                    val = i;
                    break;
                }
            }
        }
        if (val >= NUM_EX_UNIT + 2)
        {
            bool icache = val == NUM_EX_UNIT + 2;
            bool dcache = val == NUM_EX_UNIT + 3;
            pch = strtok (NULL, ":,");
            if (pch)
            {
                if (icache)
                {
                    iblksize = atoi(pch);
                }
                else if(dcache)
                {
                    dblksize = atoi(pch);
                }
            }
            pch = strtok (NULL, ":, ");
            if (pch)
            {
                if (icache)
                {
                    inblk = atoi(pch);
                }
                else if(dcache)
                {
                    dnblk = atoi(pch);
                }
            }
            pch = strtok (NULL, ":, ");
            if (pch)
            {
                if (icache)
                {
                    inassoc = atoi(pch);
                }
                else if(dcache)
                {
                    dnassoc = atoi(pch);
                }
            }
            pch = strtok (NULL, ":, ");
            if (pch)
            {
                if (icache)
                {
                    ireplace = !strcmp(pch, "LRU") ? LRU : RANDOM;
                }
                else if(dcache)
                {
                    dreplace = !strcmp(pch, "LRU") ? LRU : RANDOM;
                }
            }
        }
        else
        {
            pch = strtok (NULL, ":,");
            if (pch && val >=0 && val < NUM_EX_UNIT + 2)
            {
                cycles[val] = atoi(pch);
            }
            pch = strtok (NULL, ":, ");
            if (pch && val >= 0 && val < NUM_EX_UNIT)
            {
                pipelined[val] = !strcmp(pch, "YES");
            }
        }
    }
    cpu.init(pipelined, cycles);
    cpu.init(inblk / inassoc, inassoc, iblksize, ireplace, dnblk / dnassoc, dnassoc, dblksize, dreplace);
}

void MIPSWindow::on_bLoadConfig_clicked()
{
    QString cfg = QFileDialog::getOpenFileName(this, tr("Open MIPS Config File"),
                                                 "C:/Users/sandipan/Desktop/proj/MIPS",
                                                 tr("MIPS (*.txt *.cfg)"));
    if (cfg != QString::null)
    {
        parseConfig(cfg.toLatin1().data());
        QString text;
        char txt[MAX_BUF];
        for (int i = 0; i < NUM_EX_UNIT; ++i)
        {
           sprintf(txt, "%s: %d %d\n", ExUnit[i], cpu.cycles[i], cpu.pipelined[i]);
           text += txt;
        }
        sprintf(txt, "%s: %d\n", ExUnit[MEMORY], cpu.cycles[MEMORY]);
        text += txt;
        sprintf(txt, "%s: %d\n", ExUnit[CACHE], cpu.cycles[CACHE]);
        text += txt;
        sprintf(txt, "%s: %d %d %d %d\n", ExUnit[NUM_EX_UNIT + 2], cpu.ICache.nSets, cpu.ICache.kWay, cpu.ICache.blkSz, cpu.ICache.replacementStrategy);
        text += txt;
        sprintf(txt, "%s: %d %d %d %d\n", ExUnit[NUM_EX_UNIT + 3], cpu.DCache.nSets, cpu.DCache.kWay, cpu.DCache.blkSz, cpu.ICache.replacementStrategy);
        text += txt;
        ui->tResults->setText(text);
        ui->gResult->setTitle("Config File");
    }
}
